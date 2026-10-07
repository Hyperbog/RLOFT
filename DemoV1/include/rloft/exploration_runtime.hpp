#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/exploration_runtime_motion.hpp"
#include "rloft/inventory_runtime.hpp"
#include "rloft/level_world_state.hpp"
#include "rloft/level22_encounter.hpp"
#include "rloft/npc_catalog.hpp"
#include "rloft/npc_scene.hpp"
#include "rloft/loose_item_scene.hpp"
#include "rloft/object_catalog.hpp"
#include "rloft/original_random.hpp"
#include "rloft/placed_scene_catalog.hpp"
#include "rloft/placed_object_scene.hpp"
#include "rloft/scene_source_banks.hpp"
#include "rloft/world_scene_record.hpp"
#include "rloft/world_scene_selection.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/exploration_scene.hpp"
#include "rloft/micro_rasterizer_plan.hpp"
#include "rloft/renderer_tables.hpp"
#include "rloft/surface_prepass.hpp"
#include "rloft/surface_shading.hpp"
#include "rloft/view_sweep_runs.hpp"
#include "rloft/view_sweep_influence.hpp"
#include "rloft/view_sweep_traversal.hpp"

namespace rloft {

struct ExplorationRuntimeFrame final {
    ExplorationSceneSnapshot scene{};
    std::vector<ViewSweepRecord> sweep_records;
    std::vector<WorldSceneRecord> world_scene_records;
    std::vector<ViewSweepRun> sweep_runs;
    SurfaceShadeTable surface_shades{};
    std::vector<SurfacePrepassPlan> prepass_plans;
};

class ExplorationRuntime final {
public:
    ExplorationRuntime(
        ExplorationSessionState session,
        RendererTables renderer_tables,
        std::vector<EnvironmentProfile> environment_profiles);

    [[nodiscard]] static ExplorationRuntime load_fresh(
        const GameSource& source,
        const std::vector<std::uint8_t>& chars);

    [[nodiscard]] ExplorationSessionState& session() noexcept {
        return session_;
    }
    [[nodiscard]] const ExplorationSessionState& session() const noexcept {
        return session_;
    }
    [[nodiscard]] const RendererTables& renderer_tables() const noexcept {
        return renderer_tables_;
    }

    // Activates the mutable CODE.2 world/inventory layer for the currently
    // loaded level. Archive NPC hit points require the caller to supply the
    // faithful game dice path; direct-save records ignore that callback.
    void initialize_gameplay_state(
        const GameSource& source);

    void initialize_gameplay_state(
        const GameSource& source,
        const LevelWorldStateCodec::DiceRoller& roll);

    [[nodiscard]] bool gameplay_state_initialized() const noexcept {
        return gameplay_state_initialized_;
    }
    [[nodiscard]] LevelWorldState& world() noexcept { return world_; }
    [[nodiscard]] const LevelWorldState& world() const noexcept { return world_; }
    [[nodiscard]] InventoryRuntimeState& inventory() noexcept { return inventory_; }
    [[nodiscard]] const InventoryRuntimeState& inventory() const noexcept { return inventory_; }
    [[nodiscard]] const NpcCatalog& npc_catalog() const noexcept { return npc_catalog_; }
    [[nodiscard]] const ObjectCatalog& object_catalog() const noexcept { return object_catalog_; }

    [[nodiscard]] ExplorationRuntimeMotionResult tick_motion(
        ExplorationMotionInputs inputs);

    [[nodiscard]] NpcDamageResult damage_level22_assassin(
        std::size_t npc_index,
        std::uint16_t amount,
        std::uint16_t reaction_mode,
        const Level22EncounterModel::PlacementProbe& can_place = {});

    [[nodiscard]] LooseItemPickupResult pickup_level22_loose_item(
        std::size_t item_index);

    [[nodiscard]] std::optional<WorldSceneTarget> world_scene_target_at(
        const std::vector<WorldSceneRecord>& records,
        int row,
        int column) const;

    // Builds the canonical exploration render/update snapshot. The sweep
    // mutates only current visibility/explored state; automap is rebuilt
    // immediately afterward so presentation and simulation share one truth.
    [[nodiscard]] ExplorationRuntimeFrame build_frame(
        int effect_fade = 0,
        ViewSweepOptions sweep_options = {},
        const std::array<std::uint16_t, 32>& rng15 = {});

private:
    ExplorationSessionState session_{};
    RendererTables renderer_tables_{};
    std::vector<EnvironmentProfile> environment_profiles_;
    EnvironmentPhaseState environment_phase_state_{};
    ExplorationRuntimeMotionState motion_state_{};
    bool gameplay_state_initialized_{};
    NpcCatalog npc_catalog_{};
    ObjectCatalog object_catalog_{};
    SceneSourceBanks scene_source_banks_{};
    PlacedSceneCatalog placed_scene_catalog_{};
    std::optional<ResArchive> scene_shape_archive_;
    OriginalRandom original_random_{};
    LevelWorldState world_{};
    InventoryRuntimeState inventory_{};
};

}  // namespace rloft
