#include "rloft/exploration_locomotion.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

int high_sword(std::uint32_t value) noexcept {
    return static_cast<std::int16_t>(
        static_cast<std::uint16_t>(value >> 16U));
}

std::uint16_t header_u16(
    const LevelState& level,
    std::size_t offset) {
    if (offset + 1U >= level.header.size()) {
        throw std::runtime_error(
            "level header resource table is truncated");
    }
    return static_cast<std::uint16_t>(
        level.header[offset]) |
        (static_cast<std::uint16_t>(
             level.header[offset + 1U]) << 8U);
}

}  // namespace

const CellControlRecord*
ExplorationLocomotionModel::find_record(
    const LevelState& level,
    CellPosition cell) noexcept {
    const auto matches =
        [cell](const CellControlRecord& record) {
            return record.column() == cell.column &&
                   record.row() == cell.row;
        };

    const auto first = std::find_if(
        level.group1.begin(),
        level.group1.end(),
        matches);
    if (first != level.group1.end()) {
        return &*first;
    }

    const auto second = std::find_if(
        level.group2.begin(),
        level.group2.end(),
        matches);
    return second == level.group2.end()
        ? nullptr
        : &*second;
}

const CellControlRecord*
ExplorationLocomotionModel::find_dynamic_record(
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
    return it == level.group2.end()
        ? nullptr
        : &*it;
}

ExplorationLocomotionModel::GeometryResult
ExplorationLocomotionModel::party_geometry(
    const LevelState& level,
    std::uint32_t coordinate_a,
    std::uint32_t coordinate_b) {
    if (level.width <= 0 ||
        level.height <= 0 ||
        level.cell_words.size() !=
            static_cast<std::size_t>(level.width) *
                static_cast<std::size_t>(level.height)) {
        throw std::runtime_error(
            "invalid level geometry for locomotion");
    }

    // CODE.2 2D5D0 names are transposed locally: raw_y is coordinate B
    // (row) and raw_x is coordinate A (column). Its nested loops therefore
    // scan every stored cell touched by a 0x5800 square around the party.
    const int min_row = high_sword(
        coordinate_b - kPartyCollisionHalfExtent);
    const int min_column = high_sword(
        coordinate_a - kPartyCollisionHalfExtent);
    const int max_row = high_sword(
        coordinate_b + kPartyCollisionHalfExtent);
    const int max_column = high_sword(
        coordinate_a + kPartyCollisionHalfExtent);

    bool blocked = false;
    bool side_effect = false;

    for (int column = min_column;
         column <= max_column;
         ++column) {
        for (int row = min_row;
             row <= max_row;
             ++row) {
            const CellPosition cell{column, row};

            // The encoded inner loop skips out-of-range map coordinates.
            if (!ExplorationWorldModel::in_bounds(
                    level, cell)) {
                continue;
            }

            const auto index =
                ExplorationWorldModel::cell_index(
                    level, cell);
            const auto word = level.cell_words[index];
            const auto* record =
                find_record(level, cell);

            if ((word & 0x0080U) != 0U) {
                const auto* dynamic =
                    find_dynamic_record(
                        level, cell);
                const std::uint16_t marker =
                    dynamic == nullptr
                        ? 0x8000U
                        : static_cast<std::uint16_t>(
                              static_cast<std::uint16_t>(
                                  dynamic->raw[6]) << 12U);

                // Party mode is -1. In 2D5D0 every nonzero dynamic marker
                // blocks when a non-kind-8 control record is present.
                if (marker != 0U &&
                    record != nullptr &&
                    record->kind() != 8U) {
                    blocked = true;
                }
            }

            const auto code =
                static_cast<std::uint8_t>(
                    word & 0x003FU);
            if (code == 0U) {
                continue;
            }

            // For party mode (-1), 0x0800 geometry explicitly continues
            // without setting the blocked flag.
            if ((word & 0x0800U) != 0U) {
                continue;
            }

            if ((word & 0x1000U) == 0U) {
                // 2D004 may turn primary resources 0x242..0x247 into a
                // scripted transition. Those transition side effects are
                // intentionally outside this exploration milestone.
                const auto resource =
                    header_u16(
                        level,
                        0x00AU +
                            static_cast<std::size_t>(
                                code) * 2U);
                if (resource >= 0x0242U &&
                    resource <= 0x0247U) {
                    side_effect = true;
                } else {
                    blocked = true;
                }
                continue;
            }

            if (record == nullptr) {
                continue;
            }

            if (record->kind() == 8U) {
                // Party mode invokes 2D3D8 (level/position transition).
                side_effect = true;
                continue;
            }

            if (record->kind() != 1U) {
                blocked = true;
            }
        }
    }

    if (side_effect) {
        return GeometryResult::RequiresSideEffect;
    }
    return blocked
        ? GeometryResult::Blocked
        : GeometryResult::Clear;
}

AppliedExplorationMotion ExplorationLocomotionModel::apply(
    ExplorationSessionState& session,
    const ExplorationMotionStep& step) {
    AppliedExplorationMotion out{};
    session.facing = step.state.heading;
    out.facing = session.facing;

    if (!step.translation.has_value()) {
        out.acceptance =
            LocomotionAcceptance::NoTranslation;
        out.coordinate_a = session.coordinate_a;
        out.coordinate_b = session.coordinate_b;
        return out;
    }

    const auto old_a = session.coordinate_a;
    const auto old_b = session.coordinate_b;
    const auto dx = step.translation->delta_x;
    const auto dy = step.translation->delta_y;

    struct Attempt final {
        LocomotionAcceptance acceptance;
        std::uint32_t a;
        std::uint32_t b;
    };

    const Attempt attempts[] = {
        {
            LocomotionAcceptance::Full,
            old_a + dx,
            old_b + dy,
        },
        {
            LocomotionAcceptance::YOnly,
            old_a,
            old_b + dy,
        },
        {
            LocomotionAcceptance::XOnly,
            old_a + dx,
            old_b,
        },
    };

    bool suppressed = false;
    for (const auto& attempt : attempts) {
        const auto geometry =
            party_geometry(
                session.level,
                attempt.a,
                attempt.b);
        if (geometry ==
            GeometryResult::RequiresSideEffect) {
            suppressed = true;
            continue;
        }
        if (geometry != GeometryResult::Clear) {
            continue;
        }

        session.coordinate_a = attempt.a;
        session.coordinate_b = attempt.b;
        out.acceptance = attempt.acceptance;
        out.coordinate_a = session.coordinate_a;
        out.coordinate_b = session.coordinate_b;
        out.suppressed_side_effect = suppressed;
        return out;
    }

    session.coordinate_a = old_a;
    session.coordinate_b = old_b;
    out.acceptance = LocomotionAcceptance::Blocked;
    out.coordinate_a = old_a;
    out.coordinate_b = old_b;
    out.suppressed_side_effect = suppressed;
    return out;
}

}  // namespace rloft
