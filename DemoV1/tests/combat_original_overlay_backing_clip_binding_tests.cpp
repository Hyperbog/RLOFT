#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_overlay_backing_clip_binding.hpp"

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

int main() {
    using namespace rloft;
    CombatOriginalCompositeState composite{};
    composite.enabled_85fc=1U;
    composite.overlay_reference_count_9e37=0;
    composite.active_page_9e39=0U;
    CombatOriginalOverlayGlobals g{composite};
    g.overlay_mode_a6c0=0U;
    CombatOriginalOverlayClipLimits bounds{400,320};
    std::vector<std::string> trace;

    CombatOriginalOverlayBackingServices raster{};
    raster.capture_2a114=[&](const auto& args) {
        trace.emplace_back("2A114");
        assert(args.backing_pointer==0x9E3AU);
        assert(args.row_bits==0U&&args.column_bits==0U);
        assert(args.row_count_bits==29U&&args.column_count_bits==14U);
    };
    raster.restore_2a178=[&](const auto& args) {
        trace.emplace_back("2A178");
        assert(args.backing_pointer==0x9E3AU);
        if(args.row_count_bits==29U) {
            assert(args.row_bits==0U&&args.column_bits==0U);
            assert(args.column_count_bits==14U);
        } else {
            assert(args.row_count_bits==2U);
            assert(args.column_count_bits==0U);
        }
    };

    CombatOriginalOverlayServices services{};
    services.push_page_2e590=[&](std::uint8_t page) {
        trace.emplace_back("push");
        assert(page==0U);
    };
    services.pop_page_2e5d4=[&]() {trace.emplace_back("pop");};
    services.position_1a3b8=[&](CombatOriginalOverlayGlobals& overlay) {
        trace.emplace_back("position");
        overlay.composite.overlay_row_9e31=-3;
        overlay.composite.overlay_column_9e33=-2;
    };
    services.probe_2ae30=[&](std::int32_t row,std::int32_t col) {
        trace.emplace_back("probe");
        assert(row==-3&&col==-2);
        return 0;
    };
    auto bound=CombatOriginalOverlayBackingClipBinder::bind(
        bounds,raster,services);
    CombatOriginalOverlayModel::suspend_1a288(g,bound);
    assert(composite.overlay_reference_count_9e37==1);
    CombatOriginalOverlayModel::restore_1ab20(g,-3,-2,bound);
    assert(composite.overlay_reference_count_9e37==0);
    assert((trace==std::vector<std::string>{
        "push","position","2A114","probe","pop",
        "push","2A178","pop"
    }));

    // Verify original restored zero-column branch executes 2A178
    // and the corresponding capture branch would NOT invoke 2A114.
    trace.clear();
    bound.capture_292bc(0x9E3AU,20,320,2U,0U);
    assert(trace.empty());
    bound.restore_2932c(0x9E3AU,20,320,2U,0U);
    assert((trace==std::vector<std::string>{"2A178"}));

    // Live EF3F/EF41 may change between original callbacks.
    // A later viewport clamp must be read at the CALL site,
    // not frozen into the adapter when it was constructed.
    bounds.last_row_ef3f=10;
    bounds.last_column_ef41=9;
    trace.clear();
    bound.capture_292bc(0x9E3AU,20,20,2U,2U);
    bound.restore_2932c(0x9E3AU,20,20,2U,2U);
    assert(trace.empty());
}
