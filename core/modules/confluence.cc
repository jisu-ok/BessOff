#include "confluence.h"

void Confluence::ProcessBatch(Context *ctx, bess::PacketBatch *batch) {
    RunNextModule(ctx, batch);   
}

ADD_MODULE(Confluence, "confluence", "Accepts traffic from two gate and ouputs to a single gate")