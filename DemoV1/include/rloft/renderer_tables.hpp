#pragma once
#include <array>
#include <cstdint>
#include <vector>
#include "rloft/game_source.hpp"
namespace rloft {
struct RendererTables final {
 std::array<std::uint32_t,450> sine_q16_signmag{};
 std::array<std::uint32_t,900> tangent_q16{};
 std::array<std::uint16_t,128> projection_a{};
 std::array<std::uint16_t,128> projection_b{};
 std::array<std::uint32_t,8> constants{};
 std::array<std::int32_t,256> view_angle_tenths{};
 std::uint32_t scale_block_count{};
 std::uint32_t runtime_lookup_max{};
 std::uint32_t renderer_aux_value{};
 std::vector<std::uint16_t> runtime_lookup;
 [[nodiscard]] static RendererTables parse_prefix(const std::vector<std::uint8_t>& bytes);
 [[nodiscard]] static RendererTables load(const GameSource& source);
 [[nodiscard]] std::uint32_t c6d8() const noexcept{return constants[0];}
 [[nodiscard]] std::uint32_t c6d4() const noexcept{return constants[1];}
 [[nodiscard]] std::uint32_t c6d0() const noexcept{return constants[2];}
 [[nodiscard]] std::uint32_t c6dc() const noexcept{return constants[3];}
 [[nodiscard]] std::uint32_t c6e0() const noexcept{return constants[4];}
 [[nodiscard]] std::uint32_t grid_projection_constant() const noexcept{return constants[5];}
 [[nodiscard]] std::uint32_t c6cc() const noexcept{return constants[6];}
 [[nodiscard]] std::uint32_t depth_numerator() const noexcept{return constants[7];}
};
}  // namespace rloft
