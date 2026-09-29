#include "games/ravenloft/game/LightingSystem.h"
#include <algorithm>
#include <stdexcept>

namespace ravenloft::game {
namespace {
// CODE.2 data object DS:F924, 24 signed WORDs. 0x31F94 selects one entry by
// the high WORD of the game clock (hour modulo 24) for outdoor sky levels.
constexpr std::array<std::int16_t,24> kHourlyDarkness{
    10,10,10,10,10,10,10,9,8,7,5,4,2,1,0,1,2,4,5,7,8,9,10,10
};
// CODE.2 DS:F954. 0x31A14 indexes this with the clamped primary darkness.
constexpr std::array<std::int16_t,11> kFalloffDivisor{9,9,8,7,7,6,6,6,5,5,5};

int truncDiv(int n,int d){
    if(d==0) throw std::runtime_error("Ravenloft lighting divide by zero");
    return n/d; // C++ signed integer division truncates toward zero like IDIV.
}
int signedByte(int v){
    v&=0xff;
    return v>=128?v-256:v;
}
}

std::int32_t LightingSystem::hourlyDarkness(std::uint16_t hour){
    return kHourlyDarkness[hour%24u];
}

std::int32_t LightingSystem::partyLightOverride(const std::array<data::CharacterRecord,4>& party){
    // CODE.2 0x31E74: any active record (<0x4000 status) whose WORD at +0x132
    // has bit mask 0x0402 forces an 8-step light contribution.
    for(const auto& c:party){
        if(c.status>=0x4000u) continue;
        const auto effects=static_cast<std::uint16_t>(c.raw[0x132]) |
                           (static_cast<std::uint16_t>(c.raw[0x133])<<8u);
        if((effects&0x0402u)!=0) return 8;
    }
    return 0;
}

LightingState LightingSystem::fromQuickStart(const data::QuickStartSave& save) const{
    return build(save.worldClock,save.savedBaseDarkness,save.ambientLightModifier,save.party);
}

LightingState LightingSystem::build(std::uint32_t clock,
                                    std::int32_t savedBaseDarkness,
                                    std::int32_t ambientModifier,
                                    const std::array<data::CharacterRecord,4>& party) const{
    LightingState state;
    auto hour=static_cast<std::uint16_t>(clock>>16u);
    if(hour==0xffffu) hour=23;
    hour=static_cast<std::uint16_t>(hour%24u);
    state.hour=hour;

    // On levels with a sky selector, 0x31F94 replaces DS:F920 with the shipped
    // hourly table. Keep the loaded value as the input/fallback contract but
    // select the outdoor value here because this system is used by the demo's
    // Forgotten Realms Forest sky path.
    (void)savedBaseDarkness;
    state.baseDarkness=std::clamp<std::int32_t>(hourlyDarkness(hour),0,10);
    state.ambientModifier=std::clamp<std::int32_t>(ambientModifier,0,8);
    state.partyLightOverride=partyLightOverride(party);

    int primary=state.baseDarkness;
    int secondary=state.partyLightOverride+state.ambientModifier;
    if(secondary>8) secondary=8;
    if(primary<0||primary>10) throw std::runtime_error("Ravenloft primary darkness out of range");

    state.skyShade=static_cast<std::uint8_t>(primary); // 0x31A14 -> DS:F918

    const int c730=trig_.lightingTableExtent(); // shipped value 508
    const int c738=trig_.lightingPivot();       // shipped value 127
    if(c730<177||c738<=0) throw std::runtime_error("Unexpected Ravenloft SINTAB lighting geometry");
    state.distanceRemap.assign(static_cast<std::size_t>(c730+1),0);

    // Scratch span containing the two overlapping executable byte tables.
    // 0x31A14 writes FE14..FE93 and FE94..FF13; floor draw 0x34A74 walks
    // exactly FE15..FE94 for its 128 scanlines.
    constexpr std::uint32_t kBase=0xfe14;
    std::array<std::uint8_t,0x100> shadeMemory{};

    const int e0=primary+1;
    const int d8=c730+1;
    const int e8=secondary+1;
    const int division=10-e8;
    const int e4=e8*truncDiv(c738,division);
    const int f0=c738-e4;
    const int f4=c730-f0;
    const int denom=kFalloffDivisor[static_cast<std::size_t>(primary)];

    for(int i=0;i<=c730;++i){
        int low=truncDiv((d8-i+1)*e0+((c730+1)>>1),c730+1);
        if(low<0) low=0;
        low=truncDiv(low*5+(denom>>1),denom);
        int high=low;
        if(low>9)low=9;
        if(high>9)high=9;

        if(secondary!=0&&i!=0){
            int subtract=0;
            if(i>f0) subtract=truncDiv((i-f0)*8+(f4>>1),f4);
            low-=subtract; high-=subtract;
            if(low<0)low=0;
            if(high<0)high=0;
        }

        const int stripe=truncDiv(d8,e0*2);
        if(stripe>0 && (truncDiv(i,stripe)&1)) high=signedByte(high-1);
        if(low<=2) high=low;
        if(low==8&&high==9)high=8;
        state.distanceRemap[static_cast<std::size_t>(i)]=
            static_cast<std::uint16_t>((high&0xff)<<8u | (low&0xff));

        int shade=low;
        if(shade>8)shade=8;
        if(i>=70&&i<198){
            const auto address=static_cast<std::uint32_t>(0xfe4e + i);
            shadeMemory.at(address-kBase)=static_cast<std::uint8_t>(shade);
        }
        if(i>=50&&i<178){
            const int index=127-(i-50);
            shadeMemory.at(static_cast<std::size_t>(index))=static_cast<std::uint8_t>(shade);
        }
    }

    for(std::size_t row=0;row<128;++row)
        state.floorShade[row]=shadeMemory.at(1u+row); // FE15..FE94
    return state;
}

}
