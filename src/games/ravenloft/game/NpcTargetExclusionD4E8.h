#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include <cstddef>

namespace ravenloft::game {

// CODE.2 0xD4E8..0xD53D.
// Returns the original nonzero exclusion result when a character may not be
// selected as the victim of the supplied NPC definition's ordinary attack.
class NpcTargetExclusionD4E8 {
public:
    static int evaluate(const data::CharacterRecord& character,
                        const data::NpcDefinition& npcDefinition);
};

} // namespace ravenloft::game
