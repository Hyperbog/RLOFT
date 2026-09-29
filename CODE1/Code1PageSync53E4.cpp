#include "Code1PageSync53E4.h"

namespace ravenloft::game {

void Code1PageFlip53E4::run(
    std::uint8_t page,
    Code1PageSyncState53E4& state,
    const Code1PageSyncOps53E4& ops) {
    ops.call9AE8();

    if (state.word71DA == 0) {
        return;
    }

    state.value97C = 1;
    state.byte73EE = page;
    state.word73EC = 0;

    ops.call57DC();
    ops.call65A4(page);

    state.byte73EE =
        static_cast<std::uint8_t>(page ^ 1u);

    ops.call5780();

    state.byte73EE = page;
    state.word73EC = 1;
    state.value97C = 0;
}

void Code1PageSync5454::run(
    std::uint8_t page,
    Code1PageSyncState53E4& state,
    const Code1PageSyncOps53E4& ops) {
    ops.call9AE8();

    if (state.word71DA == 0) {
        return;
    }

    state.value97C = 1;
    state.byte73EE = page;
    state.word73EC = 0;

    ops.call57DC();
    ops.call65A4(page);
    ops.call8740(page);

    const std::uint8_t other =
        static_cast<std::uint8_t>(page ^ 1u);

    ops.callA390(other);
    state.byte73EE = other;

    ops.callD1E8(
        state.buffers73EF[other],
        state.buffers73EF[page],
        0x400);

    ops.call5780();
    ops.call8788();

    state.byte73EE = page;
    state.word73EC = 1;
    state.value97C = 0;
}

void Code1RectSync550C::run(
    std::uint8_t page,
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    Code1PageSyncState53E4& state,
    const Code1PageSyncOps53E4& ops) {
    ops.call9AE8();

    if (state.word71DA == 0) {
        return;
    }

    state.value97C = 1;
    state.byte73EE = page;
    state.word73EC = 0;

    ops.call57DC();
    ops.call65A4(page);
    ops.call8740(page);

    const std::uint8_t other =
        static_cast<std::uint8_t>(page ^ 1u);

    // 0x555B..0x5579:
    // A3E8(other, x, y, x, y, width, height).
    ops.callA3E8(
        other,
        x, y,
        x, y,
        width, height);

    state.byte73EE = other;

    ops.callD1E8(
        state.buffers73EF[other],
        state.buffers73EF[page],
        0x400);

    ops.call5780();
    ops.call8788();

    state.byte73EE = page;
    state.word73EC = 1;
    state.value97C = 0;
}

} // namespace ravenloft::game
