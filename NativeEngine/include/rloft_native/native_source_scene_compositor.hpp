#pragma once

#include "rloft_native/native_code2_wall_emission.hpp"
#include "rloft_native/native_type7_vga_dispatch.hpp"
#include "rloft_native/native_vga_rgb6_scanout.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft_native {

// The original spatial/depth ordering is STILL unknown. This is a
// transactional, source-data-backed renderer that executes an ordered
// stream of caller-owned original CODE.2 wall and EH7 sprite draw
// decisions; it does not infer a raycaster, actor pose or DOS frame.
struct NativeSceneWallCommand {
    unsigned cell_column{},cell_row{};
    NativeCode2WallPath path{NativeCode2WallPath::Banks3And2_32F8C};
    std::uint32_t original_phase{};
    bool original_sign_sentinel{};
    std::uint32_t caller_depth{},caller_screen_position{};
    unsigned resource_word_slot{};
    NativeCode2ProjectionRegisters projection{};
    unsigned viewport_column{};
    std::uint8_t original_palette_shade{};
    unsigned working_region_top{};
    std::optional<NativeCardinal> source_neighbor{};
};
struct NativeSceneSpriteCommand {
    std::size_t original_res3_id{};
    unsigned original_sintab_scale{};
    unsigned source_column{};
    std::int64_t source_base{};
    unsigned viewport_start_x{};
    std::uint8_t destination_width{};
    unsigned renderer_row{},renderer_row_byte_offset{};
    std::uint32_t original_ecx_lookup{};
    bool individual_columns_mode{};
};
enum class NativeSceneDrawKind : std::uint8_t { Wall, Sprite };
struct NativeSceneDrawCommand {
    NativeSceneDrawKind kind{NativeSceneDrawKind::Wall};
    NativeSceneWallCommand wall{};
    NativeSceneSpriteCommand sprite{};
};
struct NativeSceneCompositeResult {
    NativeRgb6Scanout scanout;
    std::size_t ordered_commands{};
    std::size_t wall_commands{};
    std::size_t sprite_commands{};
    std::size_t texture_source_reads{};
    std::size_t eh7_source_reads{};
    std::size_t indexed_plane_writes{};
    std::size_t total_eh7_descriptors{};
    std::string final_four_plane_sha256;
    bool original_spatial_draw_order_verified{false};
    bool original_live_dac_updates_verified{false};
    bool original_dos_frame_parity_verified{false};
};

// Originals: LEVELS/RES4/RES3/SINTAB/CODE.2 palettes/COLORS.
// The caller supplies EVERY unresolved view/material/sprite decision.
// The operation is atomic for both mutable world visibility AND VGA
// planes. If any later command or palette conversion rejects, the
// caller's world and video are left unchanged.
[[nodiscard]] NativeSceneCompositeResult
compose_native_source_scene_rgb6(
    NativeWorldRuntime& world,
    const NativeWorldMaterialCatalogue& materials,
    const NativeResArchive& res4,
    const NativeResArchive& res3,
    const NativeSintabPrograms& sintab,
    const NativeType7VgaMasks& original_vga_masks,
    const NativeOriginalLevelPaletteTable& palettes,
    const std::vector<std::uint8_t>& original_colors,
    unsigned active_level,unsigned active_page,
    const std::vector<NativeSceneDrawCommand>& draw_order,
    NativeIndexedVgaPlanes& video,
    unsigned output_height=400);

} // namespace rloft_native
