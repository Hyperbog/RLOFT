#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"

namespace ravenloft::game {

// Direct reconstruction of CODE.2 0xC910..0xC99E. This is the NPC defensive
// damage-category filter used by player hand attacks before 0xE284.
class PlayerDamageModifierC910 {
public:
    static int adjust(int damage,
                      const data::NpcDefinition& definition,
                      int itemId,
                      const data::ObjectDatabase& objects);
};

} // namespace ravenloft::game
