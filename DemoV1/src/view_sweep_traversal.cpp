#include "rloft/view_sweep_traversal.hpp"

#include "rloft/environment_view.hpp"
#include "rloft/exploration_world.hpp"
#include "rloft/fixed_projection.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace rloft {
namespace {

std::int16_t high_s16(std::uint32_t value) noexcept {
    return static_cast<std::int16_t>(value >> 16U);
}
std::uint16_t high_u16(std::uint32_t value) noexcept {
    return static_cast<std::uint16_t>(value >> 16U);
}
std::uint16_t low_u16(std::uint32_t value) noexcept {
    return static_cast<std::uint16_t>(value);
}
std::uint32_t set_high_u16(std::uint32_t value, std::uint16_t high) noexcept {
    return (static_cast<std::uint32_t>(high) << 16U) |
           static_cast<std::uint32_t>(low_u16(value));
}
std::uint32_t high_only(std::uint32_t value) noexcept {
    return value & 0xFFFF0000U;
}
std::uint32_t add_high_word_one(std::uint32_t value) noexcept {
    return set_high_u16(
        value,
        static_cast<std::uint16_t>(high_u16(value) + 1U));
}
std::uint32_t mul_words(std::int16_t a, std::int16_t b) noexcept {
    return static_cast<std::uint32_t>(a) *
           static_cast<std::uint32_t>(b);
}
std::uint32_t add_ax(std::uint32_t eax, std::uint16_t value) noexcept {
    return (eax & 0xFFFF0000U) |
           static_cast<std::uint16_t>(
               static_cast<std::uint16_t>(eax) + value);
}
std::uint32_t sar1(std::uint32_t value) noexcept {
    return (value >> 1U) | (value & 0x80000000U);
}
std::uint32_t ror16(std::uint32_t value) noexcept {
    return (value >> 16U) | (value << 16U);
}
int step_sign(std::uint32_t step) noexcept {
    return step == 0xFFFF0000U ? -1 : 1;
}
CellPosition cell_from_index(const LevelState& level, std::size_t index) {
    if (level.width <= 0) throw std::runtime_error("invalid map width");
    return CellPosition{
        static_cast<int>(index % static_cast<std::size_t>(level.width)),
        static_cast<int>(index / static_cast<std::size_t>(level.width)),
    };
}
std::optional<CellPosition> neighbor_from_index(
    const LevelState& level,
    std::size_t index,
    std::int64_t delta) {
    const auto candidate =
        static_cast<std::int64_t>(index) + delta;
    if (candidate < 0 ||
        candidate >= static_cast<std::int64_t>(level.cell_words.size())) {
        return std::nullopt;
    }
    return cell_from_index(
        level, static_cast<std::size_t>(candidate));
}
std::int16_t dynamic_state_q(
    const LevelState& level,
    std::size_t index) noexcept {
    if (level.width <= 0 || index >= level.cell_words.size()) {
        return static_cast<std::int16_t>(0x8000U);
    }
    const auto row = static_cast<int>(
        index / static_cast<std::size_t>(level.width));
    const auto column = static_cast<int>(
        index % static_cast<std::size_t>(level.width));
    const auto it = std::find_if(
        level.group2.begin(),
        level.group2.end(),
        [row, column](const CellControlRecord& record) {
            return record.kind() == 1 &&
                   record.row() == row &&
                   record.column() == column;
        });
    if (it == level.group2.end()) {
        return static_cast<std::int16_t>(0x8000U);
    }
    return static_cast<std::int16_t>(
        static_cast<std::uint16_t>(it->raw[6]) << 12U);
}
bool dynamic_probe_hits(std::uint32_t value, std::int16_t q) noexcept {
    const auto lo = static_cast<std::uint32_t>(
        static_cast<std::int32_t>(q));
    if (lo == 0U) return false;
    const auto hi = 0U - lo;
    const auto probe = static_cast<std::uint32_t>(
        static_cast<std::int32_t>(
            static_cast<std::int16_t>(value)));
    return probe <= lo || probe >= hi;
}
std::uint32_t depth_for(
    const RendererTables& tables,
    const ViewCameraBasis& basis,
    std::uint32_t current_row_axis,
    std::uint32_t current_column_axis) {
    return FixedProjection::sweep_depth(SweepDepthInput{
        current_row_axis,
        current_column_axis,
        basis.origin_b,
        basis.origin_a,
        basis.sine_quarter,
        basis.sine_forward,
        tables.depth_numerator(),
    });
}
ViewSweepRecord empty_record() noexcept {
    ViewSweepRecord out{};
    out.resource_outer = 0x93U;
    out.resource_inner = 0x93U;
    return out;
}
ViewSweepRecord biased_record(
    LevelState& level,
    const RendererTables& tables,
    const ViewCameraBasis& basis,
    std::size_t index,
    std::uint32_t source,
    std::int16_t q,
    std::uint32_t current_row_axis,
    std::uint32_t current_column_axis,
    bool prepass) {
    const auto flags = level.cell_words[index];
    const auto code = static_cast<std::uint8_t>(flags & 0x003FU);
    const auto banks = EnvironmentViewModel::resource_banks(level);

    const auto bias =
        static_cast<std::uint32_t>(
            static_cast<std::uint16_t>(
                0U - static_cast<std::uint16_t>(q))) -
        0x8000U;
    auto adjusted = source + bias;
    if (low_u16(source) >= 0x8000U) {
        adjusted = source - bias;
    }

    ViewSweepRecord out{};
    out.depth = depth_for(
        tables, basis, current_row_axis, current_column_axis);
    out.resource_outer = banks.alternate_a[code];
    out.resource_inner = banks.primary_a[code];
    out.prepass = prepass ? 1U : 0U;
    out.texture_column_offset = (adjusted >> 4U) & 0x0FC0U;
    out.cell_index = static_cast<std::uint32_t>(index);
    out.shade_decrement =
        static_cast<std::uint32_t>((flags & 0xC000U) >> 14U);
    return out;
}

}  // namespace

