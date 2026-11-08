#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// One native VGA palette: 256 RGB triplets, stored as 6-bit DAC values.
// No expansion to 8-bit values occurs at the serialization boundary.
struct Palette6 {
    static constexpr std::size_t kColors = 256U;
    static constexpr std::size_t kBytes = kColors * 3U;
    std::array<std::uint8_t, kBytes> rgb{};
};

struct ColorsFile {
    // Serialized count byte equals palettes.size(). Preserve original order.
    std::vector<Palette6> palettes;
};

// Complete COLORS grammar: u8 count, then count * 768 bytes.
// Rejects empty, truncated, overlong and non-VGA (component >63) files.
[[nodiscard]] ColorsFile parse_colors(const std::vector<std::uint8_t>& bytes);
[[nodiscard]] std::vector<std::uint8_t> encode_colors(const ColorsFile& file);

// Original CODE.2 32150 in-memory brightness transform, clamps 0..63.
// Does not mutate the source palette or serialized data.
[[nodiscard]] Palette6 brighten_colors(const Palette6& source, int intensity);

} // namespace rloft
