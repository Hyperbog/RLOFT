#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/WorkLevel.h"
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct NpcDamageContextE284 {
    // Type-specific death effects remain explicit call boundaries. The opening
    // type-9 branch uses spawn2195C four times with the literal original args.
    std::function<void(int,std::uint16_t,int,std::int32_t,std::int32_t,int,int)> spawn2195C;

    // CODE.2 0x28110 sound boundary: (sound,0xFF,0,flag).
    std::function<void(std::uint32_t,int,int,int)> queueSound28110;
    std::uint32_t soundBase258BC = 0;
    std::uint32_t soundBase258C0 = 0;

    // CODE.2 0xDDA4(definition +0x23), retained as a gameplay boundary until
    // the XP/reward routine is converted.
    std::function<void(std::uint16_t)> boundaryDDA4;

    // DS:BF44 scripted-event state. Type 9 writes literal 1 after its fourth
    // 0x2195C call.
    std::function<void(int)> setEventStateBF44;

    // The three DS:258AA type ids and 0xE0F8 predicate control whether the
    // generic death code marks runtime +0x0B bit 0x02.
    std::function<bool(std::uint16_t)> specialType258AA;
    std::function<int(int)> boundaryE0F8;

    // Type-specific scripted death boundaries not needed by the opening
    // assassin remain explicit rather than being guessed.
    std::function<void(std::uint16_t)> scriptedDeath;
};

struct NpcDamageResultE284 {
    bool died = false;
    bool openingAssassinDropsSpawned = false;
};

// Reconstruction of CODE.2 0xE284 with the complete common damage/death path
// and exact type-9 opening special case. Other type-specific death scripts stay
// named call boundaries until independently converted.
class NpcDamageE284 {
public:
    static NpcDamageResultE284 apply(data::RuntimePlacement22& actor,
                                     const data::NpcDefinition& definition,
                                     int npcIndex,
                                     int damage,
                                     int mode,
                                     const NpcDamageContextE284& context);
};

} // namespace ravenloft::game
