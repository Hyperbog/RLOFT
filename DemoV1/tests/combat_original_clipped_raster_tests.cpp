#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_clipped_raster.hpp"
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <cstdio>
using namespace rloft;
static void put32(std::vector<std::uint8_t>& b,unsigned at,std::uint32_t v){for(unsigned i=0;i<4U;++i)b[at+i]=static_cast<std::uint8_t>(v>>(8U*i));}
static void record(CombatOriginalDgroupCapture& scratch, std::uint32_t top,
    std::uint32_t bottom,std::uint32_t rows,std::uint32_t left,
    std::uint32_t right,std::uint32_t width){
    put32(scratch.bytes,0,top);put32(scratch.bytes,4,bottom);
    put32(scratch.bytes,8,rows);put32(scratch.bytes,12,left);
    put32(scratch.bytes,16,right);put32(scratch.bytes,20,width);
}
static CombatOriginalSpriteMemory source(unsigned rows=2,unsigned plane_stride=8,bool transparent=false){
    CombatOriginalSpriteMemory src{};src.captured_base_address=0x1000;src.resource_header_address=0x1000;
    src.bytes.assign(8U+plane_stride*4U+3U,0);
    src.bytes[0]=static_cast<std::uint8_t>(plane_stride*4U);src.bytes[4]=static_cast<std::uint8_t>(rows);
    src.bytes[6]=2;src.bytes[7]=transparent?0x80:0x00;
    unsigned p=8U;
    if(transparent)src.bytes[p++]=0x04;
    for(unsigned i=0;i<plane_stride*4U;++i)src.bytes[p+i]=static_cast<std::uint8_t>(1U+i);
    return src;
}
static CombatOriginalSpriteVideoState video(){CombatOriginalSpriteVideoState v{};v.page_base_f001=0xA0000;v.row_pitch_eff9=80;v.first_row_shift_efed=4;v.second_row_shift_eff1=2;return v;}
int main(){
VgaPlanarMemory vga{};CombatOriginalClippedRasterState st{}; auto v=video();
CombatOriginalDgroupCapture clip{};clip.captured_base_dgroup_offset=0xEF74U;clip.bytes.resize(24);
// Two visible rows and eight columns from a 16-column original.
record(clip,0,0,2,4,4,8);auto src=source();vga.clear(0xCC);
auto out=CombatOriginalClippedRasterModel::draw_2a6b0(vga,src,clip,0,10,20,0xEF74,v,st);
assert(out&&out->plane_passes==4&&out->bytes_compared==16&&out->bytes_written==16);
assert(out->frame_source_bytes_advanced==32U);assert(st.top_skip_bytes_f00d==0U);
assert(st.left_skip_bytes_f019==1U&&st.right_skip_bytes_f01d==1U);
assert(st.visible_plane_bytes_f021==2U&&st.remaining_plane_passes_f031==4U);
assert(*vga.last_sequencer_raw_mask_02()==0x88U);
// Each plane source bytes are rows of four, first/last byte cropped.
for(unsigned p=0;p<4;++p){for(unsigned y=0;y<2;++y){for(unsigned x=0;x<2;++x){
    const auto expected=static_cast<std::uint8_t>(1U+p*8U+y*4U+1U+x);
    assert(vga.read(0xA0000U+(10U+y)*80U+5U+x,static_cast<std::uint8_t>(p))==expected);
}}}
assert(vga.read(0xA0000U+10U*80U+7U,0U)==0xCC);

// Nontransparent frames STILL compare 0xFF as the source key.
record(clip,0,0,2,0,0,16);src=source();src.bytes[8]=0xFFU;vga.clear(0xCC);
out=CombatOriginalClippedRasterModel::draw_2a6b0(vga,src,clip,0,0,0,0xEF74,v,st);
assert(out&&out->transparent_skipped==1U&&out->bytes_written==31U);
assert(st.code_comparison_byte_2a816==0xFFU&& !out->transparent_resource);
assert(vga.read(0xA0000U,0U)==0xCC);

// Transparent frame takes per-frame sentinel BEFORE traversal.
src=source(2,8,true);record(clip,0,0,2,0,0,16);
src.bytes[9U+1U]=0x04U;vga.clear(0xCC);
out=CombatOriginalClippedRasterModel::draw_2a6b0(vga,src,clip,0,0,0,0xEF74,v,st);
assert(out&&out->transparent_resource&&out->transparent_skipped==2U);
assert(st.code_comparison_byte_2a816==4U);
assert(vga.read(0xA0000U+1U,0U)==0xCC);
assert(vga.read(0xA0000U,0U)==1U);

// Three pixels clipped on left; one survivor skips three
// whole source planes and ends in VGA plane zero.
src=source();record(clip,0,0,1,3,0,1);vga.clear(0xCC);
out=CombatOriginalClippedRasterModel::draw_2a6b0(vga,src,clip,0,0,0,0xEF74,v,st);
assert(out&&out->plane_passes==1&&out->bytes_written==1U);
assert(st.initial_skipped_planes_f02d==3U);
assert(st.remaining_plane_passes_f031==1U);
assert(vga.read(0xA0000U,0)==25U);
assert(*vga.last_sequencer_raw_mask_02()==0x11U);

// Plane3 -> plane0 crosses into the NEXT VGA physical byte.
// Original plane counts change at the rollover.
src=source(1,4);record(clip,0,0,1,0,8,8);vga.clear(0xCC);
out=CombatOriginalClippedRasterModel::draw_2a6b0(vga,src,clip,0,0,3,0xEF74,v,st);
assert(out&&out->plane_passes==4&&out->bytes_written==5U);
assert(vga.read(0xA0000U,3)==1U);
assert(vga.read(0xA0000U+1U,0)==5U);
assert(vga.read(0xA0000U+1U,1)==9U);
assert(vga.read(0xA0000U+1U,2)==13U);

// Top/bottom clipping selects middle frame rows on all four planes.
src=source(4,16);src.bytes[4]=4U;src.bytes[6]=2U;
record(clip,16,16,2,0,0,16);vga.clear(0xCC);
out=CombatOriginalClippedRasterModel::draw_2a6b0(vga,src,clip,0,0,0,0xEF74,v,st);
assert(out&&out->plane_passes==4&&out->bytes_written==32U);
assert(st.top_skip_bytes_f00d==4U&&st.bottom_skip_bytes_f011==4U);
for(unsigned p=0;p<4;++p)assert(vga.read(0xA0000U,static_cast<std::uint8_t>(p))==5U+p*16U);

// Zero rows and uncaptured VGA physical page fail without pixels.
record(clip,0,0,0,0,0,8);vga.clear(0x78);
auto before=st;
assert(!CombatOriginalClippedRasterModel::draw_2a6b0(vga,src,clip,0,0,0,0xEF74,v,st));
assert(vga.read(0xA0000U,0)==0x78);
assert(st.top_skip_bytes_f00d==before.top_skip_bytes_f00d);
record(clip,0,0,1,0,0,8);
v.page_base_f001=0xB0000;
assert(!CombatOriginalClippedRasterModel::draw_2a6b0(vga,src,clip,0,0,0,0xEF74,v,st));
assert(vga.read(0xA0000U,0)==0x78);
std::puts("2A6B0 source-order clipped sprite fixtures: PASS");
}
