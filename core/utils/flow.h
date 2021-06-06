#ifndef BESS_UTILS_FLOW_H_
#define BESS_UTILS_FLOW_H_

#include <rte_hash_crc.h>
#include "endian.h"

namespace bess {
namespace utils {

class alignas(16) FiveTupleFlow {
    public:
        be32_t src_ip;
        be32_t dst_ip;
        be16_t src_port;
        be16_t dst_port;
        uint8_t protocol; // uint8_t (8 bits)
        uint8_t padding[3];

        FiveTupleFlow() : padding{0, 0, 0} {}

        bool operator==(const FiveTupleFlow &other) const {
            return memcmp(this, &other, sizeof(*this)) == 0;
        }
};

static_assert(sizeof(FiveTupleFlow) == 16, "FiveTupleFlow must be 16 bytes");

struct FiveTupleFlowHash {
    std::size_t operator()(const FiveTupleFlow &f) const {
        uint32_t hash_result = 0;

#if __x86_64
        const union
        {
            FiveTupleFlow flow;
            uint64_t u64[2];
            /* data */
        } &bytes = {.flow = f};

        hash_result = crc32c_sse42_u64(bytes.u64[0], hash_result);
        hash_result = crc32c_sse42_u64(bytes.u64[1], hash_result);
#else
        hash_result = rte_hash_crc(&f, sizeof(FiveTupleFlow), init_val);
#endif

        return hash_result;
    }
};

}  // namespace utils
}  // namespace bess

#endif // BESS_UTILS_FLOW_H_