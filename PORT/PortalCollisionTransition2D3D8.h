#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/WorkLevel.h"
#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct PortalCollisionState2D3D8 {
    std::int32_t partyXFixed = 0;
    std::int32_t partyYFixed = 0;
    std::int32_t facingDegrees = 0;
    std::uint16_t currentLevel = 0;
};

struct PortalCollisionTransition2D3D8Callbacks {
    // CODE.2 0x2CA5C: source-cell type-0xAD presence test.
    std::function<bool(int sourceCellX,int sourceCellY)> sourceHasTypeAD2CA5C;
    std::function<void(std::int32_t facing)> setFacing1C0C0;
    std::function<void(std::uint16_t level)> loadLevel181B4;

    // The destination level's live NPC array and three DS:258AA type ids are
    // supplied after load. 0xE0F8 is called only for overlapping NPCs whose
    // type matches one of those three ids.
    std::function<const data::WorkLevelSnapshot&()> currentWorkLevel;
    std::function<const data::NpcDatabase&()> npcDatabase;
    std::array<std::int16_t,3> specialNpcTypes258AA{{-1,-1,-1}};
    std::function<int(std::size_t npcIndex)> npcHelperE0F8;

    std::function<void()> onLeaveLevel5_2D234;
    std::function<void()> onEnterLevel11_2D2F4;
    std::function<void(int soundId,int volume,int channel,int priority)> queueSound28110;
};

class PortalCollisionTransition2D3D8 {
public:
    // Direct reconstruction of CODE.2 0x2D3D8..0x2D5CC.
    static void execute(int sourceCellX,
                        int sourceCellY,
                        std::uint8_t destinationX,
                        std::uint8_t destinationY,
                        std::uint8_t facingDelta45,
                        std::uint8_t destinationLevel,
                        PortalCollisionState2D3D8& state,
                        const PortalCollisionTransition2D3D8Callbacks& cb);
};

} // namespace ravenloft::game
