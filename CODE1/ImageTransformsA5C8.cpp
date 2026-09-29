#include "ImageTransformsA5C8.h"

#include <cstddef>
#include <cstdint>
#include <utility>

namespace ravenloft::game {
namespace {

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1]) << 8u);
}

std::size_t frameStart(std::uint32_t frameIndex,
                       std::span<const std::uint8_t> resource) noexcept {
    const std::uint32_t frameSize = read16(resource, 0);
    std::size_t start =
        static_cast<std::size_t>(frameSize * frameIndex + 8u);

    if ((resource[7] & 0x80u) != 0) {
        ++start;
    }

    return start;
}

std::uint16_t add16(std::uint16_t a,
                    std::uint16_t b) noexcept {
    return static_cast<std::uint16_t>(a + b);
}

std::uint16_t mul16(std::uint16_t a,
                    std::uint16_t b) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(a) *
        static_cast<std::uint32_t>(b));
}

void swapRun(std::span<std::uint8_t> resource,
             std::size_t& forward,
             std::size_t& backward,
             std::uint16_t count) {
    for (std::uint16_t i = 0; i < count; ++i) {
        std::swap(resource[forward], resource[backward]);
        ++forward;
        --backward;
    }
}

} // namespace

void ImageTransformsA5C8::transformA5C8(
    std::uint32_t frameIndex,
    std::span<std::uint8_t> resource) {
    const std::uint16_t width = read16(resource, 4);

    const std::uint16_t doubledHeight =
        static_cast<std::uint8_t>(
            static_cast<std::uint8_t>(resource[6]) << 1u);

    const std::size_t start = frameStart(frameIndex, resource);

    const std::uint16_t block =
        mul16(width, doubledHeight);

    std::uint16_t separation = block;
    separation = add16(separation, block);
    separation = add16(separation, block);
    separation = add16(
        separation,
        static_cast<std::uint16_t>(doubledHeight - 1u));

    std::size_t forward = start;

    for (std::uint16_t column = 0;
         column < width;
         ++column) {
        std::size_t backward =
            forward + separation;

        swapRun(
            resource,
            forward,
            backward,
            doubledHeight);
    }

    forward = start + block;

    separation = add16(
        block,
        static_cast<std::uint16_t>(doubledHeight - 1u));

    for (std::uint16_t column = 0;
         column < width;
         ++column) {
        std::size_t backward =
            forward + separation;

        swapRun(
            resource,
            forward,
            backward,
            doubledHeight);
    }
}

void ImageTransformsA5C8::transformA678(
    std::uint32_t frameIndex,
    std::span<std::uint8_t> resource) {
    const std::uint16_t width = read16(resource, 4);
    const std::uint16_t heightUnits = resource[6];
    const std::uint16_t doubledHeight =
        static_cast<std::uint16_t>(heightUnits << 1u);

    const std::size_t start = frameStart(frameIndex, resource);

    std::uint16_t separation =
        mul16(
            static_cast<std::uint16_t>(width - 1u),
            heightUnits);
    separation = static_cast<std::uint16_t>(
        separation << 1u);

    std::size_t planeStart = start;

    for (std::int32_t plane = 0;
         plane < 4;
         ++plane) {
        std::size_t forward = planeStart;
        std::size_t backward =
            forward + separation;

        const std::uint16_t halfWidth =
            static_cast<std::uint16_t>(width >> 1u);

        for (std::uint16_t column = 0;
             column < halfWidth;
             ++column) {
            for (std::uint16_t i = 0;
                 i < doubledHeight;
                 ++i) {
                std::swap(
                    resource[forward],
                    resource[backward]);
                ++forward;
                ++backward;
            }

            backward -= doubledHeight;
            backward -= doubledHeight;
        }

        planeStart += separation;
        planeStart += doubledHeight;
    }
}

} // namespace ravenloft::game
