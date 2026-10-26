#pragma once
// Authentic CODE.2 07938 writes the unsigned -1 sentinel at C6C8
// before 181B4 reads the level number from SAVE.DAT. CODE.2 uses
// LEVELS.DAT's 32-entry index, not a host allowlist of level pairs.
// These predicates authorize bounded SOURCE-selected requests only;
// the caller must also validate real LEVELS.DAT records, live guest
// dimensions, read-only file ownership and source transition state.
#include <cstdint>
namespace rloft { namespace original_saved_level_policy {
constexpr std::uint32_t no_departing_level=0xFFFFFFFFu;
constexpr std::uint32_t archived_level_count=32u;
inline bool indexed_level(std::uint32_t level) noexcept {
    return level<archived_level_count;
}
inline bool authenticated_no_departing_level(
    std::uint32_t prior,std::uint32_t requested,
    bool restore_active,std::uint32_t source_level_callbacks,
    bool saved_world_available,std::uint32_t source_reads,
    std::uint32_t source_save_opens) noexcept {
    return prior==no_departing_level && indexed_level(requested) &&
        restore_active && source_level_callbacks==1u &&
        saved_world_available && source_reads!=0u && source_save_opens==1u;
}
inline bool source_selected_transition(std::uint32_t prior,
                                       std::uint32_t requested) noexcept {
    // Source 181B4 itself decides the destination. This only excludes
    // invalid archive indices, the save-only sentinel and no-op pairs.
    // Original 2D5D0/2D3D8 scripts, tiles and event logic decide WHICH
    // valid level should be reached; never invent portal relationships.
    return indexed_level(prior) && indexed_level(requested) &&
        prior!=requested;
}
}} // namespace rloft::original_saved_level_policy
