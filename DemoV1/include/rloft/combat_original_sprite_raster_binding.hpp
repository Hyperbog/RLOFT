#pragma once

#include <cstdint>
#include <functional>
#include <optional>

#include "rloft/combat_original_sprite_raster.hpp"
#include "rloft/combat_original_sprite_runtime.hpp"

namespace rloft {

// Preserve original 2939C/293EC argument order while resolving the
// resource pointer through a caller-owned DOS-memory capture.
// Resource and palette snapshots are never synthesized.
struct CombatOriginalSpriteMemoryReaders final {
    std::function<std::optional<CombatOriginalSpriteMemory>(std::uint32_t)>
        capture_sprite_at_pointer;
    std::function<std::optional<CombatOriginalShaderMemory>(std::uint32_t)>
        capture_shader_at_pointer;
};

class CombatOriginalSpriteRasterBinder final {
public:
    // Binds existing source-ordered 2939C/293EC dispatchers to
    // the pixel-accurate 2A380/2A584 bounded raster entry points.
    // All referenced captures, globals, and VGA memory must outlive
    // callbacks. Original F0C8 refresh executes AFTER rasterization.
    [[nodiscard]] static CombatOriginalSpriteServices bind(
        VgaPlanarMemory& real_planar_vga,
        CombatOriginalSpriteMachineState& mutable_dos_machine,
        const CombatOriginalSpriteVideoState& observed_video_registers,
        CombatOriginalSpriteMemoryReaders source_readers,
        std::function<void()> original_refresh_f0c8);
};

} // namespace rloft
