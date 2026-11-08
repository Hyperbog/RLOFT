#include "rloft/exploration_scene.hpp"
#include <cstddef>
namespace rloft {
ExplorationSceneSnapshot ExplorationSceneBuilder::build(
 const ExplorationSessionState& session,
 const std::vector<EnvironmentProfile>& profiles,
 EnvironmentPhaseState& phase_state,
 const std::array<std::uint16_t,32>& rng15){
 ExplorationSceneSnapshot out{};
 out.frame=ExplorationFrameBuilder::build(session,profiles);
 out.palette_index=LevelDisplayModel::palette_index(static_cast<int>(session.level_id));
 const auto phase=LevelDisplayModel::environment_phase(session.time_of_day_16_16_hours);
 out.environment_mode=phase.shade_mode;
 if(out.frame.environment_profile){
   auto profile=*out.frame.environment_profile;
   LevelDisplayModel::apply_phase(profile,phase,phase_state);
   out.environment_commands=EnvironmentProjectionModel::build(
      profile,static_cast<int>(session.facing),phase.shade_mode,rng15);
 }
 for(std::size_t i=0;i<out.party.size();++i) out.party[i]=PartyPresentationModel::actor(session.party,i);
 return out;
}
}  // namespace rloft
