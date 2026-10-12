#pragma once

#include "rloft_native/native_code2_projection.hpp"
#include "rloft_native/native_code2_scene_order.hpp"

#include <cstdint>
#include <vector>

namespace rloft_native {

// The two CODE.2 source-verified 22-byte world-record families used
// by 2BBA0 (INIT class-one) and 2BDF4 (OBJECTS class-two).
enum class NativeCode2NonNpcFamily : std::uint8_t {
    InitPlaced = 1,
    ObjectsLoose = 2
};
struct NativeCode2StagedNonNpc {
    NativeCode2SceneRecord22 record{};
    bool passed_original_projection_gate{false};
    bool original_live_frame_selection_verified{false};
    bool original_render_dispatch_verified{false};
};

// Executes only the directly proved portions of original CODE.2
// 2BBF4..2BCC2 / 2BE48..2BF17. The caller supplies the authentic
// per-cell 0x0040 visibility result and C02D projection threshold.
// The later type-specific 2B2F4/2B088 callbacks and final scene-record
// publication are expressly NOT emulated here.
[[nodiscard]] NativeCode2StagedNonNpc stage_native_code2_non_npc_2bbf4(
    NativeCode2NonNpcFamily family,
    const NativeCode2SceneRecord22& live_source_record,
    const std::vector<std::uint8_t>& original_definition,
    bool current_cell_visible,
    const NativeCode2ProjectionRegisters& original_projection_registers,
    std::uint8_t original_minimum_height_c02d,
    const NativeCode2SceneRecord22& preexisting_output_record = {});

} // namespace rloft_native
