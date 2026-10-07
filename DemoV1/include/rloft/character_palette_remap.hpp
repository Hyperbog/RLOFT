#pragma once

#include <array>
#include <cstdint>

#include "rloft/resource_assets.hpp"

namespace rloft {

// CODE.1 4A0C/4B18 palette-index translation tables used by the character
// controller's dissolve and wipe renderers.
struct CharacterPaletteRemaps {
    std::array<std::array<std::uint8_t, 256>, 5> bright{};
    std::array<std::array<std::uint8_t, 256>, 5> luminance{};

    [[nodiscard]] static CharacterPaletteRemaps build(
        const Palette6& palette);
};

}  // namespace rloft
