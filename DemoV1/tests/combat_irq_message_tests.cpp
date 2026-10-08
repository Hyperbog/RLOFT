#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_irq_message.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

int main() {
    using rloft::CombatIrqMessageModel;
    using rloft::CombatIrqMessageSnapshot;
    CombatIrqMessageSnapshot s{};
    s.original_irq_tick=100U;
    s.source_c_string={'L','e','v','e','l','!','\0'};
    s.destination_buffer.assign(16U,0xCCU);
    auto result=CombatIrqMessageModel::queue(s);
    assert(result);
    assert(result->deadline_cf54==190U);
    assert(result->pending_flag_cf59==1U);
    assert(result->bytes_copied==7U);
    for(std::size_t i=0U;i<7U;++i)
        assert(result->destination_buffer[i]==s.source_c_string[i]);
    assert(result->destination_buffer[7U]==0xCCU);
    assert(s.destination_buffer[0U]==0xCCU); // no mutation
    assert(!CombatIrqMessageModel::deadline_passed(190U,190U));
    assert(CombatIrqMessageModel::deadline_passed(191U,190U));
    assert(!CombatIrqMessageModel::deadline_passed(189U,190U));

    // 1C235 checks the FIRST source byte before requesting the
    // second; the empty source writes exactly one terminating NUL.
    s.source_c_string={0U,0xAAU};
    result=CombatIrqMessageModel::queue(s);
    assert(result && result->bytes_copied==1U);
    assert(result->destination_buffer[0U]==0U);
    assert(result->destination_buffer[1U]==0xCCU);
    // NUL in the *second* byte stops after the pair.
    s.source_c_string={'A',0U,0xABU};
    result=CombatIrqMessageModel::queue(s);
    assert(result && result->bytes_copied==2U);
    assert(result->destination_buffer[2U]==0xCCU);

    // Full original DWORD addition wraps. The later CODE.2 check
    // uses UNSIGNED JBE, not wrap-safe elapsed-time arithmetic.
    s.original_irq_tick=0xFFFFFFF0U;
    result=CombatIrqMessageModel::queue(s);
    assert(result && result->deadline_cf54==0x4AU);
    assert(CombatIrqMessageModel::deadline_passed(
        0xFFFFFFF1U,result->deadline_cf54));
    assert(!CombatIrqMessageModel::deadline_passed(
        0x4AU,result->deadline_cf54));
    assert(CombatIrqMessageModel::deadline_passed(
        0x4BU,result->deadline_cf54));

    // Missing terminator and incomplete destination snapshots are
    // unsupported, never allowed to write out of captured bounds.
    s.source_c_string={'a','b'};
    assert(!CombatIrqMessageModel::queue(s));
    s.source_c_string={'a','b',0U};
    s.destination_buffer.resize(2U);
    assert(!CombatIrqMessageModel::queue(s));
    s.destination_buffer.resize(3U);
    assert(CombatIrqMessageModel::queue(s));
}
