#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/NpcDamageE284.h"
#include "games/ravenloft/game/OriginalRng30CA2.h"
#include "games/ravenloft/game/PlayerHitTestC80C.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct PlayerNpcAttackContextE5F0 {
    PlayerHitTestContextC80C hitTest;
    NpcDamageContextE284 damageE284;
    std::array<std::uint8_t,4>* characterDirty=nullptr;
    std::function<std::uint32_t()> currentTick19CC4;
    std::uint8_t global258A8=0xFF;
    // CODE.2 0xE5FB..0xE614: when the caller passes target -1, E5F0
    // resolves it through 0xDB94 before clearing transient attack state.
    std::function<int(std::size_t)> autoTargetDB94;
};

struct PlayerNpcAttackResultE5F0 {
    enum class Path { NoTarget, Miss, Hit, UnsupportedSpecialWeapon };
    Path path=Path::NoTarget;
    int targetNpcIndex=-1;
    int itemId=-1;
    int hitRoll=0;
    int requiredRoll=0;
    int damage=0;
    int recovery=0;
    bool died=false;
};

// Reconstruction of the normal melee branch of CODE.2 0xE5F0..0xEA81.
// The opening Quick Start long sword and warhammer both use this exact branch.
// Special weapon IDs 0x7E..0x86 and 0x4A retain explicit unsupported boundaries
// rather than being approximated.
class PlayerNpcAttackE5F0 {
public:
    static PlayerNpcAttackResultE5F0 execute(std::array<data::CharacterRecord,4>& party,
                                             std::size_t characterIndex,
                                             int handIndex,
                                             int itemId,
                                             std::span<data::RuntimePlacement22> actors,
                                             const data::NpcDatabase& npcs,
                                             int targetNpcIndex,
                                             const data::ObjectDatabase& objects,
                                             OriginalRng30CA2& rng,
                                             const PlayerNpcAttackContextE5F0& context);

    static int strengthDamageModifier(const data::CharacterRecord& character);
    static int recoveryDuration(int itemId,const data::CharacterRecord& character,
                                const data::ObjectDatabase& objects);
};

} // namespace ravenloft::game
