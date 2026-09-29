#include "games/ravenloft/game/CharacterDefenseEFA0.h"
#include "games/ravenloft/game/CharacterDefenseHelpers.h"
#include <array>
#include <cstdint>
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::int16_t i16(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::int16_t>(static_cast<std::uint16_t>(r[o]) |
        static_cast<std::uint16_t>(static_cast<std::uint16_t>(r[o+1])<<8u));
}
std::uint32_t u32(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::uint32_t>(r[o]) |
        (static_cast<std::uint32_t>(r[o+1])<<8u) |
        (static_cast<std::uint32_t>(r[o+2])<<16u) |
        (static_cast<std::uint32_t>(r[o+3])<<24u);
}
int objectAc(const data::ObjectDatabase& objects,int objectId){
    if(objectId==-1)return 0;
    const auto& rec=objects.at(static_cast<std::size_t>(objectId)).raw;
    const int base=static_cast<std::int8_t>(rec[0x34]);
    return base+CharacterDefenseHelpers::itemAcMagicAdjustmentEF34(objectId,objects);
}
constexpr std::array<std::int8_t,21> kDexterityAcTable{{
    -1,5,5,4,3,2,1,0,0,0,0,0,0,0,0,-1,-2,-3,-4,-4,0
}};
}

int CharacterDefenseEFA0::evaluate(std::size_t characterIndex,
                                   const data::CharacterRecord& c,
                                   const data::ObjectDatabase& objects,
                                   const CharacterDefenseContextEFA0& ctx){
    int ac=static_cast<std::int8_t>(c.raw[0x1B]);

    const int body=i16(c.raw,0x60);
    ac+=objectAc(objects,body);

    const int wrists=i16(c.raw,0x68);
    if(wrists==0x51 && !CharacterDefenseHelpers::bodyIsArmorEEE0(c,objects)){
        if(ac>5)ac=5;
        ac+=CharacterDefenseHelpers::itemAcMagicAdjustmentEF34(body,objects);
    }

    const int head=i16(c.raw,0x62);
    ac+=objectAc(objects,head);

    const std::size_t dexIndex=c.raw[0x35];
    if(dexIndex>=kDexterityAcTable.size())throw std::runtime_error("EFA0 Dexterity AC index outside reconstructed table");
    ac+=kDexterityAcTable[dexIndex];

    const int left=i16(c.raw,0x5C);
    if(left!=-1 && ctx.handUsable24478 && ctx.handUsable24478(characterIndex,0))ac+=objectAc(objects,left);
    const int right=i16(c.raw,0x5E);
    if(right!=-1 && ctx.handUsable24478 && ctx.handUsable24478(characterIndex,1))ac+=objectAc(objects,right);

    const int enc=CharacterDefenseHelpers::encumbranceTierFE44(c,objects,ctx.containerWeightF5F8);
    if(enc==3)++ac;
    if(enc==4)ac+=3;

    const std::uint32_t flags=u32(c.raw,0x132);
    if((flags&0x01u)!=0 && ac>6 && !CharacterDefenseHelpers::bodyIsArmorEEE0(c,objects))ac=6;
    if((flags&0x80u)!=0)ac-=2;
    if(CharacterDefenseHelpers::ringSlotContainsF2D4(c,0x46))--ac;
    return ac;
}

} // namespace ravenloft::game
