#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_clipped_raster_binding.hpp"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <utility>
using namespace rloft;
static void put32(std::vector<std::uint8_t>& b,unsigned at,std::uint32_t v){for(unsigned i=0;i<4U;++i)b[at+i]=static_cast<std::uint8_t>(v>>(8U*i));}
int main(){
VgaPlanarMemory vga;vga.clear(0xCC);
CombatOriginalSpriteVideoState video{};video.page_base_f001=0xA0000;video.row_pitch_eff9=80;video.first_row_shift_efed=4;video.second_row_shift_eff1=2;
CombatOriginalDgroupCapture clip{};clip.captured_base_dgroup_offset=0xEF74;clip.bytes.resize(24,0);
put32(clip.bytes,8,1);put32(clip.bytes,12,3);put32(clip.bytes,20,1);
CombatOriginalSpriteMemory sprite{};sprite.captured_base_address=0x1000;sprite.resource_header_address=0x1000;sprite.bytes.resize(48,0);
sprite.bytes[0]=32;sprite.bytes[4]=2;sprite.bytes[6]=2;
for(unsigned i=0;i<32;++i)sprite.bytes[8+i]=static_cast<std::uint8_t>(1+i);
CombatOriginalClippedRasterState state{};
int fullcalls=0,captures=0;
CombatOriginalSpriteClipServices existing{};existing.full_sprite_2a380=[&](auto,auto,auto,auto){++fullcalls;};
auto bound=CombatOriginalClippedRasterBinder::bind_29414(vga,video,clip,state,
 [&](std::uint32_t addr)->std::optional<CombatOriginalSpriteMemory>{++captures;assert(addr==0x1000U);return sprite;},std::move(existing));
assert(bound.full_sprite_2a380);
bound.clipped_sprite_2a6b0(0U,0U,0U,0xEF74U,0x1000U);
assert(captures==1&&fullcalls==0&&vga.read(0xA0000U,0U)==25U);
assert(*vga.last_sequencer_raw_mask_02()==0x11U);
bound.full_sprite_2a380(0,0,0,0);assert(fullcalls==1);
video.page_base_f001=0xA8000U;
bound.clipped_sprite_2a6b0(0U,0U,0U,0xEF74U,0x1000U);
assert(vga.read(0xA8000U,0U)==25U);
bool failed=false;
auto missing=CombatOriginalClippedRasterBinder::bind_29414(vga,video,clip,state,
 [](std::uint32_t)->std::optional<CombatOriginalSpriteMemory>{return std::nullopt;},{});
try { missing.clipped_sprite_2a6b0(0,0,0,0xEF74,0x1000); }catch(const std::logic_error&){failed=true;}
assert(failed);
std::puts("2A6B0 live 29414 binder: PASS");
}
