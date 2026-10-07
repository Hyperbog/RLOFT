#pragma once
#include <array>
#include <cstddef>
#include <optional>
namespace rloft {
struct ExplorationHitRect final {
 int row_min{},column_min{},row_max{},column_max{};
 [[nodiscard]] bool contains(int row,int column) const noexcept {
  return row>=row_min&&row<=row_max&&column>=column_min&&column<=column_max;
 }
};
class ExplorationHitRegionModel final {
public:
 static constexpr std::size_t kRegionCount=21;
 [[nodiscard]] static const std::array<ExplorationHitRect,kRegionCount>& regions() noexcept;
 [[nodiscard]] static std::optional<int> region_at(int row,int column) noexcept;
 [[nodiscard]] static std::optional<std::size_t> inventory_actor_at(int row,int column) noexcept;
};
}  // namespace rloft
