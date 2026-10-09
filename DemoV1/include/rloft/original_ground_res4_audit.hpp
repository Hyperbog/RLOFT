#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/resource_assets.hpp"

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
};

struct OriginalGroundRes4Report final {
    int total_entries{};
    int type1_entries{};
    int rejected_type1_entries{};
    std::vector<OriginalGroundRes4Candidate> tiles_64x64;
};

class OriginalGroundRes4Audit final {
public:
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

    // For the 34A74 sampler: the CODE.2 loader provides a decoded
    // resource body and 34AAC adds +2. The host prefixes an unused
    // two-byte address spacer; actual 4096 texels remain unmodified.
    [[nodiscard]] static std::vector<std::uint8_t> verified_tile_source(
        const ResArchive& archive, int original_absolute_index);
};

} // namespace rloft
