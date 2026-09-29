#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include <array>

namespace ravenloft::game {

// Direct reconstruction of CODE.2 0x1B6EC.
class PartyNpcPredicate1B6EC {
public:
    static bool evaluate(const std::array<data::CharacterRecord,4>& party,
                         const data::NpcDefinition& npcDefinition);
};

} // namespace ravenloft::game
