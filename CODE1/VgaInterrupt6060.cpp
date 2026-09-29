#include "VgaInterrupt6060.h"

namespace ravenloft::game {

void VgaSave6060::run(
    VgaState6060& state,
    const VgaOps6060& ops) {
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

void VgaRestore60B4::run(
    VgaState6060& state,
    const VgaOps6060& ops) {
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

void Interrupt6104::run(
    const InterruptFrame6104& frame,
    VgaState6060& vgaState,
    InterruptUpdateState50C0& updateState,
    const InterruptOps6104& ops) {
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

    VgaSave6060::run(vgaState, ops.vga);

    InterruptUpdate50C0::run(
        frame.eax,
        frame.ebx,
        frame.ecx,
        frame.edx,
        pointer,
        updateState,
        ops.update);

    VgaRestore60B4::run(vgaState, ops.vga);

    vgaState.byte1368 = 0;
}

std::uint32_t GlobalAccessor61C0::get(
    const GlobalState61C0& state) noexcept {
    return state.dword046C;
}

void DosPrint61D0::print(
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
