#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_vga_copy_binding.hpp"
#include "rloft/combat_irq_display_runtime.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
rloft::CombatOriginalVgaCopyState video() {
    rloft::CombatOriginalVgaCopyState s{};
    s.selected_page_efe9=0U;
    s.linear_page_base_f001=0xA0000U;
    s.page_stride_f005=0x800U;
    s.row_pitch_eff9=80U;
    s.row_shift_efed=4U;
    s.row_shift_eff1=2U;
    return s;
}
} // namespace

int main() {
    using namespace rloft;
    VgaPlanarMemory vga{};
    auto video_state=video();
    CombatOriginalCompositeState overlay{};
    overlay.enabled_85fc=0U;
    overlay.redraw_busy_a6ba=0x1234U;
    const auto services=CombatOriginalVgaCopyBinder::bind_direct_copy(
        vga,video_state);

    // 1A8C8 receives page=0, invokes 2A078(page^1=1).
    // Actual original source page is live EFE9=0.
    vga.clear(0xCCU);
    for(std::uint8_t plane=0U;plane<4U;++plane)
        for(std::uint32_t row=0U;row<2U;++row)
            for(std::uint32_t byte=0U;byte<2U;++byte)
                vga.write(
                    0xA0000U+(row+1U)*80U+byte,
                    static_cast<std::uint8_t>(1U<<plane),
                    static_cast<std::uint8_t>(
                        plane*40U+row*2U+byte+1U));
    const CombatOriginalCompositeRect region{
        0U,1U,3U,2U,5U};
    assert(CombatOriginalCompositeModel::run_1a8c8(
        overlay,region,services)==
        CombatOriginalCompositeRoute::DirectWithoutOverlay);
    for(std::uint8_t plane=0U;plane<4U;++plane)
        for(std::uint32_t row=0U;row<2U;++row)
            for(std::uint32_t byte=0U;byte<2U;++byte)
                assert(vga.read(
                    0xA8000U+(row+1U)*80U+byte,plane)==
                    static_cast<std::uint8_t>(
                        plane*40U+row*2U+byte+1U));
    assert(vga.read(0xA8000U+80U+2U,0U)==0xCCU);
    assert(video_state.selected_page_efe9==0U);
    assert(video_state.linear_page_base_f001==0xA0000U);
    assert(video_state.graphics_mode_index05 &&
           *video_state.graphics_mode_index05==0x40U);
    assert(vga.sequencer_map_mask_02() &&
           *vga.sequencer_map_mask_02()==0x0FU);
    assert(overlay.redraw_busy_a6ba==0x1234U);

    // The same direct compositor bound to 1C258: the actual
    // 2A078 plane bytes are visible BEFORE 1C2C4 reads IRQ.
    video_state=video();
    vga.clear(0x0FU);
    for(std::uint8_t p=0U;p<4U;++p)
        vga.write(0xA0000U+262U*80U+8U,
            static_cast<std::uint8_t>(1U<<p),
            static_cast<std::uint8_t>(0x20U+p));
    CombatIrqDisplayRuntime timer{};
    timer.pending_cf59=1U;
    timer.deadline_cf54=500U;
    timer.queued_message_14580={'L',0U};
    timer.active_render_mode_c050=0U;
    timer.background_resource_2222c=0x3333U;
    std::vector<std::uint8_t> stages;
    CombatIrqDisplayServices display{};
    display.select_vga_page_291b0=[&](std::uint8_t){stages.push_back(1U);};
    display.draw_background_2939c=[&](
        std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t){
        stages.push_back(2U);
    };
    display.draw_text_3060c=[&](
        std::uint32_t,std::uint32_t,const std::vector<std::uint8_t>&,
        std::uint32_t){stages.push_back(3U);};
    display.composite_1a8c8=
        CombatOriginalCompositeModel::bind_irq_display_1a8c8(
            overlay,services);
    display.observed_irq_tick_30700=[&]() {
        stages.push_back(4U);
        // 1C258's composite uses row262,column32,rows12,
        // columns256 -> 64 VGA bytes/row, all four planes.
        for(std::uint8_t p=0U;p<4U;++p)
            assert(vga.read(0xA8000U+262U*80U+8U,p)==0x20U+p);
        assert(timer.pending_cf59==0U);
        return 500U;
    };
    CombatIrqDisplayRuntimeModel::update(timer,display);
    assert((stages==std::vector<std::uint8_t>{1U,2U,3U,4U}));
    assert(timer.pending_cf59==0U && timer.deadline_cf54==500U);

    // Buffered-overlap path remains unsupported until the real
    // 1A288/30CD4/1AB20 callbacks are connected. It must never
    // silently perform a direct pixel copy instead.
    overlay.enabled_85fc=1U;
    overlay.overlay_reference_count_9e37=1;
    overlay.overlay_row_9e31=1;
    overlay.overlay_column_9e33=3;
    bool rejected=false;
    try {
        (void)CombatOriginalCompositeModel::run_1a8c8(
            overlay,region,services);
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);

    // With authentic DGROUP capture and explicitly injected original
    // overlay collaborators, the buffered path also moves the exact
    // 0x400-byte backing page AFTER its VGA copy and BEFORE restore.
    CombatOriginalDgroupCapture dgroup{};
    dgroup.captured_base_dgroup_offset=0x9E3AU;
    dgroup.bytes.resize(0x800U,0xCDU);
    for(std::uint32_t i=0U;i<0x400U;++i)
        dgroup.bytes[i]=static_cast<std::uint8_t>((i*11U+7U)&0xFFU);
    auto buffered=CombatOriginalVgaCopyBinder::bind_copy_and_backing(
        vga,video_state,dgroup);
    std::vector<std::uint8_t> overlay_actions;
    overlay.active_page_9e39=3U;
    buffered.suspend_overlay_1a288=[&]() {
        overlay_actions.push_back(1U);
        assert(overlay.overlay_reference_count_9e37==0);
        assert(overlay.active_page_9e39==0U);
    };
    buffered.restore_overlay_1ab20=[&](
        std::int32_t original_row,std::int32_t original_column) {
        overlay_actions.push_back(2U);
        assert(original_row==1 && original_column==3);
        for(std::uint32_t i=0U;i<0x400U;++i)
            assert(dgroup.bytes[i]==dgroup.bytes[i+0x400U]);
    };
    assert(CombatOriginalCompositeModel::run_1a8c8(
        overlay,region,buffered)==
        CombatOriginalCompositeRoute::BufferedOverlayOverlap);
    assert((overlay_actions==std::vector<std::uint8_t>{1U,2U}));
    assert(overlay.overlay_reference_count_9e37==1);
    assert(overlay.active_page_9e39==3U);
    assert(overlay.redraw_busy_a6ba==0U);
}
