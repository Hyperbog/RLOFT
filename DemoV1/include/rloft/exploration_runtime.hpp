#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "rloft/exploration_runtime_motion.hpp"
#include "rloft/exploration_scene.hpp"
#include "rloft/micro_rasterizer_plan.hpp"
#include "rloft/renderer_tables.hpp"
#include "rloft/surface_prepass.hpp"
#include "rloft/surface_shading.hpp"
#include "rloft/view_sweep_runs.hpp"
#include "rloft/view_sweep_traversal.hpp"

namespace rloft {

struct ExplorationRuntimeFrame final {
    ExplorationSceneSnapshot scene{};
    std::vector<ViewSweepRecord> sweep_records;
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

    [[nodiscard]] ExplorationRuntimeMotionResult tick_motion(
        ExplorationMotionInputs inputs);

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
};

}  // namespace rloft
