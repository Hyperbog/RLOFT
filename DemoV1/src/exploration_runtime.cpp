#include "rloft/exploration_runtime.hpp"

#include "rloft/exploration_world.hpp"
#include "rloft/resource_assets.hpp"

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
    const GameSource& source,
    const LevelWorldStateCodec::DiceRoller& roll) {
    npc_catalog_ = NpcCatalog::load(source);
    object_catalog_ = ObjectCatalog::load(source);
    world_ = LevelWorldStateCodec::build(
        session_.level,
        npc_catalog_,
        roll);
    inventory_ = InventoryRuntimeState::fresh(source);
    gameplay_state_initialized_ = true;
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
