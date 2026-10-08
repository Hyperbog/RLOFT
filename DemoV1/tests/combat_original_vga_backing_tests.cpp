#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_vga_backing.hpp"

#include <cassert>
#include <cstdint>
#include <vector>

namespace {
rloft::CombatOriginalVgaCopyState video(
    std::uint32_t base=0xA8000U) {
    rloft::CombatOriginalVgaCopyState v{};
    v.selected_page_efe9=1U;
    v.linear_page_base_f001=base;
    v.page_stride_f005=0x800U;
    v.row_pitch_eff9=80U;
    v.row_shift_efed=4U;
    v.row_shift_eff1=2U;
    return v;
}
}
int main() {
    using namespace rloft;
    VgaPlanarMemory vga{};
    CombatOriginalDgroupCapture ram{};
    ram.captured_base_dgroup_offset=0x9E3AU;
    ram.bytes.assign(0x800U,0xA5U);
    auto vid=video();
    // Starting column3 + 5 columns rounds to TWO physical bytes
    // per row. Each plane has 2*2 contiguous backing bytes.
    const CombatOriginalOverlayBackingRequest region{
        0x9E3AU,2U,3U,2U,5U};
    for(std::uint8_t plane=0;plane<4;++plane)
        for(std::uint32_t y=0;y<2;++y)
            for(std::uint32_t x=0;x<2;++x)
                vga.write(0xA8000U+(2U+y)*80U+x,
                    static_cast<std::uint8_t>(1U<<plane),
                    static_cast<std::uint8_t>(
                        7U+plane*40U+y*2U+x));
    assert(!vga.graphics_read_map_04());
    assert(!vga.sequencer_map_mask_02());
    auto r=CombatOriginalVgaBackingModel::capture_2a114(
        vga,ram,vid,region);
    assert(r&&r->initial_linear_vga_address==0xA8000U+160U);
    assert(r->initial_column_plane==3U);
    assert(r->physical_bytes_per_row==2U);
    assert(r->backing_bytes_transferred==16U);
    for(std::uint8_t p=0;p<4;++p)
        for(std::uint32_t i=0;i<4U;++i)
            assert(ram.bytes[p*4U+i]==7U+p*40U+i);
    assert(ram.bytes[16U]==0xA5U);
    assert(vga.graphics_read_map_04()&&
           *vga.graphics_read_map_04()==3U);
    assert(!vga.sequencer_map_mask_02());
    // Clear pixels without resetting original hardware selectors.
    vga.clear(0xCCU);
    assert(*vga.graphics_read_map_04()==3U);
    r=CombatOriginalVgaBackingModel::restore_2a178(
        vga,ram,vid,region);
    assert(r&&r->backing_bytes_transferred==16U);
    for(std::uint8_t p=0;p<4;++p)
        for(std::uint32_t y=0;y<2;++y)
            for(std::uint32_t x=0;x<2;++x)
                assert(vga.read(0xA8000U+(2U+y)*80U+x,p)==
                       7U+p*40U+y*2U+x);
    assert(vga.sequencer_map_mask_02()&&
           *vga.sequencer_map_mask_02()==0x0FU);
    assert(vga.last_sequencer_raw_mask_02()&&
           *vga.last_sequencer_raw_mask_02()==0xFFU);
    assert(*vga.graphics_read_map_04()==3U);

    // Original 2932C allows a zero-column restore. 2A178
    // still rounds the starting plane into a physical byte:
    // with starting plane 3, (0+3+3)>>2 == 1.
    for(std::uint8_t p=0;p<4;++p)
        ram.bytes[p]=static_cast<std::uint8_t>(0x80U+p);
    const CombatOriginalOverlayBackingRequest zero_columns{
        0x9E3AU,0U,3U,1U,0U};
    r=CombatOriginalVgaBackingModel::restore_2a178(
        vga,ram,vid,zero_columns);
    assert(r&&r->physical_bytes_per_row==1U);
    assert(r->backing_bytes_transferred==4U);
    for(std::uint8_t p=0;p<4;++p)
        assert(vga.read(0xA8000U,p)==0x80U+p);
    assert(*vga.sequencer_map_mask_02()==0x0FU);

    // With zero columns AND aligned starting plane0, no memory
    // byte is transferred, but four OUT 03CEh/03C4h operations
    // plus a final FF02h must STILL update observed registers.
    const CombatOriginalOverlayBackingRequest aligned_empty{
        0x9E3AU,0U,0U,2U,0U};
    vga.clear(0x53U);
    r=CombatOriginalVgaBackingModel::capture_2a114(
        vga,ram,vid,aligned_empty);
    assert(r&&r->backing_bytes_transferred==0U);
    assert(*vga.graphics_read_map_04()==3U);
    r=CombatOriginalVgaBackingModel::restore_2a178(
        vga,ram,vid,aligned_empty);
    assert(r&&r->backing_bytes_transferred==0U);
    assert(*vga.sequencer_map_mask_02()==0x0FU);
    assert(vga.read(0xA8000U,0U)==0x53U);

    // Original loops on zero height without a bounded termination;
    // fail before changing source, destination or registers.
    vga.clear(0x91U);
    const auto ram_before=ram.bytes;
    const CombatOriginalOverlayBackingRequest zero_rows{
        0x9E3AU,0U,0U,0U,16U};
    assert(!CombatOriginalVgaBackingModel::capture_2a114(
        vga,ram,vid,zero_rows));
    assert(!CombatOriginalVgaBackingModel::restore_2a178(
        vga,ram,vid,zero_rows));
    assert(ram.bytes==ram_before);
    assert(vga.read(0xA8000U,0U)==0x91U);

    // Uncaptured memory or out-of-aperture source must not partially
    // change any pixel or backing byte before failing closed.
    auto missing=ram;
    missing.bytes.resize(3U);
    assert(!CombatOriginalVgaBackingModel::capture_2a114(
        vga,missing,vid,region));
    assert(!CombatOriginalVgaBackingModel::restore_2a178(
        vga,missing,vid,region));
    assert(vga.read(0xA8000U,0U)==0x91U);
    auto bad_video=video(0xB1000U);
    assert(!CombatOriginalVgaBackingModel::capture_2a114(
        vga,ram,bad_video,region));
    assert(ram.bytes==ram_before);

    // No hardcoded pitch: 16+64 is just this fixture's EFF9=80.
    // Live video row-stride changes are observed on each call.
    vid.row_pitch_eff9=40U;
    vid.row_shift_efed=3U;
    vid.row_shift_eff1=2U; // 8+32 = 40
    for(std::uint8_t p=0;p<4;++p)
        vga.write(0xA8000U+40U,static_cast<std::uint8_t>(1U<<p),
                  static_cast<std::uint8_t>(p+10U));
    const CombatOriginalOverlayBackingRequest dynamic{
        0xA23AU,1U,0U,1U,4U};
    r=CombatOriginalVgaBackingModel::capture_2a114(
        vga,ram,vid,dynamic);
    assert(r&&r->backing_bytes_transferred==4U);
    for(std::uint8_t p=0;p<4;++p)
        assert(ram.bytes[0x400U+p]==p+10U);
}
