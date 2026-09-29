#include "Code1VgaState6060.h"

#include <cstddef>

namespace ravenloft::game {

void Code1VgaSave6060::run(
    Code1VgaState6060& state,
    const Code1VgaOps6060& ops) {
    // 0x6063..0x6075: copy backwards exactly as the original loop does.
    for (std::int32_t index = 0x54; index >= 0; --index) {
        state.saved136E[static_cast<std::size_t>(index)] =
            state.live2B28[static_cast<std::size_t>(index)];
    }

    // VGA sequencer: preserve current index and register 2.
    std::uint16_t port = 0x03C4;
    state.sequencerIndex136A = ops.in8(port);
    ops.out8(port, 2);
    ++port;
    state.sequencerData1369 = ops.in8(port);

    // VGA graphics controller: preserve current index and registers 5 and 4.
    port = 0x03CE;
    state.graphicsIndex136C = ops.in8(port);

    ops.out8(port, 5);
    ++port;
    state.graphicsData5136B = ops.in8(port);

    --port;
    ops.out8(port, 4);
    ++port;
    state.graphicsData4136D = ops.in8(port);
}

void Code1VgaRestore60B4::run(
    Code1VgaState6060& state,
    const Code1VgaOps6060& ops) {
    // 0x60B7..0x60C9.
    for (std::int32_t index = 0x54; index >= 0; --index) {
        state.live2B28[static_cast<std::size_t>(index)] =
            state.saved136E[static_cast<std::size_t>(index)];
    }

    // OUT DX,AX writes register index in AL and saved data in AH.
    std::uint16_t port = 0x03C4;
    ops.out16(
        port,
        static_cast<std::uint16_t>(
            2u |
            (static_cast<std::uint16_t>(
                 state.sequencerData1369) << 8u)));
    ops.out8(port, state.sequencerIndex136A);

    port = 0x03CE;
    ops.out16(
        port,
        static_cast<std::uint16_t>(
            5u |
            (static_cast<std::uint16_t>(
                 state.graphicsData5136B) << 8u)));
    ops.out16(
        port,
        static_cast<std::uint16_t>(
            4u |
            (static_cast<std::uint16_t>(
                 state.graphicsData4136D) << 8u)));
    ops.out8(port, state.graphicsIndex136C);
}

} // namespace ravenloft::game
