#include "Code1Service9AE8.h"

namespace ravenloft::game {

void Code1Service9AE8::service(
    Code1SystemState95A8& state,
    const Code1ServiceOps9AE8& ops) {
    // 0x9AF0.
    ops.call8960();

    // 0x9AF5..0x9B25.
    if ((state.flags2B01 & 0x02u) != 0 &&
        (state.flags2B01 & 0x80u) == 0 &&
        state.busy2B02 == 0) {
        state.busy2B02 = 1;
        state.flags2B01 = 0;
        ops.call5014(0x0A);
    }

    // 0x9B28..0x9BBB.
    if (state.flags2B01 != 0 &&
        (state.flags2B01 & 0x80u) == 0 &&
        state.busy2B02 == 0) {
        state.busy2B02 = 1;
        state.flags2B01 = 0;

        // Original CODE.1 data:
        // DS:02F8 = "Quit "
        // DS:0300 = " (Y/N) ?"
        std::string text = "Quit ";

        if (state.value2B0C != 0) {
            text +=
                ops.stringFromToken(
                    state.value2B0C);
        }

        text += " (Y/N) ?";

        const std::int32_t answer =
            Code1System95A8::prompt(
                text,
                1,
                state,
                ops.system);

        if (answer == 0x59) {
            Code1System95A8::shutdownDisplay(
                1,
                state,
                ops.system);
        }

        state.busy2B02 = 0;
        state.flags2B01 = 0;
    }
}

void Code1Service9AE8::requestQuit(
    Code1SystemState95A8& state,
    const Code1ServiceOps9AE8& ops) {
    state.flags2B01 |= 0x01u;
    service(state, ops);
}

void Code1Service9AE8::dispatch5(
    std::int32_t mode,
    std::int32_t a,
    std::int32_t b,
    std::int32_t c,
    std::int32_t d,
    std::int32_t e,
    const Code1ServiceOps9AE8& ops) {
    // 0x9BE3..0x9BFC.
    if (a == b) {
        // Original push order maps to (d, e, c).
        ops.call86A4(d, e, c);
        return;
    }

    // 0x9C03..0x9C3D.
    if (mode != 0) {
        ops.callC7A0(
            a, b, c, d, e);
    } else {
        ops.callC8C4(
            a, b, c, d, e);
    }
}

void Code1Service9AE8::dispatch7(
    std::int32_t mode,
    std::int32_t a,
    std::int32_t b,
    std::int32_t c,
    std::int32_t d,
    std::int32_t e,
    std::int32_t f,
    std::int32_t g,
    const Code1ServiceOps9AE8& ops) {
    if (mode != 0) {
        ops.callCACC(
            a, b, c, d, e, f, g);
    } else {
        ops.callC9D4(
            a, b, c, d, e, f, g);
    }
}

} // namespace ravenloft::game
