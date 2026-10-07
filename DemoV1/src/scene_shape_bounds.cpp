#include "rloft/scene_shape_bounds.hpp"

#include <cstdint>

namespace rloft {
namespace {

std::int32_t trunc_div(
    std::int32_t value,
    std::int32_t divisor) noexcept {
    return value / divisor;
}

std::int32_t scaled_geometry(
    std::uint16_t value,
    std::int32_t multiplier) noexcept {
    return trunc_div(
        static_cast<std::int32_t>(value) *
                multiplier +
            0x40,
        0x80);
}

}  // namespace

SceneInteractionBounds
SceneShapeBoundsModel::npc_type7(
    std::uint16_t screen_x,
    std::uint16_t projected_extent,
    const Type7Geometry& geometry) {
    // The original caller temporarily adds 0x7D00 to scene +0x02 before
    // 21BF0; the helper immediately subtracts it. Starting from the stable
    // projected extent therefore reaches the same scale directly.
    auto scale =
        static_cast<std::int32_t>(
            projected_extent) /
        8;
    if (scale < 0x28) {
        return {};
    }

    const auto half_width =
        static_cast<std::int32_t>(
            geometry.source_width) /
        2;
    const auto x_span =
        static_cast<std::int32_t>(
            geometry.x_max) -
        geometry.x_min;
    auto y_span =
        static_cast<std::int32_t>(
            geometry.y_max) -
        geometry.y_min;

    const auto quarter_scale =
        scale / 4;
    scale -= scale / 8;
    auto multiplier = scale * 4;
    if (multiplier == 0) {
        multiplier = 4;
    }

    const auto scaled_y_min =
        scaled_geometry(
            static_cast<std::uint16_t>(
                geometry.y_min),
            multiplier);
    const auto scaled_x_min =
        scaled_geometry(
            static_cast<std::uint16_t>(
                geometry.x_min),
            multiplier);
    auto scaled_y_span =
        scaled_geometry(
            static_cast<std::uint16_t>(
                y_span),
            multiplier);
    const auto scaled_x_span =
        scaled_geometry(
            static_cast<std::uint16_t>(
                x_span),
            multiplier);
    const auto scaled_half_width =
        scaled_geometry(
            static_cast<std::uint16_t>(
                half_width),
            multiplier);

    auto left =
        static_cast<std::int32_t>(
            screen_x) -
        (scaled_half_width -
         scaled_x_min);

    auto top =
        quarter_scale +
        4 +
        trunc_div(
            0x100 - multiplier,
            2) +
        trunc_div(multiplier, 8) +
        scaled_y_min;

    if (top + scaled_y_span > 0x104) {
        scaled_y_span =
            0x104 - top;
    }

    const auto bottom =
        top + scaled_y_span;
    const auto right =
        left + scaled_x_span;

    // CODE.2 2BB73/2BB7B adds 0x20 to scene words +0x12/+0x14 after 21BF0.
    left += 0x20;

    SceneInteractionBounds out{};
    out.valid = true;
    out.top =
        static_cast<std::uint16_t>(top);
    out.bottom =
        static_cast<std::uint16_t>(
            bottom);
    out.left =
        static_cast<std::uint16_t>(left);
    out.right =
        static_cast<std::uint16_t>(
            right + 0x20);
    return out;
}

}  // namespace rloft
