#include "games/ravenloft/game/CharacterNameLookup14A60.h"
#include <cstdint>

namespace ravenloft::game {
namespace {
bool rawNameEquals(const data::CharacterRecord& c,std::string_view name){
    // strcmp semantics for the known bounded Ravenloft character-name field.
    // A candidate can only match when all requested bytes match and the next
    // source byte is NUL; a mismatch exits immediately just like strcmp().
    if(name.size()>=c.raw.size())return false;
    for(std::size_t i=0;i<name.size();++i){
        if(c.raw[i]!=static_cast<std::uint8_t>(name[i]))return false;
        if(c.raw[i]==0)return false;
    }
    return c.raw[name.size()]==0;
}
}

int CharacterNameLookup14A60::find(const std::array<data::CharacterRecord,4>& party,
                                   std::string_view name){
    for(std::size_t i=0;i<party.size();++i){
        if(!rawNameEquals(party[i],name))continue;
        // 0x14A83..0x14A95 masks only the LOW status byte with 0x80.
        if((party[i].raw[0x10]&0x80u)!=0)return static_cast<int>(i);
    }
    return -1;
}

} // namespace ravenloft::game
