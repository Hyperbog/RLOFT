#include "rloft/exploration_collision.hpp"

#include <algorithm>

namespace rloft {

const CellControlRecord* ExplorationCollisionModel::find_dynamic_record(
    const LevelState& level,
    CellPosition cell) noexcept {
    const auto it = std::find_if(
        level.group2.begin(),
        level.group2.end(),
        [cell](const CellControlRecord& record) {
            return record.kind() == 1 &&
                   record.column() == cell.column &&
                   record.row() == cell.row;
        });
    return it == level.group2.end() ? nullptr : &*it;
}

CellPassability ExplorationCollisionModel::passability(
    const LevelState& level,
    CellPosition cell) {
    const auto word = ExplorationWorldModel::cell_word(level, cell);

    CellPassability out{};
    out.special_geometry = (word & 0x0080U) != 0;

    if (!out.special_geometry) {
        out.blocked = (word & 0x003FU) != 0;
        return out;
    }

    const auto* dynamic = find_dynamic_record(level, cell);
    if (dynamic == nullptr) {
        // CODE.2 helper 1E930 returns 0x8000 when no group-2 record matches;
        // the traversal path treats every nonzero return as blocking.
        out.blocked = true;
        out.dynamic_phase = DynamicBarrierPhase::Missing;
        return out;
    }

    const auto position = dynamic->raw[6];
    out.dynamic_position = position;
    out.blocked = position != 0;

    if (position == 0) {
        out.dynamic_phase = DynamicBarrierPhase::Open;
    } else if (position == 8) {
        out.dynamic_phase = DynamicBarrierPhase::Closed;
    } else {
        out.dynamic_phase = DynamicBarrierPhase::Transitional;
    }
    return out;
}

}  // namespace rloft
