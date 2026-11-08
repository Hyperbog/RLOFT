#include "rloft/exploration_scene.hpp"
#include <cassert>
#include <vector>
int main(){
 rloft::ExplorationSessionState s{};s.level_id=22;s.coordinate_a=0x00048000;s.coordinate_b=0x00048000;s.facing=270;s.time_of_day_16_16_hours=0x000E0000;
 s.level.width=30;s.level.height=15;s.level.cell_words.assign(450,0);s.level.environment_profile=1;
 std::vector<std::uint8_t> chars(rloft::PartyImage::kSize,0xFF);chars[0]=0;chars[0x10]=0;chars[0x11]=0;s.party=rloft::PartyImage::from_bytes(chars);
 rloft::EnvironmentProfile p0{};p0.placements.push_back({});
 rloft::EnvironmentProfile p1{};p1.res8_base=45;p1.placements.push_back({69,45,64,25});p1.placements.push_back({});
 std::vector<rloft::EnvironmentProfile> ps{p0,p1};
 rloft::EnvironmentPhaseState phase{};
 auto snap=rloft::ExplorationSceneBuilder::build(s,ps,phase);
 assert(snap.palette_index==4);assert(snap.environment_mode==0);assert(!snap.environment_commands.empty());
 // First profile placement follows 14:00 phase: selector 0x18 + base45 = 69.
 const auto& last=snap.environment_commands.back();assert(last.resource_id==69);
 assert(snap.frame.party_cell.column==4&&snap.frame.party_cell.row==4);
 // Night selector state starts at direct RES8 selector 0 and advances only after a day reset.
 s.time_of_day_16_16_hours=0;
 snap=rloft::ExplorationSceneBuilder::build(s,ps,phase);
 assert(snap.environment_mode==10);
 assert(snap.environment_commands.back().resource_id==0);
 s.time_of_day_16_16_hours=0x000E0000;snap=rloft::ExplorationSceneBuilder::build(s,ps,phase);
 s.time_of_day_16_16_hours=0;snap=rloft::ExplorationSceneBuilder::build(s,ps,phase);
 assert(snap.environment_commands.back().resource_id==1);
}
