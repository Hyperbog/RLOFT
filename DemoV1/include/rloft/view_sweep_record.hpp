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
 // True only when 330E7/33278/3337F/33487 source record
 // materialization actually executed. Synthetic empty_record()
 // must not be replayed as an original 22220 side effect.
 bool original_materializer_executed{};
 // Original CODE.2 32F4C writes a 28-byte sentinel ray record.
 // It is a real DOS routine but does NOT write global nearest 22220.
 // A default-constructed native test record sets neither provenance.
 bool original_sentinel_initializer_executed{};
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
