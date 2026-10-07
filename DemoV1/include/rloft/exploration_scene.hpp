#pragma once
#include <array>
#include <cstdint>
#include <vector>
#include "rloft/environment_projection.hpp"
#include "rloft/exploration_frame.hpp"
#include "rloft/level_display.hpp"
#include "rloft/party_presentation.hpp"
#include "rloft/resource_assets.hpp"
namespace rloft {
struct ExplorationSceneSnapshot final {
    ExplorationFrameState frame{};
    int palette_index{};
    int environment_mode{};
    std::vector<EnvironmentDrawCommand> environment_commands;
    std::array<PartyActorPresentation,PartyImage::kActorCount> party;
};
class ExplorationSceneBuilder final {
public:
 [[nodiscard]] static ExplorationSceneSnapshot build(
   const ExplorationSessionState& session,
   const std::vector<EnvironmentProfile>& profiles,
   EnvironmentPhaseState& phase_state,
   const std::array<std::uint16_t,32>& rng15={});
};
}  // namespace rloft
