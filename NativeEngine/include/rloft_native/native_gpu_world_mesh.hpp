#pragma once
#include "rloft_native/native_world_runtime.hpp"
#include <cstdint>
#include <map>
#include <vector>
namespace rloft_native {
// Acceleration-friendly, immutable SOURCE-AUTHORED level geometry.
// This is a modern presentation path, NOT an original CODE.2 scene render.
struct NativeGpuVertex {
    float x{}, y{}, z{}, u{}, v{};
};
struct NativeGpuScene {
    unsigned level{}, width{}, height{};
    std::map<unsigned, std::vector<NativeGpuVertex>> textured_triangles;
    std::vector<NativeGpuVertex> fallback_floor;
    std::vector<NativeGpuVertex> fallback_ceiling;
    unsigned lower_wall_faces{}, upper_wall_faces{};
    unsigned floor_faces{}, ceiling_faces{}, skipped_boundary_faces{};
    bool original_material_bank_layout{true};
    bool original_dos_frame_verified{false};
};
[[nodiscard]] NativeGpuScene build_native_gpu_scene(
    const NativeWorldRuntime& world,unsigned level,
    unsigned wide_texture_count=600U,
    unsigned no_draw_wall_id=147U);
} // namespace rloft_native
