#pragma once
#include <cstdint>
#include <optional>
#include "rloft/environment_view.hpp"
#include "rloft/exploration_world.hpp"
namespace rloft {
enum class SweepSide { Left, Right };
struct ViewSweepRecord final {
 std::uint32_t depth{};
 std::uint32_t resource_outer{};
 std::uint32_t resource_inner{};
 std::uint32_t prepass{};
 std::uint32_t texture_column_offset{};
 std::uint32_t cell_index{};
 std::uint32_t shade_decrement{};
};
struct SweepMaterializationInput final {
 SweepSide side{SweepSide::Right};
 CellPosition cell{};
 std::optional<CellPosition> special_neighbor;
 std::uint32_t depth{};
 std::uint32_t texture_base{};
 bool axis_step_negative{};
 bool prepass{};
};
class ViewSweepRecordModel final {
public:
 [[nodiscard]] static ViewSweepRecord materialize(LevelState& level,const SweepMaterializationInput& input);
};
}  // namespace rloft
