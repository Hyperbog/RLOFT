#include "PageSync53E4.h"

namespace ravenloft::game {
namespace {

void clonePage(std::uint8_t sourcePage,
               std::uint8_t destinationPage,
               PageSyncState53E4& state) {
    state.pageBuffers73EF[destinationPage & 1u] =
        state.pageBuffers73EF[sourcePage & 1u];
}

bool cursorOverlaps(std::int32_t x,
                    std::int32_t y,
                    std::int32_t width,
                    std::int32_t height,
                    const PageSyncState53E4& state) noexcept {
    const auto mouseX =
        static_cast<std::int32_t>(state.input.mouseX73E6);
    const auto mouseY =
        static_cast<std::int32_t>(state.input.mouseY73E8);

    return mouseX + 0x20 >= x &&
           mouseX <= x + width &&
           mouseY + 0x10 >= y &&
           mouseY <= y + height;
}

} // namespace

void PageSync53E4::showCursor(
    PageSyncState53E4& state,
    const PageSyncOps53E4& ops) {
    if (state.input.word71DA == 0) {
        return;
    }

    if (state.input.word73EC == 0) {
        ops.call8740(state.input.byte73EE);

        // CODE.1 0x527C is inlined here exactly at the original call site.
        state.input.mouseX73E6 = state.input.value71D8;
        state.input.mouseY73E8 = state.input.value71D6;
        state.input.word73EA = state.input.value71D4;

        auto& page =
            state.pageBuffers73EF[state.input.byte73EE & 1u];

        ops.call66C0(
            page.data(),
            state.input.mouseX73E6,
            state.input.mouseY73E8,
            0x20,
            0x10);

        ops.call6D54(
            0,
            state.input.mouseX73E6,
            state.input.mouseY73E8,
            state.input.block71DC.data());

        ops.call8788();
    }

    ++state.input.word73EC;
}

void PageSync53E4::hideCursor(
    PageSyncState53E4& state,
    const PageSyncOps53E4& ops) {
    if (state.input.word71DA == 0) {
        return;
    }

    --state.input.word73EC;

    if (state.input.word73EC == 0) {
        ops.call8740(state.input.byte73EE);

        const auto& page =
            state.pageBuffers73EF[state.input.byte73EE & 1u];

        ops.call6744(
            page.data(),
            state.input.mouseX73E6,
            state.input.mouseY73E8,
            0x20,
            0x10);

        ops.call8788();
    }
}

void PageSync53E4::flip(
    std::uint8_t page,
    PageSyncState53E4& state,
    const PageSyncOps53E4& ops) {
    ops.call9AE8();

    if (state.input.word71DA == 0) {
        return;
    }

    state.value97C = 1;
    state.input.byte73EE = page;
    state.input.word73EC = 0;

    showCursor(state, ops);
    ops.call65A4(page);

    state.input.byte73EE =
        static_cast<std::uint8_t>(page ^ 1u);
    hideCursor(state, ops);

    state.input.byte73EE = page;
    state.input.word73EC = 1;
    state.value97C = 0;
}

void PageSync53E4::flipAndClone(
    std::uint8_t page,
    PageSyncState53E4& state,
    const PageSyncOps53E4& ops) {
    ops.call9AE8();

    if (state.input.word71DA == 0) {
        return;
    }

    state.value97C = 1;
    state.input.byte73EE = page;
    state.input.word73EC = 0;

    showCursor(state, ops);
    ops.call65A4(page);
    ops.call8740(page);
    ops.callA390(static_cast<std::uint8_t>(page ^ 1u));

    state.input.byte73EE =
        static_cast<std::uint8_t>(page ^ 1u);

    clonePage(page, state.input.byte73EE, state);
    hideCursor(state, ops);
    ops.call8788();

    state.input.byte73EE = page;
    state.input.word73EC = 1;
    state.value97C = 0;
}

void PageSync53E4::copyRectAndClone(
    std::uint8_t page,
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    PageSyncState53E4& state,
    const PageSyncOps53E4& ops) {
    ops.call9AE8();

    if (state.input.word71DA == 0) {
        return;
    }

    state.value97C = 1;
    state.input.byte73EE = page;
    state.input.word73EC = 0;

    showCursor(state, ops);
    ops.call65A4(page);
    ops.call8740(page);

    ops.callA3E8(
        static_cast<std::uint8_t>(page ^ 1u),
        x, y, x, y, width, height);

    state.input.byte73EE =
        static_cast<std::uint8_t>(page ^ 1u);

    clonePage(page, state.input.byte73EE, state);
    hideCursor(state, ops);
    ops.call8788();

    state.input.byte73EE = page;
    state.input.word73EC = 1;
    state.value97C = 0;
}

void PageSync53E4::copyRect(
    std::uint8_t page,
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    PageSyncState53E4& state,
    const PageSyncOps53E4& ops) {
    if (state.input.word71DA == 0) {
        return;
    }

    state.value97C = 1;

    if (!cursorOverlaps(x, y, width, height, state)) {
        ops.callA3E8(
            static_cast<std::uint8_t>(page ^ 1u),
            x, y, x, y, width, height);

        state.value97C = 0;
        return;
    }

    const std::uint8_t savedPage =
        state.input.byte73EE;

    state.input.byte73EE = page;
    --state.input.word73EC;

    showCursor(state, ops);

    ops.callA3E8(
        static_cast<std::uint8_t>(page ^ 1u),
        x, y, x, y, width, height);

    clonePage(
        page,
        static_cast<std::uint8_t>(page ^ 1u),
        state);

    hideCursor(state, ops);

    state.input.byte73EE = savedPage;
    ++state.input.word73EC;
    state.value97C = 0;
}

void PageSync53E4::syncAlternate(
    PageSyncState53E4& state,
    const PageSyncOps53E4& ops) {
    if (state.input.word71DA == 0) {
        return;
    }

    const std::uint8_t savedPage =
        state.input.byte73EE;

    state.value97C = 1;

    ops.call8740(state.input.byte23E6);
    ops.callA390(
        static_cast<std::uint8_t>(
            state.input.byte23E6 ^ 1u));

    state.input.byte73EE =
        static_cast<std::uint8_t>(
            state.input.byte23E6 ^ 1u);

    clonePage(savedPage, state.input.byte73EE, state);

    hideCursor(state, ops);
    ops.call8788();

    state.input.byte73EE = savedPage;
    state.value97C = 0;
    ++state.input.word73EC;
}

} // namespace ravenloft::game
