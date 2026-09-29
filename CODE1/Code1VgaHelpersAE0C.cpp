#include "Code1VgaHelpersAE0C.h"

#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {
namespace {

std::uint16_t read16(
    std::span<const std::uint8_t> bytes,
    std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1]) << 8u);
}

void transformFrame(
    std::span<std::uint8_t> resource,
    std::uint32_t frameIndex,
    const std::function<std::uint8_t(std::uint8_t)>& transform) {
    const std::uint32_t frameSize =
        read16(resource, 0);

    std::size_t position =
        static_cast<std::size_t>(
            frameSize * frameIndex + 8u);

    const bool transparent =
        (resource[7] & 0x80u) != 0;

    std::uint8_t marker = 0;
    if (transparent) {
        marker = resource[position++];
    }

    // Preserve the original DEC/JNE loop shape. If frameSize were zero the
    // x86 routine would wrap ECX and continue for 2^32 iterations.
    std::uint32_t count = frameSize;
    do {
        const std::uint8_t value =
            resource[position];

        if (!transparent ||
            value != marker) {
            resource[position] =
                transform(value);
        }

        ++position;
    } while (--count != 0);
}

} // namespace

void Code1ImageRemapAE0C::translateFrame(
    std::uint32_t frameIndex,
    std::span<const std::uint8_t, 256> translation,
    std::span<std::uint8_t> resource) {
    transformFrame(
        resource,
        frameIndex,
        [&](std::uint8_t value) {
            return translation[value];
        });
}

void Code1ImageRemapAE0C::subtractFrameRegisterConvention(
    std::span<std::uint8_t> resource,
    std::uint32_t frameIndex,
    std::uint8_t adjustment) {
    transformFrame(
        resource,
        frameIndex,
        [adjustment](std::uint8_t value) {
            return static_cast<std::uint8_t>(
                value - adjustment);
        });
}

Code1VgaMappedAddress9CA0 Code1VgaHelpersAEA8::map(
    std::int32_t first,
    std::int32_t second,
    const Code1VgaState9CA0& state) noexcept {
    // EAX is shifted cumulatively in the original:
    // first<<shift2B35 plus first<<(shift2B35+shift2B39).
    std::uint32_t scaled =
        static_cast<std::uint32_t>(first);

    scaled <<= state.shift2B35;
    std::uint32_t offset = scaled;

    scaled <<= state.shift2B39;
    offset += scaled;

    const std::uint32_t secondBits =
        static_cast<std::uint32_t>(second);

    offset += secondBits >> 2u;

    return {
        static_cast<std::uintptr_t>(
            state.frameBuffer2B49 + offset),
        static_cast<std::uint8_t>(
            secondBits & 3u)
    };
}

void Code1VgaHelpersAEA8::setActivePage(
    std::uint8_t page,
    Code1VgaState9CA0& state) noexcept {
    state.dword2B31 = page;

    state.frameBuffer2B49 =
        (static_cast<std::uint32_t>(page) *
             state.pageBytes2B4D +
         0xA000u) << 4u;
}

void Code1VgaHelpersAEA8::clearFrameBuffer(
    Code1VgaState9CA0& state,
    const Code1VgaOps9CA0& ops) {
    if (state.flag2B29 != 0) {
        return;
    }

    ops.out16(0x3C4, 0x0F02);

    // Original REP STOSD count is 0x4000, i.e. 65536 bytes.
    for (std::uint32_t i = 0;
         i < 0x10000u;
         ++i) {
        ops.writeMemory8(
            0xA0000u + i,
            0);
    }
}

void Code1VgaHelpersAEA8::waitRetraceStart(
    const Code1VgaOps9CA0& ops) {
    while ((ops.in8(0x3DA) & 0x08u) != 0) {
    }

    while ((ops.in8(0x3DA) & 0x08u) == 0) {
    }
}

void Code1VgaHelpersAEA8::waitRetraceEnd(
    const Code1VgaOps9CA0& ops) {
    while ((ops.in8(0x3DA) & 0x08u) == 0) {
    }

    while ((ops.in8(0x3DA) & 0x08u) != 0) {
    }
}

void Code1VgaHelpersAEA8::setDisplayStart(
    std::uint16_t start,
    const Code1VgaOps9CA0& ops) {
    ops.out16(
        0x3D4,
        static_cast<std::uint16_t>(
            0x0Cu |
            (static_cast<std::uint16_t>(
                 static_cast<std::uint8_t>(start >> 8u)) << 8u)));

    ops.out16(
        0x3D4,
        static_cast<std::uint16_t>(
            0x0Du |
            (static_cast<std::uint16_t>(
                 static_cast<std::uint8_t>(start)) << 8u)));

    waitRetraceStart(ops);
}

void Code1VgaHelpersAEA8::setVerticalParameter(
    std::uint32_t value,
    Code1VgaState9CA0& state,
    const Code1VgaOps9CA0& ops) {
    state.dword2B3D = value;

    waitRetraceStart(ops);

    std::uint16_t ax =
        static_cast<std::uint16_t>(value);
    ax = static_cast<std::uint16_t>(ax << 1u);

    const std::uint8_t high =
        static_cast<std::uint8_t>(ax >> 8u);

    std::uint16_t bx =
        static_cast<std::uint16_t>(
            (static_cast<std::uint16_t>(high) << 8u) |
            high);

    bx &= 0x0201u;
    bx = static_cast<std::uint16_t>(bx << 4u);

    std::uint8_t bh =
        static_cast<std::uint8_t>(bx >> 8u);
    bh = static_cast<std::uint8_t>(bh << 1u);

    const std::uint8_t bl =
        static_cast<std::uint8_t>(bx);

    ops.out16(
        0x3D4,
        static_cast<std::uint16_t>(
            0x18u |
            (static_cast<std::uint16_t>(
                 static_cast<std::uint8_t>(ax)) << 8u)));

    ops.out8(0x3D4, 0x07);
    std::uint8_t register7 =
        ops.in8(0x3D5);
    register7 =
        static_cast<std::uint8_t>(
            (register7 & 0xEFu) | bl);
    ops.out16(
        0x3D4,
        static_cast<std::uint16_t>(
            0x07u |
            (static_cast<std::uint16_t>(register7) << 8u)));

    ops.out8(0x3D4, 0x09);
    std::uint8_t register9 =
        ops.in8(0x3D5);
    register9 =
        static_cast<std::uint8_t>(
            (register9 & 0xBFu) | bh);
    ops.out16(
        0x3D4,
        static_cast<std::uint16_t>(
            0x09u |
            (static_cast<std::uint16_t>(register9) << 8u)));
}

} // namespace ravenloft::game
