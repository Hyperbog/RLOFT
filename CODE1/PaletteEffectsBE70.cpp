#include "PaletteEffectsBE70.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace ravenloft::game {
namespace {

std::int32_t wrapMultiply(
    std::int32_t a,
    std::int32_t b) noexcept {
    const std::uint32_t ua =
        std::bit_cast<std::uint32_t>(a);
    const std::uint32_t ub =
        std::bit_cast<std::uint32_t>(b);

    return std::bit_cast<std::int32_t>(ua * ub);
}

std::int32_t arithmeticShiftRight(
    std::int32_t value,
    std::uint8_t count) noexcept {
    const unsigned shift = count & 31u;

    if (shift == 0) {
        return value;
    }

    const std::uint32_t input =
        std::bit_cast<std::uint32_t>(value);

    std::uint32_t output =
        input >> shift;

    if (value < 0) {
        output |=
            (~std::uint32_t{0}) << (32u - shift);
    }

    return std::bit_cast<std::int32_t>(output);
}

} // namespace

void PaletteEffectsBE70::normalize(
    std::span<std::uint8_t, 0x300> palette,
    const PaletteEffectsStateBE70& state) noexcept {
    std::int32_t intensity =
        (*state.config2AA0)[0x6C];

    if (intensity > 0x3F) {
        intensity = 0x3F;
    }

    if (intensity < 0) {
        intensity = 0;
    }

    if (intensity == 0) {
        return;
    }

    for (std::size_t index = 0;
         index < 256;
         ++index) {
        auto* rgb =
            palette.data() + index * 3u;

        const std::int32_t maximum =
            std::max({
                static_cast<std::int32_t>(rgb[0]),
                static_cast<std::int32_t>(rgb[1]),
                static_cast<std::int32_t>(rgb[2])
            });

        const std::int32_t scale =
            maximum +
            (((0x3F - maximum) * intensity + 0x1F) /
             0x3F);

        for (std::int32_t component = 0;
             component < 3;
             ++component) {
            if (scale == 0 ||
                rgb[component] == 0) {
                continue;
            }

            const std::int32_t numerator =
                static_cast<std::int32_t>(rgb[component]) *
                    scale +
                (maximum >> 1);

            rgb[component] =
                static_cast<std::uint8_t>(
                    numerator / maximum);
        }
    }
}

void PaletteEffectsBE70::copyRange(
    std::span<std::uint8_t, 0x300> destination,
    std::int32_t destinationIndex,
    std::span<const std::uint8_t, 0x300> source,
    std::int32_t sourceIndex,
    std::int32_t count) noexcept {
    std::memmove(
        destination.data() +
            static_cast<std::size_t>(destinationIndex) * 3u,
        source.data() +
            static_cast<std::size_t>(sourceIndex) * 3u,
        static_cast<std::size_t>(count) * 3u);
}

void PaletteEffectsBE70::fillRange(
    std::span<std::uint8_t, 0x300> palette,
    std::int32_t start,
    std::uint8_t red,
    std::uint8_t green,
    std::uint8_t blue,
    std::int32_t count) noexcept {
    const std::array<std::uint8_t, 3> color{{
        red, green, blue
    }};

    auto* destination =
        palette.data() +
        static_cast<std::size_t>(start) * 3u;

    for (std::int32_t i = 0;
         i < count;
         ++i) {
        std::memcpy(
            destination,
            color.data(),
            color.size());

        destination += 3;
    }
}

void PaletteEffectsBE70::interpolateRange(
    std::span<std::uint8_t, 0x300> palette,
    std::int32_t start,
    std::int32_t count,
    std::int32_t factor,
    std::uint8_t shift,
    const std::array<std::uint8_t, 3>& target) noexcept {
    auto* destination =
        palette.data() +
        static_cast<std::size_t>(start) * 3u;

    for (std::int32_t i = 0;
         i < count;
         ++i) {
        for (std::int32_t component = 0;
             component < 3;
             ++component) {
            const std::int32_t delta =
                static_cast<std::int32_t>(
                    destination[component]) -
                static_cast<std::int32_t>(
                    target[component]);

            const std::int32_t product =
                wrapMultiply(delta, factor);

            const std::int32_t value =
                static_cast<std::int32_t>(
                    target[component]) +
                arithmeticShiftRight(
                    product,
                    shift);

            destination[component] =
                static_cast<std::uint8_t>(
                    value);
        }

        destination += 3;
    }
}

