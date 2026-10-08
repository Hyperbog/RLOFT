#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_overlay.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

int main(){
    using namespace rloft;
    CombatOriginalCompositeState state{};
    CombatOriginalOverlayGlobals g{state};
    CombatOriginalOverlayServices s{};
    std::vector<std::string> trace;
    s.push_page_2e590=[&](std::uint8_t page){
        trace.push_back("push");assert(page==state.active_page_9e39);
    };
    s.position_1a3b8=[&](CombatOriginalOverlayGlobals& x){
        trace.push_back("position");
        x.composite.overlay_row_9e31=-10;
        x.composite.overlay_column_9e33=34;
    };
    s.capture_292bc=[&](std::uint32_t p,std::int32_t r,
                          std::int32_t c,std::uint32_t h,std::uint32_t w){
        trace.push_back("capture");
        assert(p==0x9e3aU+0x400U);
        assert(r==-10&&c==34&&h==32U&&w==16U);
    };
    int probe=0;
    s.probe_2ae30=[&](std::int32_t r,std::int32_t c){
        trace.push_back("probe");assert(r==-10&&c==34);return probe;
    };
    s.draw_29804=[&](std::int32_t mode,std::int32_t row,
                      std::int32_t col,std::uint32_t resource){
        trace.push_back("draw"+std::to_string(mode));
        assert(row==-10&&col==34&&
               (resource==0x880dU||resource==0x8603U));
    };
    s.pop_page_2e5d4=[&](){trace.push_back("pop");};
    s.restore_2932c=[&](std::uint32_t ptr,std::int32_t row,
                         std::int32_t col,std::uint32_t h,std::uint32_t w){
        trace.push_back("restore");
        assert(ptr==0x9e3aU+0x400U);
        assert(row==111&&col==-2&&h==32U&&w==16U);
    };
    // Disabled 85FC: no counter write, page callback or pixels.
    CombatOriginalOverlayModel::suspend_1a288(g,s);
    CombatOriginalOverlayModel::restore_1ab20(g,111,-2,s);
    assert(trace.empty()&&state.overlay_reference_count_9e37==0);
    state.enabled_85fc=1U;
    state.active_page_9e39=1U;

    // Existing reference: only INC/DEC signed 16-bit WORD.
    state.overlay_reference_count_9e37=7;
    CombatOriginalOverlayModel::suspend_1a288(g,s);
    assert(state.overlay_reference_count_9e37==8&&trace.empty());
    CombatOriginalOverlayModel::restore_1ab20(g,111,-2,s);
    assert(state.overlay_reference_count_9e37==7&&trace.empty());
    state.overlay_reference_count_9e37=32767;
    CombatOriginalOverlayModel::suspend_1a288(g,s);
    assert(state.overlay_reference_count_9e37==-32768&&trace.empty());
    state.overlay_reference_count_9e37=0;
    CombatOriginalOverlayModel::restore_1ab20(g,111,-2,s);
    assert(state.overlay_reference_count_9e37==-1&&trace.empty());

    // First activation with A6C0=0: actual service order.
    trace.clear();state.overlay_reference_count_9e37=0;
    CombatOriginalOverlayModel::suspend_1a288(g,s);
    assert((trace==std::vector<std::string>{
        "push","position","capture","probe","pop"}));
    assert(state.overlay_reference_count_9e37==1);
    trace.clear();
    CombatOriginalOverlayModel::restore_1ab20(g,111,-2,s);
    assert((trace==std::vector<std::string>{"push","restore","pop"}));
    assert(state.overlay_reference_count_9e37==0);

    // A6C0's original 2AE30 result chooses sprite draw mode.
    g.overlay_mode_a6c0=1U;
    for(const auto& expected:
        std::vector<std::pair<int,std::string>>{
            {2,"draw1"},{1,"draw2"},{3,"draw0"}}){
        probe=expected.first;trace.clear();
        CombatOriginalOverlayModel::suspend_1a288(g,s);
        assert((trace==std::vector<std::string>{
            "push","position","capture","probe",expected.second,"pop"}));
        assert(state.overlay_reference_count_9e37==1);
        state.overlay_reference_count_9e37=0;
    }
    g.special_mode_a6bc=4;
    trace.clear();CombatOriginalOverlayModel::suspend_1a288(g,s);
    assert(trace[4]=="draw4");
    state.overlay_reference_count_9e37=0;
    g.selected_shape_85fe=7;probe=3;
    trace.clear();CombatOriginalOverlayModel::suspend_1a288(g,s);
    assert((trace==std::vector<std::string>{
        "push","position","capture","probe","draw0","draw0","pop"}));
    state.overlay_reference_count_9e37=0;

    // Reject absent service without inventing underlying graphics.
    CombatOriginalOverlayServices missing{};
    bool rejected=false;
    try{CombatOriginalOverlayModel::suspend_1a288(g,missing);}
    catch(const std::invalid_argument&){rejected=true;}
    assert(rejected&&state.overlay_reference_count_9e37==0);
    state.overlay_reference_count_9e37=1;
    rejected=false;
    try{CombatOriginalOverlayModel::restore_1ab20(g,111,-2,missing);}
    catch(const std::invalid_argument&){rejected=true;}
    assert(rejected&&state.overlay_reference_count_9e37==0);

    // Attach to 1A8C8 without discarding existing 2A078/30CD4.
    CombatOriginalCompositeServices core{};
    core.copy_rect_2a078=[](std::uint32_t,std::uint32_t,
                            std::uint32_t,std::uint32_t,
                            std::uint32_t,std::uint32_t,
                            std::uint32_t){};
    core.move_overlay_bytes_30cd4=[](
        std::uint32_t,std::uint32_t,std::uint32_t){};
    auto bound=CombatOriginalOverlayModel::bind_1a8c8(g,s,core);
    assert(bound.copy_rect_2a078&&bound.move_overlay_bytes_30cd4);
    assert(bound.suspend_overlay_1a288&&bound.restore_overlay_1ab20);
    g.overlay_mode_a6c0=0U;trace.clear();
    bound.suspend_overlay_1a288();
    assert(state.overlay_reference_count_9e37==1);
    bound.restore_overlay_1ab20(111,-2);
    assert(state.overlay_reference_count_9e37==0);
    assert((trace==std::vector<std::string>{
        "push","position","capture","probe","pop",
        "push","restore","pop"}));

    // Integrate with the real 1A8C8 wrapper: that wrapper
    // temporarily decrements the depth to zero before 1A288,
    // and its later 1AB20 decrements back to zero. It finally
    // increments depth and restores the original active page.
    std::vector<std::string> frame;
    CombatOriginalOverlayServices render{};
    render.push_page_2e590=[&](std::uint8_t page){
        assert(page==0U);
        frame.emplace_back("push");
    };
    render.position_1a3b8=[&](CombatOriginalOverlayGlobals& current){
        frame.emplace_back("position");
        current.composite.overlay_row_9e31=-10;
        current.composite.overlay_column_9e33=34;
    };
    render.capture_292bc=[&](
        std::uint32_t source,std::int32_t row,std::int32_t col,
        std::uint32_t height,std::uint32_t width){
        frame.emplace_back("capture");
        assert(source==0x9E3AU&&row==-10&&col==34);
        assert(height==0x20U&&width==0x10U);
    };
    render.probe_2ae30=[&](std::int32_t row,std::int32_t col){
        frame.emplace_back("probe");
        assert(row==-10&&col==34);
        return 0;
    };
    render.pop_page_2e5d4=[&](){frame.emplace_back("pop");};
    render.restore_2932c=[&](
        std::uint32_t source,std::int32_t row,std::int32_t col,
        std::uint32_t height,std::uint32_t width){
        frame.emplace_back("restore");
        assert(source==0x9E3AU&&row==-10&&col==34);
        assert(height==0x20U&&width==0x10U);
    };
    CombatOriginalCompositeServices blit{};
    blit.copy_rect_2a078=[&](
        std::uint32_t page,std::uint32_t row,std::uint32_t col,
        std::uint32_t dst_row,std::uint32_t dst_col,
        std::uint32_t rows,std::uint32_t columns){
        frame.emplace_back("copy");
        assert(page==1U&&row==0U&&col==34U);
        assert(dst_row==0U&&dst_col==34U);
        assert(rows==2U&&columns==2U);
    };
    blit.move_overlay_bytes_30cd4=[&](
        std::uint32_t dest,std::uint32_t src,std::uint32_t bytes){
        frame.emplace_back("backing");
        assert(dest==0xA23AU&&src==0x9E3AU&&bytes==0x400U);
    };
    auto integrated=CombatOriginalOverlayModel::bind_1a8c8(g,render,blit);
    state.overlay_reference_count_9e37=1;
    state.active_page_9e39=1U;
    state.overlay_row_9e31=-10;
    state.overlay_column_9e33=34;
    g.overlay_mode_a6c0=0U;
    assert(CombatOriginalCompositeModel::run_1a8c8(
        state,CombatOriginalCompositeRect{0U,0U,34U,2U,2U},
        integrated)==CombatOriginalCompositeRoute::BufferedOverlayOverlap);
    assert((frame==std::vector<std::string>{
        "push","position","capture","probe","pop","copy",
        "backing","push","restore","pop"}));
    assert(state.overlay_reference_count_9e37==1);
    assert(state.active_page_9e39==1U);
    assert(state.redraw_busy_a6ba==0U);
}
