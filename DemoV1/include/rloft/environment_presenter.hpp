#pragma once

#include <vector>

#include "rloft/environment_projection.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

class EnvironmentPresenter final {
public:
    static constexpr int kClipRowMin = 4;
    static constexpr int kClipRowMax = 0x83;
    static constexpr int kClipColumnMin = 0x20;
    static constexpr int kClipColumnMax = 0x11F;

    explicit EnvironmentPresenter(const RavenloftBootAssets& assets)
        : assets_(assets) {}

    [[nodiscard]] IndexedFramebuffer& framebuffer() noexcept {
        return framebuffer_;
    }
    [[nodiscard]] const IndexedFramebuffer& framebuffer() const noexcept {
        return framebuffer_;
    }

    void clear(std::uint8_t color = 0) { framebuffer_.clear(color); }

    void draw_commands(
        const std::vector<EnvironmentDrawCommand>& commands);

    // Portable equivalent of CODE.2 2A478 / clipped 2A8A0 for one decoded
    // type-1 frame: subtract an unsigned byte shade from every nontransparent
    // palette index, preserving 8-bit wraparound.
    static void draw_subtracted(
        IndexedFramebuffer& framebuffer,
        const Type1Raster& raster,
        int frame,
        int row,
        int column,
        int shade,
        bool clip_to_environment);

private:
    const RavenloftBootAssets& assets_;
    IndexedFramebuffer framebuffer_;
};

}  // namespace rloft
