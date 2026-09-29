#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/CharacterDamageCD44.h"
#include "games/ravenloft/game/OriginalRng30CA2.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct NpcAttackContextD550 {
    std::array<data::CharacterRecord,4>* party = nullptr;
    std::int32_t partyXFixed = 0; // DS:255D0
    std::int32_t partyYFixed = 0; // DS:255D4
    std::int32_t facingDegreesC658 = 0; // DS:C658 party facing angle
    std::uint8_t targetMode258A8 = 0xFF;
    std::uint32_t soundBase258C0 = 0;
    std::uint32_t soundBase258BC = 0;

    // CODE.2 0x30700 -> DS:F58C; D550 immediately passes this to 0x30CC4.
    std::function<std::uint32_t()> timer30700;

    // Existing converted call boundaries used by the dispatcher.
    std::function<int(std::size_t)> characterArmorClassEFA0;
    std::function<void(std::size_t,int)> effect871C;
    std::function<int(int)> query14A60;
    std::function<int(std::size_t)> predicateCEE4;
    std::function<void(std::size_t)> levelDrainD1E0;
    std::function<void(std::size_t,int)> adjustAbility1202C;
    std::function<int(std::size_t,int)> queryEffectF2D4;
    std::function<std::uint32_t()> currentTick19CC4;

    // CODE.2 0x8A80(0, angle, -1, actor.y, actor.x), type-8 special branch.
    std::function<void(int,int,int,std::int32_t,std::int32_t)> type8Special8A80;
    // CODE.2 0xE284(actor, definition, npcIndex, 2d6, 0).
    std::function<void(data::RuntimePlacement22&,
                       const data::NpcDefinition&,
                       int,int,int)> protectedEffectE284;

    // Miss-text/resource pair 0x3210F + 0x1C21C is kept as one explicit
    // presentation boundary; attack/special sounds remain 0x28110.
    std::function<void(std::uint16_t npcType)> missFeedback;
    std::function<void(std::uint32_t,int,int,int)> queueSound28110;

    // Shared state/callbacks already reconstructed by CODE.2 0xCD44.
    CharacterDamageContextCD44 damageContext{};
};

struct NpcAttackResultD550 {
    enum class Path { None, Type15, Type8Special, OrdinaryMiss, OrdinaryHit } path = Path::None;
    int target = -1;
    int damage = 0;
    bool hit = false;
    bool protectedEffectConsumed = false;
};

// Direct reconstruction of CODE.2 0xD550..0xDACB. The original routine is
// called as D550(actor, npcDefinition, npcIndex) from CODE.2 0x1BE19.
class NpcAttackDispatcherD550 {
public:
    static NpcAttackResultD550 execute(data::RuntimePlacement22& actor,
                                       const data::NpcDefinition& npcDefinition,
                                       int npcIndex,
                                       OriginalRng30CA2& rng,
                                       const NpcAttackContextD550& context);
};

} // namespace ravenloft::game
