#pragma once
// Original CODE.2 07938 writes the unsigned -1 sentinel at C6C8 before
// invoking 181B4 with the level decoded from SAVE.DAT. No departing world
// exists in this state; a retail portal must never enter this path.
#include <cstdint>
namespace rloft { namespace original_saved_level_policy {
constexpr std::uint32_t no_departing_level=0xFFFFFFFFu;
inline bool authenticated_no_departing_level(
    std::uint32_t prior,std::uint32_t requested,
    bool restore_active,std::uint32_t source_level_callbacks,
    bool saved_world_available,std::uint32_t source_reads,
    std::uint32_t source_save_opens) noexcept {
    return prior==no_departing_level && requested==23u &&
        restore_active && source_level_callbacks==1u &&
        saved_world_available && source_reads!=0u && source_save_opens==1u;
}
inline bool archived_pair(std::uint32_t prior,
                          std::uint32_t requested) noexcept {
    return (prior==22u && requested==23u) ||
           (prior==23u && requested==22u) ||
           (prior==23u && requested==26u) ||
           (prior==26u && requested==23u);
}
}} // namespace rloft::original_saved_level_policy
