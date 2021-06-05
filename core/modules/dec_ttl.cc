#include "dec_ttl.h"

#include "utils/ether.h"
#include "utils/ip.h"

void DecTTL::ProcessBatch(Context *ctx, bess::PacketBatch *batch) {
    using bess::utils::Ethernet;
    using bess::utils::Ipv4;

    gate_idx_t incoming_gate = ctx->current_igate;

    int cnt = batch->cnt();

    for (int i = 0; i < cnt; i++) {
        bess::Packet *pkt = batch->pkts()[i];

        Ethernet *eth = pkt->head_data<Ethernet *>();
        Ipv4 *ip = reinterpret_cast<Ipv4 *>(eth + 1);
        // size_t ip_bytes = (ip->header_length & 0xf) << 2;
        ip->ttl -= 1;

        if (ip->ttl == 0) {
            DropPacket(ctx, pkt);
        }
        else {
            EmitPacket(ctx, pkt, incoming_gate);
            // EmitPacket(ctx, pkt, 0);
        }
    }
}

ADD_MODULE(DecTTL, "decttl", "decrements IP TTL field and drop if TTL reaches 0")