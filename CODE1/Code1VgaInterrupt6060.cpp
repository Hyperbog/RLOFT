#include "Code1VgaInterrupt6060.h"

namespace ravenloft::game {

void Code1VgaSave6060::run(
    Code1VgaState6060& state,
    const Code1VgaOps6060& ops) {
    state.saved136E = state.block2B28;

    state.byte136A = ops.in8(0x3C4);
    ops.out8(0x3C4, 2);
    state.byte1369 = ops.in8(0x3C5);

    state.byte136C = ops.in8(0x3CE);
    ops.out8(0x3CE, 5);
    state.byte136B = ops.in8(0x3CF);

    ops.out8(0x3CE, 4);
    state.byte136D = ops.in8(0x3CF);
}

void Code1VgaRestore60B4::run(
    Code1VgaState6060& state,
    const Code1VgaOps6060& ops) {
    state.block2B28 = state.saved136E;

    ops.out16(
        0x3C4,
        static_cast<std::uint16_t>(
            2u |
            (static_cast<std::uint16_t>(state.byte1369) << 8)));

    ops.out8(0x3C4, state.byte136A);

    ops.out16(
        0x3CE,
        static_cast<std::uint16_t>(
            5u |
            (static_cast<std::uint16_t>(state.byte136B) << 8)));

    ops.out16(
        0x3CE,
        static_cast<std::uint16_t>(
            4u |
            (static_cast<std::uint16_t>(state.byte136D) << 8)));

    ops.out8(0x3CE, state.byte136C);
}

void Code1Interrupt6104::run(
    const Code1InterruptFrame6104& frame,
    Code1VgaState6060& vgaState,
    Code1InterruptUpdateState50C0& updateState,
    const Code1InterruptOps6104& ops) {
    // CODE.1 saves flags/general/segment registers before this point.
    ops.callCEC0();

    if (vgaState.byte1368 != 0) {
        return;
    }

    vgaState.byte1368 = 1;

    vgaState.savedSS23C3 = ops.currentSS();
    vgaState.savedESP23C5 = ops.currentESP();

    const std::uint32_t linearEsi =
        static_cast<std::uint32_t>(frame.esiSegment) << 4u;

    const auto* pointer =
        ops.linearPointer(linearEsi);

    Code1VgaSave6060::run(vgaState, ops.vga);

    Code1InterruptUpdate50C0::run(
        frame.eax,
        frame.ebx,
        frame.ecx,
        frame.edx,
        pointer,
        updateState,
        ops.update);

    Code1VgaRestore60B4::run(vgaState, ops.vga);

    vgaState.byte1368 = 0;
}

std::uint32_t Code1GlobalAccessor61C0::get(
    const Code1GlobalState61C0& state) noexcept {
    return state.dword046C;
}

void Code1DosPrint61D0::print(
    char* text,
    const std::function<void(const char*)>& dosAh09) {
    char* cursor = text;

    while (*cursor != '\0') {
        if (*cursor == '$') {
            *cursor = '*';
        }

        ++cursor;
    }

    *cursor = '$';

    dosAh09(text);

    *cursor = '\0';
}

} // namespace ravenloft::game
