#include "games/ravenloft/game/CellInteractionSystem.h"

namespace ravenloft::game {
namespace {

bool occupied(const std::function<bool(std::int32_t, std::int32_t)>& fn,
              std::int32_t xFixed,
              std::int32_t yFixed) {
    return fn ? fn(xFixed, yFixed) : false;
}

} // namespace

int CellInteractionSystem::activate(int cellX,
                                    int cellY,
                                    CellInteractionRecord& record,
                                    const CellInteractionCallbacks& callbacks) {
    // CODE.2 0x20C14..0x20C2C.
    if ((record.flags & 0x04u) == 0) return 0;
    if ((record.flags & 0x40u) != 0) return 0;
    if (record.frameDelta != 0) return 0;

    // 0x20C32..0x20C92 builds two 16.16 values whose fractional halves are
    // exactly 0x8000, then queries the broad NPC scan, filtered NPC scan, and party overlap in that order.
    const auto xFixed = static_cast<std::int32_t>((static_cast<std::uint32_t>(static_cast<std::uint16_t>(cellX)) << 16) | 0x8000u);
    const auto yFixed = static_cast<std::int32_t>((static_cast<std::uint32_t>(static_cast<std::uint16_t>(cellY)) << 16) | 0x8000u);

    if (occupied(callbacks.npcOccupied, xFixed, yFixed)) return 0;
    if (occupied(callbacks.filteredNpcOccupied2C8E4, xFixed, yFixed)) return 0;
    if (occupied(callbacks.partyOccupied, xFixed, yFixed)) return 0;

    switch (record.flags & 0x03u) {
    case 1:
        // 0x20C9F..0x20CB2. Closed/end frame begins moving positive;
        // otherwise force frame 8 and move negative.
        if (record.frame == 0) {
            record.frameDelta = 1;
        } else {
            record.frameDelta = -1;
            record.frame = 8;
        }
        break;
    case 2:
        // 0x20CCB..0x20CDA.
        record.frameDelta = -1;
        break;
    default:
        return 0;
    }

    // 0x20CB3 / 0x20CD0: every successfully initiated transition increments
    // the global active-interaction counter before CODE.2 0x20A24 is called.
    if (callbacks.activeTransitionCount)
        *callbacks.activeTransitionCount = static_cast<std::uint8_t>(*callbacks.activeTransitionCount + 1u);
    if (callbacks.transitionSideEffect20A24) callbacks.transitionSideEffect20A24(record);
    return 1;
}

} // namespace ravenloft::game
