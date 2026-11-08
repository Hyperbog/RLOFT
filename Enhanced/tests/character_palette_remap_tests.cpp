#include "rloft/character_palette_remap.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>

int main() {
    rloft::Palette6 palette{};

    // 256 unique 6-bit colors. The first 28 entries have monotonically
    // increasing red, which also gives 4B18 a controlled bucket ramp.
    for (int i = 0; i < 256; ++i) {
        palette.rgb[static_cast<std::size_t>(i) * 3U] =
            static_cast<std::uint8_t>(i & 0x3F);
        palette.rgb[static_cast<std::size_t>(i) * 3U + 1U] =
            static_cast<std::uint8_t>((i >> 6) & 0x03);
        palette.rgb[static_cast<std::size_t>(i) * 3U + 2U] = 0;
    }

    const auto maps =
        rloft::CharacterPaletteRemaps::build(palette);

    // 4A0C row zero asks 5B64 for the unmodified source color. With unique
    // palette entries, the encoded tree search must return the same index.
    for (int i = 0; i < 256; ++i) {
        assert(maps.bright[0][static_cast<std::size_t>(i)] == i);
    }

    // 4B18 computes luminance by three separately truncated weighted terms.
    // Palette index 10 is RGB(10,0,0): luminance=(10*300)/1000 = 3.
    // The first palette-red run contains 0,1,2,3, so bucket result is 3.
    assert(maps.luminance[0][10] == 3);

    // RGB(0,1,0) at index 64 truncates to luminance zero.
    assert(maps.luminance[0][64] == 0);

    // Every generated translation remains a palette byte.
    for (const auto& row : maps.bright) {
        for (const auto value : row) {
            assert(value <= 0xFF);
        }
    }
    for (const auto& row : maps.luminance) {
        for (const auto value : row) {
            assert(value <= 0xFF);
        }
    }

    std::cout << "character_palette_remap_tests: PASS\n";
}
