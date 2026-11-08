#ifdef NDEBUG
#undef NDEBUG
#endif

#include "rloft/original_ground_surface_dispatch.hpp"

#include <cassert>
#include <cstdint>
#include <limits>

int main() {
    using namespace rloft;
    using Model=OriginalGroundSurfaceDispatch;
    using UA=OriginalUpperSurfaceAction;
    using LA=OriginalLowerSurfaceAction;

    OriginalUpperSurfaceInput base{};
    base.original_nearest_22220=0x800;
    base.original_c738_scale_count=127U; // source SINTAB C738
    base.original_heading_c658=90U;
    base.original_last_ff58_heading_bff0=0U;
    base.original_c026=1U;
    base.original_c027=0U;
    base.original_c028=0U;
    base.original_selected_258a8=0xFFU;
    base.original_kind_258b0=0x12U;

    auto upper=Model::plan_upper(base);
    assert(upper && upper->action==UA::Fill33E28);
    assert(upper->recompute_original_34998);
    assert(upper->initial_upper_ff50==128);
    assert(upper->original_ebx_15618==0U);
    auto lower=Model::plan_lower(*upper,upper->initial_upper_ff50,
                                 base.original_c026,0x37U);
    assert(lower.action==LA::Textured34A74);
    assert(lower.computed_lower_ff50==128);
    assert(lower.original_flat_fill_kind_258b1==0x37U);

    // Source zero modes skip 34998 regardless of heading change,
    // fill upper by 33E28 and lower by 33DE0. FF50 is NOT rewritten
    // in this branch, even if the upper callback changed it.
    base.original_c026=0U;
    upper=Model::plan_upper(base);
    assert(upper && !upper->recompute_original_34998);
    assert(upper->action==UA::Fill33E28);
    lower=Model::plan_lower(*upper,17,base.original_c026,42U);
    assert(lower.action==LA::Fill33DE0);
    assert(lower.computed_lower_ff50==17);
    assert(lower.original_flat_fill_kind_258b1==42U);

    base.original_c027=1U;
    upper=Model::plan_upper(base);
    assert(upper && upper->recompute_original_34998);
    assert(upper->action==UA::Textured34A74);
    lower=Model::plan_lower(*upper,128,base.original_c026,0xEE);
    assert(lower.action==LA::Fill33DE0);
    // 1F239 tests actual heading equality.
    base.original_last_ff58_heading_bff0=90U;
    upper=Model::plan_upper(base);
    assert(upper && !upper->recompute_original_34998);

    // 258A8 active overrides the C027 textured-upper path.
    // Kind 0x2C selects the 29B40 overlay with original palette.
    base.original_selected_258a8=3U;
    base.original_kind_258b0=0x2CU;
    upper=Model::plan_upper(base);
    assert(upper && upper->action==UA::Overlay29B40);
    base.original_kind_258b0=1U;
    base.original_c028=1U;
    upper=Model::plan_upper(base);
    assert(upper && upper->action==UA::Overlay314B0);
    base.original_c028=0U;
    upper=Model::plan_upper(base);
    assert(upper && upper->action==UA::Fill33E28);
    // The original active 258A8 branch runs even when C027 is zero.
    base.original_c027=0U;
    base.original_kind_258b0=0x2CU;
    upper=Model::plan_upper(base);
    assert(upper && upper->action==UA::Overlay29B40);

    // Source active==FF and C027!=0: if upper FF50<=0, the
    // original branch jumps to 1F369 rather than drawing 33E28.
    base.original_selected_258a8=0xFFU;
    base.original_c027=1U;
    base.original_nearest_22220=127U*8;
    upper=Model::plan_upper(base);
    assert(upper && upper->initial_upper_ff50==-63);
    assert(upper->action==UA::SkipNonpositiveFF50);
    base.original_nearest_22220=0U;
    upper=Model::plan_upper(base);
    assert(upper && upper->action==UA::Textured34A74);

    // No-upper-call branch: 1F30E JLE 1F369 executes neither
    // VGA fill, 34A74 nor selected-actor overlays. Therefore the
    // exact post-upper FF50 is source-proven from the initial stage.
    OriginalUpperSurfaceInput skipped_mode=base;
    skipped_mode.original_selected_258a8=0xFFU;
    skipped_mode.original_c027=1U;
    skipped_mode.original_c026=1U;
    skipped_mode.original_nearest_22220=127*8;
    const auto skipped=Model::plan_upper(skipped_mode);
    assert(skipped && skipped->action==UA::SkipNonpositiveFF50);
    assert(skipped->initial_upper_ff50==-63);
    const auto proved_lower=Model::plan_lower_after_skipped_upper(
        *skipped,1U,0x21U);
    assert(proved_lower);
    assert(proved_lower->post_upper_ff50==-63);
    assert(proved_lower->computed_lower_ff50==-189);
    assert(proved_lower->action==LA::SkipNonpositiveFF50);
    assert(proved_lower->original_flat_fill_kind_258b1==0x21U);
    const auto no_lower_raster=Model::plan_lower_after_skipped_upper(
        *skipped,0U,0x33U);
    assert(no_lower_raster);
    assert(no_lower_raster->action==LA::Fill33DE0);
    assert(no_lower_raster->post_upper_ff50==-63);
    assert(no_lower_raster->computed_lower_ff50==-63);

    // No post-call FF50 can be invented for a path with an upper
    // draw. 33E28 has ES:EDI writes whose video-selector alias is
    // not accepted; reject it even though it has no direct FF50
    // instruction or nested call. Textured/overlay paths likewise
    // require an observed post-upper FF50 before 1F377.
    skipped_mode.original_c027=0U;
    auto unknown_upper=Model::plan_upper(skipped_mode);
    assert(unknown_upper && unknown_upper->action==UA::Fill33E28);
    assert(!Model::plan_lower_after_skipped_upper(
        *unknown_upper,1U,0U));
    skipped_mode.original_c027=1U;
    skipped_mode.original_nearest_22220=408;
    unknown_upper=Model::plan_upper(skipped_mode);
    assert(unknown_upper && unknown_upper->action==UA::Textured34A74);
    assert(!Model::plan_lower_after_skipped_upper(
        *unknown_upper,1U,0U));
    skipped_mode.original_selected_258a8=2U;
    skipped_mode.original_kind_258b0=0x2CU;
    unknown_upper=Model::plan_upper(skipped_mode);
    assert(unknown_upper && unknown_upper->action==UA::Overlay29B40);
    assert(!Model::plan_lower_after_skipped_upper(
        *unknown_upper,0U,0U)); // even when C026 skips floor texture
    skipped_mode.original_kind_258b0=1U;
    skipped_mode.original_c028=1U;
    unknown_upper=Model::plan_upper(skipped_mode);
    assert(unknown_upper && unknown_upper->action==UA::Overlay314B0);
    assert(!Model::plan_lower_after_skipped_upper(
        *unknown_upper,1U,0U));

    // Explicitly test the old assumption that FF50 cannot be
    // updated while upper 34A74/overlays run: only plan_lower()
    // with the actual supplied callback-result value is valid.
    auto mutable_upper=*unknown_upper;
    mutable_upper.action=UA::Fill33E28;
    assert(!Model::plan_lower_after_skipped_upper(
        mutable_upper,1U,0U));

    // Critical source order: 1F377 reads global FF50 again AFTER
    // 29B40/314B0/33E28/34A74 and any opaque callback writes.
    base.original_nearest_22220=16;
    base.original_c026=1U;
    upper=Model::plan_upper(base);
    assert(upper && upper->original_ebx_15618==8U);
    assert(upper->initial_upper_ff50==125);
    lower=Model::plan_lower(*upper,upper->initial_upper_ff50,1U,0U);
    assert(lower.action==LA::Textured34A74);
    assert(lower.computed_lower_ff50==123);
    lower=Model::plan_lower(*upper,140,1U,0U); // mutated original FF50
    assert(lower.action==LA::Textured34A74);
    assert(lower.computed_lower_ff50==108);
    lower=Model::plan_lower(*upper,248,1U,0U);
    assert(lower.action==LA::SkipNonpositiveFF50);
    assert(lower.computed_lower_ff50==0);

    // x86 32-bit wrap, followed by signed TEST/JLE, never host UB.
    lower=Model::plan_lower(*upper,std::numeric_limits<std::int32_t>::min(),
                             1U,0U);
    assert(lower.computed_lower_ff50==
           std::numeric_limits<std::int32_t>::min()+248);
    assert(lower.action==LA::SkipNonpositiveFF50);
    assert(!Model::plan_upper(OriginalUpperSurfaceInput{
        0x7FFFFFFF,127U,0U,0U,1U,1U,0U,0xFFU,0U}));

    // Stage's first C026/C027 decisions do NOT change future
    // callback-independent render: no 34A74 invoked by this test.
    constexpr OriginalUpperSurfaceInput simple{
        0x800,127U,0U,0U,1U,0U,0U,0xFFU,0U};
    constexpr auto constant_plan=Model::plan_upper(simple);
    static_assert(constant_plan.has_value(),"original source mode");
    static_assert(constant_plan->action==UA::Fill33E28,
                  "original C027=0 upper fallback");
    constexpr auto constant_lower=Model::plan_lower(
        *constant_plan,128,1U,0U);
    static_assert(constant_lower.action==LA::Textured34A74,
                  "original C026=1 lower texture");
    // Same constexpr source guard: 33E28 is deliberately not an
    // accepted no-call/no-write stage until VGA ES alias is proven.
    static_assert(!Model::plan_lower_after_skipped_upper(
        *constant_plan,1U,0U).has_value(),
        "original filled upper stage still needs live post-draw FF50");
}
