// Execute the genuine reconstructed actor dispatcher and original type-15
// saving-throw / damage / cleanup path on a media-free synthetic guest.
// Not an assertion of full gameplay, game-over UI or DOS parity.
#include "../../Code2/Code2.cpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using code2::u32;
code2::Machine* machine=nullptr;
u32 attacks=0u, cleanup_calls=0u, sound_calls=0u;
void require(bool ok,const char* message) {
    if(!ok)throw std::runtime_error(message);
}
u32 dice(u32 count,u32 sides,u32 extra,u32 flat) noexcept {
    code2::Application39Boundary b{};
    b.call_30ca2=+[]() noexcept -> u32 {
        return code2::function_30ca2(*machine);
    };
    return code2::function_1c328(*machine,b,count,sides,extra,flat);
}
u32 saving_throw(u32 slot) noexcept {
    code2::Application18Boundary b{};
    b.call_1c328=&dice;
    return code2::function_0cee4(*machine,b,slot);
}
void cleanup_original(code2::s32 selected) noexcept {
    ++cleanup_calls;
    code2::Application16Boundary cleanup{};
    cleanup.call_0893c=+[]() noexcept {
        code2::Application12Boundary timers{};
        timers.call_19cc4=+[]() noexcept -> u32 {
            return code2::function_19cc4(*machine);
        };
        code2::function_0893c(*machine,timers);
    };
    code2::function_0ad24(*machine,cleanup,selected);
}
u32 selector(u32 token) noexcept {
    code2::Application27Boundary b{};
    b.call_27c70=+[](u32 left,u32 right) noexcept -> u32 {
        return static_cast<u32>(code2::function_27c70(*machine,left,right));
    };
    return static_cast<u32>(code2::function_14a60(*machine,b,token));
}
void damage_original(u32 slot,u32 damage) noexcept {
    code2::Application18Boundary b{};
    b.call_19cc4=+[]() noexcept -> u32 {
        return code2::function_19cc4(*machine);
    };
    b.call_0ad24=&cleanup_original;
    b.call_14a60=&selector;
    b.call_1c1dc=+[](u32 bit) noexcept -> u32 {
        return code2::function_1c1dc(*machine,bit);
    };
    code2::function_0cd44(*machine,b,slot,damage);
}
void attack_source(u32 record,u32 definition,u32 actor) noexcept {
    ++attacks;
    code2::Application19Boundary b{};
    b.call_cee4=&saving_throw;
    b.call_cd44=&damage_original;
    b.call_28110=+[](u32 request,u32 aux,u32 channel,u32 prio) noexcept {
        ++sound_calls;
        code2::function_28110(*machine,request,aux,channel,prio);
    };
    code2::function_0d550(*machine,b,record,definition,actor);
}
void verify_case(bool resist_everyone) {
    std::vector<code2::u8> memory(0x300000u,0u);
    code2::Machine state{};
    state.data=memory.data();
    machine=&state;
    attacks=cleanup_calls=sound_calls=0u;
    // Exactly one source actor of type 15 is in animated state=1.
    const u32 entity=0x14D80u;
    const u32 descriptor=0x90000u;
    code2::store_le32(memory.data()+0x22230u,descriptor-15u*0x45u);
    code2::store_le16(memory.data()+0x258A2u,1u);
    code2::store_le16(memory.data()+entity+0x0Eu,15u);
    code2::store_le16(memory.data()+entity+0x14u,2u);
    memory[0xBF38u]=1u;
    code2::store_le16(memory.data()+descriptor+0x25u,5u);
    code2::store_le16(memory.data()+descriptor+0x3Fu,4u);
    code2::store_le32(memory.data()+0x258C0u,10u);
    code2::store_le32(memory.data()+0x19cc4u,2u);
    code2::store_le32(memory.data()+0xF6F4u,0x13572468u);
    code2::store_le16(memory.data()+0x218F0u,0x80u);
    code2::store_le16(memory.data()+0x21B38u,0x80u);
    code2::store_le16(memory.data()+0x21D80u,0x4000u);
    code2::store_le16(memory.data()+0x21FC8u,0x4000u);
    code2::store_le16(memory.data()+0x218FCu,14u);
    code2::store_le16(memory.data()+0x218FCu+0x248u,22u);
    memory[0x218F5u]=0u;
    memory[0x218F5u+0x248u]=1u;
    code2::store_le16(memory.data()+0x21900u,1u);
    code2::store_le16(memory.data()+0x21900u+0x248u,1u);
    // No game-state guesses: the reconstructed 0CEE4 rolls against
    // original stat-table positions. 21 means always fail d20;
    // 1 means always save d20, independent of the source RNG seed.
    code2::store_le32(memory.data()+0x6888u+4u,resist_everyone?1u:21u);
    code2::store_le32(memory.data()+0x6888u+0x5Cu+4u,1u);
    // Names used by the original 14A60/27C70 selected-slot check.
    memory[0x578u]='A';
    memory[0x218E0u]='B';
    memory[0x218E0u+0x248u]='C';

    code2::Application38Boundary actor{};
    actor.call_0d550=&attack_source;
    code2::function_1bdac(state,actor);
    require(attacks==1u && sound_calls==1u,
            "original 1BDAC omitted original 0D550 and 28110");
    require(code2::load_le16(memory.data()+entity+0x10u)==5u,
            "actor source 1BDAC timer not committed");
    require((code2::load_le16(memory.data()+entity+0x14u)&6u)==4u,
            "actor source state transition changed");
    require(code2::load_le16(memory.data()+0x218FCu+0x248u)==22u,
            "successful original 0CEE4 saving throw changed party HP");
    if(resist_everyone) {
        require(cleanup_calls==0u &&
                code2::load_le16(memory.data()+0x218FCu)==14u,
                "original successful save incorrectly applied damage");
    } else {
        require(cleanup_calls==1u &&
                code2::load_le16(memory.data()+0x218FCu)==0u &&
                (memory[0x218F1u]&0x40u)!=0u,
                "original lethal 0CD44 / 0AD24 cleanup not executed");
        require(code2::load_le32(memory.data()+0x21A92u)==14u,
                "original damage amount not recorded");
    }
}
}
int main() {
    try {
        verify_case(true);
        verify_case(false);
        std::cout<<"ORIGINAL_CODE2_1BDAC_0D550_TYPE15_SOURCE_PASS"
                 <<" scenarios=2 original_saves=YES original_lethal_damage=YES"
                 <<" original_0AD24_cleanup=YES"
                 <<" game_over_unconnected=YES DOS_parity=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"FAIL "<<e.what()<<"\n";return 1;
    }
}
