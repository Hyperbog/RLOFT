#include "UiSetup1464.h"

#include <bit>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::int32_t add32(std::int32_t lhs, std::uint32_t rhs) noexcept {
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(lhs) + rhs);
}

UiEntryState10A4 entryState(const UiSetupState1464& state) {
    UiEntryState10A4 out{};
    out.mode4D0 = state.mode4D0;
    out.x87C = state.x87C;
    out.y880 = state.y880;
    for (std::size_t i = 0; i < 9; ++i) out.current8F8[i] = state.current8F8Stride8[i];
    out.animation964 = state.animation964;
    out.flag970 = state.flag970;
    out.buffer7138 = state.buffer7138;
    out.text7140 = state.text7140;
    return out;
}

UiTransitionState07E4 transitionState(const UiSetupState1464& state) {
    UiTransitionState07E4 out{};
    out.mode4D0 = state.mode4D0;
    out.x87C = state.x87C;
    out.y880 = state.y880;
    for (std::size_t i = 0; i < 9; ++i) {
        out.previous8F4[i] = state.previous8F4Stride8[i];
        out.current8F8[i] = state.current8F8Stride8[i];
    }
    out.resource954 = state.resource954;
    out.global713C = state.global713C;
    out.textIndex940 = state.current8F8Stride8[9];
    out.textIndex948 = state.current8F8Stride8[10];
    out.buffer7138 = state.buffer7138;
    out.byte63F8 = state.byte63F8;
    return out;
}

} // namespace

void UiSetup1464::run(
    std::int32_t characterPanelStage,
    const std::array<std::int32_t, 9>& slotValues,
    std::int32_t textIndex940,
    std::int32_t textIndex948,
    std::int32_t titleSelector,
    UiSetupState1464& state,
    const UiSetupOps1464& ops) {
    // 0x1464..0x1479: EAX=0; write 0x15 to eleven DWORDs at
    // DS:08F4 + EAX*8. INC/CMP/JL visits exactly indices 0..10.
    state.previous8F4Stride8.fill(0x15);

    // 0x147B..0x14D2: the eleven stack arguments after characterPanelStage
    // are copied verbatim to DS:08F8,0900,...,0948.  The public interface
    // groups the first nine values and names the final two text indices.
    for (std::size_t i = 0; i < 9; ++i) state.current8F8Stride8[i] = slotValues[i];
    state.current8F8Stride8[9] = textIndex940;
    state.current8F8Stride8[10] = textIndex948;

    // 0x14D3..0x150A: signed selector -1 clears only BYTE DS:7140. Selector
    // 0x2B calls 0x72A0 with [DS:04B8] as source; all other values call the
    // original 0x004C decoder into the same DS:7140 buffer.
    if (titleSelector == -1) {
        state.text7140[0] = '\0';
    } else if (titleSelector == 0x2B) {
        ops.call72A0(state.text7140, state.string4B8);
    } else {
        ops.call004C(state.text7140, titleSelector);
    }

    // 0x150B..0x1516: pass the first argument straight to 0x10A4.
    auto entry = entryState(state);
    UiEntry10A4::animate(characterPanelStage, entry, ops.entry);
    state.buffer7138 = entry.buffer7138;

    // 0x1517..0x1571: EBX visits slots 0..8. Values -1 and signed <=10 are
    // skipped. The resource-window test uses signed comparisons around a
    // wrapping 32-bit ADD of 0x2E to DS:713C.
    for (std::int32_t slot = 0; slot < 9; ++slot) {
        const auto i = static_cast<std::size_t>(slot);
        const std::int32_t value = state.current8F8Stride8[i];
        if (value == -1 || value <= 0x0A) continue;

        const std::int32_t upper = add32(state.global713C, 0x2Eu);
        if (value >= state.global713C && value < upper) {
            (void)ops.entry.call954C(value, 1);
        } else {
            (void)ops.entry.call954C(value, 7);
        }
    }

    // 0x1572..0x15AD: signed mode<2 selects [2,4); mode==6 selects [6,9);
    // every other mode selects [0,6). These are direct arguments to 0x07E4.
    std::int32_t start = 0;
    std::int32_t end = 6;
    if (state.mode4D0 < 2) {
        start = 2;
        end = 4;
    } else if (state.mode4D0 == 6) {
        start = 6;
        end = 9;
    }

    auto transition = transitionState(state);
    UiTransition07E4::animate(start, end, transition, ops.transition);
    state.buffer7138 = transition.buffer7138;

    // 0x15AE..0x15AF is MOV EAX,EAX alignment padding outside the routine.
}

} // namespace ravenloft::game
