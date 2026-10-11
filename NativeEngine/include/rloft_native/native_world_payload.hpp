#pragma once

#include "rloft_native/native_world_system.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft_native {

// A decoded original cell word is DATA, not an instruction or a conjectured
// collision rule. Only the low six-bit tile code is exposed mechanically.
struct NativeOriginalControl {
    unsigned column{};
    std::array<std::uint8_t,10> bytes{};
};

struct NativeLevelPayload {
    unsigned id{};
    unsigned width{};
    unsigned height{};
    std::vector<std::uint16_t> cell_words;
    std::vector<NativeOriginalControl> controls;
    // The three original 11-byte families, in A/B/C archive-section order.
    std::array<std::vector<std::array<std::uint8_t,11>>,3> families;
    // Original 26-byte slots include their count and five 5-byte commands.
    std::array<std::array<std::uint8_t,26>,50> event_slots{};
    std::vector<std::uint8_t> automap;
    std::vector<std::uint8_t> annotation;

    [[nodiscard]] std::uint16_t cell(unsigned x,unsigned y) const;
    [[nodiscard]] std::uint8_t tile_code(unsigned x,unsigned y) const;
};

struct NativeWorldPayload {
    std::vector<NativeLevelPayload> levels;
    std::size_t total_cells{};
    std::size_t total_control_records{};
    std::size_t total_event_commands{};

    [[nodiscard]] const NativeLevelPayload& level(unsigned id) const;
};

// Copy-on-initialization host-side mutable cells/automap. No original DOS
// event interpreter, collision, save format, or scripted behavior is implied.
struct NativeMutableLevel {
    unsigned id{};
    unsigned width{};
    unsigned height{};
    std::vector<std::uint16_t> cell_words;
    std::vector<std::uint8_t> automap;
};
struct NativeMutableWorld {
    std::vector<NativeMutableLevel> levels;
    [[nodiscard]] NativeMutableLevel& level(unsigned id);
};

// Must pass independently maintained full-world structural validation before
// any source record may enter the typed payload. Inputs remain read-only.
[[nodiscard]] NativeWorldPayload decode_native_world_payload(
    const std::vector<std::uint8_t>& original_levels_dat);

[[nodiscard]] NativeWorldPayload load_native_world_payload(
    const NativeMedia& original_media);

[[nodiscard]] NativeMutableWorld clone_native_mutable_world(
    const NativeWorldPayload& original);

} // namespace rloft_native
