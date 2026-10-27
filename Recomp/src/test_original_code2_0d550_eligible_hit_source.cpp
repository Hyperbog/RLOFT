// Compiled original 1BDAC -> 0D550 -> 0D4E8 -> 0C9A0 -> 0EFA0
// -> 0CEE4 -> 0CD44 -> 28110, with original RNG and nonlethal effects.
// Synthetic guest only; no invented damage, DOS parity or complete combat.
#include "../../Code2/Code2.cpp"
#include "original_code2_near_combat_preflight.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
using code2::u32;
code2::Machine* guest=nullptr;
u32 attacks=0, hits=0, modifier_calls=0, equipment_calls=0, saves=0, damages=0, sounds=0, cleanups=0;
void require(bool condition,const char* message) {
    if(!condition)throw std::runtime_error(message);
}
u32 roll(u32 count,u32 sides,u32 per_die,u32 flat) noexcept {
    code2::Application39Boundary b{};
    b.call_30ca2=+[]() noexcept -> u32 {
        return code2::function_30ca2(*guest);
    };
    return code2::function_1c328(*guest,b,count,sides,per_die,flat);
}
u32 save(u32 slot) noexcept {
    ++saves;
    code2::Application18Boundary b{};
    b.call_1c328=&roll;
    return code2::function_0cee4(*guest,b,slot);
}
u32 hit(u32 slot,u32 descriptor,u32 actor) noexcept {
    ++hits;
    code2::Application18Boundary b{};
    b.call_1c328=&roll;
    b.call_efa0=+[](u32 selected) noexcept -> code2::s32 {
        ++modifier_calls;
        code2::Application20Boundary mod{};
        mod.call_fe44=+[](u32 s) noexcept -> u32 {
            return code2::function_0fe44(*guest,s);
        };
        mod.call_24478=+[](u32 selected,u32 hand) noexcept -> u32 {
            ++equipment_calls;
            return code2::function_24478(*guest,
                static_cast<code2::s32>(selected),hand);
        };
        return code2::function_0efa0(*guest,mod,selected);
    };
    return code2::function_0c9a0(*guest,b,slot,descriptor,actor);
}
u32 select_member(u32 token) noexcept;
void damage(u32 slot,u32 value) noexcept {
    ++damages;
    if(slot>=4u)std::terminate();
    code2::Application18Boundary b{};
    b.call_0ad24=+[](code2::s32 target) noexcept {
        ++cleanups;
        code2::Application16Boundary lifecycle{};
        lifecycle.call_0893c=+[]() noexcept {
            code2::Application12Boundary times{};
            times.call_19cc4=+[]() noexcept -> u32 {
                return code2::function_19cc4(*guest);
            };
            code2::function_0893c(*guest,times);
        };
        code2::function_0ad24(*guest,lifecycle,target);
    };
    b.call_14a60=&select_member;
    b.call_1c1dc=+[](u32 bit) noexcept -> u32 {
        return code2::function_1c1dc(*guest,bit);
    };
    b.call_19cc4=+[]() noexcept -> u32 {
        return code2::function_19cc4(*guest);
    };
    code2::function_0cd44(*guest,b,slot,value);
}
u32 select_member(u32 token) noexcept {
    code2::Application27Boundary b{};
    b.call_27c70=+[](u32 a,u32 c) noexcept -> u32 {
        return static_cast<u32>(code2::function_27c70(*guest,a,c));
    };
    return static_cast<u32>(code2::function_14a60(*guest,b,token));
}
void attack(u32 actor,u32 descriptor,u32 index) noexcept {
    ++attacks;
    const auto admitted=rloft::source_near_combat::eligible_hit(
        guest->data,0x300000u,actor,descriptor);
    if(!admitted.admitted)std::terminate();
    code2::Application19Boundary b{};
    b.call_30700=+[]() noexcept -> u32 { return code2::function_30700(*guest); };
    b.call_30cc4=+[](u32 seed) noexcept { code2::function_30cc4(*guest,seed); };
    b.call_30ca2=+[]() noexcept -> u32 { return code2::function_30ca2(*guest); };
    b.call_d4e8=+[](u32 slot,u32 desc) noexcept -> u32 {
        return code2::function_0d4e8(*guest,slot,desc);
    };
    b.call_14a60=&select_member;
    b.call_c9a0=&hit;
    b.call_1c328=&roll;
    b.call_cee4=&save;
    b.call_cd44=&damage;
    b.call_28110=+[](u32 sound,u32 aux,u32 channel,u32 priority) noexcept {
        ++sounds;
        code2::function_28110(*guest,sound,aux,channel,priority);
    };
    code2::function_0d550(*guest,b,actor,descriptor,index);
}
void run(u32 seed,bool requires_modifier,bool include_status,
         bool equipped=false,bool lethal=false) {
    std::vector<code2::u8> memory(0x300000u,0);
    code2::Machine machine{};
    machine.data=memory.data();
    guest=&machine;
    attacks=hits=modifier_calls=equipment_calls=saves=damages=sounds=cleanups=0u;
    constexpr u32 actor=0x14D80u,def=0x90000u,type=9u;
    code2::store_le32(memory.data()+0x22230u,def-type*0x45u);
    code2::store_le16(memory.data()+0x258A2u,1u);
    code2::store_le16(memory.data()+actor+0x0Eu,type);
    code2::store_le16(memory.data()+actor+0x14u,2u);
    code2::store_le16(memory.data()+def+0x25u,5u);
    memory[def+0x1Eu]=1u;
    memory[def+0x1Fu]=6u;
    memory[def+0x20u]=0u;
    memory[def+0x22u]=12u;
    memory[def+0x39u]=include_status?1u:0u;
    memory[def+0x3Au]=include_status?0x10u:0u;
    code2::store_le16(memory.data()+def+0x3Fu,7u);
    memory[0xBF38u]=1u;
    memory[0x258A8u]=0u;
    code2::store_le32(memory.data()+0xF58Cu,seed);
    code2::store_le32(memory.data()+0xF6F4u,0xAABBCCDDu);
    code2::store_le32(memory.data()+0xA7E4u,0x400u);
    code2::store_le32(memory.data()+0x258C0u,0x100u);
    code2::store_le32(memory.data()+0xEE40u,1u);
    code2::store_le32(memory.data()+0xEE48u,0xFFFFFFFFu);
    // Original 0D550 points the indexed angle to source-selected slot zero.
    for(u32 i=0u;i<8u;++i)memory[0x687Eu+i]=static_cast<code2::u8>(i);
    memory[0x218E0u]='A';
    memory[0x584u]=lethal?'X':'A';
    memory[0x578u]='X';
    code2::store_le16(memory.data()+0x218F0u,0x80u);
    code2::store_le16(memory.data()+0x218FCu,lethal?3u:30u);
    code2::store_le16(memory.data()+0x218F0u+0x248u,0x80u);
    code2::store_le16(memory.data()+0x218FCu+0x248u,20u);
    code2::store_le16(memory.data()+0x21900u,1u);
    memory[0x218F5u]=0u;
    memory[0x218FBu]=requires_modifier?5u:0u;
    memory[0x21913u]=2u;
    code2::store_le32(memory.data()+0x255D8u,0x80000u);
    for(u32 i=0u;i<0x2Au;i+=2u)
        code2::store_le16(memory.data()+0x21920u+i,0xFFFFu);
    if(equipped) {
        code2::store_le16(memory.data()+0x2193Cu,0x20u);
        code2::store_le16(memory.data()+0x2193Eu,0x21u);
        for(u32 item=0x20u;item<=0x21u;++item) {
            const u32 off=0x80000u+item*65u;
            memory[off+0x2Fu]=2u;
            memory[off+0x34u]=3u;
            memory[off+0x37u]=0u;
        }
    }
    code2::store_le32(memory.data()+0x6888u+4u,21u);
    for(u32 i=0u;i<4u;++i)
        code2::store_le16(memory.data()+0x7E8Cu+i*2u,500u);
    const auto pre=rloft::source_near_combat::eligible_hit(
        memory.data(),memory.size(),actor,def);
    require(pre.admitted&&pre.slot==0u &&
            (requires_modifier?pre.d20==10u:pre.d20==20u),
            "source near hit preflight mismatch");
    code2::Application38Boundary dispatcher{};
    dispatcher.call_0d550=&attack;
    code2::function_1bdac(machine,dispatcher);
    require(attacks==1u && hits==1u && damages==1u && sounds==1u,
            "original near hit chain did not execute");
    require(modifier_calls==(requires_modifier?1u:0u),
            "0EFA0 source modifier branch changed");
    require(equipment_calls==(equipped?2u:0u),
            "original 24478 hand callbacks missing");
    require(saves==(include_status?2u:0u),
            "0CEE4 saving throw status call count changed");
    const auto hp=code2::load_le16(memory.data()+0x218FCu);
    if(lethal) {
        require(hp==0u && cleanups==1u &&
                (memory[0x218F1u]&0x40u)!=0u &&
                code2::load_le16(memory.data()+0x218FCu+0x248u)==20u,
                "genuine 0CD44 -> 0AD24 survivor cleanup absent");
    } else {
        require(hp<30u && hp>0u && cleanups==0u,
                "original 0CD44 nonlethal HP mutation absent");
    }
    require(code2::load_le32(memory.data()+0x21A86u)==0x436u &&
            code2::load_le32(memory.data()+0x21A92u)!=0u,
            "original hit timer/damage accounting not applied");
    require(code2::load_le32(memory.data()+0xEE48u)==0x107u,
            "source 28110 audio request absent");
    if(include_status)
        require((memory[0x218F1u]&0x0Cu)==0x0Cu,
                "original failed-save status bits not set");
    // Original all-party death still requires its 0CBC8 page/audio/input
    // choreography. Never call this branch with a fabricated compositor.
    code2::store_le16(memory.data()+0x218FCu,3u);
    code2::store_le16(memory.data()+0x218F0u+0x248u,0xD040u);
    const auto rejected=rloft::source_near_combat::eligible_hit(
        memory.data(),memory.size(),actor,def);
    require(!rejected.admitted,
            "whole-party wipe incorrectly admitted without source 0CBC8");
}
}
int main() {
    try {
        run(33u,false,true); // natural 20, two source saving-throw effects
        run(34u,true,false); // original 0EFA0 modifier
        run(34u,true,false,true); // equipped original 24478 hand checks
        run(33u,false,false,false,true); // lethal 0CD44/0AD24, other survivor
        std::cout<<"ORIGINAL_CODE2_0D550_0C9A0_ELIGIBLE_HIT_PASS"
                 <<" original_0EFA0=YES original_0CEE4=YES"
                 <<" original_0CD44=YES original_28110=YES"
                 <<" lethal_survivor=YES equipment_24478=YES"
                 <<" party_wipe_and_miss=GUARDED DOS_parity=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"FAIL "<<e.what()<<"\n";return 1;
    }
}
