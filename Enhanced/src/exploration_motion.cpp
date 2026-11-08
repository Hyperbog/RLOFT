#include "rloft/exploration_motion.hpp"

#include "rloft/fixed_projection.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

std::uint32_t ExplorationMotionModel::normalize_heading(
    std::int32_t heading) noexcept {
    if (heading >= 0x168) {
        heading = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(heading) - 0x168U);
    }
    if (heading < 0) {
        heading = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(heading) + 0x168U);
    }
    return static_cast<std::uint32_t>(heading);
}

ExplorationMotionVector ExplorationMotionModel::direction_vector(
    std::uint32_t distance,
    std::uint32_t heading,
    bool grid_mode,
    const RendererTables& tables) {
    heading = normalize_heading(
        static_cast<std::int32_t>(heading));

    if (!grid_mode) {
        const auto high =
            static_cast<std::int16_t>(distance >> 16U);
        if (high == 0 &&
            static_cast<std::uint16_t>(distance) >= 0xB000U) {
            distance = 0x0000AFFFU;
        }

        const auto index =
            static_cast<std::size_t>(heading);
        if (index + 90U >=
            tables.sine_q16_signmag.size()) {
            throw std::out_of_range(
                "motion heading leaves SINTAB sine table");
        }

        const auto sin_factor =
            tables.sine_q16_signmag[index + 90U];
        const auto cos_factor =
            tables.sine_q16_signmag[index];

        const auto dy =
            FixedProjection::multiply_signed_factor(
                distance, sin_factor);
        const auto x =
            FixedProjection::multiply_signed_factor(
                distance, cos_factor);

        return ExplorationMotionVector{
            0U - x,
            dy,
        };
    }

    const auto direction =
        static_cast<std::size_t>(heading / 0x2DU);
    if (direction > 7U) {
        throw std::out_of_range(
            "grid motion heading is not eight-way");
    }

    static constexpr std::array<std::uint32_t, 8> kDy{{
        0x00010000U, 0x00010000U,
        0U,           0xFFFF0000U,
        0xFFFF0000U, 0xFFFF0000U,
        0U,           0x00010000U,
    }};
    static constexpr std::array<std::uint32_t, 8> kDx{{
        0U,           0xFFFF0000U,
        0xFFFF0000U, 0xFFFF0000U,
        0U,           0x00010000U,
        0x00010000U, 0x00010000U,
    }};

    return ExplorationMotionVector{
        kDx[direction],
        kDy[direction],
    };
}

ExplorationMotionStep ExplorationMotionModel::integrate(
    ExplorationMotionState state,
    ExplorationMotionInputs inputs,
    const RendererTables& tables) {
    auto accumulator =
        static_cast<std::int32_t>(
            static_cast<std::uint32_t>(
                state.angular_remainder) +
            static_cast<std::uint32_t>(
                inputs.angular_input));

    auto step = accumulator / 0x14;
    accumulator -= step * 0x14;

    auto heading =
        static_cast<std::int32_t>(state.heading);

    if (!inputs.grid_mode) {
        heading = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(heading) -
            static_cast<std::uint32_t>(step));
    } else {
        if (step < 0) step = -0x5A;
        else if (step > 0) step = 0x5A;

        heading = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(heading) -
            static_cast<std::uint32_t>(step));
    }
    heading = static_cast<std::int32_t>(
        normalize_heading(heading));

    auto movement_heading = heading;
    auto velocity = inputs.velocity;

    if (inputs.orthogonal_input != 0) {
        const auto raw =
            inputs.orthogonal_input < 0
                ? static_cast<std::uint32_t>(
                      0U - static_cast<std::uint32_t>(
                          inputs.orthogonal_input))
                : static_cast<std::uint32_t>(
                      inputs.orthogonal_input);
        velocity = static_cast<std::int32_t>(raw);

        movement_heading =
            static_cast<std::int32_t>(
                normalize_heading(
                    static_cast<std::int32_t>(
                        static_cast<std::uint32_t>(heading) +
                        (inputs.orthogonal_input < 0
                             ? static_cast<std::uint32_t>(-0x5A)
                             : 0x5AU))));
    }

    ExplorationMotionStep out{};
    out.state.heading =
        static_cast<std::uint32_t>(heading);
    out.state.angular_remainder = accumulator;
    out.state.velocity = velocity;

    if (velocity < 0) {
        auto low =
            static_cast<std::uint16_t>(velocity);
        low = static_cast<std::uint16_t>(
            0U - static_cast<std::uint32_t>(low));
        const auto scale =
            static_cast<std::int16_t>(low);
        const auto distance =
            static_cast<std::uint32_t>(
                static_cast<std::int32_t>(
                    0x96 * static_cast<std::int32_t>(scale)));

        out.translation_heading =
            static_cast<std::uint32_t>(movement_heading);
        out.translation_distance = distance;
        out.translation = direction_vector(
            distance,
            out.translation_heading,
            inputs.grid_mode,
            tables);
    } else if (velocity > 0) {
        const auto reverse =
            normalize_heading(
                static_cast<std::int32_t>(
                    static_cast<std::uint32_t>(
                        movement_heading) +
                    0xB4U));
        const auto distance =
            static_cast<std::uint32_t>(
                0x64 * velocity);

        out.translation_heading = reverse;
        out.translation_distance = distance;
        out.translation = direction_vector(
            distance,
            reverse,
            inputs.grid_mode,
            tables);
    }

    return out;
}

}  // namespace rloft
