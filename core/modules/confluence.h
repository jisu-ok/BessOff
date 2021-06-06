#ifndef BESS_MODULES_Confluence_H_
#define BESS_MODULES_Confluence_H_

#include "../module.h"
// #include "../pb/module_msg.pb.h"

class Confluence final : public Module {
 public:
  static const gate_idx_t kNumIGates = 2;

//   static const Commands cmds;

  Confluence() : Module() {
    max_allowed_workers_ = Worker::kMaxWorkers;
  }

//   CommandResponse Init(const bess::pb::PointMachineArg &arg);

  void ProcessBatch(Context *ctx, bess::PacketBatch *batch) override;

//   CommandResponse CommandSetOGate(
//       const bess::pb::PointMachineCommandSetOGateArg &arg
//   );

 private:
  int current_igate_; // current input gate
};

#endif  // BESS_MODULES_CONFLUENCE_H_