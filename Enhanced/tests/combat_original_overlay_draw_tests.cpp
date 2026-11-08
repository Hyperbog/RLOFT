#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_overlay_draw.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    using namespace rloft;
    CombatOriginalOverlayDrawState state{};
    CombatOriginalOverlayDrawServices callbacks{};
    std::vector<std::uint32_t> original_records;
    callbacks.draw_29414=[&](
        std::int32_t mode,std::int32_t row,std::int32_t column,
        std::uint32_t resource,std::uint32_t scratch_record) {
        assert(mode==2&&row==-3&&column==34);
        assert(resource==0x880DU);
        assert(state.nested_record_depth_efd4>=1U);
        original_records.push_back(scratch_record);
        if(state.nested_record_depth_efd4==1U) {
            CombatOriginalOverlayDrawModel::draw_29804(
                state,mode,row,column,resource,callbacks);
        }
    };
    CombatOriginalOverlayDrawModel::draw_29804(
        state,2,-3,34,0x880DU,callbacks);
    assert((original_records==std::vector<std::uint32_t>{
        0xEF74U,0xEF8CU}));
    assert(state.nested_record_depth_efd4==0U);

    // Original IMUL/ADD/INC/DEC wraps full 32-bit EFD4.
    state.nested_record_depth_efd4=0xFFFFFFFFU;
    callbacks.draw_29414=[&](
        std::int32_t,std::int32_t,std::int32_t,
        std::uint32_t,std::uint32_t record) {
        assert(record==0xEF5CU);
        assert(state.nested_record_depth_efd4==0U);
    };
    CombatOriginalOverlayDrawModel::draw_29804(
        state,0,0,0,0x8603U,callbacks);
    assert(state.nested_record_depth_efd4==0xFFFFFFFFU);

    // Original 2982E decrements the LIVE EFD4 value modified by
    // 29414, rather than restoring a saved pre-call snapshot.
    state.nested_record_depth_efd4=3U;
    callbacks.draw_29414=[&](
        std::int32_t,std::int32_t,std::int32_t,
        std::uint32_t,std::uint32_t record) {
        assert(record==0xEFBCU);
        assert(state.nested_record_depth_efd4==4U);
        state.nested_record_depth_efd4=9U;
    };
    CombatOriginalOverlayDrawModel::draw_29804(
        state,4,88,99,0x880DU,callbacks);
    assert(state.nested_record_depth_efd4==8U);

    // Bridge the existing 1A288 conditional-drawing branch to
    // real 29804 record management, leaving source sprite
    // rasterization in the lower-level 29414 service.
    state.nested_record_depth_efd4=0U;
    CombatOriginalCompositeState composite{};
    composite.enabled_85fc=1U;
    composite.overlay_reference_count_9e37=0;
    composite.overlay_row_9e31=-1;
    composite.overlay_column_9e33=25;
    CombatOriginalOverlayGlobals overlay{composite};
    overlay.overlay_mode_a6c0=1U;
    CombatOriginalOverlayServices remaining{};
    remaining.push_page_2e590=[](std::uint8_t){};
    remaining.pop_page_2e5d4=[](){};
    remaining.position_1a3b8=[](CombatOriginalOverlayGlobals&){};
    remaining.capture_292bc=[](
        std::uint32_t,std::int32_t,std::int32_t,
        std::uint32_t,std::uint32_t){};
    remaining.probe_2ae30=[](
        std::int32_t,std::int32_t){return 2;};
    callbacks.draw_29414=[&](
        std::int32_t mode,std::int32_t row,std::int32_t col,
        std::uint32_t sprite,std::uint32_t record){
        assert(mode==1&&row==-1&&col==25);
        assert(sprite==0x880DU);
        assert(record==0xEF74U);
        assert(state.nested_record_depth_efd4==1U);
        original_records.push_back(record);
    };
    auto bound=CombatOriginalOverlayDrawModel::bind_1a288(
        state,callbacks,remaining);
    assert(bound.draw_29804&&bound.capture_292bc);
    CombatOriginalOverlayModel::suspend_1a288(overlay,bound);
    assert(composite.overlay_reference_count_9e37==1);
    assert(state.nested_record_depth_efd4==0U);
    assert(original_records.size()==3U);

    // Missing 29414 must not increment the original nested depth.
    bool failed=false;
    try {
        CombatOriginalOverlayDrawModel::draw_29804(
            state,1,0,0,0U,CombatOriginalOverlayDrawServices{});
    }catch(const std::invalid_argument&){failed=true;}
    assert(failed&&state.nested_record_depth_efd4==0U);
}
