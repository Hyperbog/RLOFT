// Part 26: media-free source execution of the original scene and actor
// post-load control flow. The callbacks below are TEST INSTRUMENTATION;
// they do not draw pixels or replace the live game renderer.
#include "../../Code2/Code2.cpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using u32=code2::u32;
using s32=code2::s32;
struct Trace {
    std::vector<std::string> calls;
    bool valid=true;
    u32 next_handle=0x1001u;
    void record(std::string name) {calls.push_back(std::move(name));}
    void check(bool condition) {if(!condition)valid=false;}
};
Trace* trace=nullptr;
code2::Machine* actor_machine=nullptr;
void require(bool good,const char* reason) {
    if(!good)throw std::runtime_error(reason);
}
void page(u32 p) {trace->check(p==0u);trace->record("291B0");}
void image(u32 y,u32 x,u32 resource,u32 slot) {
    const auto n=trace->calls.size();
    trace->check(x==0u && slot==0u &&
       ((n==1u&&y==0u&&resource==22u) ||
        (n==2u&&y==200u&&resource==23u)));
    trace->record("2E510");
}
void release(u32 resource,u32 slot) {
    const auto n=trace->calls.size();
    trace->check(slot==0u &&
        ((n==3u&&resource==22u)||(n==4u&&resource==23u)));
    trace->record("2EFD0");
}
u32 capture_source(u32 y,u32 x,u32 w,u32 h,u32 mask) {
    const auto n=trace->calls.size();
    trace->check(x==0x20u&&h==0x100u&&mask==0xFFu &&
        ((n==5u&&y==0u&&w==4u) ||
         (n==6u&&y==0x104u&&w==4u) ||
         (n==7u&&y==0x106u&&w==0x0Bu)));
    trace->record("30070");
    return trace->next_handle++;
}
void palette(u32 addr,u32 first,u32 mode,u32 filename,u32 count) {
    trace->check(addr==0xC054u&&first==0u&&mode==2u&&
                 filename==0x1128u&&count==0x100u);
    trace->record("325B0");
}
void dac(u32 addr) {trace->check(addr==0xC054u);trace->record("29234");}
void pointer(u32 p,s32 xy,u32 shape) {
    trace->check(p==0u&&xy==0&&shape==0u);trace->record("1A5B0");
}
void phase22d70() {trace->record("22D70");}
void phase2f868() {trace->record("2F868");}
void phase30b2c() {trace->record("30B2C");}

void test_original_compositor_dispatch() {
    std::vector<code2::u8> memory(code2::kObject2VirtualSize,0u);
    code2::Machine machine{};machine.data=memory.data();
    code2::store_le32(memory.data()+0x258DCu,15u);
    code2::store_le32(memory.data()+0xC034u,0u);
    memory[0xC050u]=0u;
    memory[0xD681u]=1u;
    Trace local{};local.calls.reserve(24u);trace=&local;
    code2::Application45Boundary b{};
    b.call_291b0=&page;
    b.call_2e510=&image;
    b.call_2efd0=&release;
    b.call_30070=&capture_source;
    b.call_325b0=&palette;
    b.call_29234=&dac;
    b.call_1a5b0=&pointer;
    b.call_22d70=&phase22d70;
    b.call_2f868=&phase2f868;
    b.call_30b2c=&phase30b2c;
    code2::function_1f8e0(machine,b,2u);
    const std::vector<std::string> expected={
        "291B0","2E510","2E510","2EFD0","2EFD0",
        "30070","30070","30070","325B0","29234",
        "1A5B0","22D70","2F868","30B2C"};
    require(local.valid&&local.calls==expected,
            "1F8FC source compositor callback arguments/order changed");
    require(code2::load_le32(memory.data()+0xC034u)==2u &&
            memory[0xC050u]==1u && memory[0xCF58u]==0u &&
            code2::load_le32(memory.data()+0xBF48u)==0x1001u &&
            code2::load_le32(memory.data()+0xBF4Cu)==0x1002u &&
            code2::load_le32(memory.data()+0x2222Cu)==0x1003u &&
            memory[0xD681u]==2u &&
            code2::load_le32(memory.data()+0xD8E0u)==0xFFFFFFFFu &&
            code2::load_le32(memory.data()+0xD8E4u)==0xFFFFFFFFu,
            "1F8FC source scene bookkeeping or page flip differs");
    local.calls.clear();
    // The source 1F8E0 JE returns without 1F8FC for unchanged art mode.
    code2::function_1f8e0(machine,b,2u);
    require(local.calls.empty()&&memory[0xCF58u]==1u&&
            memory[0xC050u]==1u,
            "1F8E0 unchanged-mode early return differs");
    trace=nullptr;
    std::cout<<"ORIGINAL_CODE2_1F8FC_SYNTHETIC_SOURCE_DISPATCH_PASS"
             <<" frames=1 resource_requests=2 page_flips=1"
             <<" callback_order=SOURCE"
             <<" image_pixels=NOT_RENDERED"
             <<" persistent_guest=NOT_CONNECTED\n";
}
u32 quest_bit(u32 bit) {return code2::function_1c1dc(*actor_machine,bit);}
u32 actor_kind(u32 type) {return code2::function_2acdc(type);}
void test_original_actor_postload() {
    std::vector<code2::u8> memory(code2::kObject2VirtualSize,0u);
    code2::Machine machine{};machine.data=memory.data();
    actor_machine=&machine;
    code2::Application31Boundary b{};
    b.call_1c1dc=&quest_bit;
    b.call_2acdc=&actor_kind;
    // Party characters have no active status bits in this synthetic test.
    // No random, dialog or gameplay callback is silently supplied.
    code2::store_le32(memory.data()+0xC6C8u,22u);
    code2::store_le16(memory.data()+0x258A2u,1u);
    constexpr u32 actor=0x14D80u;
    code2::store_le16(memory.data()+actor+0x0Eu,9u);
    code2::store_le16(memory.data()+actor+0x12u,0u);
    code2::function_17a68(machine,b);
    require((memory[actor+0x0Bu]&2u)!=0u,
            "17A68 did not set original nonpositive-health actor mark");
    code2::store_le16(memory.data()+actor+0x12u,10u);
    code2::function_17a68(machine,b);
    require((memory[actor+0x0Bu]&2u)==0u,
            "17A68 did not clear original actor mark on re-entry");
    // Original type 8 obeys quest bit 3A rather than host fiction.
    code2::store_le16(memory.data()+actor+0x0Eu,8u);
    code2::function_17a68(machine,b);
    require((memory[actor+0x0Bu]&2u)!=0u,
            "17A68 quest-unset original type8 case changed");
    code2::function_1c124(machine,0x3Au);
    require(code2::function_1c1dc(machine,0x3Au)!=0u,
            "1C124 original quest bit was not stored");
    code2::function_17a68(machine,b);
    require((memory[actor+0x0Bu]&2u)==0u,
            "17A68 quest-set original type8 case changed");
    actor_machine=nullptr;
    std::cout<<"ORIGINAL_CODE2_17A68_SYNTHETIC_ACTOR_POSTLOAD_PASS"
             <<" original_17838=EXECUTED"
             <<" flags=SOURCE_1C1DC"
             <<" type_predicate=SOURCE_2ACDC"
             <<" actor_AI=NOT_CONNECTED\n";
}
}
int main() {
    try {
        test_original_compositor_dispatch();
        test_original_actor_postload();
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"ORIGINAL_CODE2_1F8FC_17A68_SOURCE_FAIL: "
                 <<e.what()<<"\n";
        return 1;
    }
}
