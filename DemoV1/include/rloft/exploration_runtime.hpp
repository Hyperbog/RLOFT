#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "rloft/exploration_runtime_motion.hpp"
#include "rloft/inventory_runtime.hpp"
#include "rloft/combat_irq_message.hpp"
#include "rloft/inventory_save_codec.hpp"
#include "rloft/inventory_world_drop.hpp"
#include "rloft/inventory_party_order.hpp"
#include "rloft/level_world_state.hpp"
#include "rloft/level_continuity.hpp"
#include "rloft/level22_encounter.hpp"
#include "rloft/level22_viewport_target.hpp"
#include "rloft/npc_catalog.hpp"
#include "rloft/npc_scene.hpp"
#include "rloft/loose_item_scene.hpp"
#include "rloft/object_catalog.hpp"
#include "rloft/original_random.hpp"
#include "rloft/original_ground_texture_raster.hpp"
#include "rloft/original_ground_res4_audit.hpp"
#include "rloft/placed_scene_catalog.hpp"
#include "rloft/placed_object_scene.hpp"
#include "rloft/scene_source_banks.hpp"
#include "rloft/world_scene_record.hpp"
#include "rloft/world_scene_draw_calls.hpp"
#include "rloft/world_scene_render_plan.hpp"
#include "rloft/type7_vga_frame_trace.hpp"
#include "rloft/type7_vga_plane_masks.hpp"
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

struct SurfacePrepassPlanFailure final {
    std::uint32_t resource_id{};
    std::size_t first_ray{};
    std::string reason;
};

struct ExplorationRuntimeFrame final {
    ExplorationSceneSnapshot scene{};
    std::vector<ViewSweepRecord> sweep_records;
    // Read-only diagnostic inputs, taken from the ORIGINAL loaded SINTAB
    // through RendererTables (CODE.2 C6D8 and C738 respectively).
    // Not gameplay state and never a live copy of DOS 22220 RAM.
    std::size_t original_c6d8_ray_count{};
    std::uint32_t original_c738_scale_count{};
    std::vector<WorldSceneRecord> world_scene_records;
    std::uint64_t world_scene_revision{};
    // The public session() accessor permits camera motion independently
    // of gameplay inventory/world mutations. Preserve the exact 16.16
    // camera coordinates and facing used to build the hit rectangles.
    std::uint32_t source_coordinate_a{};
    std::uint32_t source_coordinate_b{};
    std::uint32_t source_facing{};
    int source_level_id{};
    // Fully source-derived CODE.2 34998 perspective rows. These are
    // NOT equivalent to completed 34A74 ground pixels until the live
    // RES4 class-4 resource selector and texture are bound.
    bool ground_perspective_ready{};
    // Exact original CODE.2 INIT+0x0E / level-header+7 RES4 lookup.
    // This records source identity only; the 34A74 live pixel mode
    // also requires FF50 and C026/C027 runtime decisions.
    bool ground_source_verified{};
    int ground_res4_base{-1};
    int ground_res4_relative{};
    int ground_res4_absolute{-1};
    std::string ground_source_failure;
    // Authenticated source ID and original 4096 pixel indices. Exactly
    // 4098 bytes with 2 unused CODE.2 C040+2 addressing spacer bytes.
    // The live presenter currently uses this tile only under its
    // source-active 0x93 material guard. DOS 335C8 initializes
    // 22220=0x800; the four materializers can also explicitly set
    // 22220=0 on 0x93. Both cases give FF50 lower rows=128; exact
    // dynamic partial-depth clipping remains a separate source gate.
    std::vector<std::uint8_t> ground_raster_source;
    std::uint8_t ground_phase_u{};
    std::uint8_t ground_phase_v{};
    std::array<GroundPerspectiveRow,
        GroundTextureRasterSpec::kMaxRows> ground_ff58{};
    std::vector<WorldSceneDrawCall> world_scene_draw_calls;
    std::vector<WorldSceneRenderPlan> world_scene_render_plans;
    // A bad source must not suppress unrelated RES3 draw calls.
    std::vector<WorldSceneRenderPlanFailure> scene_plan_failures;
    std::optional<Type7VgaFrameTrace> scene_vga_trace;
    // This trace is diagnostics/pixel rendering, NOT a prerequisite for
    // computing movement, world state, HUD or original hit-test records.
    // Keep its failure visible instead of aborting an otherwise valid frame.
    std::optional<std::string> scene_vga_trace_failure;
    // Failures isolated to one original source, not fatal to the frame.
    std::vector<Type7VgaSceneTraceFailure> scene_vga_scene_failures;
    std::uint8_t scene_video_page{};
    bool scene_individual_columns_mode{};
    std::vector<ViewSweepRun> sweep_runs;
    SurfaceShadeTable surface_shades{};
    std::vector<SurfacePrepassPlan> prepass_plans;
    // CODE.2-derived planning can encounter unsupported SINTAB pointer
    // underflow; the rest of the world frame still must be presented.
    std::vector<SurfacePrepassPlanFailure> prepass_plan_failures;
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

