// Source-only near combat regression: 1BDAC -> 0D550 -> 0D4E8 -> 28110.
// No host-computed hit, simulated damage, fabricated target or DOS parity.
#include "../../Code2/Code2.cpp"
#include "original_code2_near_combat_preflight.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using code2::u32;
code2::Machine* machine=nullptr;
u32 action_calls=0u, target_calls=0u, sound_calls=0u, random_calls=0u;
void require(bool condition,const char* detail) {
    if(!condition)throw std::runtime_error(detail);
}
void original_near_attack(u32 actor,u32 definition,u32 index) noexcept {
    ++action_calls;
    const auto guard=rloft::source_near_combat::exhausted_targets(
        machine->data,0x300000u,actor,definition);
    if(!guard.accepted)std::terminate();
    code2::Application19Boundary source{};
    source.call_30700=+[]() noexcept -> u32 {
        return code2::function_30700(*machine);
    };
    source.call_30cc4=+[](u32 seed) noexcept {
        code2::function_30cc4(*machine,seed);
    };
    source.call_30ca2=+[]() noexcept -> u32 {
        ++random_calls;
        return code2::function_30ca2(*machine);
    };
    source.call_d4e8=+[](u32 slot,u32 desc) noexcept -> u32 {
        ++target_calls;
        return code2::function_0d4e8(*machine,slot,desc);
    };
    source.call_28110=+[](u32 request,u32 aux,u32 channel,
                            u32 priority) noexcept {
        ++sound_calls;
        code2::function_28110(*machine,request,aux,channel,priority);
    };
    code2::function_0d550(*machine,source,actor,definition,index);
}
void run(bool paired_selection,bool random_direction) {
    std::vector<code2::u8> bytes(0x300000u,0u);
    code2::Machine m{};
    m.data=bytes.data();
    machine=&m;
    action_calls=target_calls=sound_calls=random_calls=0u;
    constexpr u32 actor=0x14D80u, descriptor=0x90000u, type=9u;
    code2::store_le32(bytes.data()+0x22230u,descriptor-type*0x45u);
    code2::store_le16(bytes.data()+0x258A2u,1u);
    code2::store_le16(bytes.data()+actor+0x0Eu,type);
    code2::store_le16(bytes.data()+actor+0x14u,2u);
    code2::store_le16(bytes.data()+descriptor+0x25u,5u);
    bytes[0xBF38u]=1u;
    bytes[0x258A8u]=paired_selection?0xFFu:0u;
    bytes[0x687Eu]=random_direction?6u:0u;
    for(u32 i=1u;i<8u;++i)bytes[0x687Eu+i]=static_cast<code2::u8>(i);
    code2::store_le32(bytes.data()+0xF58Cu,0x13572468u);
    code2::store_le32(bytes.data()+0xF6F4u,0x11223344u);
    code2::store_le32(bytes.data()+0x258BCu,0x300u);
    code2::store_le32(bytes.data()+0xEE40u,1u);
    code2::store_le32(bytes.data()+0xEE48u,0xFFFFFFFFu);
    for(u32 slot=0;slot<4u;++slot) {
        code2::store_le16(bytes.data()+0x218F0u+slot*0x248u,0x80u);
        bytes[0x21A12u+slot*0x248u]=0x20u;
        code2::store_le16(bytes.data()+0x218FCu+slot*0x248u,20u);
    }
    require(rloft::source_near_combat::exhausted_targets(
        bytes.data(),bytes.size(),actor,descriptor).accepted,
        "valid near exhaustion unexpectedly rejected");

    code2::Application38Boundary dispatch{};
    dispatch.call_0d550=&original_near_attack;
    code2::function_1bdac(m,dispatch);
    require(action_calls==1u && sound_calls==1u,
            "original 1BDAC/0D550/28110 chain incomplete");
    require(target_calls==(paired_selection?2u:4u),
            "source 0D4E8 selected the wrong target-exhaustion path");
    require(random_calls==(random_direction?1u:0u),
            "original random direction path not preserved");
    require(code2::load_le32(bytes.data()+0xEE48u)==0x309u,
            "original source sound queue request missing");
    require(code2::load_le16(bytes.data()+actor+0x10u)==5u &&
            (code2::load_le16(bytes.data()+actor+0x14u)&6u)==4u,
            "original 1BDAC actor update lost");
    for(u32 slot=0;slot<4u;++slot)
        require(code2::load_le16(bytes.data()+0x218FCu+slot*0x248u)==20u,
                "target-exhaustion source branch changed party HP");

    // An eligible target MUST NOT be admitted into missing 0C9A0 effects.
    bytes[0x21A12u+0x248u]=0u;
    const auto open=rloft::source_near_combat::exhausted_targets(
        bytes.data(),bytes.size(),actor,descriptor);
    require(!open.accepted,"eligible combat target incorrectly admitted");
    // Original type-8 special projectile action is also not this branch.
    code2::store_le16(bytes.data()+actor+0x0Eu,8u);
    code2::store_le16(bytes.data()+actor+0x14u,0x8000u);
    require(!rloft::source_near_combat::exhausted_targets(
        bytes.data(),bytes.size(),actor,descriptor).accepted,
        "original type-8 spawn incorrectly admitted");
}
}
int main() {
    try {
        run(false,false); // source four-slot cycle
        run(true,true);   // original paired alternative + 30CA2
        std::cout<<"ORIGINAL_CODE2_0D550_NEAR_EXHAUSTED_PASS"
                 <<" original_1BDAC=YES original_0D4E8=YES"
                 <<" original_28110=YES eligible_hit=GUARDED"
                 <<" projectile=GUARDED DOS_parity=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"FAIL "<<e.what()<<"\n";
        return 1;
    }
}
