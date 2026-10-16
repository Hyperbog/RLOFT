#pragma once
#include <cstdint>
#include <map>
#include <vector>
#include "rloft/exploration_state.hpp"

namespace rloft {
// Ported geometry contract from NativeEngine::build_native_gpu_scene.
// Unlike the old standalone explorer this consumes the LIVE current
// LevelState used by CODE.2 gameplay, collision, visibility and events.
struct EnhancedGpuVertex final { float x{},y{},z{},u{},v{}; };
struct EnhancedGpuWorldMesh final {
    int width{},height{};
    std::map<unsigned,std::vector<EnhancedGpuVertex>> textured;
    std::vector<EnhancedGpuVertex> floor,ceiling;
    unsigned lower_faces{},upper_faces{},floor_faces{},ceiling_faces{};
    unsigned skipped_boundary_faces{};
};
// Invalid authored RES4 selectors fail explicitly: no fabricated geometry.
// Static mesh must rebuild after mutable cell changes/level transition.
class EnhancedGpuWorldMeshModel final {
public:
    static EnhancedGpuWorldMesh build(const LevelState& level,
                                     unsigned wide_texture_count=600U,
                                     unsigned no_draw_wall_id=147U);
};
} // namespace rloft
