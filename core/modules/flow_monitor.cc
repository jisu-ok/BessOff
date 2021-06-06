#include "flow_monitor.h"

#include "utils/ether.h"
#include "utils/ip.h"
#include "utils/tcp.h"
#include "utils/flow.h"

using bess::utils::Ethernet;
using bess::utils::Ipv4;
using bess::utils::Tcp;
using bess::utils::FiveTupleFlow;
using bess::utils::FiveTupleFlowHash;

const Commands FlowMonitor::cmds = {
    {"get_stats", "FlowMonitorCommandGetStatsArg", MODULE_CMD_FUNC(&FlowMonitor::CommandGetStats), Command::THREAD_SAFE},
};

void FlowMonitor::ProcessBatch(Context *ctx, bess::PacketBatch *batch) {


    uint64_t now_ns = tsc_to_ns(rdtsc());

    // mcslock_note_t mynode;
    // mcs_lock(&lock_, &mynode);

    // gate_idx_t incoming_gate = ctx->current_igate;

    int cnt = batch->cnt();

    for (int i = 0; i < cnt; i++) {
        bess::Packet *pkt = batch->pkts()[i];

        Ethernet *eth = pkt->head_data<Ethernet *>();
        Ipv4 *ip = reinterpret_cast<Ipv4 *>(eth + 1);

        // Flow monitor is now only done for TCP and UDP
        if ((ip->protocol != Ipv4::Proto::kTcp) && (ip->protocol != Ipv4::Proto::kUdp)) {
            // EmitPacket(ctx, pkt, 0);
            continue;
        }

        bool isTcp = ip->protocol == Ipv4::Proto::kTcp ? true : false;

        size_t ip_bytes = ip->header_length << 2;
        // TCP and UDP have same layout for src/dst port numbers
        Tcp *tcp = reinterpret_cast<Tcp *>(reinterpret_cast<uint8_t *>(ip) + ip_bytes);

        FiveTupleFlow flow;
        flow.src_ip = ip->src;
        flow.dst_ip = ip->dst;
        flow.src_port = tcp->src_port;
        flow.dst_port = tcp->dst_port;
        flow.protocol = ip->protocol;

        std::unordered_map<FiveTupleFlow, FlowStats, FiveTupleFlowHash>::iterator it = flow_map_.find(flow);

        if (it != flow_map_.end()) {
            // Existing flow
            if (isTcp && (tcp->flags & Tcp::Flag::kFin)) {
                flow_map_.erase(it);
            }
            else{
                it->second.SetLastPacketTime(now_ns);
                it->second.IncNumPkts();
                it->second.IncNumBytes(ip->length.value());
                if (isTcp) {
                    it->second.AddFlags(tcp->flags);
                }
            }
        }
        else {
            // New flow. If TCP, new flow must starts with SYN
            if ((isTcp && (tcp->flags & Tcp::Flag::kSyn)) || !isTcp) {
                std::tie(it, std::ignore) = flow_map_.emplace(std::piecewise_construct, std::make_tuple(flow), std::make_tuple());
                it->second.SetFirstPacketTime(now_ns);
                it->second.SetLastPacketTime(now_ns);
                it->second.IncNumPkts();
                it->second.IncNumBytes(ip->length.value());
                if (isTcp) {
                    it->second.AddFlags(tcp->flags);
                }
            }
        }

    }
    RunNextModule(ctx, batch);

}

// template <typename T1, typename T2>
// static bess:pb::FlowMonitorCommandGetStatsResponse::FlowStats SetFlowStats(
//     const T1 &flow, const T2 &stats
// ) {
//     bess:pb::FlowMonitorCommandGetStatsResponse::FlowStats flowstats;

//     flowstats.set_src_ip(std::string::ToIpv4Address(flow.src_ip));
//     flowstats.set_dst_ip(std::string::ToIpv4Address(flow.dst_ip));
//     flowstats.set_src_port(flow.src_port);
//     flowstats.set_dst_port(flow.dst_port);
//     if (flow.protocol == Ipv4::Proto::kTcp) {
//         flowstats.set_protocol("TCP");
//     }
//     else if (flow.protocol == Ipv4::Proto::kUdp) {
//         flowstats.set_protocol("UDP")
//     }
//     flowstats.set_first_pkt_time(stats.FirstPacketTime());
//     flowstats.set_last_pkt_time(stats.ListPacketTime());
//     flowstats.set_packets(stats.NumPkts());
//     flowstats.set_bytes(stats.NumBytes());
//     flowstats.set_flags(stats.TcpFlags());

//     return flowstats;
// }

CommandResponse FlowMonitor::CommandGetStats(
    const bess::pb::FlowMonitorCommandGetStatsArg &arg
) {
    bess::pb::FlowMonitorCommandGetStatsResponse r;
    
    // std::unordered_map<FiveTupleFlow, FlowStats, FiveTupleFlowHash>::iterator it = flow_map_.begin;
    for (std::pair<const bess::utils::FiveTupleFlow, FlowStats> it: flow_map_) {

        if (it.first.protocol == Ipv4::Proto::kUdp) {
            // this flow is UDP
            if (arg.udp()) {
                bess::pb::FlowMonitorCommandGetStatsResponse::Stats* stats = r.add_statistics();

                stats->set_src_ip(bess::utils::ToIpv4Address(it.first.src_ip));
                stats->set_dst_ip(bess::utils::ToIpv4Address(it.first.dst_ip));
                stats->set_src_port(it.first.src_port.value());
                stats->set_dst_port(it.first.dst_port.value());
                stats->set_protocol("UDP");
                stats->set_packets(it.second.NumPkts());
                stats->set_bytes(it.second.NumBytes());
                stats->set_age(it.second.FlowAge());
                stats->set_flags((uint32_t) it.second.TcpFlags());

                // r.add_statistics(stats);
            }
            else {
                continue;
            }
        }
        else if (it.first.protocol == Ipv4::Proto::kTcp) {
            // this flow is TCP
            if (arg.tcp()) {
                bess::pb::FlowMonitorCommandGetStatsResponse::Stats* stats = r.add_statistics();

                stats->set_src_ip(bess::utils::ToIpv4Address(it.first.src_ip));
                stats->set_dst_ip(bess::utils::ToIpv4Address(it.first.dst_ip));
                stats->set_src_port(it.first.src_port.value());
                stats->set_dst_port(it.first.dst_port.value());
                stats->set_protocol("TCP");
                stats->set_packets(it.second.NumPkts());
                stats->set_bytes(it.second.NumBytes());
                stats->set_age(it.second.FlowAge());
                stats->set_flags((uint32_t) it.second.TcpFlags());

                // r.add_statistics(stats);
            }
        }
        
    }

    return CommandSuccess(r);
}

ADD_MODULE(FlowMonitor, "flowmonitor", "monitor flow statistics")