#include "rloft/environment_presenter.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

void EnvironmentPresenter::draw_subtracted(
    IndexedFramebuffer& framebuffer,
    const Type1Raster& raster,
    int frame,
    int row,
    int column,
    int shade,
    bool clip_to_environment) {
    if (frame < 0 || frame >= raster.count) {
        throw std::out_of_range("environment type-1 frame index");
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
    auto& dst = framebuffer.mutable_pixels();

    for (int sy = 0; sy < raster.height; ++sy) {
        const int dy = row + sy;
        if (dy < 0 || dy >= IndexedFramebuffer::kHeight) continue;
        if (clip_to_environment &&
            (dy < kClipRowMin || dy > kClipRowMax)) {
            continue;
        }

        for (int sx = 0; sx < raster.width; ++sx) {
            const int dx = column + sx;
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
                static_cast<std::uint8_t>(source - delta);
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
            command.kind == EnvironmentDrawKind::ProfilePlacement);
    }
}

}  // namespace rloft
