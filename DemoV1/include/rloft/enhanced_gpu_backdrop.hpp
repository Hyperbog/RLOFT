#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "rloft/resource_assets.hpp"

namespace rloft {

// CPU-side bridge between the original indexed environment/ground and the
// optional OpenGL scene. The original presenter supplies the 256x256 WORLD
// before walls or RES3 objects are drawn, preventing ghost silhouettes.
// The 320x400 canvas is GL bottom-up RGBA; original pixels are top-down.
class EnhancedGpuBackdrop final {
public:
    static constexpr std::size_t kWorldWidth=256U;
    static constexpr std::size_t kWorldHeight=256U;
    static constexpr std::size_t kCanvasWidth=320U;
    static constexpr std::size_t kCanvasHeight=400U;

    [[nodiscard]] static constexpr std::size_t rgba_offset(
        std::size_t original_y,std::size_t original_x) noexcept {
        return ((kCanvasHeight-1U-original_y)*kCanvasWidth+
                original_x)*4U;
    }

    static void compose(const std::vector<std::uint8_t>& world_indices,
                        const Palette6& palette,
                        std::vector<std::uint8_t>& canvas,
                        bool preserve_menu) {
        if(world_indices.size()!=kWorldWidth*kWorldHeight ||
           canvas.size()!=kCanvasWidth*kCanvasHeight*4U)
            throw std::invalid_argument(
                "optional GPU source backdrop geometry does not match 256x256");
        // Preserve original CODE.2 top-edge menu and its source chrome when
        // the menu is active; that strip is composited after the 3D scene.
        for(std::size_t sy=0;sy<kWorldHeight;++sy) {
            const auto screen_y=sy+4U;
            if(preserve_menu && screen_y<64U)continue;
            for(std::size_t sx=0;sx<kWorldWidth;++sx) {
                const auto color=static_cast<std::size_t>(
                    world_indices[sy*kWorldWidth+sx])*3U;
                const auto at=rgba_offset(screen_y,sx+32U);
                for(std::size_t component=0;component<3U;++component) {
                    const auto six=palette.rgb[color+component];
                    canvas[at+component]=static_cast<std::uint8_t>(
                        (static_cast<unsigned>(six)*255U+31U)/63U);
                }
                canvas[at+3U]=255U;
            }
        }
    }

    // A failed GPU mesh/sprite upload occurs after source environment
    // replacement; restore this frame's authentic SOFTWARE world in the
    // same RGBA canvas so the fallback never shows erased ghost scenery.
    static void restore(const std::vector<std::uint32_t>& software_bgra,
                        std::vector<std::uint8_t>& canvas) {
        if(software_bgra.size()!=kCanvasWidth*kCanvasHeight ||
           canvas.size()!=kCanvasWidth*kCanvasHeight*4U)
            throw std::invalid_argument("optional GPU fallback canvas invalid");
        for(std::size_t y=4U;y<260U;++y) {
            for(std::size_t x=32U;x<288U;++x) {
                const auto raw=software_bgra[y*kCanvasWidth+x];
                const auto at=rgba_offset(y,x);
                canvas[at]=static_cast<std::uint8_t>((raw>>16U)&255U);
                canvas[at+1U]=static_cast<std::uint8_t>((raw>>8U)&255U);
                canvas[at+2U]=static_cast<std::uint8_t>(raw&255U);
                canvas[at+3U]=255U;
            }
        }
    }
};
} // namespace rloft
