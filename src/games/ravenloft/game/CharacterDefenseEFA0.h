#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include <cstddef>
#include <functional>

namespace ravenloft::game {

struct CharacterDefenseContextEFA0 {
    // CODE.2 0x24478(characterIndex,handIndex). Nonzero allows the hand item's
    // AC contribution to participate in 0xEFA0.
    std::function<int(std::size_t characterIndex,int handIndex)> handUsable24478;
    // CODE.2 0xF5F8(containerIndex) recursive contained weight.
    std::function<int(int containerIndex)> containerWeightF5F8;
};

// CODE.2 0xEFA0..0xF166.
class CharacterDefenseEFA0 {
public:
    static int evaluate(std::size_t characterIndex,
                        const data::CharacterRecord& character,
                        const data::ObjectDatabase& objects,
                        const CharacterDefenseContextEFA0& context);
};

} // namespace ravenloft::game
