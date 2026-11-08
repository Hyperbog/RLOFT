#pragma once
#include <cstdint>
namespace rloft {
struct SweepDepthInput final {
 std::uint32_t current_a{},current_b{},origin_a{},origin_b{},factor_a{},factor_b{},numerator{};
};
class FixedProjection final {
public:
 [[nodiscard]] static std::uint32_t multiply_signed_factor(std::uint32_t value,std::uint32_t factor) noexcept;
 [[nodiscard]] static std::uint32_t multiply_unsigned_factor(std::uint32_t value,std::uint32_t factor) noexcept;
 [[nodiscard]] static std::uint32_t sweep_depth(const SweepDepthInput& input);
};
}  // namespace rloft
