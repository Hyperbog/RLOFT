#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/OriginalRng30CA2.h"
#include "games/ravenloft/game/PlayerNpcAttackE5F0.h"
#include <array>
#include <cstddef>
#include <functional>
#include <span>

namespace ravenloft::game {

struct PlayerHandAttackContext26018 {
    PlayerNpcAttackContextE5F0 melee;
    // CODE.2 0x19CCC(0) input-state stack boundary at routine entry.
    std::function<void(int)> boundary19CCC;
};

struct PlayerHandAttackResult26018 {
    enum class Path { CoolingDown, Melee, Throwable, OtherUnsupported };
    Path path=Path::OtherUnsupported;
    int itemId=-1;
    int itemType=-1;
    PlayerNpcAttackResultE5F0 melee{};
};

// Reconstruction of CODE.2 0x26018's opening-relevant hand dispatcher. The
// exact normal melee item types 9..11 route to converted 0xE5F0. Other item
// families remain explicit boundaries rather than guessed behavior.
class PlayerHandAttack26018 {
public:
    static PlayerHandAttackResult26018 execute(std::array<data::CharacterRecord,4>& party,
                                                std::size_t characterIndex,
                                                int handIndex,
                                                std::span<data::RuntimePlacement22> actors,
                                                const data::NpcDatabase& npcs,
                                                int targetNpcIndex,
                                                const data::ObjectDatabase& objects,
                                                OriginalRng30CA2& rng,
                                                const PlayerHandAttackContext26018& context);

};

} // namespace ravenloft::game
