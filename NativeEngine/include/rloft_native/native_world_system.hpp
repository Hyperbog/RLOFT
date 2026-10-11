#pragma once

#include "rloft_native/native_media.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// Original mutable map state has 32 separately addressed archive levels.
// This parser is non-mutating; editable native world-state will be a
// separate layer above a byte-preserving original installed volume.
struct NativeLevelSource {
    unsigned id{};
    unsigned width{};
    unsigned height{};
    std::size_t packed_cells{};
    std::size_t control_records{};
    std::size_t family_a{};
    std::size_t family_b{};
    std::size_t family_c{};
    std::size_t event_slots_active{};
    std::size_t event_commands{};
    std::size_t annotation_bytes{};
};

struct NativeLevelSystem {
    std::vector<NativeLevelSource> levels;
    std::size_t map_cells{};
    std::size_t control_records{};
    std::size_t active_event_slots{};
    std::size_t active_event_commands{};
    std::size_t archive_bytes{};
};

// Parses all 32 original level records, all six typed section boundaries,
// 2-byte map cells, 10-byte control lists, three 11-byte actor/world
// families, 50 fixed 26-byte script slots per level, automap and notes.
[[nodiscard]] NativeLevelSystem decode_native_levels(
    const std::vector<std::uint8_t>& levels_dat);

struct NativeScaleBlock {
    std::uint16_t payload_bytes{};
    std::uint16_t initial_code_offset{};
    std::uint16_t final_code_offset{};
    std::uint32_t summed_step{};
};

struct NativeSyntabSystem {
    std::uint32_t scale_count{};
    std::uint32_t lookup_max{};
    std::uint32_t lookup_aux{};
    std::uint32_t declared_scale_bytes{};
    std::uint32_t actual_scale_bytes{};
    std::vector<NativeScaleBlock> scales;
    std::size_t file_bytes{};
};

// Decodes the original first-person projection package: trigonometry/
// lookup tables, 127 individual rasterizer microcode blocks and both
// 129-entry descriptor tables. Validates code bounds but NEVER executes
// original x86 machine code, which is unsafe and unnecessary on Win64.
[[nodiscard]] NativeSyntabSystem decode_native_sintab(
    const std::vector<std::uint8_t>& sintab);

struct NativeWorldSystem {
    NativeLevelSystem world;
    NativeSyntabSystem renderer_tables;
};

// Opens user-owned, read-only LEVELS.DAT and SINTAB through NativeMedia,
// giving the future replacement renderer real original world data.
[[nodiscard]] NativeWorldSystem load_native_world_system(
    const NativeMedia& media);

// Metadata only; no original cell words, annotations or executable SINTAB
// bytes are written to the report or executable distribution.
[[nodiscard]] std::string native_world_system_json(
    const NativeWorldSystem& world);

} // namespace rloft_native
