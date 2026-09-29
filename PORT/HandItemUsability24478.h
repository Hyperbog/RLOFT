#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

struct HandItemUsabilityContext24478 {
    const std::array<data::CharacterRecord,4>* party = nullptr;
    std::uint8_t global258A8 = 0xFF;
};

// Direct reconstruction of CODE.2 0x24478..0x24A9B.
// handIndex is the original 0=left / 1=right selector passed by callers.
class HandItemUsability24478 {
public:
    static int evaluate(std::size_t characterIndex,
                        int handIndex,
                        const data::ObjectDatabase& objects,
                        const HandItemUsabilityContext24478& context);
};

} // namespace ravenloft::game
