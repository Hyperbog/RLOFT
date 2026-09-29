#include "Code1ResourceLoad4C80.h"

namespace ravenloft::game {

std::int32_t Code1IndexedRead4C80::read(
    std::int32_t index,
    std::int32_t handle,
    const Code1ResourceLoadOps4C80& ops) {
    // 0x4C83..0x4C94.
    ops.call633C(index * 2, 0, handle);

    // 0x4C97..0x4CA6. The original uses the stack slot containing the second
    // argument as the two-byte read destination, then returns that same slot.
    std::int32_t value = handle;
    ops.call6304(&value, 2, handle);
    return value;
}

void Code1ResourceOpen4CB0::run(
    Code1ResourceLoadState4C80& state,
    const Code1ResourceLoadOps4C80& ops) {
    // 0x4CB4..0x4D0C.
    std::int32_t handle = ops.call62A8(0x6C);
    if (handle == -1) {
        ops.call9814(state.global4BC);
    }

    state.value7134 = Code1IndexedRead4C80::read(0, handle, ops);
    state.value713C = Code1IndexedRead4C80::read(2, handle, ops);
    state.value712C = Code1IndexedRead4C80::read(0x0B, handle, ops);
    ops.call62F0(handle);

    // 0x4D0F..0x4D4B.
    handle = ops.call62A8(0x74);
    if (handle == -1) {
        ops.call9814(state.global4C0);
    }

    ops.callB74C(5);
    ops.callC650(6);
    state.handle4C8 = handle;
}

void Code1RecordBlock4D54::load(
    Code1ResourceLoadState4C80& state,
    const Code1ResourceLoadOps4C80& ops) {
    const std::int32_t handle = ops.call62A8(0x78);

    // 0x4D67..0x4D74.
    ops.call6304(&state.value7120, 4, handle);

    // 0x4D77..0x4DBB. Preserve the original quirk: if value7120 is zero,
    // the handle is not closed in this routine.
    if (state.value7120 != 0) {
        const std::size_t size =
            static_cast<std::size_t>(state.value7120) * 65u;

        state.buffer7128 = ops.call71C8(size);
        ops.call6304(
            reinterpret_cast<void*>(state.buffer7128),
            static_cast<std::int32_t>(size),
            handle);
        ops.call62F0(handle);
    }
}

} // namespace ravenloft::game
