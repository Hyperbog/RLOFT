#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_vga_backing_binding.hpp"
#include "rloft/combat_original_overlay_backing_clip_binding.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>

int main() {
    using namespace rloft;
    VgaPlanarMemory vga{};
    CombatOriginalDgroupCapture ram{};
    ram.captured_base_dgroup_offset=0x9E3AU;
    ram.bytes.assign(0x800U,0xDBU);
    CombatOriginalVgaCopyState video{};
    video.linear_page_base_f001=0xA8000U;
    video.selected_page_efe9=1U;
    video.page_stride_f005=0x800U;
    video.row_pitch_eff9=80U;
    video.row_shift_efed=4U;
    video.row_shift_eff1=2U;
    CombatOriginalOverlayClipLimits bounds{400,320};
    auto backing=CombatOriginalVgaBackingBinder::bind(
        vga,ram,video);
    auto callbacks=CombatOriginalOverlayBackingClipBinder::bind(
        bounds,backing,CombatOriginalOverlayServices{});

    for(std::uint8_t p=0;p<4U;++p)
        for(std::uint32_t row=0;row<2U;++row)
            for(std::uint32_t x=0;x<2U;++x)
                vga.write(0xA8000U+(row+1U)*80U+x,
                    static_cast<std::uint8_t>(1U<<p),
                    static_cast<std::uint8_t>(17U+p*32U+row*2U+x));

    // Original 292BC performs REAL four-plane capture through
    // 2A114; resource buffer has four consecutive 2x2 blocks.
    callbacks.capture_292bc(0x9E3AU,1,3,2U,5U);
    for(std::uint8_t p=0;p<4U;++p)
        for(std::uint32_t x=0;x<4U;++x)
            assert(ram.bytes[p*4U+x]==17U+p*32U+x);
    assert(vga.graphics_read_map_04()&&
           *vga.graphics_read_map_04()==3U);
    vga.clear(0xCCU);
    // Original 2932C reads captured bytes through 2A178
    // and writes four separate VGA planes at active F001.
    callbacks.restore_2932c(0x9E3AU,1,3,2U,5U);
    for(std::uint8_t p=0;p<4U;++p)
        for(std::uint32_t y=0;y<2U;++y)
            for(std::uint32_t x=0;x<2U;++x)
                assert(vga.read(0xA8000U+(y+1U)*80U+x,p)==
                       17U+p*32U+y*2U+x);
    assert(*vga.sequencer_map_mask_02()==15U);

    // Original 2932C calls 2A178 with zero columns.
    // The starting column three produces one physical byte.
    for(std::uint8_t p=0;p<4U;++p)
        ram.bytes[p]=static_cast<std::uint8_t>(70U+p);
    callbacks.restore_2932c(0x9E3AU,0,3,1U,0U);
    for(std::uint8_t p=0;p<4U;++p)
        assert(vga.read(0xA8000U,p)==70U+p);
    vga.clear(0xB1U);
    callbacks.capture_292bc(0x9E3AU,0,3,1U,0U);
    assert(ram.bytes[0]==70U); // capture skipped
    assert(*vga.sequencer_map_mask_02()==15U);

    // Original EF3F/EF41 are LIVE signed WORD clip globals:
    // change before the next call, not in the binder factory.
    bounds.last_row_ef3f=1;
    callbacks.capture_292bc(0x9E3AU,9,3,2U,5U);
    assert(ram.bytes[0]==70U);
    bounds.last_row_ef3f=400;

    // Explicitly captured backing too short: no fallback sprite.
    ram.bytes.resize(2U);
    bool failed=false;
    try {callbacks.capture_292bc(0x9E3AU,1,3,2U,5U);}
    catch(const std::logic_error&) {failed=true;}
    assert(failed);
}
