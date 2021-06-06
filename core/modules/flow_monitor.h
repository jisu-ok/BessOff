#ifndef BESS_MODULES_FLOWMONITOR_H_
#define BESS_MODULES_FLOWMONITOR_H_

#include "../module.h"
// #include "../pb/module_msg.pb.h"
#include "../utils/mcslock.h"
#include "../utils/flow.h"

using bess::utils::FiveTupleFlow;
using bess::utils::FiveTupleFlowHash;


class FlowStats {
    public:
      FlowStats() : first_pkt_ns_(0), last_pkt_ns_(0), pkt_cnt_(0), bytes_cnt_(0), flags_(0) {}

    //   FiveTupleFlow getFiveTuple() { return flow_; }
      uint64_t FirstPacketTime() { return first_pkt_ns_; }
      void SetFirstPacketTime(uint64_t time) { first_pkt_ns_ = time; }
      uint64_t LastPacketTime() { return last_pkt_ns_; }
      void SetLastPacketTime(uint64_t time) { last_pkt_ns_ = time; }
      uint64_t FlowAge() { return last_pkt_ns_ - first_pkt_ns_; }
      uint64_t NumPkts() { return pkt_cnt_; }
      void IncNumPkts() { pkt_cnt_++; }
      uint64_t NumBytes() { return bytes_cnt_; }
      void IncNumBytes(uint64_t bytes) { bytes_cnt_ += bytes; }
      uint64_t TcpFlags() { return flags_ ;}
      void AddFlags(uint8_t flags) { flags_ |= flags; }

    private:
    //   FiveTupleFlow flow_;
      uint64_t first_pkt_ns_;
      uint64_t last_pkt_ns_;
      uint64_t pkt_cnt_;
      uint64_t bytes_cnt_;
      uint8_t flags_;
};

// Monitor flow statistics
class FlowMonitor final : public Module {
 public:
  FlowMonitor() : Module() { max_allowed_workers_ = Worker::kMaxWorkers; }

//   CommandResponse Init(const bess::pb::DecTTLArg &arg);

  void ProcessBatch(Context *ctx, bess::PacketBatch *batch) override;

  CommandResponse CommandGetStats(
      const bess::pb::FlowMonitorCommandGetStatsArg &arg
  );

  static const Commands cmds;

  private:
    std::unordered_map<FiveTupleFlow, FlowStats, FiveTupleFlowHash> flow_map_;
    // mcslock lock_;
};

#endif  // BESS_MODULES_FLOWMONITOR_H_
