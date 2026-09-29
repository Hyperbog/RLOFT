#pragma once

#include "games/ravenloft/data/SaveGame.h"
#include <array>
#include <cstdint>

namespace ravenloft::game {

// Direct gameplay-state reconstruction of CODE.2 0x1E4A0..0x1E532. It clears
// expired hand cooldown DWORDs (+0x10B/+0x10F) and the three auxiliary attack
// timer DWORDs (+0x19E..+0x1A6), marking the original CF5A dirty byte.
class PlayerAttackTimers1E4A0 {
public:
    static void tick(std::array<data::CharacterRecord,4>& party,
                     std::uint32_t currentTick19CC4,
                     std::array<std::uint8_t,4>* dirtyFlags=nullptr);
};

} // namespace ravenloft::game
