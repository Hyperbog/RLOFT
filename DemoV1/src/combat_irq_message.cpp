#include "rloft/combat_irq_message.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rloft {

std::optional<CombatIrqMessageProjection> CombatIrqMessageModel::queue(
    const CombatIrqMessageSnapshot& observed) {
    std::size_t terminated_length=0U;
    while(terminated_length<observed.source_c_string.size() &&
          observed.source_c_string[terminated_length]!=0U) {
        ++terminated_length;
    }
    // Original 1C21C loops until the FIRST NUL. Without one the
    // original reads into unrelated memory, not a safe C++ string.
    if(terminated_length==observed.source_c_string.size() ||
       terminated_length>=observed.destination_buffer.size())
        return std::nullopt;

    CombatIrqMessageProjection out{};
    // 1C21E calls 30700 and 1C227 adds literal 5Ah modulo DWORD.
    out.deadline_cf54=observed.original_irq_tick+0x5AU;
    out.pending_flag_cf59=1U;
    out.bytes_copied=terminated_length+1U;
    out.destination_buffer=observed.destination_buffer;
    // The original copies in pairs but stops on either first or
    // second-byte NUL. The final bytes and touched span are exactly
    // the prefix through its first NUL; trailing buffer bytes persist.
    for(std::size_t i=0U;i<=terminated_length;++i)
        out.destination_buffer[i]=observed.source_c_string[i];
    return out;
}

bool CombatIrqMessageModel::deadline_passed(
    std::uint32_t observed_irq_tick,
    std::uint32_t deadline_cf54) noexcept {
    // 1C2D1 CMP followed by 1C2D7 JBE: expire only when
    // unsigned IRQ tick > unsigned deadline (not >=).
    return observed_irq_tick>deadline_cf54;
}

} // namespace rloft
