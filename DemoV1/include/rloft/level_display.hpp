#pragma once
#include <cstdint>
#include "rloft/resource_assets.hpp"
namespace rloft {
struct LevelDisplayRecord final { std::uint8_t palette_index{}; std::uint8_t raw_1{}; std::uint8_t raw_2{}; };
struct EnvironmentPhaseRecord final { int shade_mode{}; int first_record_screen_y{}; int first_record_azimuth{}; };
class LevelDisplayModel final {
public:
 [[nodiscard]] static LevelDisplayRecord level_record(int level_id);
 [[nodiscard]] static int palette_index(int level_id);
 [[nodiscard]] static int hour_phase(std::uint32_t time_of_day_16_16_hours) noexcept;
 [[nodiscard]] static EnvironmentPhaseRecord environment_phase(std::uint32_t time_of_day_16_16_hours);
 static void apply_daylight_phase(EnvironmentProfile& profile,const EnvironmentPhaseRecord& phase);
};
}  // namespace rloft
