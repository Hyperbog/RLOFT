#include "games/ravenloft/game/OpeningGroundObjectHit223F4.h"
#include "games/ravenloft/game/ProjectionSystem.h"
#include "games/ravenloft/game/SpriteRasterGeometry.h"
#include "games/ravenloft/game/SpriteViewSelector.h"
#include <algorithm>
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::uint16_t u16(const std::array<std::uint8_t,65>& r,std::size_t p){
    return static_cast<std::uint16_t>(r[p]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(r[p+1])<<8u);
}
}

std::vector<OpeningGroundObjectHitRecord223F4> OpeningGroundObjectHit223F4::build(
    std::span<const data::RuntimePlacement22> runtimeObjects,
    const data::ObjectDatabase& objects,
    const data::ResourceStore& resources,
    const data::TrigTable& trig,
    const PartyState& party){

    ProjectionSystem projection(trig);
    std::vector<OpeningGroundObjectHitRecord223F4> out;
    out.reserve(runtimeObjects.size());

    for(std::size_t i=0;i<runtimeObjects.size();++i){
        const auto& placed=runtimeObjects[i];
        if(placed.type>=objects.count())
            throw std::out_of_range("CODE.2 0x2BDF4 object type outside OBJECTS.DAT");

        const auto p=projection.project(placed.xFixed,placed.yFixed,
                                        party.position.x,party.position.y,
                                        party.headingDegrees);
        if(!p.visible)continue;

        const auto& def=objects.at(placed.type).raw;
        const auto viewCount=def[0x2A];
        // The four opening assassin drops all exercise the normal 8-view path.
        // Keep unsupported original view families explicit rather than infer them.
        if(viewCount!=4u && viewCount!=8u)continue;

        const auto baseResource=u16(def,0x28);
        const auto view=SpriteViewSelector::selectNormal(baseResource,viewCount,
                                                          placed.field14,p.screenX,127,
                                                          party.headingDegrees);
        const auto sprite=resources.sprite(view.resource);
        const auto rect=SpriteRasterGeometry::compute(p.screenX,p.scale8_8,sprite,0x28);
        if(!rect.drawable)continue;

        OpeningGroundObjectHitRecord223F4 r{};
        r.sourceIndex=i;
        r.scale8_8=p.scale8_8;
        r.left=rect.x0;
        r.right=rect.x1;
        // CODE.2 0x2BFC1/0x2BFC9 adds 0x20 to the interaction bounds after
        // 0x21BF0 returns; the sprite draw rectangle itself is not moved.
        r.top=rect.y0+0x20;
        r.bottom=rect.y1+0x20;
        out.push_back(r);
    }

    // CODE.2 0x2C294 repeatedly chooses the smallest scale with strict '<'.
    // Stable ascending sort reproduces the object sub-list and preserves ties.
    std::stable_sort(out.begin(),out.end(),[](const auto& a,const auto& b){
        return a.scale8_8<b.scale8_8;
    });
    for(std::size_t i=0;i<out.size();++i)
        out[i].drawOrder=static_cast<std::uint16_t>(i);
    return out;
}

std::optional<std::size_t> OpeningGroundObjectHit223F4::select(
    std::int32_t cursorX,
    std::int32_t cursorY,
    std::uint32_t clickFlags,
    std::span<const OpeningGroundObjectHitRecord223F4> records){

    if((clickFlags&1u)==0u || records.empty())return std::nullopt;

    // 0x223FB obtains the previous render-buffer count and starts at count-1.
    // 0x22413..0x22444 resolves each draw-order number back to its record.
    for(std::int64_t order=static_cast<std::int64_t>(records.size())-1;order>=0;--order){
        const auto it=std::find_if(records.begin(),records.end(),[&](const auto& r){
            return static_cast<std::int64_t>(r.drawOrder)==order;
        });
        if(it==records.end())continue;

        // 0x224AA..0x224E0: left <= x <= right, top <= y <= bottom.
        if(cursorX<it->left || cursorX>it->right ||
           cursorY<it->top || cursorY>it->bottom)continue;
        return it->sourceIndex;
    }
    return std::nullopt;
}

} // namespace ravenloft::game
