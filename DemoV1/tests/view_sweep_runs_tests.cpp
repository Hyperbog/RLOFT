#include "rloft/view_sweep_runs.hpp"

#include <cassert>
#include <vector>

int main() {
    std::vector<rloft::ViewSweepRecord> records(6);

    // Rays 0-1: main run, no prepass.
    records[0] = {0x100, 40, 30, 0, 0x080, 1, 2};
    records[1] = {0x100, 40, 30, 0, 0x080, 1, 2};

    // Rays 2-3: one prepass run and one main run.
    records[2] = {0x180, 41, 31, 1, 0x0C0, 2, 1};
    records[3] = {0x180, 41, 31, 1, 0x0C0, 2, 1};

    // Ray 4 changes depth but stays prepass.
    records[4] = {0x200, 42, 32, 1, 0x0C0, 3, 3};

    // Ray 5 ends prepass and changes main selector.
    records[5] = {0x200, 0x93, 0x93, 0, 0x100, 0, 0};

    const auto runs = rloft::ViewSweepRunModel::build(records);

    // Prepass: rays 2-3, then ray 4.
    assert(runs.size() == 6);
    assert(runs[0].pass == rloft::SweepRenderPass::Prepass);
    assert(runs[0].first_ray == 2);
    assert(runs[0].ray_count == 2);
    assert(runs[0].resource_id == 41);
    assert(runs[0].texture_column_offset == 0x0C0);
    assert(runs[0].texture_source_byte_offset == 0x180);
    assert(runs[0].texture_column_index == 3);
    assert(runs[0].shade_decrement == 1);

    assert(runs[1].pass == rloft::SweepRenderPass::Prepass);
    assert(runs[1].first_ray == 4);
    assert(runs[1].ray_count == 1);
    assert(runs[1].resource_id == 42);

    // Main: 0-1, 2-3, 4, 5.
    assert(runs[2].pass == rloft::SweepRenderPass::Main);
    assert(runs[2].first_ray == 0);
    assert(runs[2].ray_count == 2);
    assert(runs[2].resource_id == 30);

    assert(runs[3].first_ray == 2);
    assert(runs[3].ray_count == 2);
    assert(runs[3].resource_id == 31);

    assert(runs[4].first_ray == 4);
    assert(runs[4].ray_count == 1);
    assert(runs[4].resource_id == 32);

    assert(runs[5].first_ray == 5);
    assert(runs[5].ray_count == 1);
    assert(runs[5].resource_id == 0x93);
    assert(!runs[5].drawable());
}
