#include "games/ravenloft/game/AdventureHudModel.h"
#include <algorithm>

namespace ravenloft::game {

AdventureHudModel::AdventureHudModel(const std::array<data::CharacterRecord,4>& party) {
    for(std::size_t i=0;i<party.size();++i){
        const auto& src=party[i];
        auto& dst=characters_[i];
        dst.active=src.active();
        dst.name=src.name;
        dst.portrait=src.portrait;
        dst.status=src.status;
        dst.currentHitPoints=src.currentHitPoints;
        dst.maximumHitPoints=src.maximumHitPoints;
        dst.carriedItemCount=std::count_if(src.carriedItems.begin(),src.carriedItems.end(),
                                           [](std::uint16_t id){return id!=0xFFFFu;});
    }
}

} // namespace ravenloft::game
