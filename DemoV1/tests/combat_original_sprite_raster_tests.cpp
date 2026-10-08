#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_sprite_raster.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {
rloft::CombatOriginalSpriteMemory sprite(
    std::uint16_t frame_stride,
    std::uint16_t height,
    std::uint8_t width,
    std::uint8_t flags,
    const std::vector<std::uint8_t>& pixels) {
    rloft::CombatOriginalSpriteMemory out{};
    out.captured_base_address=0x1000U;
    out.resource_header_address=0x1000U;
    out.bytes.resize(8U,0U);
    out.bytes[0U]=static_cast<std::uint8_t>(frame_stride);
    out.bytes[1U]=static_cast<std::uint8_t>(frame_stride>>8U);
    out.bytes[4U]=static_cast<std::uint8_t>(height);
    out.bytes[5U]=static_cast<std::uint8_t>(height>>8U);
    out.bytes[6U]=width;
    out.bytes[7U]=flags;
    out.bytes.insert(out.bytes.end(),pixels.begin(),pixels.end());
    return out;
}
rloft::CombatOriginalSpriteVideoState video(
    std::uint32_t row=1U,std::uint32_t column=2U) {
    rloft::CombatOriginalSpriteVideoState v{};
    v.row=row;
    v.column=column;
    v.first_row_shift_efed=4U;
    v.second_row_shift_eff1=2U;
    v.page_base_f001=0xA0000U;
    v.row_pitch_eff9=80U;
    return v;
}
} // namespace

