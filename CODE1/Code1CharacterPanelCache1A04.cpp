#include "Code1CharacterPanelCache1A04.h"

#include "Code1UiTextComposite00EC.h"

namespace ravenloft::game {
namespace {

void drawCharacterPanel(bool live,
                        const char* record,
                        std::int8_t signedField,
                        std::uint8_t unsignedField,
                        std::int32_t globalBase,
                        const Code1TextDatabase& text,
                        const Code1CharacterPanelCacheOps1A04& ops) {
    if (!live) {
        const auto& label = text.at(0);
        Code1UiTextComposite00EC::draw(0, 0, 0x7A, 0x30,
                                       label.c_str(), 0, 0x5FF8, ops.draw);
        return;
    }

    ops.draw.call6C60(0, 0, 0, 0x14, 7);
    const std::int32_t resource =
        globalBase + static_cast<std::int32_t>(signedField) +
        static_cast<std::int32_t>(unsignedField) * 0x17;
    ops.draw.call6C60(0, 0x15, 0, resource, 1);
    ops.draw.call7D80(2, 0, 0x30, record, 0);
}

} // namespace

void Code1CharacterPanelCache1A04::rebuild(
    Code1CharacterPanelCacheState1A04& state,
    const Code1TextDatabase& text,
    const Code1CharacterPanelCacheOps1A04& ops) {
    if (state.handle95C != 0) {
        ops.call7830(state.handle95C);
        state.handle95C = 0;
    }
    if (state.handle960 != 0) {
        ops.call7830(state.handle960);
        state.handle960 = 0;
    }

    ops.call6588(state.buffer7138);
    const std::uintptr_t temporary = ops.call7868(0, 0, 0x7A, 0x30, 0xFF);

    drawCharacterPanel(state.flag4C4, state.record67F8,
                       state.field680A, state.field680B,
                       state.global713C, text, ops);
    state.handle95C = ops.call7868(0, 0, 0x7A, 0x30, 0xFF);

    drawCharacterPanel(state.flag4C5, state.record6A40,
                       state.field6A52, state.field6A53,
                       state.global713C, text, ops);
    state.handle960 = ops.call7868(0, 0, 0x7A, 0x30, 0xFF);

    ops.call67C8(0, 0, 0, temporary);
    ops.call7830(temporary);
}

} // namespace ravenloft::game
