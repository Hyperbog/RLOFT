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

void ExplorationRuntime::set_scene_video_page(
    std::uint8_t page) {
    if (page > 1U) {
        throw std::out_of_range(
            "scene VGA page must be 0 or 1");
    }
    scene_video_page_ = page;
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
    placed_scene_catalog_ = PlacedSceneCatalog::load(source);
    scene_shape_archive_.emplace(source.read("RES3"));
    scene_shape_cache_.clear();
    // A real DOS install supplies CODE.2. Tests with partial synthetic
    // resource sets can omit it, leaving the diagnostic VGA trace absent.
    scene_vga_masks_.reset();
    scene_video_page_ = 0U;
    scene_individual_columns_mode_ = false;
    if (source.contains("CODE.2")) {
        scene_vga_masks_.emplace(
            Type7VgaPlaneMaskModel::load(source));
        scene_video_page_ =
            scene_vga_masks_->initial_video_page();
        scene_individual_columns_mode_ =
            scene_vga_masks_->initial_individual_columns_mode();
    }
    world_ = LevelWorldStateCodec::build(
        session_.level,
        npc_catalog_,
        roll);
    inventory_ = InventoryRuntimeState::fresh(source);
    gameplay_state_initialized_ = true;
    ++world_scene_revision_;
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
    // The original death/drop routine can mutate HP and item records
    // before a later source-data or placement error throws. Invalidate
    // prior scene indices before starting, even on exceptional exit.
    ++world_scene_revision_;
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
    // 21704 mutates held inventory and compacts the loose-item array
    // before checking Level 22 story completion. Invalidate first.
    ++world_scene_revision_;
    return Level22EncounterModel::pickup_loose_item(
        world_,
        item_index,
        inventory_,
        session_.party,
        object_catalog_);
}

std::optional<NpcDamageResult>
ExplorationRuntime::damage_level22_assassin_at(
    const ExplorationRuntimeFrame& frame,
    int row,
    int column,
    std::uint16_t amount,
    std::uint16_t reaction_mode,
    const Level22EncounterModel::PlacementProbe& can_place) {
    // 223F4's bit-0 source-class-2 gate must remain clear during
    // combat selection: loose items are pickup targets, not enemies.
    if (!gameplay_state_initialized_ ||
        frame.world_scene_revision != world_scene_revision_) {
        throw std::logic_error(
            "Assassin target was selected from a stale world frame");
    }
    const auto selected = world_scene_target_at(
        frame.world_scene_records, row, column, 0U);
    const auto npc_index =
        Level22ViewportTargetModel::assassin_index(
            selected, world_);
    if (!npc_index.has_value()) {
        return std::nullopt;
    }
    return damage_level22_assassin(
        *npc_index, amount, reaction_mode, can_place);
}

std::optional<LooseItemPickupResult>
ExplorationRuntime::pickup_level22_loose_item_at(
    const ExplorationRuntimeFrame& frame,
    int row,
    int column) {
    // 224EF requires bit 0 for class-2 hit-test eligibility.
    if (!gameplay_state_initialized_ ||
        frame.world_scene_revision != world_scene_revision_) {
        throw std::logic_error(
            "pickup target was selected from a stale world frame");
    }
    const auto selected = world_scene_target_at(
        frame.world_scene_records, row, column, 0x01U);
    const auto item_index =
        Level22ViewportTargetModel::loose_item_index(
            selected, world_);
    if (!item_index.has_value()) {
        return std::nullopt;
    }
    return pickup_level22_loose_item(*item_index);
}

