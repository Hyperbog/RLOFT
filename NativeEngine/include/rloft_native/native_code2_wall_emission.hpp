#pragma once

#include "rloft_native/native_world_materials.hpp"
#include "rloft_native/native_code2_projection.hpp"
#include <array>
#include <cstdint>
#include <optional>

namespace rloft_native {

// Original source offsets 32F8C and 33124 in CODE.2 object 1.
// This selects the TWO material IDs belonging to an emitted wall record,
// not the remaining (unrecovered) original camera/raycast emission order.
enum class NativeCode2WallPath : std::uint8_t {
    Banks3And2_32F8C,
    Banks1And0_33124
};

struct NativeCode2WallRecord {
    unsigned level{};
    unsigned column{};
    unsigned row{};
    NativeCode2WallPath path{};
    std::uint8_t original_cell_code{};
    std::uint8_t effective_cell_code{};
    std::uint8_t original_shade_class{};
    int original_state_variant_offset{-1};
    bool special_neighbor_substitution{};
    bool reflected_source_column{};
    bool sentinel_counter_increment{};
    std::uint16_t unreflected_column_selector{};
    std::uint16_t selected_column_selector{};
    unsigned source_texture_column{};
    unsigned first_source_bank{};
    unsigned second_source_bank{};
    unsigned first_original_res4_id{};
    unsigned second_original_res4_id{};
    std::uint32_t caller_depth{};
    std::uint32_t caller_screen_position{};
    bool original_viewpoint_choice_verified{false};
    bool original_camera_depth_verified{false};
    bool original_complete_frame_verified{false};
};

// Input phase is the original live DWORD 0x255E8 (32F8C) or 0x255F0
// (33124). source_sign_sentinel is an EXPLICIT caller-supplied equality
// to original 0xFFFF0000 at 0x25610 or 0x25604 respectively.
// The adjacent direction is also caller-supplied: original CODE.2
// computes it from 0x25612/0x25619 or 0x25606.
// Both original paths use (phase>>4)&0xFC0; source code 32F8C
// reflects only IF sentinel, but 33124 reflects only IF NOT sentinel.
// Source resources are two words in the original level header:
// 32F8C -> banks 3 and 2, 33124 -> banks 1 and 0.
// On actual emission, CODE.2 ORs original cell visibility bits 0x0440.
[[nodiscard]] NativeCode2WallRecord emit_native_code2_wall_record(
    NativeWorldRuntime& world,
    const NativeWorldMaterialCatalogue& materials,
    unsigned level,unsigned column,unsigned row,
    NativeCode2WallPath path,
    std::uint32_t original_phase_dword,
    bool source_sign_sentinel,
    std::uint32_t caller_depth,
    std::uint32_t caller_screen_position,
    std::optional<NativeCardinal> neighboring_direction=std::nullopt);

// Draw either of the two source resources selected by the actual
// CODE.2 wall record using the ALREADY VERIFIED EH6 column rasterizer.
// Height, shade and viewport placement are explicit upstream operands;
// no invented ray projection, resource-side selection or DOS parity.
[[nodiscard]] NativeWorldSurfaceDraw draw_native_code2_wall_record_column(
    const NativeWorldRuntime& world,
    const NativeWorldMaterialCatalogue& materials,
    const NativeResArchive& res4,
    const NativeCode2WallRecord& record,
    unsigned source_word_slot,
    unsigned viewport_x,unsigned projected_height,
    std::uint8_t shade,unsigned page,unsigned working_region_top_row,
    NativeIndexedVgaPlanes& target,
    std::optional<NativeCardinal> neighboring_direction=std::nullopt);

// Compose original CODE.2 35730's calculated wall height directly
// into the source-verified 32F8C/33124 RES4 column path. View-plane
// x, shade and source-word slot remain caller-controlled and explicit.
// A clipped value outside the currently safe 1..256 source sampler
// domain is rejected, never silently clamped to a guessed DOS frame.
struct NativeCode2ProjectedWallDraw {
    NativeCode2WallHeight projection{};
    NativeWorldSurfaceDraw raster{};
    bool original_runtime_raycaster_verified{false};
};
[[nodiscard]] NativeCode2ProjectedWallDraw
draw_native_code2_wall_record_projected_35730(
    const NativeWorldRuntime& world,
    const NativeWorldMaterialCatalogue& materials,
    const NativeResArchive& res4,
    const NativeCode2WallRecord& record,
    const NativeCode2ProjectionRegisters& projection_state,
    unsigned original_resource_word_slot,
    unsigned viewport_x,
    std::uint8_t palette_shade,
    unsigned page,unsigned region_top,
    NativeIndexedVgaPlanes& target,
    std::optional<NativeCardinal> neighboring_direction=std::nullopt);

} // namespace rloft_native