std::vector<ViewSweepRecord> ViewSweepTraversalModel::build(
    ExplorationSessionState& session,
    const RendererTables& tables,
    ViewSweepOptions options) {
    const auto count = static_cast<std::size_t>(tables.c6d8());
    if (count > tables.view_angle_tenths.size()) {
        throw std::runtime_error(
            "renderer ray count exceeds SINTAB view-angle table");
    }
    if (session.level.cell_words.size() !=
        static_cast<std::size_t>(session.level.width) *
        static_cast<std::size_t>(session.level.height)) {
        throw std::runtime_error("level cell array does not match dimensions");
    }

    ExplorationWorldModel::clear_current_visibility(session.level);
    const auto basis =
        ViewSweepSetupModel::camera_basis(session, tables);

    std::vector<ViewSweepRecord> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        const auto ray = ViewSweepSetupModel::ray(
            session, tables, basis, static_cast<int>(i));
        out.push_back(trace_ray(
            session.level, tables, basis, ray, options));
    }
    return out;
}

ViewSweepRecord ViewSweepTraversalModel::trace_ray(
    LevelState& level,
    const RendererTables& tables,
    const ViewCameraBasis& basis,
    const ViewRaySetup& ray,
    ViewSweepOptions options) {
    const auto cell_count = level.cell_words.size();
    const auto width = static_cast<std::int16_t>(level.width);

    std::uint32_t row_axis = ray.sweep_255f0;
    std::uint32_t column_axis = ray.sweep_255e8;
    std::int16_t cx = high_s16(row_axis);
    std::int16_t bx = high_s16(column_axis);

    std::uint32_t row_edge =
        (static_cast<std::uint32_t>(
             static_cast<std::int32_t>(basis.origin_b_cell)) << 16U) +
        ray.step_a;
    std::uint32_t column_edge =
        (static_cast<std::uint32_t>(
             static_cast<std::int32_t>(basis.origin_a_cell)) << 16U) +
        ray.step_b;

    auto left_expr =
        mul_words(high_s16(row_edge), width) +
        static_cast<std::uint32_t>(
            static_cast<std::int32_t>(bx));
    auto right_expr =
        add_ax(
            mul_words(cx, width),
            high_u16(column_edge));

    enum class SweepState {
        CompareLeft,
        Left,
        CompareRight,
        Right,
    };
    auto state = SweepState::CompareLeft;

    // Valid shipped maps terminate naturally. The guard only prevents a
    // malformed custom level from turning the portable host into an infinite
    // loop; it does not alter valid CODE.2 traversal.
    const auto guard_limit = cell_count * 8U + 64U;
    std::size_t guard = 0;

    for (;;) {
        if (++guard > guard_limit) {
            throw std::runtime_error(
                "first-person sweep failed to leave malformed level");
        }

        if (state == SweepState::CompareLeft) {
            const auto edge = high_s16(column_edge);
            const bool jump_right =
                ray.left_compare_ge ? (bx >= edge) : (bx <= edge);
            state = jump_right ? SweepState::Right : SweepState::Left;
            continue;
        }
        if (state == SweepState::CompareRight) {
            const auto edge = high_s16(row_edge);
            const bool jump_left =
                ray.right_compare_ge ? (cx >= edge) : (cx <= edge);
            state = jump_left ? SweepState::Left : SweepState::Right;
            continue;
        }

        if (state == SweepState::Left) {
            // CODE.2 32BF6..32BFA computes ESI=cell index, then
            // SHL ESI,1 for the 16-bit cell word address. The
            // 32C1F SHR ESI,1 cancels that address scaling. left_expr
            // already stores the INDEX, not a doubled byte address.
            const auto raw_index = left_expr;
            if (raw_index >= cell_count) {
                return empty_record();
            }
            const auto index = static_cast<std::size_t>(raw_index);
            auto flags = level.cell_words[index];
            const bool treat_empty =
                (flags & 0x003FU) == 0U ||
                (options.hidden_topology_pass_through &&
                 (flags & 0x0800U) != 0U);

            ExplorationWorldModel::mark_visible_and_explored(
                level, cell_from_index(level, index));

            if (!treat_empty) {
                const bool prepass = (flags & 0x0200U) != 0U;
                if ((flags & 0x0080U) == 0U) {
                    row_axis = high_only(row_edge);
                    column_edge =
                        set_high_u16(column_edge,
                                     static_cast<std::uint16_t>(bx));
                    if (ray.step_a == 0xFFFF0000U) {
                        row_axis = add_high_word_one(row_axis);
                    }
                    const auto base =
                        (column_axis >> 4U) & 0x0FC0U;
                    const auto neighbor = neighbor_from_index(
                        level,
                        index,
                        -static_cast<std::int64_t>(
                            step_sign(ray.step_a)) *
                            static_cast<std::int64_t>(level.width));
                    return ViewSweepRecordModel::materialize(
                        level,
                        SweepMaterializationInput{
                            SweepSide::Left,
                            cell_from_index(level, index),
                            neighbor,
                            depth_for(
                                tables, basis,
                                row_axis, column_axis),
                            base,
                            ray.step_a == 0xFFFF0000U,
                            prepass,
                        });
                }

                auto value =
                    sar1(ray.tangent_b) + column_axis;
                const auto rotated = ror16(value);
                if (static_cast<std::uint16_t>(bx) ==
                    low_u16(rotated)) {
                    value = ror16(rotated);
                    const auto q = dynamic_state_q(level, index);
                    if (dynamic_probe_hits(value, q)) {
                        column_axis = value;
                        row_axis = high_only(row_edge) | 0x8000U;
                        return biased_record(
                            level, tables, basis, index,
                            column_axis, q,
                            row_axis, column_axis, prepass);
                    }
                }
            }

            row_edge += ray.step_a;
            column_axis += ray.tangent_b;
            bx = high_s16(column_axis);
            left_expr =
                mul_words(high_s16(row_edge), width) +
                static_cast<std::uint32_t>(
                    static_cast<std::int32_t>(bx));
            state = SweepState::CompareLeft;
            continue;
        }

        // CODE.2 32C07..32C10 computes EDI=cell index, then
        // SHL EDI,1 for the 16-bit cell word address. 32D09
        // SHR EDI,1 restores the exact index before bounds check.
        // Halving right_expr here would probe unrelated cells and
        // could trap near-cardinal rays in a false infinite sweep.
        const auto raw_index = right_expr;
        if (raw_index >= cell_count) {
            return empty_record();
        }
        const auto index = static_cast<std::size_t>(raw_index);
        auto flags = level.cell_words[index];
        const bool treat_empty =
            (flags & 0x003FU) == 0U ||
            (options.hidden_topology_pass_through &&
             (flags & 0x0800U) != 0U);

        ExplorationWorldModel::mark_visible_and_explored(
            level, cell_from_index(level, index));

        if (!treat_empty) {
            const bool prepass = (flags & 0x0200U) != 0U;
            if ((flags & 0x0080U) == 0U) {
                column_axis = high_only(column_edge);
                row_edge =
                    set_high_u16(row_edge,
                                 static_cast<std::uint16_t>(cx));
                if (ray.step_b == 0xFFFF0000U) {
                    column_axis =
                        add_high_word_one(column_axis);
                }
                const auto base =
                    (row_axis >> 4U) & 0x0FC0U;
                const auto neighbor = neighbor_from_index(
                    level,
                    index,
                    -static_cast<std::int64_t>(
                        step_sign(ray.step_b)));
                return ViewSweepRecordModel::materialize(
                    level,
                    SweepMaterializationInput{
                        SweepSide::Right,
                        cell_from_index(level, index),
                        neighbor,
                        depth_for(
                            tables, basis,
                            row_axis, column_axis),
                        base,
                        ray.step_b == 0xFFFF0000U,
                        prepass,
                    });
            }

            auto value =
                sar1(ray.tangent_a) + row_axis;
            const auto rotated = ror16(value);
            if (static_cast<std::uint16_t>(cx) ==
                low_u16(rotated)) {
                value = ror16(rotated);
                const auto q = dynamic_state_q(level, index);
                if (dynamic_probe_hits(value, q)) {
                    row_axis = value;
                    column_axis =
                        high_only(column_edge) | 0x8000U;
                    return biased_record(
                        level, tables, basis, index,
                        row_axis, q,
                        row_axis, column_axis, prepass);
                }
            }
        }

        column_edge += ray.step_b;
        row_axis += ray.tangent_a;
        cx = high_s16(row_axis);
        right_expr =
            add_ax(
                mul_words(cx, width),
                high_u16(column_edge));
        state = SweepState::CompareRight;
    }
}

}  // namespace rloft
