#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_ground_ff50_source_span.hpp"

#include <cassert>
#include <cstdint>

int main() {
    using Span=rloft::OriginalGroundFF50Span;
    using Model=rloft::OriginalGroundFF50SourceModel;
    // Original supplied SINTAB C738=127. The generated source table
    // 15618 has entries [0..127], not SINTAB lookup_max=508.
    constexpr auto original_c738=127U;
    constexpr auto open=Model::from_22220(0x800,original_c738);
    static_assert(open.has_value(),"source nearest sentinel");
    static_assert(open->sentinel_open_field,"original nearest 0x800");
    static_assert(open->source_15618_value==0U,"original EBX zero");
    static_assert(open->upper_ff50==128 && open->lower_ff50==128,
                  "full open-field original FF50 two halves");
    static_assert(open->lower_four_plane_rows()==80U &&
                  open->lower_paired_plane_rows()==48U,
                  "34A74 source 80/48 split");

    for(std::uint32_t i=0U;i<=original_c738;++i) {
        // CODE.2 1F261 signed /8 is in-range here and the original
        // source 03A20 table stores the DWORD offset 4*i.
        const auto plan=Model::from_22220(
            static_cast<std::int32_t>(i*8U),original_c738);
        assert(plan);
        assert(!plan->sentinel_open_field);
        assert(plan->source_15618_value==4U*i);
        assert(plan->upper_ff50==128-static_cast<int>(2U*i)+
                                  static_cast<int>(i/2U));
        assert(plan->lower_ff50==128-static_cast<int>(2U*i)-
                                  static_cast<int>(i/2U));
        const auto next=Model::from_22220(
            static_cast<std::int32_t>(i*8U+7U),original_c738);
        assert(next && next->source_15618_value==plan->source_15618_value);
        assert(next->upper_ff50==plan->upper_ff50);
        assert(next->lower_ff50==plan->lower_ff50);
        const auto expected_rows=plan->lower_ff50<=0 ? 0U :
            static_cast<std::uint32_t>(plan->lower_ff50);
        assert(plan->lower_four_plane_rows()+
               plan->lower_paired_plane_rows()==expected_rows);
    }

    const auto first=Model::from_22220(8,original_c738);
    assert(first && first->upper_ff50==126 &&
           first->lower_ff50==126);
    assert(first->lower_four_plane_rows()==80U);
    assert(first->lower_paired_plane_rows()==46U);
    const auto second=Model::from_22220(16,original_c738);
    assert(second && second->upper_ff50==125 &&
           second->lower_ff50==123);
    assert(second->lower_paired_plane_rows()==43U);
    const auto near_visible=Model::from_22220(51*8,original_c738);
    assert(near_visible && near_visible->lower_ff50==1 &&
           near_visible->lower_four_plane_rows()==1U &&
           near_visible->lower_paired_plane_rows()==0U);
    const auto blocked=Model::from_22220(52*8,original_c738);
    assert(blocked && blocked->lower_ff50==-2 &&
           !blocked->lower_has_textured_rows() &&
           blocked->lower_four_plane_rows()==0U);

    // Source DGroup lookup is only initialized for [0..C738].
    // Unproven table memory and impossible nearest-wall states fail
    // closed instead of interpolating or inventing a screen-space cut.
    assert(!Model::from_22220(-1,original_c738));
    assert(!Model::from_22220(0x801,original_c738));
    assert(!Model::from_22220(128*8,original_c738));
    assert(!Model::from_22220(8,0U));
    const auto zero_slot=Model::from_22220(0,0U);
    assert(zero_slot && zero_slot->upper_ff50==128 &&
           zero_slot->lower_ff50==128);
    const auto zero_count_sentinel=Model::from_22220(0x800,0U);
    assert(zero_count_sentinel && zero_count_sentinel->lower_ff50==128);

    // Native contract accepts the raw depth and original count only.
    // No host frame refresh, cursor, RNG or VGA drawing is performed.
    (void)sizeof(Span);
}
