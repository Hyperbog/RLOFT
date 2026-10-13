#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "rloft/resource_assets.hpp"
#include "rloft/type7_column_traversal.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

// Enhanced PC renderer, not an attempt to emulate CODE.2's VGA page
// sequencer or self-modifying SINTAB x86. Uses the existing decoded
// RES3 EH type-7 128-row pixels, explicit transparency mask, original
// depth-clipped screen columns, and the actual SINTAB scale_step table.
// All positions are indexed in the existing 320x400 game framebuffer.
class EnhancedType7Compositor final {
public:
    static constexpr int kViewportLeft = 32;
    static constexpr int kViewportRight = 287;
    static constexpr int kViewportTop = 4;
    static constexpr int kViewportBottom = 259;
    static constexpr int kHorizonY = 132;

    // Returns the number of opaque indexed destination pixels written.
    // Rejects corrupt payloads BEFORE any framebuffer modification.
    [[nodiscard]] static std::size_t draw(
        IndexedFramebuffer& framebuffer,
        const Type7NativeRaster& sprite,
        const std::vector<Type7VisibleColumn>& columns,
        const std::array<std::uint16_t,129>& scale_steps) {
        const auto width=static_cast<std::size_t>(sprite.width);
        const auto height=static_cast<std::size_t>(sprite.height);
        if (width==0U || height!=128U ||
            width>128U || sprite.pixels.size()!=width*height ||
            sprite.mask.size()!=width*height) {
            throw std::invalid_argument("enhanced RES3 pixel/mask geometry invalid");
        }
        if (framebuffer.pixels().size()!=
            static_cast<std::size_t>(IndexedFramebuffer::kWidth)*
            static_cast<std::size_t>(IndexedFramebuffer::kHeight)) {
            throw std::invalid_argument("enhanced destination framebuffer invalid");
        }
        for (const auto& c:columns) {
            if (static_cast<std::size_t>(c.source_x)>=width ||
                c.viewport_x>=256U) {
                throw std::out_of_range("enhanced RES3 column outside source or viewport");
            }
        }

        // SINTAB scale_step[sy] is the run length of *native source row sy*.
        // Source y=64 lies on the viewport horizon; preserve this anchor at
        // all scales instead of stretching a decoded sprite with a guessed
        // screen-height ratio. Source row 128 is the exclusive end.
        std::size_t upper=0U;
        for (std::size_t y=0U;y<64U;++y)
            upper+=scale_steps[y];
        // Original 127 scale blocks have sum <=508; reject implausible
        // malformed external tables to avoid arithmetic overflow.
        if (upper>4096U) {
            throw std::out_of_range("enhanced RES3 scale outside safe range");
        }
        int dst_y=kHorizonY-static_cast<int>(upper);
        auto& dst=framebuffer.mutable_pixels();
        std::size_t writes=0U;
        for (std::size_t sy=0;sy<height;++sy) {
            const auto repeats=static_cast<std::size_t>(scale_steps[sy]);
            if (repeats>4096U) {
                throw std::out_of_range("enhanced RES3 row scale exceeds safe range");
            }
            // Preflight all rows before writes would be stricter for malformed
            // tables. These are already source-validated by RendererTables;
            // enforce a bounded cumulative sum below, too.
            if (dst_y < -4096 || dst_y > 4096) {
                throw std::out_of_range("enhanced RES3 scaled Y exceeds safe range");
            }
            for (std::size_t ry=0;ry<repeats;++ry) {
                const int yy=dst_y+static_cast<int>(ry);
                if (yy<kViewportTop || yy>kViewportBottom) continue;
                for (const auto& c:columns) {
                    const auto dx=static_cast<int>(c.viewport_x)+kViewportLeft;
                    if (dx<kViewportLeft || dx>kViewportRight) continue;
                    const auto src=sy*width+static_cast<std::size_t>(c.source_x);
                    if (sprite.mask[src]==0U) continue;
                    dst[static_cast<std::size_t>(yy)*
                        IndexedFramebuffer::kWidth+
                        static_cast<std::size_t>(dx)]=sprite.pixels[src];
                    ++writes;
                }
            }
            dst_y+=static_cast<int>(repeats);
        }
        return writes;
    }
};

} // namespace rloft
