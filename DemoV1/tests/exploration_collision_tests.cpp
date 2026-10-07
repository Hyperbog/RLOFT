#include "rloft/exploration_collision.hpp"

#include <cassert>

int main() {
    rloft::LevelState level{};
    level.width = 3;
    level.height = 2;
    level.cell_words.assign(6, 0);

    // Ordinary empty/nonempty cells.
    auto p = rloft::ExplorationCollisionModel::passability(level, {0, 0});
    assert(!p.special_geometry);
    assert(!p.blocked);

    level.cell_words[1] = 0x0007;
    p = rloft::ExplorationCollisionModel::passability(level, {1, 0});
    assert(!p.special_geometry);
    assert(p.blocked);

    // Special geometry with no group-2 record reproduces 1E930's 0x8000
    // missing-record result and therefore blocks.
    level.cell_words[2] = 0x0080 | 0x0003;
    p = rloft::ExplorationCollisionModel::passability(level, {2, 0});
    assert(p.special_geometry);
    assert(p.blocked);
    assert(p.dynamic_phase == rloft::DynamicBarrierPhase::Missing);
    assert(!p.dynamic_position.has_value());

    rloft::CellControlRecord barrier{};
    barrier.raw[0] = 1;
    barrier.raw[4] = 0;
    barrier.raw[5] = 2;
    barrier.raw[6] = 0;
    level.group2.push_back(barrier);

    p = rloft::ExplorationCollisionModel::passability(level, {2, 0});
    assert(!p.blocked);
    assert(p.dynamic_phase == rloft::DynamicBarrierPhase::Open);
    assert(*p.dynamic_position == 0);

    level.group2.back().raw[6] = 4;
    p = rloft::ExplorationCollisionModel::passability(level, {2, 0});
    assert(p.blocked);
    assert(p.dynamic_phase == rloft::DynamicBarrierPhase::Transitional);

    level.group2.back().raw[6] = 8;
    p = rloft::ExplorationCollisionModel::passability(level, {2, 0});
    assert(p.blocked);
    assert(p.dynamic_phase == rloft::DynamicBarrierPhase::Closed);
}
