#pragma once

#include <cstdint>
#include <vector>

#include "rloft/exploration_session.hpp"
#include "rloft/level_world_state.hpp"
#include "rloft/placed_scene_catalog.hpp"
#include "rloft/renderer_tables.hpp"
#include "rloft/scene_source_banks.hpp"
#include "rloft/world_scene_record.hpp"

namespace rloft {

class PlacedObjectSceneModel final {
public:
    // Level-22-safe faithful subset of CODE.2 2BBA0..2BDF0. The three
    // shipped Level 22 family-B types (62..64) bypass 2B2F4's mutable
    // class-1 cache domain, so their frame selection is fully determined by
    // the current live record, camera, and INIT descriptor.
    [[nodiscard]] static std::vector<WorldSceneRecord> build(
        const LevelWorldState& world,
        const ExplorationSessionState& session,
        const PlacedSceneCatalog& catalog,
        const RendererTables& tables,
        const SceneSourceBanks& banks,
        std::uint8_t render_threshold);
};

}  // namespace rloft
