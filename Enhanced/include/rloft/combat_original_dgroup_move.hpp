#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// Bounded capture of ORIGINAL DGROUP bytes. The base is an original
// DGROUP offset, not a host pointer or host process address.
struct CombatOriginalDgroupCapture final {
    std::uint32_t captured_base_dgroup_offset{};
    std::vector<std::uint8_t> bytes;
};

struct CombatOriginalDgroupMoveResult final {
    std::uint32_t copied_bytes{};
    bool used_original_reverse_branch{};
};

// Original CODE.2 30CD4..30D25: overlap-aware memory transfer.
// The two pointer comparisons and the src+count LEA use ORIGINAL
// unsigned 32-bit arithmetic, not modern address ordering.
class CombatOriginalDgroupMoveModel final {
public:
    [[nodiscard]] static std::optional<CombatOriginalDgroupMoveResult>
    move_30cd4(
        CombatOriginalDgroupCapture& observed_memory,
        std::uint32_t destination_dgroup_offset,
        std::uint32_t source_dgroup_offset,
        std::uint32_t byte_count);
};

} // namespace rloft
