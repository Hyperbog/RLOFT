#pragma once
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include "rloft/startup_presenter.hpp"

namespace rloft {

// Enhanced-port substitute for the in-place DOS sprite-transform bracket
// around the SECOND copy of an equipped bracer/gauntlet. Never mutate the
// shared resource or its source pixels: another party actor may reuse it.
class EnhancedInventoryBlit final {
public:
    [[nodiscard]] static std::size_t reflected_horizontal(
        IndexedFramebuffer& dest, const Type1Raster& raster,
        int frame, int y, int x) {
        if (raster.width<=0 || raster.height<=0 ||
            frame<0 || frame>=raster.count) {
            throw std::invalid_argument("invalid reflected inventory sprite geometry");
        }
        const auto w=static_cast<std::size_t>(raster.width);
        const auto h=static_cast<std::size_t>(raster.height);
        const auto frames=static_cast<std::size_t>(raster.count);
        if (w>static_cast<std::size_t>(IndexedFramebuffer::kWidth) ||
            h>static_cast<std::size_t>(IndexedFramebuffer::kHeight) ||
            frames>raster.pixels.size()/(w*h) ||
            raster.pixels.size()!=frames*w*h) {
            throw std::invalid_argument("invalid reflected inventory sprite payload");
        }
        auto& out=dest.mutable_pixels();
        const auto frame_at=static_cast<std::size_t>(frame)*w*h;
        std::size_t writes=0;
        for (int sy=0;sy<raster.height;++sy) {
            const auto dy=static_cast<long long>(y)+sy;
            if(dy<0 || dy>=IndexedFramebuffer::kHeight) continue;
            for (int sx=0;sx<raster.width;++sx) {
                const auto dx=static_cast<long long>(x)+sx;
                if(dx<0 || dx>=IndexedFramebuffer::kWidth) continue;
                const auto original_x=w-1U-static_cast<std::size_t>(sx);
                const auto color=raster.pixels[
                    frame_at+static_cast<std::size_t>(sy)*w+original_x];
                if (raster.transparent_index!=0xFF &&
                    color==raster.transparent_index) continue;
                out[static_cast<std::size_t>(dy)*
                    IndexedFramebuffer::kWidth+
                    static_cast<std::size_t>(dx)]=color;
                ++writes;
            }
        }
        return writes;
    }
};
} // namespace rloft
