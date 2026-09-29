#pragma once

#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct VgaModeDescriptor9CA0 {
    std::uint8_t byte0 = 0;
    std::uint8_t byte1 = 0;
    std::uint16_t word3 = 0;
    std::uint16_t word5 = 0;
};

struct VgaMappedAddress9CA0 {
    std::uintptr_t address = 0;
    std::uint8_t plane = 0;
};

struct VgaState9CA0 {
    std::uint32_t flag2B29 = 0;
    std::uint32_t mode2B2D = 0;
    std::uint32_t dword2B31 = 0;
    std::uint32_t shift2B35 = 4;
    std::uint32_t shift2B39 = 2;
    std::uint32_t dword2B3D = 0xC8;
    std::uint32_t rowStride2B41 = 0x50;
    std::uint32_t rowSpan2B45 = 0xC8;
    std::uint32_t frameBuffer2B49 = 0xA0000;
    std::uint32_t pageBytes2B4D = 0x400;
    std::uint8_t logicalOp2B28 = 0;
};

struct VgaOps9CA0 {
    std::function<void()> callAF08;
    std::function<std::uint8_t()> biosGetMode;
    std::function<void(std::uint8_t)> biosSetMode;

    std::function<std::uint8_t(std::uint16_t)> in8;
    std::function<void(std::uint16_t, std::uint8_t)> out8;
    std::function<void(std::uint16_t, std::uint16_t)> out16;

    std::function<void(std::int32_t)> callAF64;
    std::function<void(std::int32_t)> callAF80;

    std::function<VgaMappedAddress9CA0(
        std::int32_t, std::int32_t)> mapAEA8;

    std::function<std::uint8_t(std::uintptr_t)> readMemory8;
    std::function<void(std::uintptr_t, std::uint8_t)> writeMemory8;

    std::function<void()> callAF34;
};

class Vga9CA0 {
public:
    // Direct reconstruction of CODE.1 0x9CA0..0x9DF3.
    static std::uint8_t configure(
        std::uint8_t mode,
        VgaModeDescriptor9CA0& descriptor,
        VgaState9CA0& state,
        const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0x9DF4..0x9E0E.
    // CODE.1 0x9E0F is alignment.
    static void restoreText(
        const VgaOps9CA0& ops);

    // CODE.1 0x9E10..0x9E13 is a jump thunk to 0x9E8C.
    static void setLogicalOperationThunk(
        std::int32_t index,
        VgaState9CA0& state) noexcept;

    // Direct reconstruction of CODE.1 0x9E14..0x9E5F.
    static void setPixel(
        std::int32_t first,
        std::int32_t second,
        std::uint8_t color,
        VgaState9CA0& state,
        const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0x9E60..0x9E88.
    // CODE.1 0x9E89..0x9E8B is alignment.
    static std::uint8_t getPixel(
        std::int32_t first,
        std::int32_t second,
        const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0x9E8C..0x9EA2.
    // CODE.1 0x9EA3..0x9EA6 is the embedded table {0,8,16,24}.
    // CODE.1 0x9EA7..0x9EAD is the shared epilogue used by 0x9EB0.
    // CODE.1 0x9EAE..0x9EAF is alignment.
    static void setLogicalOperation(
        std::int32_t index,
        VgaState9CA0& state) noexcept;

    // Direct reconstruction of CODE.1 0x9EB0..0xA00D.
    // CODE.1 0xA00E..0xA00F is alignment.
    static void fillRect(
        std::int32_t first,
        std::int32_t second,
        std::int32_t firstCount,
        std::int32_t secondCount,
        std::uint8_t color,
        VgaState9CA0& state,
        const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xA010..0xA058.
    // CODE.1 0xA059..0xA05B is alignment.
    static void uploadPalette(
        std::span<const std::uint8_t> bytes,
        std::uint8_t startIndex,
        std::int32_t byteCount,
        const VgaOps9CA0& ops);
};

} // namespace ravenloft::game
