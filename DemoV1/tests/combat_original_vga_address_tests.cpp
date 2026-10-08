#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_vga_address.hpp"
#include "rloft/combat_irq_display_runtime.hpp"

#include <cassert>
#include <cstdint>

int main() {
    using rloft::CombatOriginalVgaAddressModel;
    using rloft::CombatVgaPageSelectionModel;
    const auto p0=CombatVgaPageSelectionModel::inspect(0U,0x800U);
    const auto p1=CombatVgaPageSelectionModel::inspect(1U,0x800U);
    const auto p2=CombatVgaPageSelectionModel::inspect(2U,0x800U);
    assert(p0.original_f001_vga_linear_base==0xA0000U);
    assert(p1.original_f001_vga_linear_base==0xA8000U);
    assert(p2.original_f001_vga_linear_base==0xB0000U);

    // The original 80-byte row layout is one VALID observation,
    // not a universal hardcoded assumption. EFED=4,EFF1=2
    // produces 16+64=80 bytes per scanline.
    for(std::uint32_t column=0U;column<320U;++column) {
        const auto out=CombatOriginalVgaAddressModel::inspect_2ab38(
            263U,column,4U,2U,p1.original_f001_vga_linear_base);
        assert(out.original_linear_address_ebx==
               0xA8000U+263U*80U+column/4U);
        assert(out.original_plane_ecx==(column&3U));
    }
    const auto last=CombatOriginalVgaAddressModel::inspect_2ab38(
        399U,319U,4U,2U,p0.original_f001_vga_linear_base);
    assert(last.original_linear_address_ebx==0xA0000U+31999U);
    assert(last.original_plane_ecx==3U);

    // Original SHL CL count uses just five low bits, and the
    // second shift is applied to an ALREADY shifted row value.
    const auto shifts=CombatOriginalVgaAddressModel::inspect_2ab38(
        5U,0U,33U,32U,0U);
    assert(shifts.original_linear_address_ebx==20U);
    assert(shifts.original_plane_ecx==0U);

    // Original column SHR is UNSIGNED even if the caller supplied
    // a signed-negative-looking position.
    const auto high=CombatOriginalVgaAddressModel::inspect_2ab38(
        0U,0xFFFFFFFFU,0U,0U,0U);
    assert(high.original_linear_address_ebx==0x3FFFFFFFU);
    assert(high.original_plane_ecx==3U);

    // Both original DWORD SHL and ADD operations wrap modulo 2^32.
    const auto wrapped=CombatOriginalVgaAddressModel::inspect_2ab38(
        0x80000001U,16U,1U,2U,0xFFFFFFF0U);
    assert(wrapped.original_linear_address_ebx==0xFFFFFFFEU);
    assert(wrapped.original_plane_ecx==0U);

    // A changed observed row mode remains a changed *physical*
    // address. No framebuffer clipping, coercion or modern
    // 320-column flattening is performed by 2AB38 itself.
    const auto alternate=CombatOriginalVgaAddressModel::inspect_2ab38(
        3U,7U,3U,1U,0xA0000U);
    assert(alternate.original_linear_address_ebx==0xA0000U+73U);
    assert(alternate.original_plane_ecx==3U);
}
