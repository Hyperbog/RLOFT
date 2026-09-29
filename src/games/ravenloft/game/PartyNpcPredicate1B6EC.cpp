#include "games/ravenloft/game/PartyNpcPredicate1B6EC.h"

namespace ravenloft::game {
bool PartyNpcPredicate1B6EC::evaluate(const std::array<data::CharacterRecord,4>& party,
                                      const data::NpcDefinition& npcDefinition){
    int living=0,matching=0;bool anyFlag132_20=false;
    for(const auto& c:party){
        const auto status=static_cast<std::uint16_t>(c.raw[0x10] | (std::uint16_t(c.raw[0x11])<<8u));
        if(status>=0x4000u)continue;
        ++living;
        if((c.raw[0x132]&0x20u)!=0)anyFlag132_20=true;
        if((c.raw[0x133]&0x02u)!=0 &&
           (npcDefinition.raw[0x3A]&0x20u)!=0 &&
           npcDefinition.raw[0x1D]<=4u)++matching;
    }
    return anyFlag132_20 || matching==living;
}
} // namespace ravenloft::game
