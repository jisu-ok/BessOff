#include "point_machine.h"

const Commands PointMachine::cmds = {
    {"set_ogate", "PointMachineCommandSetOGateArg", MODULE_CMD_FUNC(&PointMachine::CommandSetOGgate), Command::THREAD_SAFE},
};

void PointMachine::ProcessBatch(Context *ctx, bess::PacketBatch *batch) {
    RunChooseModule(ctx, current_ogate_, batch);
}

CommandResponse PointMachine::CommandSetOGgate{
    const bess::pb::PointMachineCommandSetOGateArg &arg
} {
    bess::pb::PointMachineCommandSetOGateResponse r;

    current_ogate_ = arg.ogate();
    r.set_ogate(curretnt_ogate_);
    return CommandSuccess(r);
}