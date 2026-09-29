#include "VgaBlitA6F0.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1]) << 8u);
}

struct FrameView {
    std::size_t source = 0;
    std::uint16_t width = 0;
    std::uint16_t bytesPerColumn = 0;
    bool transparent = false;
    std::uint8_t transparentValue = 0xFF;
};

FrameView frameView(std::uint32_t frameIndex,
                    std::span<const std::uint8_t> resource) noexcept {
    FrameView view{};

    const std::uint32_t frameSize = read16(resource, 0);
    view.width = read16(resource, 4);
    view.bytesPerColumn =
        static_cast<std::uint16_t>(
            static_cast<std::uint16_t>(resource[6]) << 1u);
    view.transparent = (resource[7] & 0x80u) != 0;

    view.source =
        static_cast<std::size_t>(frameSize * frameIndex + 8u);

    if (view.transparent) {
        view.transparentValue = resource[view.source++];
    }

    return view;
}

void selectPlane(std::uint8_t plane,
                 const VgaOps9CA0& ops) {
    ops.out16(
        0x3C4,
        static_cast<std::uint16_t>(
            0x02u |
            (static_cast<std::uint16_t>(
                 static_cast<std::uint8_t>(1u << plane)) << 8u)));
}

void drawImpl(std::uint32_t frameIndex,
              std::int32_t first,
              std::int32_t second,
              std::span<const std::uint8_t> resource,
              std::uint8_t adjustment,
              bool adjust,
              const VgaState9CA0& state,
              const VgaOps9CA0& ops) {
    const auto mapped = ops.mapAEA8(first, second);
    const std::uintptr_t initialDestination = mapped.address;
    const std::uint8_t initialPlane = mapped.plane;

    auto view = frameView(frameIndex, resource);

    const std::int32_t rowAdvance =
        static_cast<std::int32_t>(state.rowStride2B41) -
        static_cast<std::int32_t>(view.bytesPerColumn);

    std::size_t source = view.source;

    for (std::uint8_t pass = 0; pass < 4; ++pass) {
        const std::uint8_t combined =
            static_cast<std::uint8_t>(initialPlane + pass);
        const std::uint8_t physicalPlane =
            static_cast<std::uint8_t>(combined & 3u);

        selectPlane(physicalPlane, ops);

        std::uintptr_t destination =
            initialDestination + (combined >= 4u ? 1u : 0u);

        for (std::uint16_t column = 0;
             column < view.width;
             ++column) {
            for (std::uint16_t i = 0;
                 i < view.bytesPerColumn;
                 ++i) {
                const std::uint8_t sourceValue = resource[source++];

                if (!view.transparent ||
                    sourceValue != view.transparentValue) {
                    const std::uint8_t output =
                        adjust
                            ? static_cast<std::uint8_t>(sourceValue - adjustment)
                            : sourceValue;
                    ops.writeMemory8(destination, output);
                }

                ++destination;
            }

            if (rowAdvance >= 0) {
                destination += static_cast<std::uintptr_t>(rowAdvance);
            } else {
                destination -= static_cast<std::uintptr_t>(-rowAdvance);
            }
        }
    }
}

} // namespace

void VgaBlitA6F0::draw(
    std::uint32_t frameIndex,
    std::int32_t first,
    std::int32_t second,
    std::span<const std::uint8_t> resource,
    const VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    drawImpl(
        frameIndex,
        first,
        second,
        resource,
        0,
        false,
        state,
        ops);
}

void VgaBlitA6F0::drawAdjusted(
    std::uint32_t frameIndex,
    std::int32_t first,
    std::int32_t second,
    std::span<const std::uint8_t> resource,
    std::uint8_t adjustment,
    const VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    drawImpl(
        frameIndex,
        first,
        second,
        resource,
        adjustment,
        true,
        state,
        ops);
}

} // namespace ravenloft::game
