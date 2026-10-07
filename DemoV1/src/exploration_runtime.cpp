#include "rloft/exploration_runtime.hpp"

#include "rloft/exploration_world.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/options_menu.hpp"

#include <stdexcept>
#include <utility>

namespace rloft {

ExplorationRuntime::ExplorationRuntime(
    ExplorationSessionState session,
    RendererTables renderer_tables,
    std::vector<EnvironmentProfile> environment_profiles)
    : session_(std::move(session)),
      renderer_tables_(std::move(renderer_tables)),
      environment_profiles_(std::move(environment_profiles)) {
    motion_state_.integrator.heading = session_.facing;
}

ExplorationRuntime ExplorationRuntime::load_fresh(
    const GameSource& source,
    const std::vector<std::uint8_t>& chars) {
    return ExplorationRuntime(
        ExplorationSessionLoader::load_fresh_party(
            source, chars),
        RendererTables::load(source),
        decode_environment_profiles(
            source.read("INIT")));
}

void ExplorationRuntime::initialize_gameplay_state(
    const GameSource& source) {
    initialize_gameplay_state(
        source,
        [this](
            std::uint8_t count,
            std::uint8_t sides) {
            return original_random_.dice(
                count,
                sides);
        });
}

void ExplorationRuntime::initialize_gameplay_state(
    const GameSource& source,
    const LevelWorldStateCodec::DiceRoller& roll) {
    npc_catalog_ = NpcCatalog::load(source);
    object_catalog_ = ObjectCatalog::load(source);
    scene_source_banks_ = SceneSourceBanks::load(source);
    scene_shape_archive_.emplace(source.read("RES3"));
    world_ = LevelWorldStateCodec::build(
        session_.level,
        npc_catalog_,
        roll);
    inventory_ = InventoryRuntimeState::fresh(source);
    gameplay_state_initialized_ = true;
}

NpcDamageResult ExplorationRuntime::damage_level22_assassin(
    std::size_t npc_index,
    std::uint16_t amount,
    std::uint16_t reaction_mode,
    const Level22EncounterModel::PlacementProbe& can_place) {
    if (!gameplay_state_initialized_) {
        throw std::logic_error("gameplay state is not initialized");
    }
    if (session_.level_id != Level22Story::kLevelId) {
        throw std::logic_error("Assassin damage outside Level 22");
    }
    return Level22EncounterModel::damage_assassin(
        world_,
        npc_index,
        amount,
        reaction_mode,
        session_.facing,
        npc_catalog_,
        renderer_tables_,
        can_place);
}

LooseItemPickupResult ExplorationRuntime::pickup_level22_loose_item(
    std::size_t item_index) {
    if (!gameplay_state_initialized_) {
        throw std::logic_error("gameplay state is not initialized");
    }
    if (session_.level_id != Level22Story::kLevelId) {
        throw std::logic_error("Level 22 pickup outside Level 22");
    }
    return Level22EncounterModel::pickup_loose_item(
        world_,
        item_index,
        inventory_,
        session_.party,
        object_catalog_);
}

std::optional<WorldSceneTarget>
ExplorationRuntime::world_scene_target_at(
    const std::vector<WorldSceneRecord>& records,
    int row,
    int column) const {
    if (!gameplay_state_initialized_ ||
        !scene_shape_archive_.has_value()) {
        throw std::logic_error(
            "gameplay scene state is not initialized");
    }

    return WorldSceneSelectionModel::target_at(
        records,
        row,
        column,
        [this](std::uint16_t resource_id) {
            return scene_shape_archive_->
                decode_type7_geometry(
                    static_cast<int>(
                        resource_id));
        });
}


ExplorationRuntimeMotionResult ExplorationRuntime::tick_motion(
    ExplorationMotionInputs inputs) {
    return ExplorationRuntimeMotionModel::tick(
        session_,
        motion_state_,
        inputs,
        renderer_tables_);
}

ExplorationRuntimeFrame ExplorationRuntime::build_frame(
    int effect_fade,
    ViewSweepOptions sweep_options,
    const std::array<std::uint16_t, 32>& rng15) {
    ExplorationRuntimeFrame out{};

    out.sweep_records =
        ViewSweepTraversalModel::build(
            session_,
            renderer_tables_,
            sweep_options);

    ExplorationWorldModel::rebuild_automap_cache(
        session_.level,
        ExplorationWorldModel::party_cell(session_));

    out.scene = ExplorationSceneBuilder::build(
        session_,
        environment_profiles_,
        environment_phase_state_,
        rng15);

    if (gameplay_state_initialized_) {
        const auto defaults =
            OptionsState::defaults();
        const auto render_threshold =
            OptionsMenuModel::render_threshold(
                defaults.distancing(),
                session_.level.render_threshold_floor);
        auto npc_records =
            NpcSceneModel::build(
                world_,
                session_,
                npc_catalog_,
                renderer_tables_,
                scene_source_banks_,
                render_threshold);
        auto loose_records =
            LooseItemSceneModel::build(
                world_,
                session_,
                object_catalog_,
                renderer_tables_,
                scene_source_banks_,
                render_threshold);

        out.world_scene_records.reserve(
            npc_records.size() +
            loose_records.size());
        out.world_scene_records.insert(
            out.world_scene_records.end(),
            npc_records.begin(),
            npc_records.end());

        // CODE.2 emits class-1 placed objects between NPCs and family-C.
        // That family is not yet promoted to the live scene model; retain the
        // exact relative order of the integrated class-0 and class-2 records.
        out.world_scene_records.insert(
            out.world_scene_records.end(),
            loose_records.begin(),
            loose_records.end());

        if (out.world_scene_records.size() >
            NpcSceneModel::kMaxSceneRecords) {
            out.world_scene_records.resize(
                NpcSceneModel::kMaxSceneRecords);
        }
        WorldSceneOrderModel::assign_render_order(
            out.world_scene_records);
    }

    out.sweep_runs =
        ViewSweepRunModel::build(
            out.sweep_records);

    out.surface_shades =
        SurfaceShadingModel::build(
            out.scene.environment_mode,
            effect_fade);

    for (const auto& run : out.sweep_runs) {
        if (run.pass != SweepRenderPass::Prepass ||
            !run.drawable()) {
            continue;
        }
        const auto plan =
            SurfacePrepassModel::build(
                run,
                session_.level,
                renderer_tables_,
                out.surface_shades);
        if (plan.has_value()) {
            out.prepass_plans.push_back(*plan);
        }
    }

    return out;
}

}  // namespace rloft
