#include "rloft/level_display.hpp"
#include <array>
#include <stdexcept>
namespace rloft { namespace {
constexpr std::array<LevelDisplayRecord,32> kLevelDisplay{{
{4,10,3},{4,10,3},{4,10,3},{2,10,3},{2,10,3},{3,10,3},{3,10,3},{2,10,3},
{4,0,0},{2,10,3},{3,10,3},{3,0,0},{2,10,3},{2,10,3},{2,10,3},{4,10,3},
{4,0,0},{4,0,0},{3,0,0},{4,10,3},{3,10,3},{3,10,3},{4,0,0},{3,0,0},
{3,0,0},{4,0,0},{4,0,0},{3,10,3},{2,10,3},{0,10,3},{3,10,3},{4,0,0}}};
constexpr std::array<int,24> kShade{{10,10,10,10,10,10,10,9,8,7,5,4,2,1,0,1,2,4,5,7,8,9,10,10}};
constexpr std::array<int,24> kY{{25,-22,-70,-22,25,73,120,120,93,66,39,12,-15,-42,-70,-42,-15,12,39,66,93,120,120,73}};
constexpr std::array<int,24> kAz{{45,45,45,225,225,225,225,0,0,0,0,0,0,0,0,180,180,180,180,180,180,180,45,45}};
} 
LevelDisplayRecord LevelDisplayModel::level_record(int id){if(id<0||id>=32)throw std::out_of_range("level display record");return kLevelDisplay[static_cast<std::size_t>(id)];}
int LevelDisplayModel::palette_index(int id){return level_record(id).palette_index;}
int LevelDisplayModel::hour_phase(std::uint32_t t) noexcept {auto p=static_cast<std::uint16_t>(t>>16U);if(p==0xFFFFU)p=0x17U;return static_cast<int>(p%24U);}
EnvironmentPhaseRecord LevelDisplayModel::environment_phase(std::uint32_t t){const auto i=static_cast<std::size_t>(hour_phase(t));return {kShade[i],kY[i],kAz[i]};}
void LevelDisplayModel::apply_daylight_phase(EnvironmentProfile& p,const EnvironmentPhaseRecord& ph){
 if(ph.shade_mode>=10)throw std::invalid_argument("night environment selector cycling is stateful");
 if(p.placements.empty())throw std::runtime_error("environment profile has no first placement");
 auto& f=p.placements.front();f.resource_selector=0x18;f.azimuth_degrees=static_cast<std::int16_t>(ph.first_record_azimuth);f.screen_y=static_cast<std::int16_t>(ph.first_record_screen_y);
}
}  // namespace rloft
