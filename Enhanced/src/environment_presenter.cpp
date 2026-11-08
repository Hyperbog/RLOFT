#include "rloft/environment_presenter.hpp"

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <optional>
#include <stdexcept>

namespace rloft {

void EnvironmentPresenter::draw_subtracted(
    IndexedFramebuffer& framebuffer,
    const Type1Raster& raster,
    int frame,
    int row,
    int column,
    int shade,
    bool clip_to_environment,
    bool clamp_shade_for_enhanced_pc,
    std::optional<int> authored_width) {
    if (frame < 0 || frame >= raster.count) {
        throw std::out_of_range("environment type-1 frame index");
    }
    if (authored_width && *authored_width <= 0) {
        throw std::invalid_argument("environment authored draw width must be positive");
    }
    if (raster.width <= 0 || raster.height <= 0) {
        throw std::invalid_argument("environment raster dimensions");
    }

    const auto frame_stride =
        static_cast<std::size_t>(raster.width) *
        static_cast<std::size_t>(raster.height);
    const auto frame_at =
        static_cast<std::size_t>(frame) * frame_stride;
    if (frame_at > raster.pixels.size() ||
        frame_stride > raster.pixels.size() - frame_at) {
        throw std::invalid_argument("environment raster payload is truncated");
    }

    const auto delta = static_cast<std::uint8_t>(shade);
    const int drawable_width=authored_width
        ? std::min(raster.width,*authored_width)
        : raster.width;
    auto& dst = framebuffer.mutable_pixels();

    for (int sy = 0; sy < raster.height; ++sy) {
        const auto dy=static_cast<std::int64_t>(row)+sy;
        if (dy < 0 || dy >= IndexedFramebuffer::kHeight) continue;
        if (clip_to_environment &&
            (dy < kClipRowMin || dy > kClipRowMax)) {
            continue;
        }

        for (int sx = 0; sx < drawable_width; ++sx) {
            const auto dx=static_cast<std::int64_t>(column)+sx;
            if (dx < 0 || dx >= IndexedFramebuffer::kWidth) continue;
            if (clip_to_environment &&
                (dx < kClipColumnMin || dx > kClipColumnMax)) {
                continue;
            }

            const auto source = raster.pixels[
                frame_at +
                static_cast<std::size_t>(sy) *
                    static_cast<std::size_t>(raster.width) +
                static_cast<std::size_t>(sx)];

            if (raster.transparent_index != 0xFFU &&
                source == raster.transparent_index) {
                continue;
            }

            dst[
                static_cast<std::size_t>(dy) *
                    IndexedFramebuffer::kWidth +
                static_cast<std::size_t>(dx)] =
                clamp_shade_for_enhanced_pc && source < delta
                    ? 0U
                    : static_cast<std::uint8_t>(source - delta);
        }
    }
}

void EnvironmentPresenter::draw_commands(
    const std::vector<EnvironmentDrawCommand>& commands) {
    for (const auto& command : commands) {
        const auto raster = assets_.type1(8, command.resource_id);
        draw_subtracted(
            framebuffer_,
            raster,
            0,
            command.y,
            command.x,
            command.shade,
            command.kind == EnvironmentDrawKind::ProfilePlacement,
            false,command.width);
    }
}

}  // namespace rloft
