#include "rloft/view_sweep_influence.hpp"

#include <cassert>
#include <vector>

int main() {
    constexpr int width = 30;
    constexpr std::uint32_t center = 7U * 30U + 13U;

    std::vector<rloft::ViewSweepRecord> records(6);
    records[0].cell_index = center;
    records[0].shade_decrement = 1U;

    // Consecutive duplicate inherits the first record's carried result even
    // though its own starting value differs.
    records[1].cell_index = center;
    records[1].shade_decrement = 9U;

    records[2].cell_index = center + 1U;
    records[2].shade_decrement = 0U;

    records[3].cell_index = center + width + 1U;
    records[3].shade_decrement = 2U;

    records[4].cell_index = center + 2U;
    records[4].shade_decrement = 0U;

    records[5].cell_index = center + 3U;
    records[5].shade_decrement = 3U;

    rloft::ViewSweepInfluenceModel::apply(
        records,
        3U,
        center,
        width);

    assert(records[0].shade_decrement == 4U);
    assert(records[1].shade_decrement == 4U);
    assert(records[2].shade_decrement == 2U);
    assert(records[3].shade_decrement == 4U);
    assert(records[4].shade_decrement == 1U);
    assert(records[5].shade_decrement == 3U);

    // Step two affects only center and the first ring.
    std::vector<rloft::ViewSweepRecord> step2(3);
    step2[0].cell_index = center;
    step2[1].cell_index = center - width;
    step2[2].cell_index = center + 2U;
    rloft::ViewSweepInfluenceModel::apply(
        step2, 2U, center, width);
    assert(step2[0].shade_decrement == 2U);
    assert(step2[1].shade_decrement == 1U);
    assert(step2[2].shade_decrement == 0U);

    return 0;
}
