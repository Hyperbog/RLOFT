#pragma once

#include "games/ravenloft/data/SaveGame.h"
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct PlayerAttackCooldownContext24B48 {
    std::function<std::uint32_t()> currentTick19CC4;
};

// Gameplay-state portion of CODE.2 0x24B48..0x24BF0. Rendering/UI calls are
// presentation boundaries; the original cooldown write at character
// +0x10B+hand*4 is reproduced exactly when duration is nonzero.
class PlayerAttackCooldown24B48 {
public:
    static void apply(data::CharacterRecord& character,
                      int handIndex,
                      int duration,
                      const PlayerAttackCooldownContext24B48& context);
};

} // namespace ravenloft::game
