#include "rloft/automap_controls.hpp"
#include "rloft/automap_screen_presenter.hpp"

#include <cassert>

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
}
