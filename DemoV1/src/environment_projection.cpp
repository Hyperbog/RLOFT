#include "rloft/environment_projection.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace rloft {
namespace {

std::int32_t wrap32(std::uint32_t value) noexcept {
    return static_cast<std::int32_t>(value);
}

std::int32_t scaled_x(std::int32_t delta) noexcept {
    const auto shifted =
        static_cast<std::uint32_t>(delta) << 7U;
    const auto numerator =
        wrap32(shifted + 0x12U);
    return numerator / 0x24;
}

int screen_for(
    int world,
    int camera,
    int width,
    int wrap) noexcept {
    const auto delta = wrap32(
        static_cast<std::uint32_t>(world) -
        static_cast<std::uint32_t>(camera) +
        static_cast<std::uint32_t>(wrap));
    return 0xA0 - scaled_x(delta) - width / 2;
}

bool visible(int x, int width) noexcept {
    return x < 0x120 && x + width >= 0x20;
}

}  // namespace

int EnvironmentProjectionModel::projected_x(
    int world_azimuth_degrees,
    int camera_degrees,
    int width_pixels) noexcept {
    int x = screen_for(
        world_azimuth_degrees,
        camera_degrees,
        width_pixels,
        0);
    if (!visible(x, width_pixels)) {
        x = screen_for(
            world_azimuth_degrees,
            camera_degrees,
            width_pixels,
            0x168);
        if (!visible(x, width_pixels)) {
            x = screen_for(
                world_azimuth_degrees,
                camera_degrees,
                width_pixels,
                -0x168);
        }
    }
    return x;
}

std::vector<EnvironmentDrawCommand> EnvironmentProjectionModel::build(
    const EnvironmentProfile& profile,
    int camera_degrees,
    int mode,
    const std::array<std::uint16_t, 32>& rng15) {
    std::vector<EnvironmentDrawCommand> out;

    if (mode <= 10) {
        int set_bias = 0;
        if (mode == 9) set_bias = 8;
        else if (mode == 10) set_bias = 16;

        for (std::size_t i = 0; i < rng15.size(); ++i) {
            const auto random =
                static_cast<std::uint32_t>(rng15[i] & 0x7FFFU);
            const auto selector =
                static_cast<int>((random * 7U) >> 15U) +
                static_cast<int>(profile.res8_base) +
                set_bias;
            out.push_back(EnvironmentDrawCommand{
                EnvironmentDrawKind::RandomBackdropStrip,
                selector,
                static_cast<int>(i) * 8 + 0x20,
                4,
                8,
                0,
            });
        }
    }

    for (std::size_t i = 0; i < profile.placements.size(); ++i) {
        const auto& placement = profile.placements[i];
        if (i != 0 && placement.resource_selector == 0) {
            break;
        }

        int shade = std::min(mode, 8);
        if (i == 0) {
            shade = mode / 2;
            if (placement.resource_selector != 0x18) {
                shade = 0;
            }
        }

        int resource_id = placement.resource_selector;
        if (resource_id >= 8) {
            resource_id += static_cast<int>(profile.res8_base);
        }

        const int width = placement.draw_width_pixels;
        const int x = projected_x(
            placement.azimuth_degrees,
            camera_degrees,
            width);

        out.push_back(EnvironmentDrawCommand{
            EnvironmentDrawKind::ProfilePlacement,
            resource_id,
            x,
            placement.screen_y,
            width,
            static_cast<unsigned char>(shade),
        });
    }

    return out;
}

}  // namespace rloft
