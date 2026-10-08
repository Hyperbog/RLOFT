#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_dgroup_move.hpp"

#include <cassert>
#include <cstdint>
#include <vector>

int main() {
    using rloft::CombatOriginalDgroupCapture;
    using rloft::CombatOriginalDgroupMoveModel;
    CombatOriginalDgroupCapture memory{};
    memory.captured_base_dgroup_offset=0x9E3AU;
    memory.bytes.assign(0x800U,0xDAU);
    for(std::uint32_t i=0U;i<0x400U;++i)
        memory.bytes[i]=static_cast<std::uint8_t>(
            (i*37U+11U)&0xFFU);

    // Original 1A8C8 calls 30CD4(dst=9E3A+400,src=9E3A,400)
    // when page0's backing is copied to page1.
    auto result=CombatOriginalDgroupMoveModel::move_30cd4(
        memory,0xA23AU,0x9E3AU,0x400U);
    assert(result && result->copied_bytes==0x400U);
    assert(!result->used_original_reverse_branch);
    for(std::uint32_t i=0U;i<0x400U;++i)
        assert(memory.bytes[i+0x400U]==memory.bytes[i]);

    // Overlapping src<dst<src+count follows original reverse
    // copying even when the length is an odd byte count.
    CombatOriginalDgroupCapture odd{};
    odd.captured_base_dgroup_offset=0x200U;
    odd.bytes={0,1,2,3,4,5,6,7,8,9,10};
    result=CombatOriginalDgroupMoveModel::move_30cd4(
        odd,0x202U,0x200U,7U);
    assert(result && result->used_original_reverse_branch);
    assert((odd.bytes==std::vector<std::uint8_t>{
        0,1,0,1,2,3,4,5,6,9,10
    }));
    // Reverse direction: destination precedes the overlapping
    // source so original 30D16 performs a forward REP MOVSD.
    odd.bytes={0,1,2,3,4,5,6,7,8,9,10};
    result=CombatOriginalDgroupMoveModel::move_30cd4(
        odd,0x200U,0x202U,7U);
    assert(result && !result->used_original_reverse_branch);
    assert((odd.bytes==std::vector<std::uint8_t>{
        2,3,4,5,6,7,8,7,8,9,10
    }));

    // Identical addresses and zero byte count never touch memory.
    const auto before=odd.bytes;
    result=CombatOriginalDgroupMoveModel::move_30cd4(
        odd,0x201U,0x201U,100U);
    assert(result && odd.bytes==before);
    result=CombatOriginalDgroupMoveModel::move_30cd4(
        odd,0xFFFFFFFFU,0xAAAA0000U,0U);
    assert(result && odd.bytes==before);

    // Reject incomplete original capture before modifying bytes.
    assert(!CombatOriginalDgroupMoveModel::move_30cd4(
        odd,0x200U,0x202U,100U));
    assert(odd.bytes==before);
    assert(!CombatOriginalDgroupMoveModel::move_30cd4(
        odd,0x1FFU,0x202U,1U));

    // Original LEA/CMP unsigned wrap comparison can select
    // forward even when source < destination numerically.
    // Those high original addresses are not in this DGROUP
    // capture, so the host fails closed on missing backing.
    assert(!CombatOriginalDgroupMoveModel::move_30cd4(
        odd,0xFFFFFFF0U,0xFFFFFFE0U,0x50U));
}
