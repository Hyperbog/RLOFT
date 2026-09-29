#include "games/ravenloft/game/SceneProjector.h"

namespace ravenloft::game {

ProjectedScene SceneProjector::project(const WorldScene& scene,
                                       std::int32_t cameraXFixed,
                                       std::int32_t cameraYFixed,
                                       int headingDegrees) const {
    ProjectedScene out;
    for(std::size_t i=0;i<scene.scenery().size();++i){
        const auto& s=scene.scenery()[i];
        auto p=projection_.project(s.xFixed,s.yFixed,cameraXFixed,cameraYFixed,headingDegrees);
        if(!p.visible) continue;
        ProjectedScenery q;
        q.sourceIndex=i;
        q.projection=p;
        q.lowerView=SpriteViewSelector::selectNormal(s.baseResource,s.viewStride,s.placementField14,p.screenX,127,headingDegrees);
        q.hasPairedUpper=s.hasPairedUpper;
        q.upperVerticalOffsetRows=s.pairedVerticalOffsetRows;
        if(s.hasPairedUpper)
            q.upperView=SpriteViewSelector::selectNormal(s.pairedUpperResource,s.viewStride,s.placementField14,p.screenX,127,headingDegrees);
        out.scenery.push_back(q);
    }
    for(std::size_t i=0;i<scene.npcs().size();++i){
        const auto& n=scene.npcs()[i];
        auto p=projection_.project(n.xFixed,n.yFixed,cameraXFixed,cameraYFixed,headingDegrees);
        if(!p.visible) continue;
        ProjectedNpc q;
        q.sourceIndex=i;
        q.projection=p;
        q.view=SpriteViewSelector::selectNormal(n.baseResource,n.viewCount,n.placementField14,p.screenX,127,headingDegrees);
        out.npcs.push_back(q);
    }
    return out;
}

} // namespace ravenloft::game
