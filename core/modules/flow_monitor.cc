#include "flow_monitor.h"

#include "utils/ether.h"
#include "utils/ip.h"
#include "utils/tcp.h"
#include "utils/flow.h"

void FlowMonitor::ProcessBatch(Context *ctx, bess::PacketBatch *batch) {
    using bess::utils::Ethernet;
    using bess::utils::Ipv4;
    using bess::utils::Tcp;
    using bess::utils::FiveTupleFlow;
    using bess::utils::FiveTupleFlowHash;


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

ADD_MODULE(FlowMonitor, "flowmonitor", "monitor flow statistics")