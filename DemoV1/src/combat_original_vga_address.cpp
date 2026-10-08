#include "rloft/combat_original_vga_address.hpp"

#include <cstdint>

namespace rloft {

CombatOriginalVgaAddress CombatOriginalVgaAddressModel::inspect_2ab38(
    std::uint32_t row,
    std::uint32_t column,
    std::uint32_t row_shift_efed,
    std::uint32_t next_shift_eff1,
    std::uint32_t base_f001) noexcept {
    // 2AB3F..2AB55 first shifts EDX=row by the low 5 bits of
    // CL loaded from EFED, copies to EBX, then shifts EAX
    // AGAIN by the low 5 bits from EFF1 and adds the second
    // term, with original unsigned 32-bit wrap.
    const auto first=static_cast<std::uint32_t>(
        row << (row_shift_efed&31U));
    const auto second=static_cast<std::uint32_t>(
        first << (next_shift_eff1&31U));

    // 2AB57..2AB63 uses SHR column,2 (UNSIGNED), collects
    // low two bits of the column as the active VGA plane,
    // then adds the live linear page base F001.
    CombatOriginalVgaAddress out{};
    out.original_linear_address_ebx=
        first+second+(column>>2U)+base_f001;
    out.original_plane_ecx=static_cast<std::uint8_t>(column&3U);
    return out;
}

} // namespace rloft