int main() {
    using rloft::CombatOriginalSpriteRasterModel;
    using rloft::VgaPlanarMemory;
    using rloft::CombatOriginalSpriteMemory;
    using rloft::CombatOriginalShaderMemory;
    VgaPlanarMemory mem{};

    // 2A380 raw branch: 2 rows x 2 bytes in each of 4 source
    // planes. Starting at column2 rotates write masks 4,8,1,2.
    auto raw=sprite(16U,2U,1U,0U,{
        10U,11U,12U,13U,20U,21U,22U,23U,
        30U,31U,32U,33U,40U,41U,42U,43U});
    mem.clear(0xAAU);
    auto result=CombatOriginalSpriteRasterModel::draw_2a380(
        mem,raw,0U,video());
    assert(result && !result->used_transparent_branch);
    assert(result->vga_bytes_written==16U);
    assert(result->source_bytes_consumed==16U);
    assert(result->transparent_bytes_skipped==0U);
    assert(result->final_sequencer_plane_mask==0x02U);
    for(std::uint32_t y=0U;y<2U;++y) {
        const auto addr=0xA0000U+80U*(y+1U);
        assert(mem.read(addr+0U,2U)==10U+y*2U);
        assert(mem.read(addr+1U,2U)==11U+y*2U);
        assert(mem.read(addr+0U,3U)==20U+y*2U);
        assert(mem.read(addr+1U,3U)==21U+y*2U);
        assert(mem.read(addr+1U,0U)==30U+y*2U);
        assert(mem.read(addr+2U,0U)==31U+y*2U);
        assert(mem.read(addr+1U,1U)==40U+y*2U);
        assert(mem.read(addr+2U,1U)==41U+y*2U);
        assert(mem.read(addr+0U,0U)==0xAAU);
        assert(mem.read(addr+0U,1U)==0xAAU);
    }

    // Source-backed 2AB38 + 2A380 interaction across ALL 320
    // initial columns: source plane p lands at (column+p)&3 and
    // advances the physical byte when that sum crosses plane3.
    const auto one_row=sprite(8U,1U,1U,0U,{
        1U,2U,11U,12U,21U,22U,31U,32U});
    for(std::uint32_t column=0U;column<320U;++column) {
        mem.clear(0xCCU);
        result=CombatOriginalSpriteRasterModel::draw_2a380(
            mem,one_row,0U,video(2U,column));
        assert(result && result->vga_bytes_written==8U);
        const auto rowbase=0xA0000U+2U*80U;
        for(std::uint32_t pass=0U;pass<4U;++pass) {
            const auto plane=(column+pass)&3U;
            const auto byteOffset=(column+pass)>>2U;
            const auto color=static_cast<std::uint8_t>(1U+pass*10U);
            assert(mem.read(rowbase+byteOffset,plane)==color);
            assert(mem.read(rowbase+byteOffset+1U,plane)==color+1U);
        }
    }

    // Source WORD+0 multiplies the frame index. No assumption
    // that the first indexed frame is always requested.
    raw=sprite(16U,2U,1U,0U,{
        10U,11U,12U,13U,20U,21U,22U,23U,
        30U,31U,32U,33U,40U,41U,42U,43U,
        50U,51U,52U,53U,60U,61U,62U,63U,
        70U,71U,72U,73U,80U,81U,82U,83U});
    mem.clear();
    result=CombatOriginalSpriteRasterModel::draw_2a380(
        mem,raw,1U,video());
    assert(result && result->vga_bytes_written==16U);
    assert(mem.read(0xA0000U+80U,2U)==50U);
    assert(mem.read(0xA0000U+81U,0U)==70U);

    // 2A422 transparent branch consumes ONE frame sentinel
    // and tests every byte, including high-byte source colors.
    // Each sentinel leaves the existing plane byte unchanged.
    auto transparent=sprite(17U,2U,1U,0x80U,{
        0xEEU,
        0xEEU,61U,62U,0xEEU,
        71U,0xEEU,0xEEU,74U,
        81U,82U,0xEEU,84U,
        0xEEU,92U,93U,94U
    });
    mem.clear(0xB7U);
    result=CombatOriginalSpriteRasterModel::draw_2a380(
        mem,transparent,0U,video());
    assert(result && result->used_transparent_branch);
    assert(result->sentinel==0xEEU);
    assert(result->source_bytes_consumed==17U);
    assert(result->transparent_bytes_skipped==6U);
    assert(result->vga_bytes_written==10U);
    assert(mem.read(0xA0000U+80U,2U)==0xB7U);
    assert(mem.read(0xA0000U+81U,2U)==61U);
    assert(mem.read(0xA0000U+160U,2U)==62U);
    assert(mem.read(0xA0000U+161U,2U)==0xB7U);
    assert(mem.read(0xA0000U+80U,3U)==71U);
    assert(mem.read(0xA0000U+81U,3U)==0xB7U);
    assert(mem.read(0xA0000U+161U,3U)==74U);
    assert(mem.read(0xA0000U+81U,0U)==81U);
    assert(mem.read(0xA0000U+162U,0U)==84U);

    // 2A584 does NOT subtract a shade constant from color bytes:
    // it writes a FULL DWORD LUT pointer at code operands 2A60B
    // and 2A672 and performs MOV AL,[EBX+pointer] per source byte.
    CombatOriginalShaderMemory lut{};
    lut.captured_base_address=0x11220000U;
    lut.original_lookup_pointer=0x11220080U;
    lut.bytes.resize(0x80U+256U,0U);
    for(std::uint32_t color=0U;color<256U;++color)
        lut.bytes[0x80U+color]=
            static_cast<std::uint8_t>(color^0x5AU);
    const auto opaque_shader=sprite(16U,2U,1U,0U,{
        10U,11U,12U,13U,20U,21U,22U,23U,
        30U,31U,32U,33U,40U,41U,42U,43U});
    mem.clear(0xA7U);
    result=CombatOriginalSpriteRasterModel::draw_2a584(
        mem,opaque_shader,0U,video(),lut);
    assert(result && result->used_shader_lookup);
    assert(!result->used_transparent_branch);
    assert(result->shader_pointer_self_patched==0x11220080U);
    assert(result->vga_bytes_written==16U);
    assert(result->source_bytes_consumed==16U);
    assert(result->final_sequencer_plane_mask==2U);
    assert(mem.read(0xA0000U+80U,2U)==(10U^0x5AU));
    assert(mem.read(0xA0000U+81U,3U)==(21U^0x5AU));
    assert(mem.read(0xA0000U+81U,0U)==(30U^0x5AU));
    assert(mem.read(0xA0000U+82U,1U)==(41U^0x5AU));

    // Transparency comparison is made against unmodified input
    // BEFORE the 256-entry LUT; sentinel pixels are never mapped.
    mem.clear(0xA7U);
    result=CombatOriginalSpriteRasterModel::draw_2a584(
        mem,transparent,0U,video(),lut);
    assert(result && result->used_shader_lookup);
    assert(result->used_transparent_branch);
    assert(result->transparent_bytes_skipped==6U);
    assert(result->vga_bytes_written==10U);
    assert(result->source_bytes_consumed==17U);
    assert(mem.read(0xA0000U+80U,2U)==0xA7U);
    assert(mem.read(0xA0000U+81U,2U)==(61U^0x5AU));
    assert(mem.read(0xA0000U+161U,2U)==0xA7U);
    assert(mem.read(0xA0000U+80U,3U)==(71U^0x5AU));
    assert(mem.read(0xA0000U+81U,3U)==0xA7U);

    // Host captures that cannot cover all 256 original table
    // bytes are rejected before any native VGA byte is written.
    lut.bytes.resize(0x80U+255U);
    mem.clear(0x5CU);
    assert(!CombatOriginalSpriteRasterModel::draw_2a584(
        mem,opaque_shader,0U,video(),lut));
    assert(mem.read(0xA0000U+80U,2U)==0x5CU);
    lut.bytes.resize(0x80U+256U);
    lut.original_lookup_pointer=0x11220200U;
    assert(!CombatOriginalSpriteRasterModel::draw_2a584(
        mem,opaque_shader,0U,video(),lut));
    lut.original_lookup_pointer=0x11220080U;

    // DOS negative frame indexes are unsigned MUL bit patterns.
    // A source view may begin BEFORE the referenced font header.
    CombatOriginalSpriteMemory negative{};
    negative.captured_base_address=0x2000U;
    negative.resource_header_address=0x2040U;
    negative.bytes.resize(0x48U,0U);
    negative.bytes[0x40U]=48U; // frame stride
    negative.bytes[0x44U]=1U;  // height
    negative.bytes[0x46U]=1U;  // opaque, two bytes per plane
    for(std::uint8_t i=0U;i<8U;++i)
        negative.bytes[0x18U+i]=static_cast<std::uint8_t>(100U+i);
    mem.clear();
    result=CombatOriginalSpriteRasterModel::draw_2a380(
        mem,negative,0xFFFFFFFFU,video(0U,0U));
    assert(result && result->vga_bytes_written==8U);
    assert(mem.read(0xA0000U,0U)==100U);
    assert(mem.read(0xA0000U,1U)==102U);
    assert(mem.read(0xA0000U,2U)==104U);
    assert(mem.read(0xA0000U,3U)==106U);

    // Transparent BL is computed from 2*width and truncated
    // to 8 bits. At width129, only 2 bytes are processed per
    // row, and EFF9-258+2 moves next row BACK 176 bytes.
    auto wide=sprite(0U,2U,129U,0x80U,{
        0xEEU,1U,2U,3U,4U,5U,6U,7U,8U,
        9U,10U,11U,12U,13U,14U,15U,16U});
    mem.clear();
    result=CombatOriginalSpriteRasterModel::draw_2a380(
        mem,wide,0U,video(10U,0U));
    assert(result && result->vga_bytes_written==16U);
    assert(mem.read(0xA0000U+800U,0U)==1U);
    assert(mem.read(0xA0000U+624U,0U)==3U);

    // Invalid capture must fail BEFORE writing even the first
    // byte. Original DOS would read outside its memory; do not
    // silently substitute a blank sprite or partial frame.
    raw=sprite(16U,2U,1U,0U,{42U,43U});
    mem.clear(0xA5U);
    assert(!CombatOriginalSpriteRasterModel::draw_2a380(
        mem,raw,0U,video()));
    assert(mem.read(0xA0000U+80U,2U)==0xA5U);
    raw=sprite(16U,0U,1U,0U,{});
    assert(!CombatOriginalSpriteRasterModel::draw_2a380(
        mem,raw,0U,video()));
    raw=sprite(16U,1U,128U,0x80U,{0xEEU});
    assert(!CombatOriginalSpriteRasterModel::draw_2a380(
        mem,raw,0U,video()));
    raw=sprite(8U,1U,1U,0U,{1,2,3,4,5,6,7,8});
    auto bad=video();
    bad.page_base_f001=0xB1000U;
    assert(!CombatOriginalSpriteRasterModel::draw_2a380(
        mem,raw,0U,bad));
    assert(mem.read(0xA0000U+80U,2U)==0xA5U);

    // A zero-width OPAQUE REP MOVSW performs no memory writes,
    // but still rotates through the four sequencer map masks.
    auto empty=sprite(0U,1U,0U,0U,{});
    result=CombatOriginalSpriteRasterModel::draw_2a380(
        mem,empty,0U,video());
    assert(result && result->vga_bytes_written==0U);
    assert(result->source_bytes_consumed==0U);
    assert(result->final_sequencer_plane_mask==2U);
}