std::optional<WorldSceneTarget>
ExplorationRuntime::world_scene_target_at(
    const std::vector<WorldSceneRecord>& records,
    int row,
    int column,
    std::uint8_t action_flags) const {
    if (!gameplay_state_initialized_ ||
        !scene_shape_archive_.has_value()) {
        throw std::logic_error(
            "gameplay scene state is not initialized");
    }

    return WorldSceneSelectionModel::target_at(
        records,
        row,
        column,
        action_flags,
        [this](std::uint16_t resource_id) {
            return scene_shape_archive_->
                decode_type7_geometry(
                    static_cast<int>(
                        resource_id));
        },
        [this](std::uint16_t source_index) {
            const auto& placed =
                world_.placed_objects.at(
                    source_index);
            return WorldSceneSelectionModel::
                class1_type_is_interactive(
                    placed.object_type_id);
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
    out.world_scene_revision = world_scene_revision_;
    out.scene_video_page = scene_video_page_;
    out.scene_individual_columns_mode =
        scene_individual_columns_mode_;

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
        auto placed_records =
            PlacedObjectSceneModel::build(
                world_,
                session_,
                placed_scene_catalog_,
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
            placed_records.size() +
            loose_records.size());
        out.world_scene_records.insert(
            out.world_scene_records.end(),
            npc_records.begin(),
            npc_records.end());
        out.world_scene_records.insert(
            out.world_scene_records.end(),
            placed_records.begin(),
            placed_records.end());
        out.world_scene_records.insert(
            out.world_scene_records.end(),
            loose_records.begin(),
            loose_records.end());

        // CODE.2 applies each emitted source's optional scene-cell influence
        // after projection/threshold acceptance and before final rendering.
        for (const auto& record :
             out.world_scene_records) {
            std::uint8_t step = 0U;
            std::uint32_t cell_index = 0U;

            if (record.source_class == 0U) {
                const auto& live =
                    world_.npcs.at(
                        record.source_index);
                step =
                    npc_catalog_.at(
                        live.npc_type_id).
                        scene_influence_step;
                cell_index =
                    live.cached_cell_index;
            } else if (
                record.source_class == 1U) {
                const auto& live =
                    world_.placed_objects.at(
                        record.source_index);
                step =
                    placed_scene_catalog_.at(
                        live.object_type_id).
                        scene_influence_step;
                cell_index =
                    live.cached_cell_index;
            } else if (
                record.source_class == 2U) {
                const auto& live =
                    world_.loose_items.at(
                        record.source_index);
                step =
                    object_catalog_.at(
                        live.object_type_id).
                        scene_influence_step;
                cell_index =
                    live.cached_cell_index;
            }

            if (step != 0U) {
                ViewSweepInfluenceModel::apply(
                    out.sweep_records,
                    step,
                    cell_index,
                    session_.level.width);
            }
        }

        if (out.world_scene_records.size() >
            NpcSceneModel::kMaxSceneRecords) {
            out.world_scene_records.resize(
                NpcSceneModel::kMaxSceneRecords);
        }
        WorldSceneOrderModel::assign_render_order(
            out.world_scene_records);
        out.world_scene_draw_calls =
            WorldSceneDrawCallModel::build(
                out.world_scene_records,
                world_);
        if (!scene_shape_archive_.has_value()) {
            throw std::logic_error(
                "gameplay scene archive is not initialized");
        }
        // Scene RES3 shapes are immutable during a level session. Cache the
        // decoded display lists once, and preserve reference stability on
        // insertion through std::unordered_map.
        const auto shape_for_id =
            [this](std::uint16_t resource_id)
                -> const Type7SceneShape& {
                const auto existing =
                    scene_shape_cache_.find(resource_id);
                if (existing != scene_shape_cache_.end()) {
                    return existing->second;
                }
                const auto inserted =
                    scene_shape_cache_.emplace(
                        resource_id,
                        scene_shape_archive_->decode_type7(
                            static_cast<int>(resource_id)));
                return inserted.first->second;
            };
        out.world_scene_render_plans =
            WorldSceneRenderPlanModel::build(
                out.world_scene_draw_calls,
                renderer_tables_,
                out.sweep_records,
                shape_for_id);
        if (scene_vga_masks_.has_value()) {
            out.scene_vga_trace =
                Type7VgaFrameTraceModel::build(
                    out.world_scene_render_plans,
                    renderer_tables_,
                    *scene_vga_masks_,
                    scene_video_page_,
                    scene_individual_columns_mode_,
                    shape_for_id);
        }
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
