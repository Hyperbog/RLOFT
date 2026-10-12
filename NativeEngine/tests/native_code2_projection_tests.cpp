#include "rloft_native/native_code2_projection.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace rloft_native;
namespace {
template<class Fn> void rejects(Fn fn){
    bool threw=false;
    try{fn();}catch(const std::exception&){threw=true;}
    assert(threw);
}
std::uint32_t mul_from_original_16bit_partials(
    std::uint32_t a,std::uint32_t b){
    const bool minus_a=(a&0x80000000U)!=0;
    const bool minus_b=(b&0x80000000U)!=0;
    const auto abs_a=minus_a?0U-a:a;
    const auto lo=abs_a&0xffffU;
    const auto hi=abs_a>>16U;
    const auto factor=b&0xffffU;
    const auto lo_partial=lo*factor;
    const auto hi_partial=hi*factor;
    const auto result=hi_partial+(lo_partial>>16U);
    return minus_a!=minus_b?0U-result:result;
}
void run(){
    assert(native_code2_mul_35680(0x20000U,0x8000U)==0x10000U);
    assert(native_code2_mul_35680(0x20000U,0x80008000U)==0xffff0000U);
    assert(native_code2_mul_35680(0xffff0000U,0x8000U)==0xffff8000U);
    assert(native_code2_mul_35680(0xffff0000U,0x80008000U)==0x8000U);
    assert(native_code2_mul_35680(0x7fffffffU,0xffffU)==0x7fff7fffU);
    assert(native_code2_mul_35680(0x80000000U,0xffffU)==0x80008000U);
    // Exhaust the 16-bit low-word coefficient 0..65535 for a
    // negative source; also both original sign-state combinations.
    for(unsigned coefficient=0;coefficient<=65535;++coefficient){
        const auto y=std::uint32_t(coefficient);
        for(const auto x:{0U,1U,0x10000U,0x7fffffffU,
                          0x80000000U,0xffffffffU}){
            assert(native_code2_mul_35680(x,y)==
                   mul_from_original_16bit_partials(x,y));
            assert(native_code2_mul_35680(x,y|0x80000000U)==
                   mul_from_original_16bit_partials(x,y|0x80000000U));
        }
    }
    NativeCode2ProjectionRegisters s;
    s.live_x_255f0=0x20000;
    s.origin_x_255ec=0;
    s.live_y_255e8=0;
    s.origin_y_255f4=0;
    s.direction_x_255f8=0x8000;
    s.direction_y_255fc=0;
    s.height_numerator_255e0=0x10000;
    s.horizontal_scale_c6cc=0x100;
    s.horizontal_origin_c6d0=100;
    auto wall=project_native_code2_wall_height_35730(s);
    assert(wall.source_depth==0x10000U);
    assert(wall.clamped_depth==0x10000U);
    assert(wall.unsigned_divisor==256);
    assert(wall.original_return_word==256);
    assert(!wall.clamped_to_original_near_limit);
    assert(!wall.original_runtime_viewpoint_verified);

    auto point=project_native_code2_point_35798(
        s,0x20000U,0x8000U,777);
    assert(point.accepted);
    assert(point.source_depth==0xe000U);
    assert(point.transverse==0x4000U);
    assert(point.original_screen_x==173);
    assert(point.original_projected_height==292);
    assert(!point.original_dos_frame_parity_verified);
    auto culled=project_native_code2_point_35798(
        s,0xefffU,0,777);
    assert(!culled.accepted);
    assert(culled.original_screen_x==777);
    assert(culled.original_projected_height==0);
    auto edge=project_native_code2_point_35798(
        s,0xf000U,0,777);
    assert(edge.accepted);
    assert(edge.source_depth==0x5800U);
    assert(edge.original_screen_x==100);
    assert(edge.original_projected_height==744);

    s.live_x_255f0=0; // depth below 0x1680 clamps in original 35730
    wall=project_native_code2_wall_height_35730(s);
    assert(wall.source_depth==0);
    assert(wall.clamped_depth==0x1680U);
    assert(wall.unsigned_divisor==0x16);
    assert(wall.clamped_to_original_near_limit);
    assert(wall.original_return_word==static_cast<std::uint16_t>(65536U/22U));
    s.height_numerator_255e0=0xffffffffU;
    wall=project_native_code2_wall_height_35730(s);
    assert(wall.original_return_word==
           static_cast<std::uint16_t>(0xffffffffU/22U));
    // 35798 uses SIGNED IDIV, and overflow is not a valid framebuffer.
    s.direction_y_255fc=0x8000U;
    s.horizontal_scale_c6cc=0x7fffffffU;
    s.height_numerator_255e0=0x10000U;
    rejects([&]{static_cast<void>(project_native_code2_point_35798(
        s,0x20000U,0x1000U,777));});
    // Deterministic 4,096 diagnostic input sets; the dedicated native
    // auditor will export digestable details for Python independent check.
    std::uint32_t seed=0xC2A35730U;
    for(unsigned i=0;i<4096;++i){
        auto next=[&](){seed=1664525U*seed+1013904223U;return seed;};
        s.origin_x_255ec=next()&0x1ffffU;
        s.origin_y_255f4=next()&0x1ffffU;
        s.live_x_255f0=s.origin_x_255ec+(next()&0x7ffffU);
        s.live_y_255e8=s.origin_y_255f4+(next()&0x7ffffU);
        s.direction_x_255f8=(next()&0x7fffU);
        s.direction_y_255fc=(next()&0x7fffU);
        s.height_numerator_255e0=0x10000U+(next()&0x7ffffU);
        s.horizontal_scale_c6cc=256U+(next()&0x3ffU);
        s.horizontal_origin_c6d0=static_cast<std::uint16_t>(next());
        const auto w=project_native_code2_wall_height_35730(s);
        assert(w.unsigned_divisor>=22U);
        const auto x=s.origin_x_255ec+(next()&0x7ffffU);
        const auto y=s.origin_y_255f4+(next()&0x7ffffU);
        const auto p=project_native_code2_point_35798(s,x,y,1234U);
        if(!p.accepted){
            assert(p.original_projected_height==0);
            assert(p.original_screen_x==1234);
        }
    }
}
}
int main(){run();}
