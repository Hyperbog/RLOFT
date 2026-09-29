#include "Code1CreationDispatch5014.h"

namespace ravenloft::game {

void Code1CreationFinalize5014::run(
    std::int32_t result,
    Code1CreationDispatchState5014& state,
    const Code1CreationDispatchOps5014& ops) {
    // 0x5017..0x5024.
    ops.callBB80(0x9C4);
    ops.callBCB4();

    // 0x5029..0x5049.
    if (result != 0x0B) {
        ops.call62F0(state.handle4C8);
        ops.call538C();
        ops.call9778();
    } else {
        result = 0x0A;
    }

    // 0x5050..0x507C.
    if (result != 0x0A) {
        ops.call64DC(0xFF);
    } else if (state.flag4C4 != 0 && state.flag4C5 != 0) {
        ops.call5F4C();
        ops.call4EF0();
    }

    // 0x5081 is an original tail call.
    ops.callCBF0(result);
}

void Code1CreationEntry508C::run(
    std::int32_t mode,
    const Code1CreationEntryOps508C& ops) {
    // 0x508F..0x50A4.
    if (mode != 2) {
        ops.call61D0(0x9C);
        ops.callCBF0(1);
        return;
    }

    // 0x50A9..0x50B9.
    ops.call4DC4();
    const std::int32_t result = ops.call4978();
    ops.call5014(result);
}

} // namespace ravenloft::game
