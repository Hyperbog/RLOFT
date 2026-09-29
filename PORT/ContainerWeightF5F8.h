#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"

namespace ravenloft::game {

// Direct reconstruction of CODE.2 0xF5F8..0xF66B.
class ContainerWeightF5F8 {
public:
    static int evaluate(int containerIndex,
                        const data::ObjectDatabase& objects,
                        const data::QuickStartSave& save);
};

} // namespace ravenloft::game
