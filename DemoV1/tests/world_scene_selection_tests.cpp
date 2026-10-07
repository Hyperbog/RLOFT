#include "rloft/world_scene_selection.hpp"

#include <cassert>
#include <vector>

int main() {
    const rloft::Type7Geometry shape{
        64U, 15, 51, 27, 128
    };

    std::vector<rloft::WorldSceneRecord> records(2);

    records[0].screen_x = 128U;
    records[0].projected_extent = 400U;
    records[0].resource_id = 100U;
    records[0].source_index = 4U;
    records[0].source_class = 2U;
    records[0].render_order = 0U;

    records[1] = records[0];
    records[1].resource_id = 200U;
    records[1].source_index = 7U;
    records[1].source_class = 0U;
    records[1].render_order = 1U;

    const auto lookup =
        [shape](std::uint16_t) {
            return shape;
        };

    // Both overlap; higher render rank wins exactly as 223F4 scans.
    auto target =
        rloft::WorldSceneSelectionModel::target_at(
            records, 115, 128, lookup);
    assert(target.has_value());
    assert(target->record_index == 1U);
    assert(target->source_class == 0U);
    assert(target->source_index == 7U);

    // Inclusive rectangle edges are accepted.
    target =
        rloft::WorldSceneSelectionModel::target_at(
            records, 254, 155, lookup);
    assert(target.has_value());
    assert(target->record_index == 1U);

    assert(
        !rloft::WorldSceneSelectionModel::target_at(
            records, 114, 128, lookup).has_value());
    assert(
        !rloft::WorldSceneSelectionModel::target_at(
            records, 115, 156, lookup).has_value());

    // Class 3 is skipped, allowing the next lower rank to win.
    records[1].source_class = 3U;
    target =
        rloft::WorldSceneSelectionModel::target_at(
            records, 115, 128, lookup);
    assert(target.has_value());
    assert(target->record_index == 0U);
    assert(target->source_class == 2U);
    assert(target->source_index == 4U);

    return 0;
}
