#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/WorkLevel.h"
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct NpcHitTestContextC9A0 {
    // Exact original call boundary CODE.2 0xEFA0.
    std::function<int(std::size_t characterIndex)> characterArmorClassEFA0;
    // Exact original d20 result from CODE.2 0x1C328(1,20,0,0).
    std::function<int()> rollD20;
    // CODE.2 0x871C(characterIndex,6), used by the +0x132 bit 0x40 branch.
    std::function<void(std::size_t characterIndex,int effectId)> effect871C;
};

// CODE.2 0xC9A0..0xCA35.
class NpcHitTestC9A0 {
public:
    static int evaluate(std::size_t characterIndex,
                        const data::CharacterRecord& character,
                        const data::NpcDefinition& npcDefinition,
                        const data::RuntimePlacement22& actor,
                        const NpcHitTestContextC9A0& context);
};

} // namespace ravenloft::game
