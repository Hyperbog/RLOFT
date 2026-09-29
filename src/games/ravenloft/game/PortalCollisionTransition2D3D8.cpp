#include "games/ravenloft/game/PortalCollisionTransition2D3D8.h"

namespace ravenloft::game {
namespace {
bool wrappedInclusive(std::int32_t v,std::int32_t lo,std::int32_t hi) {
    const auto u=static_cast<std::uint32_t>(v);
    return u>=static_cast<std::uint32_t>(lo) && u<=static_cast<std::uint32_t>(hi);
}
}

void PortalCollisionTransition2D3D8::execute(
    int sourceCellX,
    int sourceCellY,
    std::uint8_t destinationX,
    std::uint8_t destinationY,
    std::uint8_t facingDelta45,
    std::uint8_t destinationLevel,
    PortalCollisionState2D3D8& state,
    const PortalCollisionTransition2D3D8Callbacks& cb) {

    bool queueTransitionSound = cb.sourceHasTypeAD2CA5C &&
                                cb.sourceHasTypeAD2CA5C(sourceCellX, sourceCellY);

    state.partyXFixed = static_cast<std::int32_t>((std::uint32_t(destinationX)<<16)|0x8000u);
    state.partyYFixed = static_cast<std::int32_t>((std::uint32_t(destinationY)<<16)|0x8000u);
    state.facingDegrees += static_cast<int>(facingDelta45) * 45;
    while (state.facingDegrees >= 360) state.facingDegrees -= 360;
    if (cb.setFacing1C0C0) cb.setFacing1C0C0(state.facingDegrees);

    const auto oldLevel = state.currentLevel;
    if (cb.loadLevel181B4) cb.loadLevel181B4(destinationLevel);
    state.currentLevel = destinationLevel;

    // 0x2D45F..0x2D564: after the load, scan live NPCs overlapping the new
    // party point using 2* NPCS.DAT +0x16 as the half extent. Matching one of
    // the three DS:258AA types invokes 0xE0F8.
    if (cb.currentWorkLevel && cb.npcDatabase) {
        const auto& work = cb.currentWorkLevel();
        const auto& npcs = cb.npcDatabase();
        for (std::size_t i=0;i<work.npcs.size();++i) {
            const auto& actor=work.npcs[i];
            if ((actor.state & 0x02000000u)!=0) continue; // runtime +0x0B bit 2
            if (actor.type>=npcs.count()) continue;
            const auto r=static_cast<std::uint32_t>(npcs.at(actor.type).collisionHalfExtent)*2u;
            const auto xLo=static_cast<std::int32_t>(static_cast<std::uint32_t>(actor.xFixed)-r);
            const auto xHi=static_cast<std::int32_t>(static_cast<std::uint32_t>(actor.xFixed)+r);
            const auto yLo=static_cast<std::int32_t>(static_cast<std::uint32_t>(actor.yFixed)-r);
            const auto yHi=static_cast<std::int32_t>(static_cast<std::uint32_t>(actor.yFixed)+r);
            if (!wrappedInclusive(state.partyXFixed,xLo,xHi) ||
                !wrappedInclusive(state.partyYFixed,yLo,yHi)) continue;
            for (const auto t:cb.specialNpcTypes258AA) {
                if (actor.type != static_cast<std::uint16_t>(t)) continue;
                if (cb.npcHelperE0F8 && cb.npcHelperE0F8(i)!=0) break;
            }
        }
    }

    if (oldLevel == state.currentLevel) queueTransitionSound = true;
    if (oldLevel == 5 && state.currentLevel != 5 && cb.onLeaveLevel5_2D234)
        cb.onLeaveLevel5_2D234();
    if (state.currentLevel == 0x0B && oldLevel != 0x0B && cb.onEnterLevel11_2D2F4)
        cb.onEnterLevel11_2D2F4();

    // 0x2D5AF..0x2D5C5. The level-10 comparison immediately before this is
    // side-effect free in the recovered code; the sound depends only on e0.
    if (queueTransitionSound && cb.queueSound28110)
        cb.queueSound28110(7,0xFF,1,1);
}

} // namespace ravenloft::game
