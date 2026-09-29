#include "VgaDrawA05C.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

void setMapMask(std::uint8_t mask,
                const VgaOps9CA0& ops) {
    ops.out16(0x3C4,
              static_cast<std::uint16_t>(
                  0x02u |
                  (static_cast<std::uint16_t>(mask) << 8u)));
}

void setWriteMode1(const VgaOps9CA0& ops) {
    ops.out16(0x3CE, 0x4105);
    ops.out16(0x3C4, 0x0F02);
}

void restoreWriteMode0(const VgaOps9CA0& ops) {
    ops.out16(0x3CE, 0x4005);
}

void copyForward(std::uintptr_t destination,
                 std::uintptr_t source,
                 std::uint32_t count,
                 const VgaOps9CA0& ops) {
    for (std::uint32_t i = 0; i < count; ++i) {
        const std::uint8_t value = ops.readMemory8(source++);
        ops.writeMemory8(destination++, value);
    }
}

void copyBackward(std::uintptr_t destination,
                  std::uintptr_t source,
                  std::uint32_t count,
                  const VgaOps9CA0& ops) {
    for (std::uint32_t i = 0; i < count; ++i) {
        const std::uint8_t value = ops.readMemory8(source--);
        ops.writeMemory8(destination--, value);
    }
}

void writeLinePixel(std::int32_t first,
                    std::int32_t second,
                    std::uint8_t color,
                    const VgaOps9CA0& ops) {
    const auto mapped = ops.mapAEA8(first, second);
    setMapMask(static_cast<std::uint8_t>(1u << mapped.plane), ops);
    (void)ops.readMemory8(mapped.address);
    ops.writeMemory8(mapped.address, color);
}

} // namespace

void VgaDrawA05C::line(
    std::int32_t first1,
    std::int32_t second1,
    std::int32_t first2,
    std::int32_t second2,
    std::uint8_t color,
    VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    ops.out16(
        0x3CE,
        static_cast<std::uint16_t>(
            0x03u |
            (static_cast<std::uint16_t>(state.logicalOp2B28) << 8u)));

    std::int32_t secondDelta = second2 - second1;
    if (secondDelta < 0) {
        secondDelta = -secondDelta;
        const auto oldSecond1 = second1;
        second1 = second2;
        second2 = oldSecond1;
        const auto oldFirst1 = first1;
        first1 = first2;
        first2 = oldFirst1;
    }

    std::int32_t firstDelta = first2 - first1;

    if (secondDelta == 0) {
        if (firstDelta < 0) {
            first1 = first2;
            firstDelta = -firstDelta;
        }

        for (std::int32_t i = 0; i <= firstDelta; ++i) {
            writeLinePixel(first1, second1, color, ops);
            ++first1;
        }

        ops.out16(0x3CE, 0x0003);
        return;
    }

    if (firstDelta == 0) {
        for (std::int32_t i = 0; i <= secondDelta; ++i) {
            writeLinePixel(first1, second1 + i, color, ops);
        }

        ops.out16(0x3CE, 0x0003);
        return;
    }

    const std::int32_t firstStep = firstDelta < 0 ? -1 : 1;
    if (firstDelta < 0) {
        firstDelta = -firstDelta;
    }

    if (firstDelta <= secondDelta) {
        const std::int32_t addMinor = firstDelta * 2;
        const std::int32_t addDiagonal = addMinor - secondDelta * 2;
        std::int32_t error = addMinor - secondDelta;

        for (std::int32_t i = 0; i <= secondDelta; ++i) {
            writeLinePixel(first1, second1, color, ops);

            ++second1;
            if (error < 0) {
                error += addMinor;
            } else {
                error += addDiagonal;
                first1 += firstStep;
            }
        }
    } else {
        const std::int32_t addMinor = secondDelta * 2;
        const std::int32_t addDiagonal = addMinor - firstDelta * 2;
        std::int32_t error = addMinor - firstDelta;

        for (std::int32_t i = 0; i <= firstDelta; ++i) {
            writeLinePixel(first1, second1, color, ops);

            first1 += firstStep;
            if (error < 0) {
                error += addMinor;
            } else {
                error += addDiagonal;
                ++second1;
            }
        }
    }

    ops.out16(0x3CE, 0x0003);
}

