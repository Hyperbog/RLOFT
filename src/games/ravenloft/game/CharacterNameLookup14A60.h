#pragma once

#include "games/ravenloft/data/SaveGame.h"
#include <array>
#include <cstddef>
#include <string_view>

namespace ravenloft::game {

// Direct reconstruction of CODE.2 0x14A60..0x14AB1.
// The original routine strcmp()s the supplied literal against the four
// character-name strings and returns the first matching slot whose status
// low byte has bit 0x80 set. Otherwise it returns -1.
class CharacterNameLookup14A60 {
public:
    static int find(const std::array<data::CharacterRecord,4>& party,
                    std::string_view name);
};

} // namespace ravenloft::game
