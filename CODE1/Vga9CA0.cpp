#include "Vga9CA0.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint8_t arithmeticShiftRight8(
    std::uint8_t value,
    unsigned count) noexcept {
    if (count >= 8) {
        return (value & 0x80u) != 0 ? 0xFFu : 0u;
    }

    const auto signedValue =
        static_cast<std::int16_t>(
            static_cast<std::int8_t>(value));

    return static_cast<std::uint8_t>(
        signedValue >> count);
}

void setMapMask(
    std::uint8_t mask,
    const VgaOps9CA0& ops) {
    ops.out16(
        0x3C4,
        static_cast<std::uint16_t>(
            0x02u |
            (static_cast<std::uint16_t>(mask) << 8u)));
}

void advanceAddress(
    std::uintptr_t& address,
    std::int32_t delta) noexcept {
    if (delta >= 0) {
        address += static_cast<std::uintptr_t>(delta);
    } else {
        address -= static_cast<std::uintptr_t>(-delta);
    }
}

} // namespace

std::uint8_t Vga9CA0::configure(
    std::uint8_t mode,
    VgaModeDescriptor9CA0& descriptor,
    VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    // 0x9CA4..0x9CF4.
    state.flag2B29 = mode & 0x80u;
    state.mode2B2D = mode & 0x7Fu;
    state.dword2B3D = 0xC8;
    state.rowStride2B41 = 0x50;
    state.frameBuffer2B49 = 0xA0000;
    state.dword2B31 = 0;
    state.logicalOp2B28 = 0;
    state.pageBytes2B4D = 0x400;
    state.shift2B35 = 4;
    state.shift2B39 = 2;

    ops.callAF08();

    if (state.mode2B2D == 3) {
        state.pageBytes2B4D = 0x800;
    }

    // 0x9D0B..0x9D17.
    if (ops.biosGetMode() != 0x13) {
        ops.biosSetMode(0x13);
    }

    // 0x9D19..0x9D55.
    ops.out16(
        0x3CE,
        static_cast<std::uint16_t>(
            0x03u |
            (static_cast<std::uint16_t>(
                 state.logicalOp2B28) << 8u)));

    ops.out8(0x3C4, 4);
    std::uint8_t value = ops.in8(0x3C5);
    value =
        static_cast<std::uint8_t>(
            (value & 0xF7u) | 0x04u);
    ops.out8(0x3C5, value);

    ops.out8(0x3CE, 5);
    value = ops.in8(0x3CF);
    value &= 0xEFu;
    ops.out8(0x3CF, value);

    ops.out8(0x3CE, 6);
    value = ops.in8(0x3CF);
    value &= 0xFDu;
    ops.out8(0x3CF, value);

    ops.callAF08();

    // 0x9D5A..0x9DB1.
    ops.out8(0x3D4, 9);
    value = ops.in8(0x3D5);

    if (state.mode2B2D == 3) {
        value &= 0xE0u;
    } else {
        value = 0x41u;
    }

    ops.out8(0x3D5, value);

    ops.out8(0x3D4, 0x14);
    value = ops.in8(0x3D5);
    ops.out8(
        0x3D5,
        static_cast<std::uint8_t>(
            value & 0xBFu));

    ops.out8(0x3D4, 0x17);
    value = ops.in8(0x3D5);
    ops.out8(
        0x3D5,
        static_cast<std::uint8_t>(
            value | 0x40u));

    ops.callAF64(0);
    ops.callAF80(0xFF);

    // 0x9DB4..0x9DE7.
    std::int32_t rowSpan = 0xC8;
    if (state.mode2B2D == 3) {
        rowSpan *= 2;
    }

    state.rowSpan2B45 =
        static_cast<std::uint32_t>(rowSpan);

    descriptor.byte1 = 2;
    descriptor.word3 =
        static_cast<std::uint16_t>(rowSpan);
    descriptor.word5 = 0x140;

    state.flag2B29 = 0;

    // Original AL return.
    return 0x0D;
}

void Vga9CA0::restoreText(
    const VgaOps9CA0& ops) {
    ops.callAF08();
    ops.biosSetMode(3);
}

void Vga9CA0::setLogicalOperationThunk(
    std::int32_t index,
    VgaState9CA0& state) noexcept {
    // CODE.1 0x9E10 is a jump thunk to 0x9E8C.
    setLogicalOperation(index, state);
}

void Vga9CA0::setPixel(
    std::int32_t first,
    std::int32_t second,
    std::uint8_t color,
    VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    const auto mapped =
        ops.mapAEA8(first, second);

    ops.out16(
        0x3CE,
        static_cast<std::uint16_t>(
            0x04u |
            (static_cast<std::uint16_t>(
                 mapped.plane) << 8u)));

    setMapMask(
        static_cast<std::uint8_t>(
            1u << mapped.plane),
        ops);

    ops.out16(
        0x3CE,
        static_cast<std::uint16_t>(
            0x03u |
            (static_cast<std::uint16_t>(
                 state.logicalOp2B28) << 8u)));

    // Original read loads VGA latches before the write.
    (void)ops.readMemory8(mapped.address);
    ops.writeMemory8(mapped.address, color);

    ops.out16(0x3CE, 0x0003);
}

std::uint8_t Vga9CA0::getPixel(
    std::int32_t first,
    std::int32_t second,
    const VgaOps9CA0& ops) {
    const auto mapped =
        ops.mapAEA8(first, second);

    ops.out16(
        0x3CE,
        static_cast<std::uint16_t>(
            0x04u |
            (static_cast<std::uint16_t>(
                 mapped.plane) << 8u)));

    return ops.readMemory8(mapped.address);
}

