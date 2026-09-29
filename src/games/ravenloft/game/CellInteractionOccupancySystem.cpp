#include "games/ravenloft/game/CellInteractionOccupancySystem.h"
#include "games/ravenloft/game/PartyOverlapSystem.h"

namespace ravenloft::game {

CellInteractionCallbacks CellInteractionOccupancySystem::callbacks(
    std::int32_t partyXFixed,
    std::int32_t partyYFixed,
    std::uint8_t* activeTransitionCount,
    std::function<void(CellInteractionRecord&)> sideEffect20A24) const {
    CellInteractionCallbacks out;
    out.npcOccupied = [this](std::int32_t xFixed, std::int32_t yFixed) {
        // Original 0x20C5C passes mover id -1.
        return broadNpc_.overlaps(yFixed, xFixed, -1);
    };
    out.filteredNpcOccupied2C8E4 = [this](std::int32_t xFixed, std::int32_t yFixed) {
        // Original 0x20C76 passes mover id -1.
        return filteredNpc_.overlaps(yFixed, xFixed, -1);
    };
    out.partyOccupied = [partyXFixed, partyYFixed](std::int32_t xFixed,
                                                   std::int32_t yFixed) {
        return PartyOverlapSystem::overlaps(yFixed, xFixed,
                                            partyXFixed, partyYFixed);
    };
    out.activeTransitionCount = activeTransitionCount;
    out.transitionSideEffect20A24 = std::move(sideEffect20A24);
    return out;
}

} // namespace ravenloft::game
