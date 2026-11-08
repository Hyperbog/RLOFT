#include "rloft/level_display.hpp"
#include <cassert>
#include <stdexcept>
int main(){
 assert(rloft::LevelDisplayModel::palette_index(22)==4);
 assert(rloft::LevelDisplayModel::palette_index(29)==0);
 assert(rloft::LevelDisplayModel::hour_phase(0x000E0000U)==14);
 auto p=rloft::LevelDisplayModel::environment_phase(0x000E0000U);
 assert(p.shade_mode==0&&p.first_record_screen_y==-70&&p.first_record_azimuth==0);
 rloft::EnvironmentProfile ep{};ep.placements.push_back({69,45,64,25});
 rloft::LevelDisplayModel::apply_daylight_phase(ep,p);
 assert(ep.placements[0].resource_selector==0x18&&ep.placements[0].azimuth_degrees==0&&ep.placements[0].screen_y==-70);
 bool rejected=false;try{rloft::LevelDisplayModel::apply_daylight_phase(ep,rloft::LevelDisplayModel::environment_phase(0));}catch(const std::invalid_argument&){rejected=true;}assert(rejected);
}
