#include "games/ravenloft/game/OriginalRenderQueue.h"
#include <algorithm>

namespace ravenloft::game {

std::vector<OriginalRenderRecord> OriginalRenderQueue::build(const ProjectedScene& projected,
                                                              const WorldScene& scene){
    std::vector<OriginalRenderRecord> out;
    out.reserve(projected.npcs.size()+projected.scenery.size());

    // CODE.2 0x2C2EF calls 0x2B6D0 first: NPC category 0 records.
    for(const auto& pn:projected.npcs){
        OriginalRenderRecord r;
        r.category=OriginalRenderCategory::Npc;
        r.sourceIndex=pn.sourceIndex;
        r.scale8_8=pn.projection.scale8_8;
        r.draws.push_back({r.category,OriginalSpritePart::Main,pn.sourceIndex,
                           pn.view.resource,pn.projection.screenX,pn.projection.scale8_8,0});
        out.push_back(std::move(r));
    }

    // CODE.2 0x2C315 calls 0x2BBA0 next: scenery category 1 records.
    for(const auto& ps:projected.scenery){
        OriginalRenderRecord r;
        r.category=OriginalRenderCategory::Scenery;
        r.sourceIndex=ps.sourceIndex;
        r.scale8_8=ps.projection.scale8_8;

        // At 0x2C541..0x2C5AB scenery definitions 60..67 receive the paired
        // upper resource (+8) first, with vertical source offset 0x58.  The
        // ordinary/main image is then emitted by the common draw path.
        if(ps.hasPairedUpper){
            r.draws.push_back({r.category,OriginalSpritePart::PairedUpper,ps.sourceIndex,
                               ps.upperView.resource,ps.projection.screenX,ps.projection.scale8_8,
                               ps.upperVerticalOffsetRows});
        }
        r.draws.push_back({r.category,OriginalSpritePart::Main,ps.sourceIndex,
                           ps.lowerView.resource,ps.projection.screenX,ps.projection.scale8_8,0});
        out.push_back(std::move(r));
    }

    // The original scan uses '<', never '<=', so first-built record wins a
    // tie. std::stable_sort preserves the category/source construction order.
    std::stable_sort(out.begin(),out.end(),[](const auto& a,const auto& b){
        return a.scale8_8 < b.scale8_8;
    });
    return out;
}

} // namespace ravenloft::game
