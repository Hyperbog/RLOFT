#include "Code1InterruptUpdate50C0.h"

namespace ravenloft::game {
namespace {

std::int32_t arithmeticShiftRightOne(std::int32_t value) noexcept {
    // Preserve x86 SAR rounding toward negative infinity.
    return value >= 0
        ? value / 2
        : -(((-value) + 1) / 2);
}

} // namespace

void Code1InterruptUpdate50C0::run(
    std::int32_t,
    std::int32_t valueEbx,
    std::int32_t valueEcx,
    std::int32_t valueEdx,
    const std::array<std::int16_t, 2>* pointerEsi,
    Code1InterruptUpdateState50C0& state,
    const Code1InterruptUpdateOps50C0& ops) {
    // 0x50C4.
    valueEcx = arithmeticShiftRightOne(valueEcx);

    // 0x50C7..0x511E.
    if (ops.call84E0() != 0) {
        const std::int32_t second =
            static_cast<std::int32_t>((*pointerEsi)[1]);
        valueEdx =
            ((second + 0x1FC0) * 0x18F + 0x1FA0) / 0x3F40;

        const std::int32_t first =
            static_cast<std::int32_t>((*pointerEsi)[0]);
        valueEcx =
            ((first + 0x1FC0) * 0x13F + 0x1FA0) / 0x3F40;
    }

    // 0x5121..0x5229.
    if (state.word73EC >= 1 && state.value97C == 0) {
        if (state.mouseX73E6 != valueEdx ||
            state.mouseY73E8 != valueEcx) {
            state.value71D8 =
                static_cast<std::int16_t>(valueEdx);
            state.value71D6 =
                static_cast<std::int16_t>(valueEcx);
            state.value71D4 =
                static_cast<std::int16_t>(valueEbx);

            // Preserve four original WORDs around the redraw.
            state.saved7188 = state.words2401;

            ops.call6C44(state.rect7178);
            ops.call6BC0(0, 0, 0x18F, 0x13F);
            ops.call5780();
            ops.call57DC();

            // Original argument order:
            // 7178, 7180, 717C, 7184.
            ops.call6BC0(
                state.rect7178[0],
                state.rect7178[2],
                state.rect7178[1],
                state.rect7178[3]);

            state.words2401 = state.saved7188;
        }

        state.value71D4 =
            static_cast<std::int16_t>(valueEbx);
    }
}

} // namespace ravenloft::game