    // CODE.2 EF4D and C029 live state. Initialize from the original
    // executable's data object; later input/event paths may change them.
    [[nodiscard]] std::uint8_t scene_video_page() const noexcept {
        return scene_video_page_;
    }
    [[nodiscard]] bool scene_individual_columns_mode() const noexcept {
        return scene_individual_columns_mode_;
    }
    void set_scene_video_page(std::uint8_t page);
    void set_scene_individual_columns_mode(bool individual) noexcept {
        scene_individual_columns_mode_ = individual;
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

    // Original CODE.2 object-2 DGROUP BF44 story/message selector.
    // Shipped CODE.2 initializes it to zero. Existing Level22
    // pickup/death code now writes the original source values.
    // Rendering 1C21C and other global writers remain to be wired.
    [[nodiscard]] std::uint32_t original_story_message_bf44() const noexcept {
        return original_story_message_bf44_;
    }

    // Actual original CODE.2 225DF -> 1C21C queued source text,
    // with CF54=original IRQ30700+0x5A and CF59=1. The source
    // message drawing/compositor 1C258 remains unfinished.
    [[nodiscard]] const std::optional<CombatIrqMessageProjection>&
    original_pickup_ribbon() const noexcept {
        return original_pickup_ribbon_;
    }

    // Engine-only entry: the caller must have proven this destination from
    // original CODE.2 event/exit code. No invented portals or level order.
    void transition_to_level(
        const GameSource& source, const LevelTransitionLocation& target);

    [[nodiscard]] std::size_t stored_level_count() const noexcept {
        return level_continuity_.stored_count();
    }

    // This deliberately is NOT a general game SAVE/LOAD operation.
    // For an already initialized matching level/pose/TOD with
    // zero moving effects, accept the original 6FA0 four-byte
    // per-actor inactive or verified phase3 container records.
    // A selected OPEN live hand container prevents replacement;
    // imported actor contexts need source OBJECTS/hand validation.
    // Keep the SAVE image for lossless inventory-owned byte
    // re-emission; unsupported world-state effects fail before
    // changing the live session. No file IO or menu action.
    void import_matching_save_inventory(
        const std::vector<std::uint8_t>& original_save);

    // Requires an import plus unchanged source world revision/pose.
    // Emits a full original-format byte image over the preserved
    // backing, but only inventory-owned bytes have been refreshed.
    // Callers must NOT present this as a complete game save.
    [[nodiscard]] std::vector<std::uint8_t>
    export_matching_save_inventory() const;

    [[nodiscard]] bool has_inventory_save_backing() const noexcept {
        return inventory_save_backing_.has_value();
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

    [[nodiscard]] InventoryPartyOrderResult exchange_party_order(
        std::size_t first,std::size_t second,
        std::array<bool,4>& per_actor_inventory_modes);

    [[nodiscard]] LooseItemPickupResult pickup_level22_loose_item(
        std::size_t item_index,
        std::optional<std::uint32_t> original_irq_tick_30700 = std::nullopt);

    // Original CODE.2 inventory 13118, logical D key 0x44 ->
    // 2195C family-C loose item creation. The default probe
    // now reconstructs 2D5D0 mode 0x190's original collision
    // rectangle, source cell flags and live NPC/player checks.
    // Tests may inject a different probe. The world revision
    // changes only after a successful drop.
    [[nodiscard]] InventoryWorldDropResult drop_held_inventory_item(
        const InventoryWorldDropModel::PlacementProbe& can_place={});

    // Original Level16 item-89 completion is the ONLY route that
    // invokes CODE.2 numbered event 8 from an inventory D drop.
    // The caller supplies actual CINE08 playback; the runtime
    // stages source order, preflights reward, and rolls back on
    // interrupted playback to avoid half-completed native quests.
    [[nodiscard]] Level16RitualCompletion complete_pending_seed_ritual(
        const Level16RitualCompletionModel::Event8Playback& present_event_8,
        const InventoryWorldDropModel::PlacementProbe& can_place={});

    // Bind the previously reconstructed 223F4 viewport hit test to the
    // actual Level 22 encounter. Attack actions pass flags=0 so loose
    // pickups do not occlude NPC combat; pickup actions pass flags=1.
    // Both still honor the original highest-render-rank hit order.
    // Snapshot revisions reject selections after item array compaction.
    [[nodiscard]] std::optional<NpcDamageResult>
    damage_level22_assassin_at(
        const ExplorationRuntimeFrame& frame,
        int row,
        int column,
        std::uint16_t amount,
        std::uint16_t reaction_mode,
        const Level22EncounterModel::PlacementProbe& can_place = {});

    [[nodiscard]] std::optional<LooseItemPickupResult>
    pickup_level22_loose_item_at(
        const ExplorationRuntimeFrame& frame,
        int row,
        int column,
        std::optional<std::uint32_t> original_irq_tick_30700 = std::nullopt);

    [[nodiscard]] std::optional<WorldSceneTarget> world_scene_target_at(
        const std::vector<WorldSceneRecord>& records,
        int row,
        int column,
        std::uint8_t action_flags = 0x01U) const;

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
    std::uint32_t original_story_message_bf44_{};
    std::vector<std::uint8_t> original_pickup_ribbon_prefix_;
    std::optional<CombatIrqMessageProjection> original_pickup_ribbon_;
    NpcCatalog npc_catalog_{};
    ObjectCatalog object_catalog_{};
    SceneSourceBanks scene_source_banks_{};
    PlacedSceneCatalog placed_scene_catalog_{};
    std::optional<ResArchive> scene_shape_archive_;
    std::optional<OriginalGroundRes4Selection> ground_source_;
    std::string ground_source_failure_;
    std::optional<Type7VgaPlaneMaskModel> scene_vga_masks_;
    std::uint8_t scene_video_page_{};
    bool scene_individual_columns_mode_{};
    std::unordered_map<std::uint16_t, Type7SceneShape> scene_shape_cache_;
    OriginalRandom original_random_{};
    std::uint64_t world_scene_revision_{};
    LevelWorldState world_{};
    LevelContinuityStore level_continuity_{};
    InventoryRuntimeState inventory_{};
    std::optional<InventorySaveSnapshot> inventory_save_backing_;
    std::uint64_t inventory_save_world_revision_{};
    bool inventory_drop_c02f_center_fraction_{};
};

}  // namespace rloft
