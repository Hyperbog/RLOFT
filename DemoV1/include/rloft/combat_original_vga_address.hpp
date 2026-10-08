#pragma once

#include <cstdint>

namespace rloft {

// Original CODE.2 2AB38..2AB6B computes the current sprite
// destination VGA byte address and plane from EDX=row and ECX=column.
// Observed runtime DWORDs EFED/EFF1/F001 are required: the source
// CODE.2 file initializes them to zero and does not prove a fixed
// runtime width or page stride.
struct CombatOriginalVgaAddress final {
    std::uint32_t original_linear_address_ebx{};
    std::uint8_t original_plane_ecx{};
};

class CombatOriginalVgaAddressModel final {
public:
    [[nodiscard]] static CombatOriginalVgaAddress inspect_2ab38(
        std::uint32_t observed_row_edx,
        std::uint32_t observed_column_ecx,
        std::uint32_t observed_efed_first_row_shift,
        std::uint32_t observed_eff1_second_row_shift,
        std::uint32_t observed_f001_linear_page_base) noexcept;
};

} // namespace rloft
