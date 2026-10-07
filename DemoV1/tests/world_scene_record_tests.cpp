#include "rloft/world_scene_record.hpp"

#include <cassert>
#include <vector>

int main() {
    std::vector<rloft::WorldSceneRecord> records(4);
    records[0].projected_extent = 400;
    records[1].projected_extent = 100;
    records[2].projected_extent = 400;
    records[3].projected_extent = 250;

    rloft::WorldSceneOrderModel::assign_render_order(records);

    assert(records[1].render_order == 0U);
    assert(records[3].render_order == 1U);
    // Equal extents preserve original emission order, matching the strict
    // less-than replacement rule in CODE.2 2C294.
    assert(records[0].render_order == 2U);
    assert(records[2].render_order == 3U);

    return 0;
}
