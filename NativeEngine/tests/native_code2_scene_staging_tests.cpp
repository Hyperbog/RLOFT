#include "rloft_native/native_code2_scene_staging.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>
using namespace rloft_native;
namespace {
void put16(NativeCode2SceneRecord22& b,unsigned p,unsigned v){
    b[p]=static_cast<std::uint8_t>(v);
    b[p+1]=static_cast<std::uint8_t>(v>>8U);
}
unsigned get16(const NativeCode2SceneRecord22& b,unsigned p){
    return unsigned(b[p])|(unsigned(b[p+1])<<8U);
}
void put32(NativeCode2SceneRecord22& b,unsigned p,unsigned v){
    put16(b,p,v);put16(b,p+2,v>>16U);
}
template<class F>void rejects(F fn){
    bool did=false;
    try{fn();}catch(const std::invalid_argument&){did=true;}
    assert(did);
}
}
int main(){
    NativeCode2SceneRecord22 source{},scratch{};
    scratch.fill(0x5a);
    put16(scratch,0,33); // pre-existing screen X from the original buffer
    put32(source,0,0x20000U);
    put32(source,4,0U);
    put16(source,0x14,0x01c0U); // original packed view 7
    NativeCode2ProjectionRegisters regs{};
    regs.direction_x_255f8=0x8000U;
    regs.horizontal_origin_c6d0=9;
    regs.height_numerator_255e0=0x8000U;
    std::vector<std::uint8_t> init(7,0);
    init[0]=0x34;init[1]=0x12;init[4]=3;
    auto a=stage_native_code2_non_npc_2bbf4(
        NativeCode2NonNpcFamily::InitPlaced,source,init,true,regs,8,scratch);
    assert(a.passed_original_projection_gate);
    assert(!a.original_live_frame_selection_verified);
    assert(!a.original_render_dispatch_verified);
    assert(get16(a.record,0)==9);
    assert(get16(a.record,2)==146); // (0x8000 / (0xE000>>8))
    assert(get16(a.record,4)==0x1249); // 0x1234+7*3
    assert(get16(a.record,12)==0);
    for(unsigned p=14;p<=20;p+=2)assert(get16(a.record,p)==0);
    for(unsigned p:{6U,8U,10U})assert(get16(a.record,p)==0x5a5a);
    assert(get16(scratch,0)==33);

    std::vector<std::uint8_t> objects(0x41,0);
    objects[0x28]=0xfa; objects[0x29]=0xff;objects[0x2a]=20;
    auto b=stage_native_code2_non_npc_2bbf4(
        NativeCode2NonNpcFamily::ObjectsLoose,source,objects,true,regs,8);
    assert(get16(b.record,4)==0x86); // 16-bit original wrap
    assert(b.passed_original_projection_gate);
    auto invisible=stage_native_code2_non_npc_2bbf4(
        NativeCode2NonNpcFamily::InitPlaced,source,init,false,regs,8,scratch);
    assert(!invisible.passed_original_projection_gate);
    assert(invisible.record==scratch);
    auto low=stage_native_code2_non_npc_2bbf4(
        NativeCode2NonNpcFamily::InitPlaced,source,init,true,regs,146,scratch);
    assert(!low.passed_original_projection_gate);
    assert(get16(low.record,2)==146); // gate uses strict greater-than
    auto near_source=source;
    put32(near_source,0,0U);
    auto near=stage_native_code2_non_npc_2bbf4(
        NativeCode2NonNpcFamily::InitPlaced,near_source,init,true,regs,0,scratch);
    assert(!near.passed_original_projection_gate);
    assert(get16(near.record,0)==33); // 35798 rejected: screen X unchanged
    assert(get16(near.record,2)==0);
    assert(get16(near.record,4)==0x1249);
    rejects([&]{static_cast<void>(stage_native_code2_non_npc_2bbf4(
        NativeCode2NonNpcFamily::InitPlaced,source,objects,true,regs,0));});
    rejects([&]{static_cast<void>(stage_native_code2_non_npc_2bbf4(
        NativeCode2NonNpcFamily::ObjectsLoose,source,init,true,regs,0));});
    rejects([&]{static_cast<void>(stage_native_code2_non_npc_2bbf4(
        static_cast<NativeCode2NonNpcFamily>(3),source,init,true,regs,0));});
}
