#pragma once

#include "games/ravenloft/data/SaveGame.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

// Direct reconstruction of CODE.2 0xE578..0xE5CB. It clears the transient
// party attack-state bits before 0xE5F0 resolves a new hand attack.
class PlayerAttackStateE578 {
public:
    static void clear(std::array<data::CharacterRecord,4>& party,
                      std::size_t characterIndex,
                      std::array<std::uint8_t,4>* dirtyFlags=nullptr);
};

} // namespace ravenloft::game
