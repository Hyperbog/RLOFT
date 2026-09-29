#pragma once

#include "games/ravenloft/game/CellInteractionSystem.h"
#include "games/ravenloft/game/FilteredNpcOverlapSystem.h"
#include "games/ravenloft/game/NpcOverlapSystem.h"
#include <cstdint>
#include <functional>

namespace ravenloft::game {

// Reconstructs the concrete occupancy dependencies used by CODE.2 0x20C0C:
// 0x2C800 -> 0x2C8E4 -> 0x2C9DC, all queried at the cell center.
class CellInteractionOccupancySystem {
public:
    CellInteractionOccupancySystem(const data::WorkLevelSnapshot& work,
                                   const data::NpcDatabase& npcs)
        : broadNpc_(work, npcs), filteredNpc_(work, npcs) {}

    CellInteractionCallbacks callbacks(
        std::int32_t partyXFixed,
        std::int32_t partyYFixed,
        std::uint8_t* activeTransitionCount = nullptr,
        std::function<void(CellInteractionRecord&)> sideEffect20A24 = {}) const;

private:
    NpcOverlapSystem broadNpc_;
    FilteredNpcOverlapSystem filteredNpc_;
};

} // namespace ravenloft::game
