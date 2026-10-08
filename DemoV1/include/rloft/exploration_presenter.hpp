#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "rloft/environment_presenter.hpp"
#include "rloft/exploration_hud_presenter.hpp"
#include "rloft/exploration_runtime.hpp"
#include "rloft/party_hud.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/surface_fill.hpp"
#include "rloft/surface_wall_presenter.hpp"
#include "rloft/type7_scene_pixel_executor.hpp"

namespace rloft {

struct ExplorationSurfaceCoverage final {
    std::size_t fallback_main_runs{};
    std::size_t sintab_main_runs_pending{};
    std::size_t sintab_prepass_runs_pending{};
    std::size_t type7_scene_calls_pending{};
    std::size_t type7_visible_columns_pending{};
    std::size_t type7_unvalidated_wide_runs{};
    std::size_t type7_calls_rasterized{};
    std::size_t type7_columns_rasterized{};
    std::size_t type7_indexed_writes{};
    std::size_t type7_calls_failed{};

    [[nodiscard]] bool complete() const noexcept {
        return sintab_main_runs_pending == 0 &&
               sintab_prepass_runs_pending == 0 &&
               type7_scene_calls_pending == 0 &&
               type7_visible_columns_pending == 0 &&
               type7_unvalidated_wide_runs == 0;
    }
};

// A failed draw is not silently equated with an occluded object. Preserve
// the original source identity and exact reason for offline diagnosis.
struct Type7SceneRasterFailure final {
    std::uint16_t resource_id{};
    std::uint16_t source_class{};
    std::uint16_t source_index{};
    std::string reason;
};

struct ExplorationPresentedFrame final {
    Palette6 palette{};
    ExplorationSurfaceCoverage surface_coverage{};
    std::vector<Type7SceneRasterFailure> type7_failures;
};

class ExplorationPresenter final {
public:
    explicit ExplorationPresenter(
        const RavenloftBootAssets& assets);

    [[nodiscard]] IndexedFramebuffer& framebuffer() noexcept {
        return framebuffer_;
    }
    [[nodiscard]] const IndexedFramebuffer& framebuffer() const noexcept {
        return framebuffer_;
    }

    // Composes the currently reconstructed CODE.2 exploration presentation:
    // persistent HUD base, upper environment/fill, lower gradient fill,
    // available surface columns, and read-only party panels.
    //
    // SINTAB-generated surface and prepass runs remain pending. Original
    // RES3/type-7 sprites now execute their certified SINTAB pixel bytecode
    // when the complete original VGA call trace is supported; all other
    // sprite calls remain explicitly pending with no approximate fallback.
    [[nodiscard]] ExplorationPresentedFrame present(
        const ExplorationRuntimeFrame& frame,
        const RendererTables& tables,
        int presentation_mode = 0);

    [[nodiscard]] static ExplorationSurfaceCoverage surface_coverage(
        const ExplorationRuntimeFrame& frame,
        const RendererTables& tables);

private:
    void draw_environment(
        const ExplorationRuntimeFrame& frame);

    const RavenloftBootAssets& assets_;
    IndexedFramebuffer framebuffer_;
    ExplorationHudPresenter hud_;
    SurfaceWallPresenter walls_;
    SurfaceFillTable fill_table_{};
    std::uint16_t res0_raster_base_{};
    std::uint16_t res1_portrait_base_{};
    std::unordered_map<std::uint16_t, std::vector<std::uint8_t>>
        type7_source_cache_;
};

}  // namespace rloft
