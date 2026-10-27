// Genuine 0D550 miss -> compiled 3210F -> 385B0 -> 1C21C.
// Only a source-validated bare %s and literal text are admitted.
#include "../../Code2/Code2.cpp"
#include "original_code2_near_combat_preflight.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
namespace {
using code2::u32;
code2::Machine* machine=nullptr;
u32 hit_checks=0, text_formats=0, overlays=0, sounds=0, damages=0;
void require(bool v,const char* why){if(!v)throw std::runtime_error(why);}
u32 rng(u32 dice,u32 sides,u32 per,u32 bonus) noexcept {
    code2::Application39Boundary b{};
    b.call_30ca2=+[]() noexcept -> u32 {
        return code2::function_30ca2(*machine);
    };
    return code2::function_1c328(*machine,b,dice,sides,per,bonus);
}
u32 miss_check(u32 slot,u32 desc,u32 rec) noexcept {
    ++hit_checks;
    code2::Application18Boundary b{};
    b.call_1c328=&rng;
    return code2::function_0c9a0(*machine,b,slot,desc,rec);
}
void format_message(u32 output,u32 format,u32 name) noexcept {
    ++text_formats;
    const auto result=rloft::source_near_combat::miss_text(
        machine->data,0x300000u,format,name);
    if(!result.accepted)std::terminate();
    code2::Application78Boundary b{};
    b.call_385b0=+[](u32 dst,u32 fmt,const u32* vars,u32) noexcept -> u32 {
        code2::Runtime143Boundary fmtb{};
        fmtb.emit=+[](u32 out,code2::u8 ch,u32& count) noexcept {
            machine->data[out+count++]=ch;
        };
        fmtb.convert=+[](const code2::u8*& args,
                         code2::Runtime143FormatState& state,
                         u32& offset,code2::u16& selector) noexcept {
            if(state.conversion!='s')std::terminate();
            offset=code2::load_le32(args);
            args+=4;selector=0;
            state.data_length=static_cast<u32>(
                std::strlen(reinterpret_cast<const char*>(machine->data+offset)));
        };
        fmtb.read_far8=+[](code2::u16,u32 off) noexcept -> code2::u8 {
            return machine->data[off];
        };
        const code2::u8* args=reinterpret_cast<const code2::u8*>(vars);
        return code2::function_385b0(fmtb,machine->data+fmt,args,dst);
    };
    const u32 args[1]={name};
    (void)code2::function_3210f(*machine,b,output,format,args);
}
void stage_overlay(u32 src) noexcept {
    ++overlays;
    code2::Application39Boundary b{};
    b.call_30700=+[]() noexcept -> u32 {
        return code2::function_30700(*machine);
    };
    code2::function_1c21c(*machine,b,src);
}
void actor_attack(u32 rec,u32 desc,u32 index) noexcept {
    const auto eligible=rloft::source_near_combat::eligible_hit(
        machine->data,0x300000u,rec,desc);
    if(!eligible.admitted || eligible.d20!=1u)std::terminate();
    const u32 fmt=code2::load_le32(machine->data+0x67E0u);
    if(!rloft::source_near_combat::miss_text(
        machine->data,0x300000u,fmt,desc).accepted)std::terminate();
    code2::Application19Boundary b{};
    b.call_30700=+[]() noexcept -> u32 {
        return code2::function_30700(*machine);
    };
    b.call_30cc4=+[](u32 seed) noexcept {code2::function_30cc4(*machine,seed);};
    b.call_d4e8=+[](u32 slot,u32 ptr) noexcept -> u32 {
        return code2::function_0d4e8(*machine,slot,ptr);
    };
    b.call_14a60=+[](u32 token) noexcept -> u32 {
        code2::Application27Boundary names{};
        names.call_27c70=+[](u32 a,u32 c) noexcept -> u32 {
            return static_cast<u32>(code2::function_27c70(*machine,a,c));
        };
        return static_cast<u32>(code2::function_14a60(*machine,names,token));
    };
    b.call_c9a0=&miss_check;
    b.call_3210f=&format_message;
    b.call_1c21c=&stage_overlay;
    b.call_cd44=+[](u32,u32) noexcept {++damages;};
    b.call_28110=+[](u32 sound,u32 aux,u32 channel,u32 priority) noexcept {
        ++sounds;
        code2::function_28110(*machine,sound,aux,channel,priority);
    };
    code2::function_0d550(*machine,b,rec,desc,index);
}
void verify(bool substituted) {
    std::vector<code2::u8> memory(0x300000u,0);
    code2::Machine guest{};
    guest.data=memory.data();machine=&guest;
    hit_checks=text_formats=overlays=sounds=damages=0u;
    constexpr u32 actor=0x14D80u,desc=0x90000u,fmt=0x81000u;
    code2::store_le32(memory.data()+0x22230u,desc-9u*0x45u);
    code2::store_le16(memory.data()+0x258A2u,1u);
    code2::store_le16(memory.data()+actor+0x0Eu,9u);
    code2::store_le16(memory.data()+actor+0x14u,2u);
    memory[0xBF38u]=1u;
    code2::store_le16(memory.data()+desc+0x25u,5u);
    memory[desc]='W';memory[desc+1u]='o';memory[desc+2u]='l';
    memory[desc+3u]='f';
    memory[desc+0x1Eu]=1u;memory[desc+0x1Fu]=6u;
    code2::store_le32(memory.data()+0x67E0u,fmt);
    const char* format=substituted?"Missed %s":"Missed";
    std::memcpy(memory.data()+fmt,format,std::strlen(format)+1u);
    memory[0x218E0u]='A';memory[0x584u]='A';
    code2::store_le16(memory.data()+0x218F0u,0x80u);
    code2::store_le16(memory.data()+0x218FCu,30u);
    memory[0x21913u]=2u;
    code2::store_le32(memory.data()+0x255D8u,0x80000u);
    for(u32 j=0;j<8u;++j)memory[0x687Eu+j]=static_cast<code2::u8>(j);
    for(u32 j=0;j<0x2Au;j+=2u)
        code2::store_le16(memory.data()+0x21920u+j,0xFFFFu);
    code2::store_le32(memory.data()+0xF58Cu,0u); // genuine 30CA2 d20=1
    code2::store_le32(memory.data()+0x258BCu,0x300u);
    code2::store_le32(memory.data()+0xEE40u,1u);
    code2::store_le32(memory.data()+0xEE48u,0xFFFFFFFFu);
    code2::Application38Boundary dispatcher{};
    dispatcher.call_0d550=&actor_attack;
    code2::function_1bdac(guest,dispatcher);
    require(hit_checks==1u && text_formats==1u && overlays==1u &&
            sounds==1u && damages==0u,"original miss source call order incorrect");
    const char* expected=substituted?"Missed Wolf":"Missed";
    require(std::strcmp(reinterpret_cast<const char*>(
            memory.data()+0x14580u),expected)==0,
            "original source 3210F/385B0/1C21C text differs");
    require(memory[0xCF59u]==1u &&
            code2::load_le32(memory.data()+0xCF54u)==0x5Au,
            "original 1C21C overlay scheduling absent");
    require(code2::load_le16(memory.data()+0x218FCu)==30u &&
            code2::load_le32(memory.data()+0xEE48u)==0x309u,
            "miss changed health or original queue result");
    memory[fmt]='%';memory[fmt+1u]='n';memory[fmt+2u]=0;
    require(!rloft::source_near_combat::miss_text(
        memory.data(),memory.size(),fmt,desc).accepted,
        "unsupported %n must remain blocked before guest mutation");
}
}
int main() {
    try {
        verify(false);verify(true);
        std::cout<<"ORIGINAL_CODE2_MISS_3210F_385B0_1C21C_PASS"
                 <<" source_text=YES original_overlay=YES"
                 <<" unsupported_format=GUARDED DOS_parity=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"FAIL "<<e.what()<<"\n";return 1;
    }
}
