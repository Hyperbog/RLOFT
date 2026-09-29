#include "games/ravenloft/game/NpcCellInteractionSystem.h"

namespace ravenloft::game {

NpcCellInteractionResult NpcCellInteractionSystem::evaluate(
    std::uint16_t interactionCode,
    CellInteractionRecord* record,
    std::uint8_t npcTraits39,
    const CellInteractionCallbacks& callbacks) {

    // Caller reaches this branch only after a nonzero interaction code is found.
    // 0x2D70A..0x2D71B: a missing runtime record, or runtime tag/type 8, does
    // not contribute a block at this stage.
    if (interactionCode == 0 || record == nullptr || record->type == 8)
        return {};

    if (interactionCode == 0x8000u) {
        // 0x2D72D..0x2D76D: activation must be enabled, not locked out, and
        // the NPC definition must carry trait +0x39 bit 0x40.
        if ((record->flags & 0x04u) == 0 ||
            (record->flags & 0x40u) != 0 ||
            (npcTraits39 & 0x40u) == 0)
            return {true, false};

        // 0x2D76F..0x2D7BD: a transition already in progress blocks movement.
        if (record->frameDelta != 0)
            return {true, false};

        // The assembly saves/restores bit 0x04 around the call even though the
        // precondition above requires it set. Preserve that ordering literally.
        const auto originalBit04 = static_cast<std::uint8_t>(record->flags & 0x04u);
        record->flags = static_cast<std::uint8_t>(record->flags | 0x04u);
        const bool triggered = CellInteractionSystem::activate(
            static_cast<int>(record->cellX),
            static_cast<int>(record->cellY),
            *record,
            callbacks) != 0;
        record->flags = static_cast<std::uint8_t>((record->flags & ~0x04u) | originalBit04);

        // 0x2D7A5..0x2D7C4 does not convert a failed 0x20C0C return into the
        // local blocked flag. Report only the observed transition separately.
        return {false, triggered};
    }

    // 0x2D7D5..0x2D808: for ordinary NPC movers, all other nonzero interaction
    // codes block unless NPCS.DAT +0x39 bit 0x80 grants pass-through.
    return {(npcTraits39 & 0x80u) == 0, false};
}

} // namespace ravenloft::game
