#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/exploration_session.hpp"
#include "rloft/level_world_state.hpp"
#include "rloft/object_catalog.hpp"
#include "rloft/renderer_tables.hpp"
#include "rloft/scene_source_banks.hpp"
#include "rloft/world_scene_record.hpp"

namespace rloft {

class LooseItemSceneModel final {
public:
    // CODE.2 2BDF4..2BFDC. Emits family-C loose/pickable items into the
    // common generated first-person scene-record format.
    [[nodiscard]] static std::vector<WorldSceneRecord> build(
        const LevelWorldState& world,
        const ExplorationSessionState& session,
        const ObjectCatalog& catalog,
        const RendererTables& tables,
        const SceneSourceBanks& banks,
        std::uint8_t render_threshold);
};

}  // namespace rloft
