#include "rloft/surface_column_scaler.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

ScaledSurfaceColumn SurfaceColumnScaler::scale(
    const SurfaceTexture& texture,
    int source_column,
    std::uint16_t subtract_pair,
    std::uint32_t output_height,
    SurfaceScaleState& state) {
    if (source_column < 0 || source_column >= texture.width) {
        throw std::out_of_range("surface source column");
    }
    if (output_height == 0U ||
        output_height > static_cast<std::uint32_t>(0x7FFFFFFF)) {
        throw std::invalid_argument("surface output height");
    }

    const auto divisor =
        static_cast<std::int32_t>(output_height);
    const auto signed_step =
        static_cast<std::int32_t>(0x00800000U) / divisor;
    const auto step =
        static_cast<std::uint32_t>(signed_step);

    std::uint32_t count = output_height;
    std::uint32_t start =
        static_cast<std::uint32_t>(
            static_cast<std::int32_t>(
                0x100U - output_height) >> 1);
    start += static_cast<std::uint32_t>(
        static_cast<std::int32_t>(output_height) >> 3);

    std::uint32_t phase = state.phase_residue;
    if (static_cast<std::int32_t>(start) < 0) {
        const auto magnitude = 0U - start;
        phase = step * magnitude;
        state.phase_residue = phase;
        start = 0U;
        count = 0x100U;
    }

    auto combined = count + start;
    if (static_cast<std::int32_t>(combined) > 0x100) {
        combined -= 0x100U;
        count -= combined;
    }

    auto iterations = count & 0xFFU;
    if (iterations == 0U) {
        iterations = 0x100U;
    }

    ScaledSurfaceColumn out{};
    out.start_row = static_cast<int>(start);
    out.pixels.reserve(iterations);

    const auto low_subtract =
        static_cast<std::uint8_t>(subtract_pair & 0xFFU);
    const auto high_subtract =
        static_cast<std::uint8_t>((subtract_pair >> 8U) & 0xFFU);

    for (std::uint32_t i = 0; i < iterations; ++i) {
        const auto source_index = phase >> 16U;
        if (source_index >=
            static_cast<std::uint32_t>(SurfaceTexture::kHeight)) {
            throw std::runtime_error(
                "surface scaler source phase exceeds 128-texel column");
        }

        const auto sample = texture.texel(
            source_column,
            static_cast<int>(source_index));
        const auto subtract_byte =
            (i & 1U) != 0U ? high_subtract : low_subtract;
        out.pixels.push_back(
            static_cast<std::uint8_t>(sample - subtract_byte));
        phase += step;
    }

    return out;
}

}  // namespace rloft
