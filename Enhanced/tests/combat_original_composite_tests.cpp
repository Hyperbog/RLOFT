#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_composite.hpp"
#include "rloft/combat_irq_display_runtime.hpp"

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
    state.overlay_row_9e31=100;
    state.overlay_column_9e33=200;
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
        assert(x1==rect.row && x2==rect.row);
        assert(y1==rect.column && y2==rect.column);
        assert(width==rect.rows && height==rect.columns);
        assert(state.redraw_busy_a6ba==1U ||
               state.redraw_busy_a6ba==9U);
    };
    callback.suspend_overlay_1a288=[&](){
        events.emplace_back("1A288");
        assert(state.overlay_reference_count_9e37==0);
        assert(state.active_page_9e39==1U);
        // Nested original services may modify live coordinates. The
        // outer function retains its saved EBP/EDI position arguments.
        state.overlay_row_9e31=-30;
        state.overlay_column_9e33=5;
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
    assert(state.overlay_row_9e31==-30); // callback's change persists

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
    state.overlay_row_9e31=100;
    state.overlay_column_9e33=200;
    rect.row=133U;
    events.clear();
    assert(Model::run_1a8c8(state,rect,callback)==
           Route::DirectOutsideOverlay);
    assert((events==std::vector<std::string>{"2A078"}));
    assert(state.redraw_busy_a6ba==0U);
    assert(state.overlay_reference_count_9e37==1);

    // Original inclusive dimensions 32x16. Touching either
    // high or low boundary triggers the buffered path.
    rect.row=132U;
    rect.column=216U;
    rect.rows=0U;
    rect.columns=0U;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::BufferedOverlayOverlap);
    rect.row=133U;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::DirectOutsideOverlay);
    rect.row=90U;
    rect.rows=10U; // rectangle right = 100 exactly
    assert(Model::inspect_1a8c8(state,rect)==
           Route::BufferedOverlayOverlap);
    rect.rows=9U;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::DirectOutsideOverlay);
    rect.row=120U;
    rect.rows=5U;
    rect.column=184U;
    rect.columns=16U; // 184+16 meets the overlay's inclusive left edge (200).
    assert(Model::inspect_1a8c8(state,rect)==
           Route::BufferedOverlayOverlap);
    rect.column=183U;
    assert(Model::inspect_1a8c8(state,rect)==
           Route::DirectOutsideOverlay);

    // CPU signed compare and 32-bit ADD wrap: do not use a
    // modern unsigned rectangle intersection algorithm.
    state.overlay_row_9e31=0;
    state.overlay_column_9e33=0;
    rect={0U,0x7FFFFFFFU,0U,0x80000002U,0U};
    assert(Model::inspect_1a8c8(state,rect)==
           Route::DirectOutsideOverlay);
    rect.row=0xFFFFFFFEU;
    rect.rows=4U;
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

    // Actual shared original call chain: 1C258 presents a pending
    // message, calls 1A8C8 with (page,0x106,0x20,0x0C,0x100),
    // and only THEN 1C2C4 reads 30700 for deadline comparison.
    CombatOriginalCompositeState overlay{};
    overlay.enabled_85fc=0U;
    overlay.redraw_busy_a6ba=0x1234U;
    std::vector<std::string> integrated;
    CombatOriginalCompositeServices original{};
    original.copy_rect_2a078=[&](
        std::uint32_t page,std::uint32_t x1,std::uint32_t y1,
        std::uint32_t x2,std::uint32_t y2,
        std::uint32_t width,std::uint32_t height) {
        integrated.emplace_back("2A078");
        assert(page==1U && x1==0x106U && x2==0x106U);
        assert(y1==0x20U && y2==0x20U);
        assert(width==0x0CU && height==0x100U);
    };
    CombatIrqDisplayRuntime display{};
    display.active_render_mode_c050=0U;
    display.pending_cf59=1U;
    display.deadline_cf54=500U;
    display.queued_message_14580={'X',0U};
    CombatIrqDisplayServices renderer{};
    renderer.select_vga_page_291b0=[&](std::uint8_t) {
        integrated.emplace_back("291B0");
    };
    renderer.draw_background_2939c=[&](
        std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t) {
        integrated.emplace_back("2939C");
    };
    renderer.draw_text_3060c=[&](
        std::uint32_t,std::uint32_t,
        const std::vector<std::uint8_t>&,std::uint32_t) {
        integrated.emplace_back("3060C");
    };
    renderer.composite_1a8c8=Model::bind_irq_display_1a8c8(
        overlay,original);
    renderer.observed_irq_tick_30700=[&](){
        integrated.emplace_back("30700");
        assert(display.pending_cf59==0U);
        return 500U;
    };
    CombatIrqDisplayRuntimeModel::update(display,renderer);
    assert((integrated==std::vector<std::string>{
        "291B0","2939C","3060C","2A078","30700"
    }));
    assert(display.deadline_cf54==500U &&
           display.pending_cf59==0U);
    assert(overlay.redraw_busy_a6ba==0x1234U);
}
