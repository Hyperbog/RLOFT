#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

#include "rloft/exploration_session.hpp"
#include "rloft/level_world_state.hpp"
#include "rloft/npc_catalog.hpp"
#include "rloft/renderer_tables.hpp"
#include "rloft/scene_source_banks.hpp"
#include "rloft/world_scene_record.hpp"

namespace rloft {

using NpcSceneEmission = WorldSceneRecord;

class NpcSceneModel final {
public:
    static constexpr std::size_t kMaxSceneRecords = 0x190U;

    using Random15 = std::function<std::uint16_t()>;

    // CODE.2 2B6D0..2BB91, stopping immediately before the 21BF0
    // interaction-bound calculation. Visibility, projection, threshold,
    // animation/directional frame selection, source identity, and scale bias
    // are preserved exactly for the supplied-build NPC definitions.
    [[nodiscard]] static std::vector<NpcSceneEmission> build(
        const LevelWorldState& world,
        const ExplorationSessionState& session,
        const NpcCatalog& catalog,
        const RendererTables& tables,
        const SceneSourceBanks& banks,
        std::uint8_t render_threshold,
        const Random15& shared_random15 = {});
};

}  // namespace rloft