void PaletteEffectsBE70::fadeToColor(
    std::span<const std::uint8_t, 0x300> source,
    std::int32_t start,
    std::int32_t end,
    std::uint8_t red,
    std::uint8_t green,
    std::uint8_t blue,
    PaletteEffectsStateBE70& state,
    const PaletteEffectsOpsBE70& ops) {
    const std::int32_t count =
        end - start + 1;

    copyRange(
        state.working2C48,
        0,
        source,
        0,
        256);

    const std::array<std::uint8_t, 3> target{{
        red, green, blue
    }};

    for (std::int32_t factor = 7;
         factor >= 0;
         --factor) {
        interpolateRange(
            state.working2C48,
            start,
            count,
            factor,
            3,
            target);

        ops.upload6618(
            state.working2C48);

        copyRange(
            state.working2C48,
            start,
            source,
            start,
            count);
    }

    ops.suspend9AB4();
}

void PaletteEffectsBE70::fadeFromColor(
    std::span<const std::uint8_t, 0x300> source,
    std::int32_t start,
    std::int32_t end,
    std::uint8_t red,
    std::uint8_t green,
    std::uint8_t blue,
    PaletteEffectsStateBE70& state,
    const PaletteEffectsOpsBE70& ops) {
    const std::int32_t count =
        end - start + 1;

    copyRange(
        state.working2C48,
        0,
        source,
        0,
        256);

    const std::array<std::uint8_t, 3> target{{
        red, green, blue
    }};

    for (std::int32_t factor = 1;
         factor <= 8;
         ++factor) {
        interpolateRange(
            state.working2C48,
            start,
            count,
            factor,
            3,
            target);

        ops.upload6618(
            state.working2C48);

        copyRange(
            state.working2C48,
            start,
            source,
            start,
            count);
    }

    ops.resume9AC4();
}

void PaletteEffectsBE70::clearAndSuspend(
    PaletteEffectsStateBE70& state,
    const PaletteEffectsOpsBE70& ops) {
    fillRange(
        state.working2C48,
        0,
        0,
        0,
        0,
        256);

    ops.upload6618(
        state.working2C48);

    ops.suspend9AB4();
}

void PaletteEffectsBE70::rotateTriplets(
    std::span<std::uint8_t, 0x300> palette,
    std::int32_t baseIndex,
    std::int32_t offset,
    std::int32_t count) noexcept {
    if (count <= 1) {
        return;
    }

    auto* begin =
        palette.data() +
        static_cast<std::size_t>(
            baseIndex + offset) * 3u;

    std::array<std::uint8_t, 3> temporary{};
    std::memcpy(
        temporary.data(),
        begin,
        3);

    const std::size_t bytes =
        static_cast<std::size_t>(count) * 3u - 3u;

    std::memmove(
        begin,
        begin + 3,
        bytes);

    std::memcpy(
        begin + bytes,
        temporary.data(),
        3);
}

std::int32_t PaletteEffectsBE70::openPaletteFile(
    std::int32_t fileToken,
    const PaletteEffectsOpsBE70& ops) {
    const std::int32_t handle =
        ops.openRead62A8(fileToken);

    if (handle == -1) {
        // Exact CODE.1 data at DS:0334.
        ops.fatal9814(
            "Unable to open palette file");
    }

    return handle;
}

void PaletteEffectsBE70::loadPaletteRange(
    std::span<std::uint8_t, 0x300> palette,
    std::int32_t destinationIndex,
    std::int32_t filePaletteIndex,
    std::int32_t fileToken,
    std::int32_t count,
    PaletteEffectsStateBE70& state,
    const PaletteEffectsOpsBE70& ops) {
    const std::int32_t handle =
        openPaletteFile(
            fileToken,
            ops);

    const std::int32_t offset =
        filePaletteIndex * 3 * 256 + 1;

    (void)ops.seek633C(
        offset,
        1,
        handle);

    (void)ops.read6304(
        palette.data() +
            static_cast<std::size_t>(
                destinationIndex) * 3u,
        count * 3,
        handle);

    normalize(
        palette,
        state);

    (void)ops.close62F0(handle);
}

} // namespace ravenloft::game
