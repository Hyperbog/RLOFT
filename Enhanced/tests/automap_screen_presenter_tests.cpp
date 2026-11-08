#include "rloft/automap_controls.hpp"
#include "rloft/automap_screen_presenter.hpp"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    rloft::ExplorationSessionState session{};
    session.coordinate_a = 0x00048000U;
    session.coordinate_b = 0x00048000U;

    const auto state =
        rloft::AutomapScreenStateModel::initial(
            session);
    assert(state.column == -20);
    assert(state.row == -20);

    const auto& rects =
        rloft::AutomapControlModel::rectangles();
    assert(rects[8].row_min == 282);
    assert(rects[8].column_min == 256);
    assert(rects[9].row_min == 310);
    assert(rects[10].row_min == 338);
    assert(rects[11].row_min == 366);

    // Actual CODE.2 01834 loop obtains its 13 icon frames from
    // object-2 DGROUP:5FCC, not from a newly guessed UI legend.
    constexpr std::size_t at=0x75F24U+0x5FCCU;
    std::vector<std::uint8_t> original(at+13U,0U);
    constexpr std::array<std::uint8_t,13> original_frames{{
        6U,34U,42U,47U,48U,49U,50U,51U,52U,53U,54U,55U,56U,
    }};
    for(std::size_t i=0;i<original_frames.size();++i)
        original[at+i]=original_frames[i];
    const auto recovered=
        rloft::AutomapScreenPresenter::legend_frames_from_code2(
            original);
    assert(recovered==original_frames);
    original.pop_back();
    bool truncated=false;
    try{
        (void)rloft::AutomapScreenPresenter::
            legend_frames_from_code2(original);
    }catch(const std::runtime_error&){truncated=true;}
    assert(truncated);

    // 018F4..01905 skips legend entry 9 in all non-FF map
    // modes, but displays the other 12 at every level.
    for(std::size_t i=0;i<13U;++i) {
        assert(rloft::AutomapScreenPresenter::
            legend_entry_visible(i,0xFFU));
        assert(rloft::AutomapScreenPresenter::
            legend_entry_visible(i,0x00U)==(i!=9U));
        assert(rloft::AutomapScreenPresenter::
            legend_entry_visible(i,0x01U)==(i!=9U));
    }
    assert(!rloft::AutomapScreenPresenter::legend_entry_visible(
        13U,0xFFU));

    // Original CODE.2 object1 018A8..018E6: 10 columns x 25
    // rows, one RES0+35h frame drawn every 8x16 VGA pixels.
    const auto seed=0x12345U;
    const auto frames=rloft::AutomapScreenPresenter::
        sidebar_filler_frames(seed);
    assert(frames.size()==250U);
    rloft::OriginalRandom source(seed&0xFFFFU);
    std::array<std::size_t,4> frequency{};
    for(const auto frame:frames) {
        const auto expected=static_cast<std::uint8_t>(
            (static_cast<std::uint32_t>(source.next15())*4U)>>15U);
        assert(frame==expected);
        assert(frame<4U);
        ++frequency[frame];
    }
    for(const auto count:frequency)assert(count>0U);
    // The modal's wallpaper is stable on every redraw, while the
    // original IRQ seed changes the pattern on a later map opening.
    assert(frames==rloft::AutomapScreenPresenter::
        sidebar_filler_frames(seed));
    assert(frames!=rloft::AutomapScreenPresenter::
        sidebar_filler_frames(seed+1U));

    // 01978..019C9: 38 pairs of 9/10 and 5/6 top/bottom
    // edge frames. 019D8..01A4C: 23 triples 12/13,
    // 18/19, 14/15 for the left/interior/right edges.
    const auto border=rloft::AutomapScreenPresenter::
        border_filler_frames(seed);
    assert(border.size()==145U);
    rloft::OriginalRandom source_border(seed&0xFFFFU);
    const auto bit=[&source_border]() {
        return static_cast<unsigned>(
            (static_cast<std::uint32_t>(
                source_border.next15())*2U)>>15U);
    };
    for(std::size_t x=0U;x<76U;x+=2U) {
        assert(border[x]==9U+bit());
        assert(border[x+1U]==5U+bit());
    }
    for(std::size_t y=76U;y<border.size();y+=3U) {
        assert(border[y]==12U+bit());
        assert(border[y+1U]==18U+bit());
        assert(border[y+2U]==14U+bit());
    }
    assert(border==rloft::AutomapScreenPresenter::
        border_filler_frames(seed));
    assert(border!=rloft::AutomapScreenPresenter::
        border_filler_frames(seed+1U));
}
