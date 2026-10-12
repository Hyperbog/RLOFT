#pragma once

#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_scene_view.hpp"
#include "rloft_native/native_type6_column_sampler.hpp"
#include "rloft_native/native_world_runtime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace rloft_native {

// Original LEVELS.DAT 0x2A4 header stores five 64-word lookups.
// CODE.2 exposes four texture-candidate resource banks; the fifth
// word is retained as authored metadata and is NOT a RES4 ID.
struct NativeWorldMaterialLevel {
    unsigned level{};
    std::array<std::array<std::uint16_t,5>,64> source_lookups{};
    std::string original_lookup_sha256;
};
struct NativeWorldMaterialCatalogue {
    std::vector<NativeWorldMaterialLevel> levels;
    std::string original_lookup_sha256;
    std::size_t candidate_slots{};
    std::size_t nonzero_candidate_slots{};
    std::size_t unique_wide_textures{};
    std::uint16_t original_fifth_bank_word{};
    unsigned wide_texture_count{};
    bool original_camera_material_branch_verified{false};
    bool original_dos_frame_parity_verified{false};
};

// Exhaustive source/texture join. Validate every map code and candidate
// bank, not only codes currently in use. ID 0 is a valid archive entry
// and must NOT be converted to "no texture".
// Every unique referenced EH6 resource is decoded by the native decoder.
[[nodiscard]] NativeWorldMaterialCatalogue
decode_native_world_material_catalogue(
    const NativeWorldPayload& world, const NativeResArchive& res4,
    unsigned original_wide_texture_count);

struct NativeWorldSurfaceDraw {
    unsigned level{};
    unsigned column{};
    unsigned row{};
    unsigned lookup_bank{};
    std::uint8_t original_tile_code{};
    std::uint8_t effective_tile_code{};
    std::uint16_t selected_original_res4_id{};
    int original_variant_offset{-1};
    NativeType6ColumnDraw raster{};
    bool original_wall_direction_branch_verified{false};
    bool original_camera_projection_verified{false};
};

// A real original world cell -> level table -> RES4 EH6 source ->
// 338C8 fixed-point VGA column composition. The selected one of the
// FOUR candidate banks, perspective column, output height and shade
// are EXPLICIT inputs, not an inferred CODE.2 camera/view branch.
// The original CELL record may add its proven variant offset; any
// resulting source ID outside the wide texture class fails closed.
[[nodiscard]] NativeWorldSurfaceDraw draw_native_world_surface_column(
    const NativeWorldRuntime& world,
    const NativeWorldMaterialCatalogue& materials,
    const NativeResArchive& original_res4,
    unsigned level,unsigned cell_column,unsigned cell_row,
    unsigned candidate_bank,unsigned original_source_column,
    unsigned viewport_x,unsigned output_height,std::uint8_t shade,
    unsigned vga_page,unsigned working_region_top_row,
    NativeIndexedVgaPlanes& target,
    std::optional<NativeCardinal> source_neighbor=std::nullopt);

} // namespace rloft_native