void VgaDrawA05C::shiftPageMemory(
    std::int32_t amount,
    VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    const bool nonNegative =
        static_cast<std::int16_t>(amount) >= 0;

    setWriteMode1(ops);

    if (nonNegative) {
        const std::uint32_t byteShift =
            static_cast<std::uint32_t>(amount) >> 2u;
        const std::uint32_t count = state.pageBytes2B4D << 4u;

        copyForward(
            state.frameBuffer2B49,
            state.frameBuffer2B49 + byteShift,
            count,
            ops);
    } else {
        const std::uint32_t magnitude =
            static_cast<std::uint32_t>(-amount);
        const std::uint32_t byteShift = magnitude >> 2u;
        const std::uint32_t count = state.pageBytes2B4D << 4u;

        std::uintptr_t destination =
            state.frameBuffer2B49 + count - 1u;
        std::uintptr_t source = destination - byteShift;

        copyBackward(destination, source, count, ops);
    }

    restoreWriteMode0(ops);
}

void VgaDrawA05C::copyRows(
    std::int32_t rowCount,
    std::int32_t offset,
    VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    const bool nonNegative =
        static_cast<std::int16_t>(offset) >= 0;

    setWriteMode1(ops);

    const std::uint32_t count =
        static_cast<std::uint32_t>(rowCount) *
        state.rowStride2B41;

    if (nonNegative) {
        const auto source =
            ops.mapAEA8(offset, 0).address;

        copyForward(
            state.frameBuffer2B49,
            source,
            count,
            ops);
    } else {
        const std::int32_t magnitude = -offset;

        std::uintptr_t source =
            ops.mapAEA8(magnitude, 0).address;
        std::uintptr_t destination =
            ops.mapAEA8(
                static_cast<std::int32_t>(state.rowSpan2B45),
                0).address;

        --source;
        --destination;

        copyBackward(
            destination,
            source,
            count,
            ops);
    }

    restoreWriteMode0(ops);
}

void VgaDrawA05C::setActivePage(
    std::uint8_t page,
    VgaState9CA0& state) noexcept {
    state.dword2B31 = page;
    state.frameBuffer2B49 =
        (static_cast<std::uint32_t>(page) *
             state.pageBytes2B4D +
         0xA000u) << 4u;
}

void VgaDrawA05C::setDisplayPage(
    std::uint8_t page,
    const VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    const std::uint32_t product =
        static_cast<std::uint32_t>(page) *
        (state.pageBytes2B4D >> 4u);

    ops.callAF64(
        static_cast<std::int32_t>(
            (product & 0xFFu) << 8u));
}

void VgaDrawA05C::cloneCurrentPageTo(
    std::uint8_t page,
    VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    const std::uintptr_t destination =
        (static_cast<std::uint32_t>(page) *
             state.pageBytes2B4D +
         0xA000u) << 4u;

    setWriteMode1(ops);

    const std::uint32_t count =
        static_cast<std::uint16_t>(
            static_cast<std::uint16_t>(state.pageBytes2B4D) << 4u);

    copyForward(
        destination,
        state.frameBuffer2B49,
        count,
        ops);

    restoreWriteMode0(ops);
}

void VgaDrawA05C::copyRectBetweenPages(
    std::uint8_t destinationPage,
    std::int32_t sourceFirst,
    std::int32_t sourceSecond,
    std::int32_t destinationFirst,
    std::int32_t destinationSecond,
    std::int32_t firstCount,
    std::int32_t secondCount,
    VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    const auto sourceMapped =
        ops.mapAEA8(sourceFirst, sourceSecond);
    std::uintptr_t source = sourceMapped.address;

    secondCount += sourceMapped.plane;

    const std::uint8_t savedPage =
        static_cast<std::uint8_t>(state.dword2B31);

    setActivePage(destinationPage, state);
    const auto destinationMapped =
        ops.mapAEA8(destinationFirst, destinationSecond);
    std::uintptr_t destination = destinationMapped.address;
    setActivePage(savedPage, state);

    setWriteMode1(ops);

    const std::uint32_t bytesPerRow =
        static_cast<std::uint32_t>(secondCount + 3) >> 2u;

    const std::int32_t sourceAdvance =
        static_cast<std::int32_t>(state.rowStride2B41) -
        static_cast<std::int32_t>(bytesPerRow);
    const std::int32_t destinationAdvance = sourceAdvance;

    for (std::int32_t row = 0;
         row < firstCount;
         ++row) {
        copyForward(
            destination,
            source,
            bytesPerRow,
            ops);

        source += bytesPerRow;
        destination += bytesPerRow;

        if (sourceAdvance >= 0) {
            source += static_cast<std::uintptr_t>(sourceAdvance);
            destination += static_cast<std::uintptr_t>(destinationAdvance);
        } else {
            source -= static_cast<std::uintptr_t>(-sourceAdvance);
            destination -= static_cast<std::uintptr_t>(-destinationAdvance);
        }
    }

    restoreWriteMode0(ops);
}

void VgaDrawA05C::setDisplayParameter(
    std::int32_t value,
    const VgaOps9CA0& ops) {
    ops.callAF80(value);
}

} // namespace ravenloft::game
