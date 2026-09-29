#include "Code1Display64DC.h"

namespace ravenloft::game {

void Code1Display64DC::setActivePage(
    std::uint8_t page,
    Code1DisplayState64DC& state,
    const Code1DisplayOps64DC& ops) {
    ops.callA35C(page);
    state.byte23E5 = page;
}

void Code1Display64DC::setDrawPage(
    std::uint8_t page,
    Code1DisplayState64DC& state,
    const Code1DisplayOps64DC& ops) {
    ops.callA36C(page);
    state.byte23E6 = page;
}

void Code1Display64DC::configure(
    std::int32_t mode,
    Code1DisplayState64DC& state,
    const Code1DisplayOps64DC& ops) {
    if (mode == 0xFF) {
        ops.call9DF4();
        state.byte23D4 = 0xFF;
        return;
    }

    const std::uint8_t flags =
        ops.call9CA0(
            static_cast<std::uint8_t>(mode),
            state.byte23D4);

    state.word23DB = 0x100;
    state.word23DD =
        (flags & 0x04u) != 0 ? 1 : 0;

    if (state.word23DF < 0x00FF) {
        ops.callA474(0xFF);
    }

    state.byte23E6 = 0;
    state.byte23E5 = 0;

    setActivePage(0, state, ops);
    setDrawPage(0, state, ops);

    ops.call6BC0(
        0,
        0,
        static_cast<std::int32_t>(state.word23D7) - 1,
        static_cast<std::int32_t>(state.word23D9) - 1);

    state.byte23D4 =
        static_cast<std::uint8_t>(mode) & 0x7Fu;
}

void Code1Display64DC::verticalLine(
    std::int32_t x,
    std::int32_t y,
    std::int32_t length,
    std::uint8_t color,
    const Code1DisplayOps64DC& ops) {
    ops.callA05C(
        x,
        y,
        x,
        y + length - 1,
        color);
}

void Code1Display64DC::horizontalLine(
    std::int32_t x,
    std::int32_t y,
    std::int32_t length,
    std::uint8_t color,
    const Code1DisplayOps64DC& ops) {
    if (length != 0) {
        ops.callA05C(
            x,
            y,
            x + length - 1,
            y,
            color);
    }
}

void Code1Display64DC::palette(
    void* buffer,
    const Code1DisplayOps64DC& ops) {
    ops.callA010(buffer, 0, 0x300);
}

void Code1Display64DC::paletteRange(
    std::int32_t offset,
    std::int32_t colorCount,
    void* buffer,
    const Code1DisplayOps64DC& ops) {
    ops.callA010(
        buffer,
        offset,
        colorCount * 3);
}

void Code1Display64DC::shiftA(
    std::int32_t value,
    const Code1DisplayState64DC& state,
    const Code1DisplayOps64DC& ops) {
    ops.callA2B0(
        static_cast<std::int32_t>(state.word23D7) - value,
        value);
}

void Code1Display64DC::shiftB(
    std::int32_t value,
    const Code1DisplayState64DC& state,
    const Code1DisplayOps64DC& ops) {
    ops.callA2B0(
        static_cast<std::int32_t>(state.word23D7) - value,
        value - static_cast<std::int32_t>(state.word23D7));
}

void Code1Display64DC::scroll(
    std::int32_t value,
    const Code1DisplayOps64DC& ops) {
    ops.callA21C(value);
}

void Code1Display64DC::scrollNegative(
    std::int32_t value,
    const Code1DisplayOps64DC& ops) {
    ops.callA21C(-value);
}

} // namespace ravenloft::game
