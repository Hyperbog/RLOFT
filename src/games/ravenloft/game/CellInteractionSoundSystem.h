#pragma once

#include "games/ravenloft/game/CellInteractionSystem.h"
#include <cstdint>
#include <optional>

namespace ravenloft::game {

struct CellInteractionSoundEvent {
    std::uint32_t resourceHandle = 0;
    std::uint8_t volume = 0;
    std::uint8_t channel = 0;
    std::uint8_t priorityDistance = 0;
};

class CellInteractionSoundSystem {
public:
    // Pure selection/spatialization core of CODE.2 0x20A24. The caller supplies
    // the wall-definition id already resolved from the level cell's low-6-bit
    // wall slot and the level's runtime sound-resource base (DS:258BC).
    static std::optional<CellInteractionSoundEvent> select(
        const CellInteractionRecord& record,
        int partyCellX,
        int partyCellY,
        std::uint16_t wallDefinitionId,
        std::uint32_t soundResourceBase);
};

} // namespace ravenloft::game
