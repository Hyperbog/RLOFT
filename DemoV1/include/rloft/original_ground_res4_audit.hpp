#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/resource_assets.hpp"
#include "rloft/exploration_state.hpp"

namespace rloft {

// Reports what ORIGINAL RES4 actually contains. A decoded 64x64 shape
// is a CANDIDATE, not proof that 258F8 selects it on fresh Level 22.
// Code.2 1F388 uses signed(level header +7) plus mutable 258F8.
// Do not silently interpret the Level 22 relative selector 55 as
// an absolute RES4 resource index.
struct OriginalGroundRes4Candidate final {
    int absolute_entry{};
    std::size_t compressed_entry_bytes{};
    std::uint8_t compression{};
    std::uint8_t transform{};
    std::uint8_t transparent_index{};
    std::uint32_t decoded_fnv1a32{}; // noncryptographic triage checksum
    std::uint8_t original_eh_type{1U}; // 1=EH1, 6=EH6 -> loaded runtime type1
};

struct OriginalGroundRes4Report final {
    int total_entries{};
    int type1_entries{};
    int rejected_type1_entries{};
    int type6_entries{};
    int rejected_type6_entries{};
    std::vector<OriginalGroundRes4Candidate> tiles_64x64;
};

// An original archive image PLUS the exact binary source IDs that
// selected it. There is no manual "try texture 55" fallback.
struct OriginalGroundRes4Selection final {
    int source_init_base{};
    int signed_level_selector{};
    int absolute_archive_entry{};
    std::vector<std::uint8_t> raster_source;
};

class OriginalGroundRes4Audit final {
public:
    [[nodiscard]] static OriginalGroundRes4Selection resolve(
        const std::vector<std::uint8_t>& original_init,
        const LevelState& original_level,
        const ResArchive& archive);

    [[nodiscard]] static OriginalGroundRes4Report inspect(
        const ResArchive& archive);

    // CODE.2 16E58..16E90 reads 2 bytes from INIT +0x0E into
    // a zeroed DWORD, then copies it verbatim to mutable 258F8.
    [[nodiscard]] static int base_from_init(
        const std::vector<std::uint8_t>& original_init);

    // CODE.2 1F388 selects signed(level header+7) + INIT[0x0E].
    // Only the value from the original INIT is accepted by the
    // production caller; callers may inject bases in synthetic tests.
    [[nodiscard]] static int absolute_index(
        int source_relative_selector, int verified_runtime_base,
        int original_entry_count);

    // CODE.2 2ECB4 maps on-disk EH6 through 37590/37700 into a
    // runtime type1 cache record (2EF38), so 2F29C can return its
    // decoded block. Both original EH1 and EH6 paths can yield runtime
    // type1. 34AAC skips the ORIGINAL two-byte block-length prefix;
    // the 4096 palette indices are never substituted or fabricated.
    [[nodiscard]] static std::vector<std::uint8_t> verified_tile_source(
        const ResArchive& archive, int original_absolute_index);
};

} // namespace rloft
