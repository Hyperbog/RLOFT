#pragma once

#include <optional>
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
    // type-1 frame: default remains source-authentic 8-bit SUB wrap.
    // Enhanced PC exploration can opt into clamping negative palette
    // indices rather than wrapping to an unrelated bright high index.
    // Optional authored draw width limits source pixels without changing
    // the decoded resource. When omitted, historical full-raster drawing
    // is unchanged. The source can be narrower than the authored width;
    // it must never be read past its actual decoded stride.
    static void draw_subtracted(
        IndexedFramebuffer& framebuffer,
        const Type1Raster& raster,
        int frame,
        int row,
        int column,
        int shade,
        bool clip_to_environment,
        bool clamp_shade_for_enhanced_pc = false,
        std::optional<int> authored_width = std::nullopt);

private:
    const RavenloftBootAssets& assets_;
    IndexedFramebuffer framebuffer_;
};

}  // namespace rloft
