#ifndef BESS_MODULES_DECTTL_H_
#define BESS_MODULES_DECTTL_H_

#include "../module.h"
// #include "../pb/module_msg.pb.h"

// Decrement IP TTL field by 1 and drop if TTL reaches 0
class DecTTL final : public Module {
 public:
  DecTTL() : Module() { max_allowed_workers_ = Worker::kMaxWorkers; }

//   CommandResponse Init(const bess::pb::DecTTLArg &arg);

  void ProcessBatch(Context *ctx, bess::PacketBatch *batch) override;
};

#endif  // BESS_MODULES_DECTTL_H_
