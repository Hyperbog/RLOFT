#pragma once
#include <cstddef>
#include <cstdint>
#include "rloft/exploration_session.hpp"
#include "rloft/renderer_tables.hpp"
namespace rloft {
struct ViewCameraBasis final {
 std::uint32_t sine_forward{};
 std::uint32_t sine_quarter{};
 std::uint32_t origin_b{};
 std::uint32_t origin_a{};
 std::uint16_t origin_b_fraction{};
 std::uint16_t origin_b_fraction_neg{};
 std::uint16_t origin_a_fraction{};
 std::uint16_t origin_a_fraction_neg{};
 std::int16_t origin_b_cell{};
 std::int16_t origin_a_cell{};
 std::int16_t player_b_cell{};
 std::int16_t player_a_cell{};
};
struct ViewRaySetup final {
 int ray_index{};
 std::int32_t angle_tenths{};
 int quadrant{};
 std::uint32_t step_a{};
 std::uint32_t step_b{};
 bool left_compare_ge{};
 bool right_compare_ge{};
 std::uint32_t tangent_a{};
 std::uint32_t tangent_b{};
 std::uint16_t fractional_arg_a{};
 std::uint16_t fractional_arg_b{};
 std::uint32_t sweep_255f0{};
 std::uint32_t sweep_255e8{};
 std::int16_t cross_cx{};
 std::int16_t cross_bx{};
};
class ViewSweepSetupModel final {
public:
 [[nodiscard]] static ViewCameraBasis camera_basis(const ExplorationSessionState& session,const RendererTables& tables);
 [[nodiscard]] static ViewRaySetup ray(const ExplorationSessionState& session,const RendererTables& tables,const ViewCameraBasis& basis,int ray_index);
};
}  // namespace rloft
