// Name was inspired by point machine in railway turnouts
// (https://en.wikipedia.org/wiki/Point_machine)


#ifndef BESS_MODULES_PointMachine_H_
#define BESS_MODULES_PointMachine_H_

#include "../module.h"
#include "../pb/module_msg.pb.h"

class PointMachine final : public Module {
 public:
  static const gate_idx_t kNumOGates = 2;

  static const Commands cmds;

  PointMachine() : Module(), current_ogate_(0) {
    max_allowed_workers_ = Worker::kMaxWorkers;
  }

//   CommandResponse Init(const bess::pb::PointMachineArg &arg);

  void ProcessBatch(Context *ctx, bess::PacketBatch *batch) override;

  CommandResponse CommandSetOGate(
      const bess::pb::PointMachineCommandSetOGateArg &arg
  );

 private:
  int current_ogate_; // output gate curretly set
};

#endif  // BESS_MODULES_POINTMACHINE_H_