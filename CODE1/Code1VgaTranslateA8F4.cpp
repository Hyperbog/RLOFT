#include "Code1VgaTranslateA8F4.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1]) << 8u);
}

void selectPlane(std::uint8_t plane,
                 const Code1VgaOps9CA0& ops) {
    ops.out16(
        0x3C4,
        static_cast<std::uint16_t>(
            0x02u |
            (static_cast<std::uint16_t>(
                 static_cast<std::uint8_t>(1u << plane)) << 8u)));
}

} // namespace

void Code1VgaTranslateA8F4::draw(
    std::uint32_t frameIndex,
    std::int32_t first,
    std::int32_t second,
    std::span<const std::uint8_t> resource,
    std::span<const std::uint8_t, 256> translation,
    const Code1VgaState9CA0& state,
    const Code1VgaOps9CA0& ops) {
    const auto mapped = ops.mapAEA8(first, second);
    const std::uintptr_t initialDestination = mapped.address;
    const std::uint8_t initialPlane = mapped.plane;

    const std::uint32_t frameSize = read16(resource, 0);
    const std::uint16_t width = read16(resource, 4);
    const std::uint16_t bytesPerColumn =
        static_cast<std::uint16_t>(
            static_cast<std::uint16_t>(resource[6]) << 1u);
    const bool transparent = (resource[7] & 0x80u) != 0;

    std::size_t source =
        static_cast<std::size_t>(frameSize * frameIndex + 8u);

    std::uint8_t transparentValue = 0xFFu;
    if (transparent) {
        transparentValue = resource[source++];
    }

    const std::int32_t rowAdvance =
        static_cast<std::int32_t>(state.rowStride2B41) -
        static_cast<std::int32_t>(bytesPerColumn);

    for (std::uint8_t pass = 0; pass < 4; ++pass) {
        const std::uint8_t combined =
            static_cast<std::uint8_t>(initialPlane + pass);
        const std::uint8_t physicalPlane =
            static_cast<std::uint8_t>(combined & 3u);

        selectPlane(physicalPlane, ops);

        std::uintptr_t destination =
            initialDestination + (combined >= 4u ? 1u : 0u);

        for (std::uint16_t column = 0;
             column < width;
             ++column) {
            for (std::uint16_t i = 0;
                 i < bytesPerColumn;
                 ++i) {
                const std::uint8_t sourceValue = resource[source++];

                if (!transparent ||
                    sourceValue != transparentValue) {
                    ops.writeMemory8(
                        destination,
                        translation[sourceValue]);
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

} // namespace ravenloft::game
