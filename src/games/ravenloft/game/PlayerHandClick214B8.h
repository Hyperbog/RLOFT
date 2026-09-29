#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/game/HandItemUsability24478.h"
#include "games/ravenloft/game/PlayerHandAttack26018.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct PlayerHandClickContext214B8 {
    HandItemUsabilityContext24478 usability24478;
    // CODE.2 0x216EE..0x216F7 call boundary. The original literal target is -1;
    // 0xE5F0 resolves it through 0xDB94.
    std::function<PlayerHandAttackResult26018(int requestedTargetNpcIndex)> attack26018;
};

struct PlayerHandClickResult214B8 {
    enum class Path {
        InactiveCharacter,
        ItemExchangeBranch,
        IgnoredClick,
        CharacterStateBlocked,
        HandUnusable,
        AttackDispatched
    };
    Path path=Path::IgnoredClick;
    PlayerHandAttackResult26018 attack{};
};

// Opening-relevant reconstruction of CODE.2 0x214B8..0x21701. The click-8
// inventory/item-exchange arm is deliberately kept as a distinct boundary;
// the normal click-1 attack arm is reconstructed through 0x24478 -> 0x26018.
class PlayerHandClick214B8 {
public:
    static PlayerHandClickResult214B8 execute(std::array<data::CharacterRecord,4>& party,
                                               std::size_t characterIndex,
                                               int handIndex,
                                               std::uint32_t clickFlags,
                                               const data::ObjectDatabase& objects,
                                               const PlayerHandClickContext214B8& context);
};

} // namespace ravenloft::game
