#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_vga_copy.hpp"

#include <cassert>
#include <cstdint>

namespace {
rloft::CombatOriginalVgaCopyState original_state() {
    rloft::CombatOriginalVgaCopyState s{};
    s.selected_page_efe9=0U;
    s.linear_page_base_f001=0xA0000U;
    s.page_stride_f005=0x800U;
    s.row_pitch_eff9=80U;
    s.row_shift_efed=4U;
    s.row_shift_eff1=2U;
    return s;
}
} // namespace

int main() {
    using rloft::CombatOriginalVgaCopyModel;
    using rloft::CombatOriginalVgaCopyRect;
    using rloft::VgaPlanarMemory;

    VgaPlanarMemory mem{};
    auto s=original_state();
    // Source column 3 extends 5 columns to TWO physical bytes;
    // destination column 2 does not determine the copy width.
    const CombatOriginalVgaCopyRect copy{
        1U,1U,3U,2U,2U,2U,5U};
    mem.clear(0xEEU);
    for(std::uint8_t p=0U;p<4U;++p)
        for(std::uint32_t y=0U;y<2U;++y)
            for(std::uint32_t x=0U;x<2U;++x)
                mem.write(0xA0000U+(y+1U)*80U+x,
                    static_cast<std::uint8_t>(1U<<p),
                    static_cast<std::uint8_t>(10U+p*30U+y*2U+x));
    assert(!mem.sequencer_map_mask_02());
    assert(!s.graphics_mode_index05);
    const auto r=CombatOriginalVgaCopyModel::copy_2a078(
        mem,s,copy);
    assert(r);
    assert(r->source_linear_address==0xA0000U+80U);
    assert(r->destination_linear_address==0xA8000U+160U);
    assert(r->original_bytes_per_row==2U);
    assert(r->vga_address_pairs_copied==4U);
    assert(r->source_page_restored==0U);
    assert(r->destination_page_selected==1U);
    for(std::uint8_t p=0U;p<4U;++p)
        for(std::uint32_t y=0U;y<2U;++y)
            for(std::uint32_t x=0U;x<2U;++x)
                assert(mem.read(0xA8000U+(y+2U)*80U+x,p)==
                    static_cast<std::uint8_t>(10U+p*30U+y*2U+x));
    assert(mem.read(0xA8000U+162U,0U)==0xEEU);
    assert(*mem.sequencer_map_mask_02()==0x0FU);
    assert(s.graphics_mode_index05 &&
           *s.graphics_mode_index05==0x40U);
    assert(s.selected_page_efe9==0U &&
           s.linear_page_base_f001==0xA0000U);
    // The source page is not modified by a separate-page copy.
    assert(mem.read(0xA0000U+80U,3U)==100U);

    // Original write mode 1 reads all four latch planes BEFORE
    // each destination byte. On SAME PAGE overlap, REP MOVSB
    // propagates overwritten bytes forward (not memmove).
    s=original_state();
    mem.clear(0xAAU);
    for(std::uint8_t p=0U;p<4U;++p)
        for(std::uint32_t at=0U;at<4U;++at)
            mem.write(0xA0000U+at,static_cast<std::uint8_t>(1U<<p),
                static_cast<std::uint8_t>(p*20U+at+1U));
    const CombatOriginalVgaCopyRect overlap{
        0U,0U,0U,0U,4U,1U,12U};
    const auto ov=CombatOriginalVgaCopyModel::copy_2a078(
        mem,s,overlap);
    assert(ov && ov->original_bytes_per_row==3U);
    for(std::uint8_t p=0U;p<4U;++p) {
        const auto first=static_cast<std::uint8_t>(p*20U+1U);
        for(std::uint32_t at=1U;at<4U;++at)
            assert(mem.read(0xA0000U+at,p)==first);
    }

    // Initial current EFE9 DWORD's LOW byte is saved via AH and
    // reselected through 2AB6C, normalizing high bits on return.
    s=original_state();
    s.selected_page_efe9=0x100U;
    mem.clear(0xA5U);
    const auto small=CombatOriginalVgaCopyModel::copy_2a078(
        mem,s,CombatOriginalVgaCopyRect{
            1U,0U,0U,0U,4U,1U,4U});
    assert(small && small->original_bytes_per_row==1U);
    assert(s.selected_page_efe9==0U);
    assert(s.linear_page_base_f001==0xA0000U);

    // An original zero COLUMN count with starting plane 0
    // produces zero REP MOVSB iterations but STILL sets ports.
    s=original_state();
    s.graphics_mode_index05=0x13U;
    const auto empty=CombatOriginalVgaCopyModel::copy_2a078(
        mem,s,CombatOriginalVgaCopyRect{
            0U,0U,0U,0U,0U,1U,0U});
    assert(empty && empty->vga_address_pairs_copied==0U);
    assert(s.graphics_mode_index05==0x40U);
    assert(*mem.sequencer_map_mask_02()==0x0FU);

    // A zero ROW count enters the original DEC EAX/JNE loop
    // without a bound; do not invent a DOS no-op for it.
    s=original_state();
    s.graphics_mode_index05=0x17U;
    assert(!CombatOriginalVgaCopyModel::copy_2a078(
        mem,s,CombatOriginalVgaCopyRect{
            1U,0U,0U,0U,0U,0U,4U}));
    assert(*s.graphics_mode_index05==0x17U);

    // Invalid video aperture must fail BEFORE mutation of host
    // VGA pixels and observed DOS register captures.
    s=original_state();
    s.graphics_mode_index05=0x17U;
    mem.clear(0x5AU);
    assert(!CombatOriginalVgaCopyModel::copy_2a078(
        mem,s,CombatOriginalVgaCopyRect{
            1U,399U,0U,400U,0U,2U,4U}));
    assert(mem.read(0xA0000U+399U*80U,0U)==0x5AU);
    assert(s.selected_page_efe9==0U);
    assert(*s.graphics_mode_index05==0x17U);

    // Original 2AB6C truncates destination page to byte,
    // but source F001 calculation uses an observed page stride.
    s=original_state();
    const auto page_mask=CombatOriginalVgaCopyModel::copy_2a078(
        mem,s,CombatOriginalVgaCopyRect{
            0x100U,1U,0U,2U,0U,1U,4U});
    assert(page_mask && page_mask->destination_page_selected==0U);
}
