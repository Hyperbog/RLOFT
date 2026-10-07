#include "rloft/surface_wall_presenter.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

void SurfaceWallPresenter::draw_main_runs(
    IndexedFramebuffer& framebuffer,
    const std::vector<ViewSweepRun>& runs,
    const SurfaceShadeTable& shades) {
    auto& pixels = framebuffer.mutable_pixels();

    for (const auto& run : runs) {
        if (run.pass != SweepRenderPass::Main ||
            !run.drawable()) {
            continue;
        }

        if (run.first_ray >=
                static_cast<std::size_t>(kViewportWidth) ||
            run.ray_count >
                static_cast<std::size_t>(kViewportWidth) -
                    run.first_ray) {
            throw std::runtime_error(
                "surface run exceeds first-person viewport");
        }

        const auto extent =
            (run.depth & 0xFFF8U) >> 1U;
        if (extent == 0U) {
            continue;
        }

        const auto texture =
            textures_.texture(static_cast<int>(run.resource_id));
        const auto source_column =
            static_cast<int>(run.texture_column_index);
        if (source_column < 0 ||
            source_column >= texture.width) {
            throw std::runtime_error(
                "sweep texture column exceeds RES4 surface width");
        }

        const auto subtract_pair =
            SurfaceShadingModel::packed_subtract(
                shades,
                run.resource_id,
                extent,
                run.shade_decrement,
                run.first_ray);

        const auto scaled =
            SurfaceColumnScaler::scale(
                texture,
                source_column,
                subtract_pair,
                extent,
                scale_state_);

        const int y0 = kViewportY + scaled.start_row;
        const int x0 =
            kViewportX + static_cast<int>(run.first_ray);

        for (std::size_t dx = 0; dx < run.ray_count; ++dx) {
            const int x = x0 + static_cast<int>(dx);
            for (std::size_t sy = 0;
                 sy < scaled.pixels.size();
                 ++sy) {
                const int y = y0 + static_cast<int>(sy);
                if (x < 0 || x >= IndexedFramebuffer::kWidth ||
                    y < 0 || y >= IndexedFramebuffer::kHeight) {
                    continue;
                }
                pixels[
                    static_cast<std::size_t>(y) *
                        IndexedFramebuffer::kWidth +
                    static_cast<std::size_t>(x)] =
                    scaled.pixels[sy];
            }
        }
    }
}

}  // namespace rloft
