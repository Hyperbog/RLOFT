#pragma once

#include <cstddef>

#include "rloft/environment_presenter.hpp"
#include "rloft/exploration_hud_presenter.hpp"
#include "rloft/exploration_runtime.hpp"
#include "rloft/party_hud.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/surface_fill.hpp"
#include "rloft/surface_wall_presenter.hpp"

namespace rloft {

struct ExplorationSurfaceCoverage final {
    std::size_t fallback_main_runs{};
    std::size_t sintab_main_runs_pending{};
    std::size_t sintab_prepass_runs_pending{};

    [[nodiscard]] bool complete() const noexcept {
        return sintab_main_runs_pending == 0 &&
               sintab_prepass_runs_pending == 0;
    }
};

struct ExplorationPresentedFrame final {
    Palette6 palette{};
    ExplorationSurfaceCoverage surface_coverage{};
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
    // SINTAB generated-rasterizer runs are intentionally counted rather than
    // approximated. They remain pending until their bytecode semantics can be
    // validated against canonical SINTAB bytes.
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
};

}  // namespace rloft
