#include "rloft/combat_original_dgroup_move.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rloft {
namespace {
std::optional<std::size_t> captured_offset(
    const CombatOriginalDgroupCapture& memory,
    std::uint32_t original_address,
    std::uint32_t length) noexcept {
    if(original_address<memory.captured_base_dgroup_offset)
        return std::nullopt;
    const auto at=static_cast<std::uint64_t>(
        original_address-memory.captured_base_dgroup_offset);
    if(at>memory.bytes.size() ||
       length>memory.bytes.size()-at)
        return std::nullopt;
    return static_cast<std::size_t>(at);
}
} // namespace

std::optional<CombatOriginalDgroupMoveResult>
CombatOriginalDgroupMoveModel::move_30cd4(
    CombatOriginalDgroupCapture& memory,
    std::uint32_t dest,
    std::uint32_t src,
    std::uint32_t count) {
    // 30CE2 source=destination short-circuits even when count
    // is nonzero. 30CE6/30CE8/30CEB use unsigned comparisons
    // against LEA (src+count) with original DWORD wrap.
    const bool reverse=(src!=dest && src<dest &&
        static_cast<std::uint32_t>(src+count)>dest);

    // For zero-count or identical address there are no observed
    // memory reads or writes. Preserve that DOS no-op behavior.
    if(count==0U || src==dest)
        return CombatOriginalDgroupMoveResult{count,reverse};

    const auto from=captured_offset(memory,src,count);
    const auto to=captured_offset(memory,dest,count);
    if(!from || !to) return std::nullopt;

    // Source 30CFB sets DF before backward REP MOVSW/byte,
    // while 30D16 forward REP MOVSD/byte preserves ascending
    // write order. This explicit branch also honors wrap in
    // the original unsigned overlap decision.
    if(reverse) {
        for(std::size_t i=count;i>0U;--i)
            memory.bytes[*to+i-1U]=memory.bytes[*from+i-1U];
    } else {
        for(std::size_t i=0U;i<count;++i)
            memory.bytes[*to+i]=memory.bytes[*from+i];
    }
    return CombatOriginalDgroupMoveResult{count,reverse};
}

} // namespace rloft
