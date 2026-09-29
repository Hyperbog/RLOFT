#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/TrigTable.h"
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/NpcAttackDispatcherD550.h"
#include "games/ravenloft/game/NpcDecision1B760.h"
#include "games/ravenloft/game/OriginalRng30CA2.h"
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

// Concrete dependencies of the first actor loop in CODE.2 0x1BDAC..0x1BEF2.
// The later loops in the same original routine operate different runtime arrays
// and remain separate until their semantics are converted.
struct NpcSchedulerContext1BDAC {
    bool enabledBF38 = true;
    std::uint16_t levelWidth = 0;          // DS:25619
    const data::TrigTable* trig = nullptr; // movement vector dependency
    bool gridSnapC02F = false;

    NpcDecisionContext1B760 decision{};
    NpcAttackContextD550 attack{};

    // CODE.2 0x2D5D0 candidate collision callback used by 0x1BBD8.
    std::function<int(std::int32_t candidateYFixed,
                      std::int32_t candidateXFixed,
                      int moverId)> collision2D5D0;

    // CODE.2 0x1B4F8 branch selected by runtime +0x0B bit 0x04.
    // Not used by the shipped opening assassin but kept as the exact boundary.
    std::function<void(data::RuntimePlacement22& actor,
                       const data::NpcDefinition& definition,
                       int npcIndex)> behavior1B4F8;
};

// Direct reconstruction of the primary NPC scheduler loop at
// CODE.2 0x1BDAC..0x1BEF2.  This is the original live caller of 0xD550.
class NpcScheduler1BDAC {
public:
    static void execute(std::span<data::RuntimePlacement22> actors,
                        const data::NpcDatabase& definitions,
                        OriginalRng30CA2& rng,
                        const NpcSchedulerContext1BDAC& context);
};

} // namespace ravenloft::game
