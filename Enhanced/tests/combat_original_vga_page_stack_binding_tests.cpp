#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_vga_page_stack_binding.hpp"

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

int main() {
    using namespace rloft;
    CombatOriginalCompositeState composite{};
    composite.enabled_85fc=1U;
    composite.overlay_reference_count_9e37=1;
    composite.overlay_row_9e31=0;
    composite.overlay_column_9e33=34;
    composite.active_page_9e39=1U;

    CombatOriginalOverlayGlobals overlay{composite};
    overlay.overlay_mode_a6c0=0U;

    CombatOriginalVgaPageStackState page{};
    page.depth_f0d0=0U;
    page.active_page_ef4d=3U;
    page.selected_page_efe9=0U; // intentionally differs from EF4D
    page.physical_page_base_f001=0xA0000U;
    page.page_stride_f005=0x800U;
    page.saved_pages_2b644.assign(10U,0U);

    CombatOriginalVgaCopyState live_video{};
    live_video.selected_page_efe9=page.selected_page_efe9;
    live_video.linear_page_base_f001=page.physical_page_base_f001;
    live_video.page_stride_f005=0x800U;

    std::vector<std::string> trace;
    CombatOriginalVgaPageStackServices paging{};
    paging.selected_page_291b0=[&](std::uint8_t selected){
        trace.push_back("page"+std::to_string(selected));
        assert(selected==0U||selected==2U||selected==3U);
        assert(page.active_page_ef4d==selected);
        assert(page.selected_page_efe9==selected);
        assert(live_video.selected_page_efe9==selected);
        assert(live_video.linear_page_base_f001==
               page.physical_page_base_f001);
    };
    paging.diagnostic_2f580=[&](std::uint32_t){
        assert(false&&"no original stack error on this valid path");
    };

    CombatOriginalOverlayServices overlay_callbacks{};
    overlay_callbacks.position_1a3b8=[&](CombatOriginalOverlayGlobals& g){
        trace.push_back("position");
        assert(g.composite.active_page_9e39==0U);
        assert(page.active_page_ef4d==0U);
        assert(page.depth_f0d0==1U);
    };
    overlay_callbacks.capture_292bc=[&](
        std::uint32_t pointer,std::int32_t row,std::int32_t column,
        std::uint32_t rows,std::uint32_t columns) {
        trace.push_back("capture");
        assert(pointer==0x9E3AU);
        assert(row==0&&column==34);
        assert(rows==0x20U&&columns==0x10U);
        assert(page.depth_f0d0==1U);
    };
    overlay_callbacks.probe_2ae30=[&](
        std::int32_t row,std::int32_t column) {
        trace.push_back("probe");
        assert(row==0&&column==34);
        return 0;
    };
    overlay_callbacks.restore_2932c=[&](
        std::uint32_t pointer,std::int32_t row,std::int32_t column,
        std::uint32_t rows,std::uint32_t columns) {
        trace.push_back("restore");
        assert(pointer==0x9E3AU&&row==0&&column==34);
        assert(rows==0x20U&&columns==0x10U);
        assert(page.depth_f0d0==1U);
        assert(page.active_page_ef4d==0U);
    };
    auto linked=CombatOriginalVgaPageStackOverlayBinder::bind_with_live_video(
        page,live_video,paging,overlay_callbacks);
    assert(linked.push_page_2e590&&linked.pop_page_2e5d4);
    assert(linked.capture_292bc&&linked.restore_2932c);

    CombatOriginalCompositeServices copy{};
    copy.copy_rect_2a078=[&](
        std::uint32_t dest,std::uint32_t row,std::uint32_t column,
        std::uint32_t dest_row,std::uint32_t dest_column,
        std::uint32_t rows,std::uint32_t columns) {
        trace.push_back("copy");
        assert(dest==1U && row==0U && column==34U);
        assert(dest_row==0U && dest_column==34U);
        assert(rows==2U&&columns==2U);
        assert(page.depth_f0d0==0U);
        assert(page.active_page_ef4d==3U);
    };
    copy.move_overlay_bytes_30cd4=[&](
        std::uint32_t destination,std::uint32_t source,
        std::uint32_t length) {
        trace.push_back("backing");
        assert(destination==0xA23AU&&source==0x9E3AU);
        assert(length==0x400U);
        assert(page.depth_f0d0==0U);
    };
    const auto combined=CombatOriginalOverlayModel::bind_1a8c8(
        overlay,linked,copy);
    const auto route=CombatOriginalCompositeModel::run_1a8c8(
        composite,CombatOriginalCompositeRect{0U,0U,34U,2U,2U},
        combined);
    assert(route==CombatOriginalCompositeRoute::BufferedOverlayOverlap);
    assert((trace==std::vector<std::string>{
        "page0","position","capture","probe","page3",
        "copy","backing","page0","restore","page3"
    }));
    assert(page.depth_f0d0==0U);
    assert(page.saved_pages_2b644[0U]==3U);
    assert(page.active_page_ef4d==3U);
    assert(page.selected_page_efe9==3U);
    assert(page.physical_page_base_f001==0xB8000U);
    assert(live_video.selected_page_efe9==3U);
    assert(live_video.linear_page_base_f001==0xB8000U);
    assert(composite.overlay_reference_count_9e37==1);
    assert(composite.active_page_9e39==1U);
    assert(composite.redraw_busy_a6ba==0U);

    // An already active overlay makes no page-stack call:
    // the reused binding must preserve original nested behavior.
    trace.clear();
    composite.overlay_reference_count_9e37=2;
    CombatOriginalOverlayModel::suspend_1a288(overlay,linked);
    assert(trace.empty());
    assert(composite.overlay_reference_count_9e37==3);
    assert(page.depth_f0d0==0U);

    // Live F005 is read at each 2AB6C call, not frozen when
    // this binding was created. A later original-mode change
    // must alter both stack projection and VGA copy addresses.
    live_video.page_stride_f005=0x123U;
    linked.push_page_2e590(2U);
    assert(page.active_page_ef4d==2U);
    assert(page.selected_page_efe9==2U);
    assert(live_video.selected_page_efe9==2U);
    assert(trace.back()=="page2");
    assert(page.page_stride_f005==0x123U);
    assert(page.physical_page_base_f001==
           ((0xA000U+2U*0x123U)<<4U));
    assert(live_video.linear_page_base_f001==
           page.physical_page_base_f001);
    linked.pop_page_2e5d4();
    assert(page.depth_f0d0==0U);
    assert(live_video.selected_page_efe9==3U);
}
