#include "games/ravenloft/game/WorldScene.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
constexpr std::uint16_t kNoDrawWallResource=147; // CODE.2 0x6D567 -> epilogue.
constexpr std::uint16_t kUpperPassBit=0x0200;    // CODE.2 0x6DBD5.
constexpr std::uint8_t kPairedTreeVerticalOffset=0x58; // CODE.2 0x674AB.
bool pairedTreeType(std::uint16_t t){return t>=60 && t<=67;} // CODE.2 0x6747F..0x67489.
}

WorldScene::WorldScene(const data::LevelRecord& level,
                       const data::InitDatabase& init,
                       const data::NpcDatabase& npcDefs,
                       const std::optional<data::RuntimePlacement22>& runtimeAssassin)
    :width_(level.width),height_(level.height),paletteIndex_(level.paletteIndex),skySelector_(level.skySelector),ceilingFallbackColor_(level.ceilingFallbackColor),floorFallbackColor_(level.floorFallbackColor),upperSourceRows_(level.upperSourceRows),cells_(level.cells){
    const auto flatBase=init.flatResourceBase();
    if(level.floorPlane>=0) floorResource_=static_cast<std::uint16_t>(flatBase+level.floorPlane);
    if(level.ceilingPlane>=0) ceilingResource_=static_cast<std::uint16_t>(flatBase+level.ceilingPlane);

    walls_.reserve(level.cells.size());
    for(const auto cell:level.cells){
        WallMaterialRef w;
        w.slot=static_cast<std::uint16_t>(cell&0x3F);
        w.upperPass=(cell&kUpperPassBit)!=0;
        w.xLower=level.wallBanks[0][w.slot];
        w.xUpper=level.wallBanks[1][w.slot];
        w.yLower=level.wallBanks[2][w.slot];
        w.yUpper=level.wallBanks[3][w.slot];
        w.noDrawXLower=(w.xLower==kNoDrawWallResource);
        w.noDrawYLower=(w.yLower==kNoDrawWallResource);
        walls_.push_back(w);
    }

    const auto sceneryBase=init.sceneryResourceBase();
    scenery_.reserve(level.scenery.size());
    for(const auto& p:level.scenery){
        if(p.type>=init.scenery().size()) throw std::runtime_error("Scenery type outside INIT definition table");
        const auto& d=init.scenery()[p.type];
        SceneryRenderRef s;
        s.type=p.type;s.x=p.x.value();s.y=p.y.value();s.xFixed=(static_cast<std::int32_t>(p.x.integer)<<16)|p.x.fraction;s.yFixed=(static_cast<std::int32_t>(p.y.integer)<<16)|p.y.fraction;s.placementFlags=p.flags;s.placementField14=static_cast<std::uint16_t>(p.flags)<<3;
        s.baseResource=static_cast<std::uint16_t>(sceneryBase+d.base);
        s.collisionExtent=d.radius;s.viewStride=d.views;s.animationFrames=d.frames;s.effect=d.effect;
        if(pairedTreeType(p.type)){
            s.hasPairedUpper=true;
            s.pairedUpperResource=static_cast<std::uint16_t>(s.baseResource+8); // CODE.2 0x674C4.
            s.pairedVerticalOffsetRows=kPairedTreeVerticalOffset;
        }
        scenery_.push_back(s);
    }

    npcs_.reserve(level.npcs.size());
    for(const auto& p:level.npcs){
        if(p.type>=npcDefs.count())throw std::runtime_error("NPC type outside NPCS.DAT");
        const auto& d=npcDefs.at(p.type);
        NpcRenderRef n;
        n.type=p.type;n.x=p.x.value();n.y=p.y.value();n.xFixed=(static_cast<std::int32_t>(p.x.integer)<<16)|p.x.fraction;n.yFixed=(static_cast<std::int32_t>(p.y.integer)<<16)|p.y.fraction;n.placementField14=static_cast<std::uint16_t>(p.flags)<<3;n.baseResource=static_cast<std::uint16_t>(init.npcResourceBase()+d.baseSprite);
        n.collisionExtent=d.collisionHalfExtent;n.facingStride=d.facingStride;n.viewCount=d.viewCount;
        if(runtimeAssassin && p.type==9){n.x=runtimeAssassin->x();n.y=runtimeAssassin->y();n.xFixed=runtimeAssassin->xFixed;n.yFixed=runtimeAssassin->yFixed;n.placementField14=runtimeAssassin->field14;}
        npcs_.push_back(n);
    }
}

} // namespace ravenloft::game
