#include "CursorSync55DC.h"

namespace ravenloft::game {

void ConditionalRectSync55DC::run(
    std::uint8_t page,
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    CursorSyncState55DC& state,
    const CursorSyncOps55DC& ops) {
    if (state.word71DA == 0) {
        return;
    }

    state.value97C = 1;

    const bool overlapsCursor =
        !(static_cast<std::int32_t>(state.mouseX73E6) + 0x20 < x ||
          static_cast<std::int32_t>(state.mouseX73E6) > x + width ||
          static_cast<std::int32_t>(state.mouseY73E8) + 0x10 < y ||
          static_cast<std::int32_t>(state.mouseY73E8) > y + height);

    const std::uint8_t other =
        static_cast<std::uint8_t>(page ^ 1u);

    if (!overlapsCursor) {
        ops.callA3E8(
            other,
            x, y,
            x, y,
            width, height);
    } else {
        const std::uint8_t savedPage = state.byte73EE;

        state.byte73EE = page;
        --state.word73EC;

        ops.call57DC();

        ops.callA3E8(
            other,
            x, y,
            x, y,
            width, height);

        ops.callD1E8(
            state.buffers73EF[other],
            state.buffers73EF[page],
            0x400);

        ops.call5780();

        state.byte73EE = savedPage;
        ++state.word73EC;
    }

    state.value97C = 0;
}

void CursorPageSync56DC::run(
    CursorSyncState55DC& state,
    const CursorSyncOps55DC& ops) {
    if (state.word71DA == 0) {
        return;
    }

    const std::uint8_t savedPage = state.byte73EE;
    state.value97C = 1;

    ops.call8740(state.byte23E6);

    const std::uint8_t other =
        static_cast<std::uint8_t>(state.byte23E6 ^ 1u);

    ops.callA390(other);
    state.byte73EE = other;

    ops.callD1E8(
        state.buffers73EF[other],
        state.buffers73EF[savedPage],
        0x400);

    ops.call5780();
    ops.call8788();

    state.byte73EE = savedPage;
    state.value97C = 0;
    ++state.word73EC;
}

void CursorHide5780::run(
    CursorSyncState55DC& state,
    const CursorSyncOps55DC& ops) {
    if (state.word71DA == 0) {
        return;
    }

    --state.word73EC;

    if (state.word73EC != 0) {
        return;
    }

    ops.call8740(state.byte73EE);

    ops.call6744(
        state.buffers73EF[state.byte73EE],
        state.mouseX73E6,
        state.mouseY73E8,
        0x20,
        0x10);

    ops.call8788();
}

void CursorShow57DC::run(
    CursorSyncState55DC& state,
    const CursorSyncOps55DC& ops) {
    if (state.word71DA == 0) {
        return;
    }

    if (state.word73EC == 0) {
        ops.call8740(state.byte73EE);

        // CODE.1 0x527C snapshot semantics.
        state.mouseX73E6 = state.value71D8;
        state.mouseY73E8 = state.value71D6;
        state.word73EA = state.value71D4;

        ops.call66C0(
            state.buffers73EF[state.byte73EE],
            state.mouseX73E6,
            state.mouseY73E8,
            0x20,
            0x10);

        ops.call6D54(
            0,
            state.mouseX73E6,
            state.mouseY73E8,
            state.cursor71DC);

        ops.call8788();
    }

    ++state.word73EC;
}

} // namespace ravenloft::game
