#pragma once

#include <vector>

#include "rloft/startup_presenter.hpp"
#include "rloft/renderer_tables.hpp"
#include "rloft/surface_column_scaler.hpp"
#include "rloft/surface_shading.hpp"
#include "rloft/surface_texture.hpp"
#include "rloft/view_sweep_runs.hpp"

namespace rloft {

class SurfaceWallPresenter final {
public:
    static constexpr int kViewportX = 32;
    static constexpr int kViewportY = 4;
    static constexpr int kViewportWidth = 256;
    static constexpr int kViewportWorkingHeight = 256;

    explicit SurfaceWallPresenter(const GameSource& source)
        : textures_(source) {}

    // Shares the same original decoded RES4 texture source with the
    // separate SINTAB prepass; no second archive decoder is needed.
    [[nodiscard]] SurfaceTexture texture(int resource_id) const {
        return textures_.texture(resource_id);
    }

    void draw_main_runs(
        IndexedFramebuffer& framebuffer,
        const std::vector<ViewSweepRun>& runs,
        const SurfaceShadeTable& shades,
        const RendererTables& tables);

private:
    SurfaceTextureArchive textures_;
    SurfaceScaleState scale_state_{};
};

}  // namespace rloft
