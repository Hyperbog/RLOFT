#include "games/ravenloft/game/PlayerDamageModifierC910.h"
#include <cstdint>
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::uint32_t u32(const std::array<std::uint8_t,69>& r,std::size_t o){
    return static_cast<std::uint32_t>(r[o]) |
           (static_cast<std::uint32_t>(r[o+1])<<8u) |
           (static_cast<std::uint32_t>(r[o+2])<<16u) |
           (static_cast<std::uint32_t>(r[o+3])<<24u);
}
}

int PlayerDamageModifierC910::adjust(int damage,
                                     const data::NpcDefinition& definition,
                                     int itemId,
                                     const data::ObjectDatabase& objects){
    const std::uint32_t flags=u32(definition.raw,0x31);
    int category=9;
    const data::ObjectDefinition* object=nullptr;
    if(itemId!=-1){
        if(itemId<0 || static_cast<std::size_t>(itemId)>=objects.count())
            throw std::runtime_error("C910 item outside OBJECTS.DAT");
        object=&objects.at(static_cast<std::size_t>(itemId));
        category=object->raw[0x2F];
        if(object->raw[0x37]!=0)category=11;
    }

    int out=damage;
    if(category==9){
        if((flags&0x00008000u)!=0)out=0;
    }else if(category==10){
        if((flags&0x00004000u)!=0)out=0;
    }else if(category==11 && object){
        // Preserve the original MOVZX + signed comparisons against -2. Since
        // the source byte is zero-extended, the first comparison is always
        // >= -2 and the second is likewise always >= -2. This oddity is part
        // of the executable and intentionally not "fixed" here.
        const int enhancement=object->raw[0x37];
        if((flags&0x00020000u)!=0 && enhancement>=-2)out=0;
        if((flags&0x00040000u)!=0 && enhancement<-2)out=0;
    }
    // 0xC993..0xC99D forces all outcomes, including immunity-zero, to at
    // least one point of damage.
    if(out<1)out=1;
    return out;
}

} // namespace ravenloft::game
