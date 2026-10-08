#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_composite.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    using namespace rloft;
    using Route=CombatOriginalCompositeRoute;
    using Model=CombatOriginalCompositeModel;
    CombatOriginalCompositeState state{};
    state.enabled_85fc=1U;
    state.overlay_reference_count_9e37=1;
    state.overlay_left_9e31=100;
    state.overlay_top_9e33=200;
    state.active_page_9e39=7U;
    state.redraw_busy_a6ba=9U;
    CombatOriginalCompositeRect rect{1U,120U,210U,12U,8U};
    assert(Model::inspect_1a8c8(state,rect)==
           Route::BufferedOverlayOverlap);

    std::vector<std::string> events;
    CombatOriginalCompositeServices callback{};
    callback.copy_rect_2a078=[&](
        std::uint32_t page,std::uint32_t x1,std::uint32_t y1,
        std::uint32_t x2,std::uint32_t y2,
        std::uint32_t width,std::uint32_t height) {
        events.emplace_back("2A078");
        assert(page==(rect.page^1U));
        assert(x1==rect.left && x2==rect.left);
        assert(y1==rect.top && y2==rect.top);
        assert(width==rect.width && height==rect.height);
        assert(state.redraw_busy_a6ba==1U ||
               state.redraw_busy_a6ba==9U);
    };
    callback.suspend_overlay_1a288=[&](){
        events.emplace_back("1A288");
        assert(state.overlay_reference_count_9e37==0);
        assert(state.active_page_9e39==1U);
        // Nested original services may modify live coordinates. The
        // outer function retains its saved EBP/EDI position arguments.
        state.overlay_left_9e31=-30;
        state.overlay_top_9e33=5;
    };
    callback.move_overlay_bytes_30cd4=[&](
        std::uint32_t dst,std::uint32_t src,std::uint32_t n) {
        events.emplace_back("30CD4");
        assert(dst==0x9E3AU && src==0xA23AU && n==0x400U);
        assert(state.active_page_9e39==1U);
    };
    callback.restore_overlay_1ab20=[&](std::int32_t x,std::int32_t y) {
        events.emplace_back("1AB20");
        assert(x==100 && y==200);
        assert(state.overlay_reference_count_9e37==0);
    };
    assert(Model::run_1a8c8(state,rect,callback)==
           Route::BufferedOverlayOverlap);
    assert((events==std::vector<std::string>{
        "1A288","2A078","30CD4","1AB20"}));
    assert(state.overlay_reference_count_9e37==1);
    assert(state.active_page_9e39==7U);
    assert(state.redraw_busy_a6ba==0U);
    assert(state.overlay_left_9e31==-30); // callback's change persists

    // No overlay state: direct call and NO synthetic writes to A6BA.
    events.clear();
    state.enabled_85fc=0U;
    state.redraw_busy_a6ba=9U;
    assert(Model::run_1a8c8(state,rect,callback)==
           Route::DirectWithoutOverlay);
    assert((events==std::vector<std::string>{"2A078"}));
    assert(state.redraw_busy_a6ba==9U);
    state.enabled_85fc=1U;
    state.overlay_reference_count_9e37=-1;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::DirectWithoutOverlay);
    state.overlay_reference_count_9e37=0;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::DirectWithoutOverlay);

    // Active overlay, no overlap: set A6BA before the rectangular
    // copy and CLEAR after. Never suspend, move, or restore.
    state.overlay_reference_count_9e37=1;
    state.overlay_left_9e31=100;
    state.overlay_top_9e33=200;
    rect.left=133U;
    events.clear();
    assert(Model::run_1a8c8(state,rect,callback)==
           Route::DirectOutsideOverlay);
    assert((events==std::vector<std::string>{"2A078"}));
    assert(state.redraw_busy_a6ba==0U);
    assert(state.overlay_reference_count_9e37==1);

    // Original inclusive dimensions 32x16. Touching either
    // high or low boundary triggers the buffered path.
    rect.left=132U;
    rect.top=216U;
    rect.width=0U;
    rect.height=0U;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::BufferedOverlayOverlap);
    rect.left=133U;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::DirectOutsideOverlay);
    rect.left=90U;
    rect.width=10U; // rectangle right = 100 exactly
    assert(Model::inspect_1a8c8(state,rect)==
           Route::BufferedOverlayOverlap);
    rect.width=9U;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::DirectOutsideOverlay);
    rect.left=120U;
    rect.width=5U;
    rect.top=184U;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::BufferedOverlayOverlap);
    rect.top=183U;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::DirectOutsideOverlay);

    // CPU signed compare and 32-bit ADD wrap: do not use a
    // modern unsigned rectangle intersection algorithm.
    state.overlay_left_9e31=0;
    state.overlay_top_9e33=0;
    rect={0U,0x7FFFFFFFU,0U,0x80000002U,0U};
    assert(Model::inspect_1a8c8(state,rect)==
           Route::DirectOutsideOverlay);
    rect.left=0xFFFFFFFEU;
    rect.width=4U;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::BufferedOverlayOverlap);

    // Unsupported services fail before any wrapper-side mutation.
    state.redraw_busy_a6ba=5U;
    rect={1U,0U,0U,2U,2U};
    bool unsupported=false;
    try {
        CombatOriginalCompositeServices only_copy{};
        only_copy.copy_rect_2a078=callback.copy_rect_2a078;
        (void)Model::run_1a8c8(state,rect,only_copy);
    } catch(const std::invalid_argument&) {unsupported=true;}
    assert(unsupported && state.redraw_busy_a6ba==5U);

    // No-overlap copy sends ALL high page bits to 2A078 except
    // its low toggle bit; no mode truncation in the direct call.
    state.enabled_85fc=0U;
    rect.page=0x12345670U;
    CombatOriginalCompositeServices check_high{};
    check_high.copy_rect_2a078=[&](
        std::uint32_t page,std::uint32_t,std::uint32_t,
        std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t) {
        assert(page==0x12345671U);
    };
    assert(Model::run_1a8c8(state,rect,check_high)==
           Route::DirectWithoutOverlay);
}
