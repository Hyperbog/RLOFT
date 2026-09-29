#pragma once

#include "Code1Vga9CA0.h"

#include <cstdint>
#include <span>

namespace ravenloft::game {

class Code1ImageRemapAE0C {
public:
    // Direct reconstruction of CODE.1 0xAE0C..0xAE59.
    // CODE.1 0xAE5A..0xAE5B is alignment.
    static void translateFrame(
        std::uint32_t frameIndex,
        std::span<const std::uint8_t, 256> translation,
        std::span<std::uint8_t> resource);

    // Direct reconstruction of CODE.1 0xAE5C..0xAEA5.
    // No direct or indirect caller was recovered for this routine. The
    // original ABI receives resource in ESI and frame index in EDX, while
    // only the adjustment byte is stack-based. The C++ interface makes those
    // register inputs explicit.
    // CODE.1 0xAEA6..0xAEA7 is alignment.
    static void subtractFrameRegisterConvention(
        std::span<std::uint8_t> resource,
        std::uint32_t frameIndex,
        std::uint8_t adjustment);
};

class Code1VgaHelpersAEA8 {
public:
    // Direct reconstruction of CODE.1 0xAEA8..0xAEDB.
    static Code1VgaMappedAddress9CA0 map(
        std::int32_t first,
        std::int32_t second,
        const Code1VgaState9CA0& state) noexcept;

    // Direct reconstruction of CODE.1 0xAEDC..0xAF04.
    // CODE.1 0xAF05..0xAF07 is alignment.
    static void setActivePage(
        std::uint8_t page,
        Code1VgaState9CA0& state) noexcept;

    // Direct reconstruction of CODE.1 0xAF08..0xAF33.
    static void clearFrameBuffer(
        Code1VgaState9CA0& state,
        const Code1VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xAF34..0xAF4A.
    // CODE.1 0xAF4B is alignment.
    static void waitRetraceStart(
        const Code1VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xAF4C..0xAF62.
    // CODE.1 0xAF63 is alignment.
    static void waitRetraceEnd(
        const Code1VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xAF64..0xAF7F.
    static void setDisplayStart(
        std::uint16_t start,
        const Code1VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xAF80..0xAFD7.
    static void setVerticalParameter(
        std::uint32_t value,
        Code1VgaState9CA0& state,
        const Code1VgaOps9CA0& ops);
};

} // namespace ravenloft::game
