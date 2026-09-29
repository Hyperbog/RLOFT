#include "games/ravenloft/game/CharacterDefenseHelpers.h"
#include <array>
#include <cstddef>
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::int16_t i16(const std::array<std::uint8_t,0x248>& r, std::size_t o) {
    const std::uint16_t v=static_cast<std::uint16_t>(r[o]) |
        static_cast<std::uint16_t>(static_cast<std::uint16_t>(r[o+1])<<8u);
    return static_cast<std::int16_t>(v);
}
constexpr std::array<std::uint16_t,110> kEncumbranceThresholds{{
1,2,3,4,6, 5,6,7,9,10, 10,13,16,19,25, 10,13,16,19,25,
20,29,38,46,55, 20,29,38,46,55, 35,50,65,80,90, 35,50,65,80,90,
40,58,76,96,110, 40,58,76,96,110, 45,69,93,117,140, 45,69,93,117,140,
55,85,115,145,170, 55,85,115,145,170, 70,100,130,160,195,
85,121,157,193,220, 110,149,188,227,255, 135,174,213,252,280,
160,199,238,277,305, 185,224,263,302,330, 235,274,313,352,380,
335,374,413,452,480
}};
}

int CharacterDefenseHelpers::bodyIsArmorEEE0(const data::CharacterRecord& c,
                                               const data::ObjectDatabase& objects) {
    const int objectId=i16(c.raw,0x60);
    if(objectId==-1)return 0;
    return objects.at(static_cast<std::size_t>(objectId)).raw[0x2F]==4u ? 1 : 0;
}

int CharacterDefenseHelpers::itemAcMagicAdjustmentEF34(int objectId,
                                                        const data::ObjectDatabase& objects) {
    if(objectId==-1)return 0;
    const auto& r=objects.at(static_cast<std::size_t>(objectId)).raw;
    const int category=r[0x2F];
    if(category>=2 && category<=8)return -static_cast<int>(r[0x37]);
    return 0;
}

int CharacterDefenseHelpers::ringSlotContainsF2D4(const data::CharacterRecord& c,
                                                   int objectId) {
    return (i16(c.raw,0x58)==objectId || i16(c.raw,0x5A)==objectId) ? 1 : 0;
}

int CharacterDefenseHelpers::carriedWeightFD84(const data::CharacterRecord& c,
                                                const data::ObjectDatabase& objects,
                                                const std::function<int(int)>& containerWeightF5F8) {
    int total=0;
    for(std::size_t slot=0;slot<21;++slot){
        const int objectId=i16(c.raw,0x40+slot*2);
        if(objectId==-1)continue;
        const auto& r=objects.at(static_cast<std::size_t>(objectId)).raw;
        total+=static_cast<int>(r[0x30]);
        if(r[0x2F]==1u){
            const int containerIndex=static_cast<std::int8_t>(r[0x31]);
            if(containerWeightF5F8)total+=containerWeightF5F8(containerIndex);
        }
    }
    // Assembly uses signed division by two, i.e. truncation toward zero.
    return total/2;
}

int CharacterDefenseHelpers::encumbranceTierFE44(const data::CharacterRecord& c,
                                                  const data::ObjectDatabase& objects,
                                                  const std::function<int(int)>& containerWeightF5F8) {
    int strength=c.raw[0x33];
    if(strength==18){
        const int exceptional=c.raw[0x3F];
        if(exceptional>=1 && exceptional<=50)strength+=1;
        if(exceptional>=51 && exceptional<=75)strength+=2;
        if(exceptional>=76 && exceptional<=90)strength+=3;
        if(exceptional>=91 && exceptional<=99)strength+=4;
        if(exceptional==100)strength+=5;
    }
    const int row=strength-2;
    if(row<0 || row>=22)throw std::runtime_error("FE44 strength table index out of reconstructed range");
    const int weight=carriedWeightFD84(c,objects,containerWeightF5F8);
    for(int tier=0;tier<5;++tier){
        if(weight<=static_cast<int>(kEncumbranceThresholds[static_cast<std::size_t>(row*5+tier)]))return tier;
    }
    return 4;
}

} // namespace ravenloft::game
