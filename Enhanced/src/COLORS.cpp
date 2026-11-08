#include "rloft/COLORS.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

void require_vga(const Palette6& palette) {
    if (std::any_of(palette.rgb.begin(), palette.rgb.end(),
                    [](std::uint8_t component) { return component > 63U; })) {
        throw std::invalid_argument("COLORS RGB component exceeds 6-bit VGA range");
    }
}

} // namespace

ColorsFile parse_colors(const std::vector<std::uint8_t>& bytes) {
    if (bytes.empty() || bytes[0] == 0U) {
        throw std::runtime_error("COLORS has no palette count");
    }
    const auto count = static_cast<std::size_t>(bytes[0]);
    if (bytes.size() != 1U + count * Palette6::kBytes) {
        throw std::runtime_error("COLORS size disagrees with palette count");
    }
    ColorsFile file{};
    file.palettes.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        Palette6 palette{};
        const auto at = bytes.begin() + static_cast<std::ptrdiff_t>(
            1U + index * Palette6::kBytes);
        std::copy_n(at, Palette6::kBytes, palette.rgb.begin());
        require_vga(palette);
        file.palettes.push_back(palette);
    }
    return file;
}

std::vector<std::uint8_t> encode_colors(const ColorsFile& file) {
    if (file.palettes.empty() || file.palettes.size() > 255U) {
        throw std::invalid_argument("COLORS palette count exceeds u8 range");
    }
    std::vector<std::uint8_t> bytes;
    bytes.reserve(1U + file.palettes.size() * Palette6::kBytes);
    bytes.push_back(static_cast<std::uint8_t>(file.palettes.size()));
    for (const auto& palette : file.palettes) {
        require_vga(palette);
        bytes.insert(bytes.end(), palette.rgb.begin(), palette.rgb.end());
    }
    return bytes;
}

Palette6 brighten_colors(const Palette6& source, int intensity) {
    require_vga(source);
    const int level = std::clamp(intensity, 0, 63);
    Palette6 result = source;
    if (level == 0) return result;
    for (std::size_t i = 0; i < Palette6::kColors; ++i) {
        const auto at = i * 3U;
        const int red = result.rgb[at];
        const int green = result.rgb[at + 1U];
        const int blue = result.rgb[at + 2U];
        const int peak = std::max({red, green, blue});
        if (peak == 0) continue;
        const int boost = ((63 - peak) * level + 31) / 63;
        const int target = peak + boost;
        for (std::size_t c = 0; c < 3U; ++c) {
            const int channel = result.rgb[at + c];
            result.rgb[at + c] = static_cast<std::uint8_t>(
                (channel * target + peak / 2) / peak);
        }
    }
    return result;
}

} // namespace rloft
