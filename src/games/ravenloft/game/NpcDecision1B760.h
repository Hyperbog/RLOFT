#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/NpcType8Relocate1B1C4.h"
#include "games/ravenloft/game/OriginalRng30CA2.h"
#include <array>
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct NpcDecisionContext1B760 {
    const std::array<data::CharacterRecord,4>* party = nullptr;
    std::span<const std::uint16_t> levelCells{}; // CODE.2 DS:198E0 WORD cell table
    std::int32_t partyXFixed = 0;                // DS:255D0
    std::int32_t partyYFixed = 0;                // DS:255D4
    std::int16_t levelWidth = 0;                 // DS:25619
    std::int16_t levelHeight = 0;                // DS:2561B
    std::uint32_t soundBase258C0 = 0;
    std::function<std::uint32_t()> biosTick311E0;
    // CODE.2 0x28110(soundBase+definition+0x3D, 0xFF, 0, 1).
    std::function<void(std::uint32_t,int,int,int)> queueSound28110;
    NpcType8RelocateCallbacks1B1C4 type8Relocate{};
};

// Direct reconstruction of CODE.2 0x1B760.
class NpcDecision1B760 {
public:
    static void execute(data::RuntimePlacement22& actor,
                        const data::NpcDefinition& definition,
                        int moverId,
                        int retry,
                        OriginalRng30CA2& rng,
                        const NpcDecisionContext1B760& context);
};

} // namespace ravenloft::game
