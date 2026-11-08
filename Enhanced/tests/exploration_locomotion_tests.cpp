#include "rloft/exploration_locomotion.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace {

rloft::ExplorationSessionState base_session() {
    rloft::ExplorationSessionState s{};
    s.coordinate_a = 0x00018000U;
    s.coordinate_b = 0x00018000U;
    s.facing = 270;
    s.level.width = 4;
    s.level.height = 4;
    s.level.cell_words.assign(16, 0);
    return s;
}

std::size_t index(int column, int row) {
    return static_cast<std::size_t>(
        row * 4 + column);
}

rloft::ExplorationMotionStep diagonal_step() {
    rloft::ExplorationMotionStep step{};
    step.state.heading = 90;
    step.translation =
        rloft::ExplorationMotionVector{
            0x00010000U,
            0x00010000U,
        };
    return step;
}

}  // namespace

int main() {
    {
        auto s = base_session();
        auto step = diagonal_step();

        const auto applied =
            rloft::ExplorationLocomotionModel::apply(
                s, step);

        assert(applied.acceptance ==
               rloft::LocomotionAcceptance::Full);
        assert(s.coordinate_a == 0x00028000U);
        assert(s.coordinate_b == 0x00028000U);
        assert(s.facing == 90);
        assert(!applied.suppressed_side_effect);
    }

    {
        // Full (2,2) blocked; Y-only (1,2) remains clear.
        auto s = base_session();
        s.level.cell_words[index(2, 2)] = 1U;

        const auto applied =
            rloft::ExplorationLocomotionModel::apply(
                s, diagonal_step());

        assert(applied.acceptance ==
               rloft::LocomotionAcceptance::YOnly);
        assert(s.coordinate_a == 0x00018000U);
        assert(s.coordinate_b == 0x00028000U);
    }

    {
        // Full and Y-only blocked; X-only (2,1) is clear.
        auto s = base_session();
        s.level.cell_words[index(2, 2)] = 1U;
        s.level.cell_words[index(1, 2)] = 1U;

        const auto applied =
            rloft::ExplorationLocomotionModel::apply(
                s, diagonal_step());

        assert(applied.acceptance ==
               rloft::LocomotionAcceptance::XOnly);
        assert(s.coordinate_a == 0x00028000U);
        assert(s.coordinate_b == 0x00018000U);
    }

    {
        auto s = base_session();
        s.level.cell_words[index(2, 2)] = 1U;
        s.level.cell_words[index(1, 2)] = 1U;
        s.level.cell_words[index(2, 1)] = 1U;

        const auto applied =
            rloft::ExplorationLocomotionModel::apply(
                s, diagonal_step());

        assert(applied.acceptance ==
               rloft::LocomotionAcceptance::Blocked);
        assert(s.coordinate_a == 0x00018000U);
        assert(s.coordinate_b == 0x00018000U);
        // 2E120 commits heading before 2DBAC attempts translation.
        assert(s.facing == 90);
    }

    {
        // Dynamic position zero does not block party mode.
        auto s = base_session();
        s.level.cell_words[index(2, 2)] = 0x0080U;
        rloft::CellControlRecord dynamic{};
        dynamic.raw[0] = 1;
        dynamic.raw[4] = 2;
        dynamic.raw[5] = 2;
        dynamic.raw[6] = 0;
        s.level.group2.push_back(dynamic);

        const auto applied =
            rloft::ExplorationLocomotionModel::apply(
                s, diagonal_step());
        assert(applied.acceptance ==
               rloft::LocomotionAcceptance::Full);

        // Nonzero position blocks when the control record is present.
        s = base_session();
        s.level.cell_words[index(2, 2)] = 0x0080U;
        dynamic.raw[6] = 8;
        s.level.group2.push_back(dynamic);
        const auto closed =
            rloft::ExplorationLocomotionModel::apply(
                s, diagonal_step());
        assert(closed.acceptance ==
               rloft::LocomotionAcceptance::YOnly);
    }

    {
        // Party mode explicitly ignores 0x0800 collision geometry.
        auto s = base_session();
        s.level.cell_words[index(2, 2)] =
            static_cast<std::uint16_t>(
                1U | 0x0800U);
        const auto applied =
            rloft::ExplorationLocomotionModel::apply(
                s, diagonal_step());
        assert(applied.acceptance ==
               rloft::LocomotionAcceptance::Full);
    }

    {
        // 0x1000 with no control record is passable in 2D5D0.
        auto s = base_session();
        s.level.cell_words[index(2, 2)] =
            static_cast<std::uint16_t>(
                1U | 0x1000U);
        const auto applied =
            rloft::ExplorationLocomotionModel::apply(
                s, diagonal_step());
        assert(applied.acceptance ==
               rloft::LocomotionAcceptance::Full);
    }

    {
        // Transition resource is recognized but not executed in this
        // non-event milestone; sliding falls back to Y-only.
        auto s = base_session();
        s.level.cell_words[index(2, 2)] = 1U;
        s.level.header[0x00C] = 0x42U;
        s.level.header[0x00D] = 0x02U;

        const auto applied =
            rloft::ExplorationLocomotionModel::apply(
                s, diagonal_step());
        assert(applied.acceptance ==
               rloft::LocomotionAcceptance::YOnly);
        assert(applied.suppressed_side_effect);
    }

    {
        // Turning without translation still updates authoritative facing.
        auto s = base_session();
        rloft::ExplorationMotionStep turn{};
        turn.state.heading = 180;

        const auto applied =
            rloft::ExplorationLocomotionModel::apply(
                s, turn);
        assert(applied.acceptance ==
               rloft::LocomotionAcceptance::NoTranslation);
        assert(s.facing == 180);
        assert(s.coordinate_a == 0x00018000U);
        assert(s.coordinate_b == 0x00018000U);
    }
}
