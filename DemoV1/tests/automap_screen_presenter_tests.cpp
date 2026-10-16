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
}
