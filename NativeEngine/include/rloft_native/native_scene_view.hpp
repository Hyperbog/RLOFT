#pragma once

#include "rloft_native/native_world_runtime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

namespace rloft_native {

// Original level fixed header +0x00A..0x28A has five 64-word tables:
// primary/alternate A/B resource IDs plus automap style lookup.
// Keeping all candidate values avoids asserting which original renderer
// branch selected a table without CODE.2/field-frame evidence.
struct NativeSceneResourceCandidates {
    unsigned level{};
    unsigned column{};
    unsigned row{};
    std::uint8_t original_code{};
    std::uint8_t effective_code{};
    // -1 means no proven source record offset, not "guess zero".
    int selected_variant_offset{-1};
    bool direction_neighbor_substituted{};
    bool state_record_found{};
    bool selection_branch_verified{false};
    std::array<std::uint16_t,5> raw_lookup{};
    std::array<std::uint16_t,4> candidate_resource_ids{};
};

enum class NativeCardinal : std::uint8_t { North, East, South, West };

// The named direction is an explicit input from the caller. Which direction
// the DOS ray path chooses is NOT proven by this helper.
[[nodiscard]] NativeSceneResourceCandidates native_scene_resource_candidates(
    const NativeWorldRuntime& world,unsigned level,unsigned column,
    unsigned row,std::optional<NativeCardinal> adjacent_direction=std::nullopt);

// Host-side, bounded first-person *probe* traversal; uses original-code
// collision categories but is NOT the original Watcom CODE.2 raycaster,
// occlusion algorithm, projection, floor renderer or SINTAB compositor.
struct NativeProbeCamera {
    unsigned level{};
    double column_position{};
    double row_position{};
    double heading_radians{};
    double field_of_view_radians{1.0471975511965976};
    // Optional host-only mouse pitch: positive tilts view upward.
    double pitch_radians{};
};

struct NativeProbeRay {
    unsigned column_index{};
    bool hit{};
    bool left_archive{};
    unsigned hit_column{};
    unsigned hit_row{};
    std::uint8_t hit_code{};
    NativeCollision hit_collision{NativeCollision::Passable};
    double ray_distance{};
    double corrected_distance{};
    // 0 x-facing side, 1 y-facing side; for host probe shading ONLY.
    unsigned side{};
    std::vector<std::pair<unsigned,unsigned>> reached;
};
struct NativeProbeView {
    NativeProbeCamera camera{};
    std::vector<NativeProbeRay> columns;
    std::size_t rays_that_hit{};
    std::size_t rays_that_left_archive{};
    std::size_t walked_cells{};
    bool original_raycaster_verified{false};
    bool original_renderer_verified{false};
    bool original_texture_sampling_verified{false};
};

[[nodiscard]] NativeProbeView trace_native_probe_view(
    const NativeWorldRuntime& world,const NativeProbeCamera& camera,
    unsigned columns=160,unsigned max_steps=128);

// Mark cells seen by our provisional host probe. Original CODE.2 proves
// OR 0x0440 on cells that its OWN raycaster reaches. It does NOT establish
// that this host traversal reaches the same cells.
void apply_native_probe_visibility(NativeWorldRuntime& world,
                                    const NativeProbeView& view);

// Shared host-gameplay intent -> world-space movement. This boundary
// prevents renderer-specific A/D handedness errors. +right moves to the
// player's right for all headings; +forward faces the camera direction.
// Preview kinematics are NOT asserted to match original DOS movement.
struct NativeRelativeMovement {
    double delta_columns{};
    double delta_rows{};
};
[[nodiscard]] NativeRelativeMovement native_relative_movement(
    double heading_radians,double forward_axis,double right_axis,
    double units_per_second,double elapsed_seconds);

// Swept host navigation using the PROVEN normal collision query only.
// Does not implement actor occupancy, 16.16 motion, hidden passage mode,
// the original player collision footprint, event triggers, or interpolation.
[[nodiscard]] bool move_native_probe_camera(
    const NativeWorldRuntime& world, NativeProbeCamera& camera,
    double delta_columns,double delta_rows);

// Bounded swept host-footprint preview move. Unlike original DOS movement,
// this is a safe circular player clearance with axis slide. Stops at walls
// before entering a blocked cell, and reports contact rather than moving
// the world forward through walls. Original DOS footprint is not proven.
struct NativeProbeMoveResult {
    bool moved{};
    bool blocked{};
    double applied_columns{};
    double applied_rows{};
};
[[nodiscard]] NativeProbeMoveResult move_native_probe_camera_clearance(
    const NativeWorldRuntime& world,NativeProbeCamera& camera,
    double delta_columns,double delta_rows,double clearance=0.26);

// Own experimental probe pixels. No original palette, textures, shapes or
// SINTAB rasterizer are decoded or depicted here.
[[nodiscard]] std::vector<std::uint8_t> render_native_probe_rgb(
    const NativeProbeView& view,unsigned width,unsigned height);

} // namespace rloft_native
