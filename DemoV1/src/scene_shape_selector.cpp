#include "rloft/scene_shape_selector.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

std::int32_t normalize_degrees(
    std::int32_t value) noexcept {
    while (value >= 360) value -= 360;
    while (value < 0) value += 360;
    return value;
}

std::uint16_t add_word(
    std::uint16_t value,
    std::int32_t addend) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(value) +
        static_cast<std::uint32_t>(addend));
}

}  // namespace

std::uint16_t
SceneShapeSelectorModel::select_bank_local(
    std::uint16_t initial_bank_local_resource,
    std::uint16_t screen_x,
    std::uint16_t packed_runtime_state,
    std::uint16_t type_id,
    std::uint8_t source_class,
    std::uint8_t shape_stride,
    const ExplorationSessionState& session,
    const RendererTables& tables) {
    // 2B2F4's early cache path is exclusive to source class 1.
    if (source_class == 1U) {
        throw std::logic_error(
            "class-1 scene selector cache path is not implemented");
    }
    if (source_class > 3U) {
        throw std::out_of_range(
            "scene source class");
    }
    if (shape_stride == 0U) {
        // The original routine would execute IDIV by zero. Shipped records
        // reaching the first-person emitter must therefore have nonzero
        // stride; reject malformed/custom data deterministically.
        throw std::logic_error(
            "zero scene shape stride reached CODE.2 selector");
    }

    const auto live_facing =
        static_cast<std::int32_t>(
            (packed_runtime_state >> 3U) & 7U);

    auto relative =
        (static_cast<std::int32_t>(
             tables.c6d0()) -
         static_cast<std::int32_t>(
             screen_x)) /
        8;
    relative +=
        static_cast<std::int32_t>(
            session.facing);
    relative += live_facing * 45;
    relative -= 180;

    auto resource =
        initial_bank_local_resource;

    if (shape_stride == 16U) {
        relative += 22;
        relative -=
            (16 / static_cast<std::int32_t>(
                      shape_stride)) *
            11;
        relative =
            normalize_degrees(relative);

        auto selector =
            (relative * 10) / 225;
        selector %= 16;

        // The source-class-1 mirror predicate at 2B4A0 is unreachable here.
        resource =
            add_word(resource, selector);
        return resource;
    }

    if (shape_stride == 2U) {
        relative += 22;
        relative =
            normalize_degrees(relative);

        auto selector =
            relative / 45;
        selector %= 8;
        selector %= 2;
        resource =
            add_word(resource, selector);
        return resource;
    }

    relative += 44;
    relative -=
        (8 / static_cast<std::int32_t>(
                 shape_stride)) *
        22;
    relative =
        normalize_degrees(relative);

    auto selector =
        (relative * 10) / 225;
    selector %= 8;

    const auto action_state =
        static_cast<std::uint16_t>(
            packed_runtime_state & 0x0006U);

    if (action_state == 0x0002U ||
        action_state == 0x0004U) {
        // 2B61E..2B67B: class-0 NPC type 0x21 bypasses this reaction offset.
        if (!(source_class == 0U &&
              type_id == 0x21U)) {
            const auto divisor =
                8 /
                static_cast<std::int32_t>(
                    shape_stride);
            selector /= divisor;
            selector %= 5;
            selector %= 3;
            resource =
                add_word(resource, selector);
        }
    } else if (action_state == 0U) {
        const auto divisor =
            8 /
            static_cast<std::int32_t>(
                shape_stride);
        resource =
            add_word(
                resource,
                selector / divisor);
    }
    // action_state == 6 performs no common view adjustment.

    return resource;
}

}  // namespace rloft
