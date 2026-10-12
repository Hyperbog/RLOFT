#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft_native {

// CODE.2 2C294 emits 22-byte scene records into the active 0x2260-byte
// selected buffer (400 records), of which 2C2AF initially clears 0x1130 bytes. This type describes the staged records, NOT original
// live camera, NPC animation, or automatic RES3 frame selection.
using NativeCode2SceneRecord22=std::array<std::uint8_t,22>;

struct NativeCode2SceneOrder {
    // The source record ordinal drawn at each iteration.
    std::vector<std::size_t> source_indices;
    // Copy of records after source 2C7B0..2C7C1 mutations: +0x0A
    // becomes the draw ordinal; high BYTE of +0x02 gains 0x7D.
    std::vector<NativeCode2SceneRecord22> marked_records;
    bool original_live_scene_generation_verified{false};
    bool original_dos_frame_parity_verified{false};
};

// Original 2C3F1..2C47B chooses the smallest unsigned WORD at +2
// strictly below 0x7D00, preserving first occurrence on a tie.
// Original 2C7B0..2C7C1 records the draw index and prevents reselection.
// Fail closed when a record starts outside the observed source invariant;
// original stale-selection behavior for this case is not claimed.
[[nodiscard]] NativeCode2SceneOrder order_native_code2_scene_records_2c3f1(
    const std::vector<NativeCode2SceneRecord22>& records);

} // namespace rloft_native
