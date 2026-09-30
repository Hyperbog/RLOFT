#include "UiSetup1464.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

UiEntryState10A4 entryState(const UiSetupState1464& state) {
    UiEntryState10A4 out{};
    out.mode4D0 = state.mode4D0;
    out.x87C = state.x87C;
    out.y880 = state.y880;
    for (std::size_t i = 0; i < 9; ++i) out.current8F8[i] = state.current8F8Stride8[i];
    out.animation964 = state.animation964;
    out.flag970 = state.flag970;
    out.buffer7138 = state.buffer7138;
    out.text7140 = state.text7140.c_str();
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
    const TextDatabase& text,
    const UiSetupOps1464& ops) {
    state.previous8F4Stride8.fill(0x15);

    for (std::size_t i = 0; i < 9; ++i) state.current8F8Stride8[i] = slotValues[i];
    state.current8F8Stride8[9] = textIndex940;
    state.current8F8Stride8[10] = textIndex948;

    if (titleSelector == -1) {
        state.text7140.clear();
    } else if (titleSelector == 0x2B) {
        state.text7140 = state.string4B8;
    } else {
        state.text7140 = text.at(static_cast<std::size_t>(titleSelector));
    }

    auto entry = entryState(state);
    UiEntry10A4::animate(characterPanelStage, entry, ops.entry);
    state.buffer7138 = entry.buffer7138;

    for (std::size_t i = 0; i < 9; ++i) {
        const std::int32_t value = state.current8F8Stride8[i];
        if (value == -1 || value <= 0x0A) continue;
        if (value >= state.global713C && value < state.global713C + 0x2E) {
            (void)ops.entry.call954C(value, 1);
        } else {
            (void)ops.entry.call954C(value, 7);
        }
    }

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
}

} // namespace ravenloft::game
