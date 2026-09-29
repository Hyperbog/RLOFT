#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <span>
#include <string>

namespace ravenloft::game {

struct PaletteEffectsStateBE70 {
    std::array<std::uint8_t, 0x300> working2C48{};
    std::array<std::uint8_t, 0x100>* config2AA0 = nullptr;
};

struct PaletteEffectsOpsBE70 {
    std::function<void(const std::array<std::uint8_t, 0x300>&)> upload6618;
    std::function<void()> suspend9AB4;
    std::function<void()> resume9AC4;

    std::function<std::int32_t(std::int32_t)> openRead62A8;
    std::function<void(const std::string&)> fatal9814;
    std::function<std::int32_t(std::int32_t, std::int32_t, std::int32_t)> seek633C;
    std::function<std::int32_t(void*, std::int32_t, std::int32_t)> read6304;
    std::function<std::int32_t(std::int32_t)> close62F0;
};

class PaletteEffectsBE70 {
public:
    // Direct reconstruction of CODE.1 0xBE70..0xBF34.
    // CODE.1 0xBF35..0xBF37 is alignment.
    static void normalize(
        std::span<std::uint8_t, 0x300> palette,
        const PaletteEffectsStateBE70& state) noexcept;

    // Direct reconstruction of CODE.1 0xBF38..0xBFDE.
    // CODE.1 0xBFDF is alignment.
    static void fadeToColor(
        std::span<const std::uint8_t, 0x300> source,
        std::int32_t start,
        std::int32_t end,
        std::uint8_t red,
        std::uint8_t green,
        std::uint8_t blue,
        PaletteEffectsStateBE70& state,
        const PaletteEffectsOpsBE70& ops);

    // Direct reconstruction of CODE.1 0xBFE0..0xC078.
    // CODE.1 0xC079..0xC07B is alignment.
    static void fadeFromColor(
        std::span<const std::uint8_t, 0x300> source,
        std::int32_t start,
        std::int32_t end,
        std::uint8_t red,
        std::uint8_t green,
        std::uint8_t blue,
        PaletteEffectsStateBE70& state,
        const PaletteEffectsOpsBE70& ops);

    // Direct reconstruction of CODE.1 0xC07C..0xC0B7.
    static void copyRange(
        std::span<std::uint8_t, 0x300> destination,
        std::int32_t destinationIndex,
        std::span<const std::uint8_t, 0x300> source,
        std::int32_t sourceIndex,
        std::int32_t count) noexcept;

    // Direct reconstruction of CODE.1 0xC0B8..0xC10E.
    // CODE.1 0xC10F is alignment.
    static void fillRange(
        std::span<std::uint8_t, 0x300> palette,
        std::int32_t start,
        std::uint8_t red,
        std::uint8_t green,
        std::uint8_t blue,
        std::int32_t count) noexcept;

    // Direct reconstruction of CODE.1 0xC110..0xC17A.
    // CODE.1 0xC17B is alignment.
    static void interpolateRange(
        std::span<std::uint8_t, 0x300> palette,
        std::int32_t start,
        std::int32_t count,
        std::int32_t factor,
        std::uint8_t shift,
        const std::array<std::uint8_t, 3>& target) noexcept;

    // Direct reconstruction of CODE.1 0xC17C..0xC1AE.
    // CODE.1 0xC1AF is alignment.
    static void clearAndSuspend(
        PaletteEffectsStateBE70& state,
        const PaletteEffectsOpsBE70& ops);

    // Direct reconstruction of CODE.1 0xC1B0..0xC226.
    // CODE.1 0xC227 is alignment.
    static void rotateTriplets(
        std::span<std::uint8_t, 0x300> palette,
        std::int32_t baseIndex,
        std::int32_t offset,
        std::int32_t count) noexcept;

    // Direct reconstruction of CODE.1 0xC228..0xC253.
    static std::int32_t openPaletteFile(
        std::int32_t fileToken,
        const PaletteEffectsOpsBE70& ops);

    // Direct reconstruction of CODE.1 0xC254..0xC2C2.
    // CODE.1 0xC2C3..0xC2CF is zero/data padding.
    static void loadPaletteRange(
        std::span<std::uint8_t, 0x300> palette,
        std::int32_t destinationIndex,
        std::int32_t filePaletteIndex,
        std::int32_t fileToken,
        std::int32_t count,
        PaletteEffectsStateBE70& state,
        const PaletteEffectsOpsBE70& ops);
};

} // namespace ravenloft::game
