#include "point_machine.h"

const Commands PointMachine::cmds = {
    {"set_ogate", "PointMachineCommandSetOGateArg", MODULE_CMD_FUNC(&PointMachine::CommandSetOGate), Command::THREAD_SAFE},
};

void PointMachine::ProcessBatch(Context *ctx, bess::PacketBatch *batch) {
    RunChooseModule(ctx, current_ogate_, batch);
}

CommandResponse PointMachine::CommandSetOGate (
    const bess::pb::PointMachineCommandSetOGateArg &arg
 ) {
    bess::pb::PointMachineCommandSetOGateResponse r;

    current_ogate_ = arg.ogate();
    r.set_ogate(current_ogate_);
    return CommandSuccess(r);
}

ADD_MODULE(PointMachine, "pointmachine", "switch between two output gate")