#include "rloft/surface_shading.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

constexpr std::array<std::int16_t, 11> kScale{{
    9, 9, 8, 7, 7, 6, 6, 6, 5, 5, 5,
}};

std::int32_t wrap_add(std::int32_t a, std::int32_t b) noexcept {
    return static_cast<std::int32_t>(
        static_cast<std::uint32_t>(a) +
        static_cast<std::uint32_t>(b));
}

std::int32_t wrap_mul(std::int32_t a, std::int32_t b) noexcept {
    return static_cast<std::int32_t>(
        static_cast<std::uint32_t>(
            static_cast<std::uint64_t>(
                static_cast<std::uint32_t>(a)) *
            static_cast<std::uint32_t>(b)));
}

std::int32_t sar1(std::int32_t value) noexcept {
    const auto raw = static_cast<std::uint32_t>(value);
    return static_cast<std::int32_t>(
        (raw >> 1U) | (raw & 0x80000000U));
}

std::int8_t nonnegative(std::int8_t value) noexcept {
    return value < 0 ? 0 : value;
}

std::int8_t cap9(std::int8_t value) noexcept {
    return value > 9 ? 9 : value;
}

std::uint16_t pack(std::int8_t high, std::int8_t low) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<std::uint8_t>(low)) |
        (static_cast<std::uint16_t>(
             static_cast<std::uint8_t>(high)) << 8U);
}

}  // namespace

SurfaceShadeTable SurfaceShadingModel::build(
    int environment_shade,
    int fade) {
    if (environment_shade < 0 ||
        environment_shade >= static_cast<int>(kScale.size())) {
        throw std::out_of_range("environment shade mode");
    }
    if (fade < 0) {
        throw std::invalid_argument("negative renderer fade");
    }
    if (fade > 8) fade = 8;

    const auto arg = static_cast<std::int32_t>(environment_shade);
    const auto fade32 = static_cast<std::int32_t>(fade);
    const std::int32_t step = wrap_add(arg, 1);
    constexpr std::int32_t limit = kDepthLimit + 1;
    const std::int32_t fade1 = wrap_add(fade32, 1);
    const std::int32_t denom = wrap_add(10, -fade1);
    const std::int32_t span = kSpan;
    const std::int32_t used =
        wrap_mul(fade32, span / denom);
    const std::int32_t remaining =
        wrap_add(span, -used);
    const std::int32_t threshold =
        wrap_add(kDepthLimit, -remaining);

    SurfaceShadeTable out{};
    std::uint16_t last = 0;

    for (std::int32_t i = 0; i < limit; ++i) {
        const std::int32_t term = wrap_mul(
            wrap_add(wrap_add(limit, -i), 1),
            step);
        const std::int32_t num =
            wrap_add(term, sar1(limit));

        auto lo = nonnegative(
            static_cast<std::int8_t>(num / limit));

        const std::int32_t scale =
            static_cast<std::int32_t>(
                kScale[static_cast<std::size_t>(
                    environment_shade)]);
        const std::int32_t scaled_num =
            wrap_add(sar1(scale),
                     static_cast<std::int32_t>(lo) * 5);
        lo = static_cast<std::int8_t>(
            scaled_num / scale);
        auto hi = lo;

        lo = cap9(lo);
        hi = cap9(hi);

        if (fade32 != 0 && i != 0) {
            std::int32_t adjust = 0;
            if (i > remaining) {
                const auto dist =
                    wrap_add(i, -remaining);
                const auto n = wrap_add(
                    wrap_mul(dist, 8),
                    sar1(threshold));
                adjust = n / threshold;
            }

            lo = static_cast<std::int8_t>(
                static_cast<std::uint8_t>(lo) -
                static_cast<std::uint8_t>(adjust));
            hi = static_cast<std::int8_t>(
                static_cast<std::uint8_t>(hi) -
                static_cast<std::uint8_t>(adjust));
            lo = nonnegative(lo);
            hi = nonnegative(hi);
        }

        const std::int32_t half_period =
            limit / wrap_mul(step, 2);
        const std::int32_t phase =
            i / half_period;
        if ((static_cast<std::uint8_t>(phase) & 1U) != 0U) {
            --hi;
        }
        if (static_cast<std::int32_t>(lo) <= 2) {
            hi = lo;
        }
        if (lo == 8 && hi == 9) {
            hi = 8;
        }

        last = pack(hi, lo);
        out.packed[static_cast<std::size_t>(i)] = last;
    }

    out.fallback = last;
    return out;
}

std::uint16_t SurfaceShadingModel::packed_subtract(
    const SurfaceShadeTable& table,
    std::uint32_t resource_id,
    std::uint32_t extent,
    std::uint32_t shade_decrement,
    std::size_t first_ray) noexcept {
    std::uint16_t shade = table.packed[0];

    if (resource_id < 0x242U ||
        resource_id > 0x24DU) {
        shade = table.fallback;

        if (extent <=
            static_cast<std::uint32_t>(kDepthLimit)) {
            auto ax =
                table.packed[static_cast<std::size_t>(extent)];
            const auto sub =
                static_cast<std::uint8_t>(shade_decrement);
            const auto repeated =
                static_cast<std::uint16_t>(sub) |
                (static_cast<std::uint16_t>(sub) << 8U);
            ax = static_cast<std::uint16_t>(ax - repeated);

            auto lo = static_cast<std::uint8_t>(ax);
            auto hi = static_cast<std::uint8_t>(ax >> 8U);
            if (static_cast<std::int8_t>(hi) < 0) hi = 0;
            if (static_cast<std::int8_t>(lo) < 0) lo = 0;
            shade = static_cast<std::uint16_t>(lo) |
                    (static_cast<std::uint16_t>(hi) << 8U);
        }

        if ((first_ray & 1U) != 0U) {
            shade = static_cast<std::uint16_t>(
                ((shade & 0x00FFU) << 8U) |
                ((shade & 0xFF00U) >> 8U));
        }
    }

    if (shade > 0x0808U) {
        shade = 0x0808U;
    }
    return shade;
}

}  // namespace rloft
