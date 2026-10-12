#pragma once

#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_sintab_raster.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// Original CODE.2 object 2 display triples at file 0x75F24+0x81AC.
// The palette is the FIRST byte of each of 32 triples. Two remaining
// original source bytes are retained, not mislabelled as palette IDs.
struct NativeOriginalLevelPaletteTable {
    std::array<std::array<std::uint8_t,3>,32> original_display_triples{};
    std::string original_triplets_sha256;
    std::string original_code2_sha256;
    bool live_dos_palette_switching_verified{false};
};

[[nodiscard]] NativeOriginalLevelPaletteTable
decode_native_original_level_palettes(
    const std::vector<std::uint8_t>& original_code2);

// Original planar 256-color VGA scanout. Output remains true DAC6 samples
// (0..63) to avoid introducing any unproven host gamma/DAC8 mapping.
// The caller selects an original palette bank via source-backed context.
struct NativeRgb6Scanout {
    unsigned width{};
    unsigned height{};
    unsigned page{};
    unsigned bank{};
    std::vector<std::uint8_t> rgb6; // interleaved RGB, exact DAC6 values
    std::string rgb6_sha256;
    bool original_crtc_page_selection_verified{false};
    bool original_dos_frame_parity_verified{false};
};

[[nodiscard]] NativeRgb6Scanout scanout_native_vga_rgb6(
    const NativeIndexedVgaPlanes& original_planar_frame,
    const NativePalette6& original_color_bank,
    unsigned palette_bank,
    unsigned page=0,unsigned width=320,unsigned height=400);

// Convenience for the source-exact 32-level palette mapping. Original
// COLOR palette bank selection is fixed per level table; the actual
// game's live fade/cycle/DAC updates remain outside this function.
[[nodiscard]] NativeRgb6Scanout scanout_native_world_vga_rgb6(
    const NativeIndexedVgaPlanes& original_planar_frame,
    const NativeOriginalLevelPaletteTable& original_level_table,
    const std::vector<std::uint8_t>& original_colors,
    unsigned level,unsigned page=0,
    unsigned width=320,unsigned height=400);

} // namespace rloft_native
