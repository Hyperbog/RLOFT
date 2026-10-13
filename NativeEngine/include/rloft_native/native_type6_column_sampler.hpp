#pragma once

#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_sintab_raster.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// Recovered CODE.2 338C8 source sampler. Original EH6 source is 128
// vertical texels; the integer 16.16 phase step is 0x800000 / height.
// Caller owns chosen RES4 texture, column, projected height and shade:
// neither visibility nor perspective is inferred here.
struct NativeType6ProjectedColumn {
    const NativeType1Raster* texture{};
    unsigned original_source_column{};
    unsigned viewport_x{};
    unsigned projected_height{};
    std::uint8_t palette_index_subtraction{};
    unsigned active_400_line_page{};    // 0 or 1, 0x8000 bytes apart
    unsigned working_region_top_row{}; // 256-line region in 0..399
    // Host-only optional mouse-look offset. 0 retains exact original
    // 338C8 sampler placement; nonzero shifts/clips inside same region.
    int host_pitch_pixels{};
    // Bounded optional host upper-wall tier. Default 128 rows and
    // offset 0 retain byte-equivalent CODE.2 338C8 core behavior.
    int host_vertical_offset{};
    unsigned host_source_rows{128U};
};

struct NativeType6ColumnDraw {
    std::size_t columns{};
    std::size_t source_texel_reads{};
    std::size_t plane_writes{};
    std::size_t first_scanline{};
    std::size_t last_scanline{};
    std::string resulting_four_plane_sha256;
    bool original_camera_projection_verified{false};
    bool original_dos_frame_parity_verified{false};
};

// Draws an ordered batch of projected surface strips. The viewport
// working region is exactly 256 lines; height is caller-supplied
// 1..256, centered by integer division and never extended beyond the
// original 320x400 hardware page. The source column selection follows
// the original CODE.2 0x40-step selector -> 2*selector=128-byte stride.
// Writes every color index to exactly one VGA plane. All input
// geometry/data are validated before committing ANY strip of the batch.
[[nodiscard]] NativeType6ColumnDraw draw_native_type6_columns(
    const std::vector<NativeType6ProjectedColumn>& calls,
    NativeIndexedVgaPlanes& target);

} // namespace rloft_native