void Vga9CA0::setLogicalOperation(
    std::int32_t index,
    VgaState9CA0& state) noexcept {
    // Exact CODE.1 bytes at 0x9EA3..0x9EA6.
    static constexpr std::array<std::uint8_t, 4>
        kLogicalOperations{{0x00, 0x08, 0x10, 0x18}};

    state.logicalOp2B28 =
        kLogicalOperations[
            static_cast<std::size_t>(index)];
}

void Vga9CA0::fillRect(
    std::int32_t first,
    std::int32_t second,
    std::int32_t firstCount,
    std::int32_t secondCount,
    std::uint8_t color,
    VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    // 0x9EC8..0x9ED0 jumps to the shared function epilogue.
    if (firstCount <= 0 ||
        secondCount <= 0) {
        return;
    }

    auto mapped =
        ops.mapAEA8(first, second);

    // DH in the original.
    std::uint8_t startMask =
        static_cast<std::uint8_t>(
            0x0Fu << mapped.plane);

    // DL in the original.
    std::uint8_t endMask = 0;

    std::int32_t fullGroups = 0;

    // 0x9EE1..0x9F13.
    const std::int32_t remainingAfterStart =
        secondCount - 1;

    const std::int32_t toGroupEnd =
        static_cast<std::int32_t>(
            mapped.plane ^ 3u);

    if (static_cast<std::uint32_t>(
            remainingAfterStart) <=
        static_cast<std::uint32_t>(
            toGroupEnd)) {
        std::int32_t value =
            remainingAfterStart -
            toGroupEnd +
            3;

        std::uint8_t shift =
            static_cast<std::uint8_t>(value);
        shift ^= 3u;

        endMask =
            static_cast<std::uint8_t>(
                ~arithmeticShiftRight8(
                    0xF0u,
                    shift));

        startMask &= endMask;
        endMask = 0;
        fullGroups = 0;
    } else {
        std::int32_t value =
            remainingAfterStart -
            toGroupEnd -
            1;

        std::uint8_t shift =
            static_cast<std::uint8_t>(
                value & 3);

        value -= shift;
        shift ^= 3u;

        endMask =
            static_cast<std::uint8_t>(
                ~arithmeticShiftRight8(
                    0xF0u,
                    shift));

        fullGroups = value >> 2;
    }

    // 0x9F15..0x9F23.
    std::int32_t rowAdvance =
        static_cast<std::int32_t>(
            state.rowStride2B41) -
        fullGroups -
        1;

    if (endMask != 0) {
        --rowAdvance;
    }

    ops.out16(
        0x3CE,
        static_cast<std::uint16_t>(
            0x03u |
            (static_cast<std::uint16_t>(
                 state.logicalOp2B28) << 8u)));

    // 0x9F38..0x9F8F: logical-operation path.
    if (state.logicalOp2B28 != 0) {
        std::uintptr_t address =
            mapped.address;

        for (std::int32_t row = 0;
             row < firstCount;
             ++row) {
            setMapMask(startMask, ops);

            (void)ops.readMemory8(address);
            ops.writeMemory8(address, color);
            ++address;

            if (fullGroups != 0) {
                setMapMask(0xFFu, ops);

                for (std::int32_t i = 0;
                     i < fullGroups;
                     ++i) {
                    (void)ops.readMemory8(address);
                    ops.writeMemory8(address, color);
                    ++address;
                }
            }

            if (endMask != 0) {
                setMapMask(endMask, ops);

                (void)ops.readMemory8(address);
                ops.writeMemory8(address, color);
                ++address;
            }

            advanceAddress(
                address,
                rowAdvance);
        }

        // Reset graphics-controller logical operation.
        ops.out16(0x3CE, 0x0003);
        return;
    }

    // 0x9FB0..0xA006: straight-copy path.
    //
    // Original compares EBX == 0x00000F0F:
    // BH=start mask 0x0F and BL=end mask 0x0F.
    if (rowAdvance == 0 &&
        startMask == 0x0Fu &&
        endMask == 0x0Fu) {
        setMapMask(0xFFu, ops);

        std::uintptr_t address =
            mapped.address;

        const std::int32_t total =
            static_cast<std::int32_t>(
                state.rowStride2B41) *
            firstCount;

        for (std::int32_t i = 0;
             i < total;
             ++i) {
            ops.writeMemory8(
                address++,
                color);
        }

        return;
    }

    std::uintptr_t address =
        mapped.address;

    for (std::int32_t row = 0;
         row < firstCount;
         ++row) {
        setMapMask(startMask, ops);
        ops.writeMemory8(address++, color);

        if (fullGroups != 0) {
            setMapMask(0xFFu, ops);

            for (std::int32_t i = 0;
                 i < fullGroups;
                 ++i) {
                ops.writeMemory8(
                    address++,
                    color);
            }
        }

        if (endMask != 0) {
            setMapMask(endMask, ops);
            ops.writeMemory8(address++, color);
        }

        advanceAddress(
            address,
            rowAdvance);
    }
}

void Vga9CA0::uploadPalette(
    std::span<const std::uint8_t> bytes,
    std::uint8_t startIndex,
    std::int32_t byteCount,
    const VgaOps9CA0& ops) {
    ops.out8(0x3C8, startIndex);

    std::size_t offset = 0;
    std::int32_t remaining = byteCount;

    while (remaining > 0) {
        // 0xA02D is called once per output chunk,
        // including the final <= 0xC0-byte chunk.
        ops.callAF34();

        const std::int32_t chunk =
            std::min(remaining, 0xC0);

        for (std::int32_t i = 0;
             i < chunk;
             ++i) {
            ops.out8(
                0x3C9,
                bytes[offset++]);
        }

        remaining -= chunk;
    }
}

} // namespace ravenloft::game
