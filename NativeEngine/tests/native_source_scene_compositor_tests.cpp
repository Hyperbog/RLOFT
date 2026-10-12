#include "rloft_native/native_source_scene_compositor.hpp"
#include "rloft_native/native_palette_context.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>
using namespace rloft_native;
namespace {
template<class F> void rejects(F fn){
    bool failed=false;
    try{fn();}catch(const std::exception&){failed=true;}
    assert(failed);
}
void u32(std::vector<std::uint8_t>& b,std::size_t p,unsigned v){
    for(unsigned i=0;i<4;++i)b.at(p+i)=static_cast<std::uint8_t>(v>>(8U*i));
}
std::vector<std::uint8_t> eh6(unsigned width,unsigned seed){
    const auto length=width*128U;
    std::vector<std::uint8_t> out{'E','H',6,0,0,
        static_cast<std::uint8_t>(length),static_cast<std::uint8_t>(length>>8U)};
    for(unsigned pos=0;pos<length;pos+=8){
        out.push_back(255);
        for(unsigned j=0;j<8;++j)out.push_back(
            static_cast<std::uint8_t>(seed+((pos+j)/128U)*3U+((pos+j)%128U)));
    }
    return out;
}
NativeResArchive archive(){
    const auto t0=eh6(64,0),t1=eh6(64,50),t2=eh6(32,80);
    std::vector<std::uint8_t> data(16,0);
    u32(data,0,3);u32(data,4,16);
    u32(data,8,16+static_cast<unsigned>(t0.size()));
    u32(data,12,16+static_cast<unsigned>(t0.size()+t1.size()));
    for(const auto* v:{&t0,&t1,&t2})data.insert(data.end(),v->begin(),v->end());
    return NativeResArchive(std::move(data));
}
NativeWorldPayload world_source(){
    NativeWorldPayload result;
    for(unsigned level=0;level<32;++level){
        NativeLevelPayload map;
        map.id=level;map.width=2;map.height=2;
        map.cell_words={0,1,0,0};
        map.automap.assign(4,0);
        for(unsigned code=0;code<64;++code){
            const unsigned values[]={1,0,1,0,0xB5B1};
            for(unsigned bank=0;bank<5;++bank){
                const auto pos=0xA+(bank*64U+code)*2U;
                map.original_header[pos]=static_cast<std::uint8_t>(values[bank]);
                map.original_header[pos+1]=static_cast<std::uint8_t>(values[bank]>>8U);
            }
        }
        result.total_cells+=4;
        result.levels.push_back(std::move(map));
    }
    return result;
}
}

std::vector<std::uint8_t> original_code2_fixture(){
    constexpr std::size_t obj=0x75F24;
    std::vector<std::uint8_t> code(obj+0xEF4D+1U,0);
    for(unsigned phase=0;phase<4;++phase)
        for(unsigned count=1;count<=7;++count)
            for(unsigned c=0;c<count;++c) {
                const unsigned x=phase+c;
                const unsigned offset[] = {0xC668,0xC688,0xC6A8};
                code[obj+offset[x/4U]+phase*8U+count] |=
                    static_cast<std::uint8_t>(1U<<(x&3U));
            }
    code[obj+0xC029]=1;code[obj+0xEF4D]=0;
    for(unsigned i=0;i<32;++i)
        code[obj+0x81AC+i*3U]=
            static_cast<std::uint8_t>(native_world_palette_bank(i));
    return code;
}
std::vector<std::uint8_t> colors_fixture(){
    std::vector<std::uint8_t> b(1U+15U*768U,0);
    b[0]=15;
    for(unsigned bank=0;bank<15;++bank)
        for(unsigned i=0;i<256;++i)
            for(unsigned k=0;k<3;++k)
                b[1+bank*768U+i*3U+k]=
                    static_cast<std::uint8_t>((bank+i+k*7U)&63U);
    return b;
}
std::vector<std::uint8_t> synthetic_sintab(){
    const std::vector<std::uint8_t> code={
        0x8A,0x86,0,0,0,0,0x88,0xC4,0x29,0xC8,
        0x88,0x87,0,0,0,0,0x88,0xA7,80,0,0,0,0xC3};
    constexpr std::size_t offset=0x1F42;
    std::vector<std::uint8_t> s(offset,0);
    u32(s,0x1B38,127);u32(s,0x1B3C,508);u32(s,0x1B40,128);
    u32(s,0x1F3E,127U*(0x204U+23U));
    for(unsigned block=0;block<127;++block){
        std::vector<std::uint8_t> b(0x204+23,0);
        for(unsigned k=0;k<129;++k){
            const unsigned p=k*2U;
            const unsigned w=0x204U+(k==128?22U:0U);
            b[p]=static_cast<std::uint8_t>(w);
            b[p+1]=static_cast<std::uint8_t>(w>>8U);
        }
        for(unsigned j=0;j<code.size();++j)b[0x204+j]=code[j];
        s.push_back(static_cast<std::uint8_t>(b.size()));
        s.push_back(static_cast<std::uint8_t>(b.size()>>8U));
        s.insert(s.end(),b.begin(),b.end());
    }
    s.resize(s.size()+512U,0);
    return s;
}
std::vector<std::uint8_t> synthetic_res3(){
    // One legal EH7 source column, one entry=0,stop=256 span,
    // source column starts at body+0. Only original x86 grammar used.
    std::vector<std::uint8_t> decoded(20,0);
    decoded[0]=64;
    decoded[8]=127;
    decoded[10]=12;
    decoded[12]=0;decoded[13]=1; // stop selector 256
    // source adjustment 0, entry selector 0
    // descriptor terminator at offset 18
    std::vector<std::uint8_t> eh{'E','H',7,0,0,20,0};
    for(unsigned at=0;at<decoded.size();){
        eh.push_back(0xff);
        for(unsigned k=0;k<8 && at<decoded.size();++k)
            eh.push_back(decoded[at++]);
    }
    std::vector<std::uint8_t> res(8,0);
    u32(res,0,1);u32(res,4,8);
    res.insert(res.end(),eh.begin(),eh.end());
    return res;
}
int main(){
    const auto media=archive();
    const auto sprites=NativeResArchive(synthetic_res3());
    const auto level_data=world_source();
    const auto material=decode_native_world_material_catalogue(
        level_data,media,2);
    const auto code=original_code2_fixture();
    const auto masks=decode_native_type7_vga_masks(code);
    const auto palettes=decode_native_original_level_palettes(code);
    const auto sintab=decode_native_sintab_programs(synthetic_sintab());
    const auto colors=colors_fixture();
    NativeSceneDrawCommand wall,sprite;
    wall.kind=NativeSceneDrawKind::Wall;
    wall.wall.cell_column=1;wall.wall.cell_row=0;
    wall.wall.path=NativeCode2WallPath::Banks3And2_32F8C;
    wall.wall.original_phase=0x5555;
    wall.wall.original_sign_sentinel=true;
    wall.wall.resource_word_slot=0;
    wall.wall.viewport_column=9;
    wall.wall.projection.live_x_255f0=0x20000;
    wall.wall.projection.direction_x_255f8=0x8000;
    wall.wall.projection.height_numerator_255e0=0x8000; // height128
    sprite.kind=NativeSceneDrawKind::Sprite;
    sprite.sprite.original_res3_id=0;
    sprite.sprite.original_sintab_scale=126;
    sprite.sprite.source_column=0;
    sprite.sprite.viewport_start_x=9;
    sprite.sprite.destination_width=3;
    sprite.sprite.renderer_row=64;
    sprite.sprite.original_ecx_lookup=0;
    NativeWorldRuntime world(level_data);
    NativeIndexedVgaPlanes video;
    const auto first=compose_native_source_scene_rgb6(
        world,material,media,sprites,sintab,masks,palettes,colors,
        0,0,{wall,sprite},video);
    assert(first.ordered_commands==2);
    assert(first.wall_commands==1 && first.sprite_commands==1);
    assert(first.texture_source_reads==128);
    assert(first.eh7_source_reads==1);
    assert(first.total_eh7_descriptors==1);
    assert(first.indexed_plane_writes==134);
    assert(first.scanout.width==320 && first.scanout.height==400);
    assert(first.scanout.bank==4);
    assert(!first.original_spatial_draw_order_verified);
    assert(!first.original_dos_frame_parity_verified);
    assert((world.cell(0,1,0).word&0x0440U)==0x0440U);
    assert(video.indexed_pixel(9,64)==64);
    assert(video.indexed_pixel(10,64)==64);
    assert(video.indexed_pixel(11,64)==64);
    assert(first.scanout.rgb6_sha256==
           scanout_native_world_vga_rgb6(
               video,palettes,colors,0,0).rgb6_sha256);
    const auto after_sprite=video.planes_sha256();

    NativeWorldRuntime reverse_world(level_data);
    NativeIndexedVgaPlanes reverse_video;
    const auto reverse=compose_native_source_scene_rgb6(
        reverse_world,material,media,sprites,sintab,masks,palettes,colors,
        0,0,{sprite,wall},reverse_video);
    assert(reverse.wall_commands==1 && reverse.sprite_commands==1);
    assert(reverse_video.indexed_pixel(9,64)==126);
    assert(reverse_video.indexed_pixel(9,65)==127);
    assert(reverse_video.planes_sha256()!=after_sprite);
    assert(reverse.scanout.rgb6_sha256!=first.scanout.rgb6_sha256);

    // One invalid later sprite cannot leak an earlier accepted
    // wall's visibility changes or its VGA pixels.
    NativeWorldRuntime undrawn(level_data);
    NativeIndexedVgaPlanes untouched;
    const auto prior=untouched.planes_sha256();
    const auto original_cell=undrawn.cell(0,1,0).word;
    auto invalid_sprite=sprite;
    invalid_sprite.sprite.original_res3_id=1;
    rejects([&]{static_cast<void>(compose_native_source_scene_rgb6(
        undrawn,material,media,sprites,sintab,masks,palettes,colors,
        0,0,{wall,invalid_sprite},untouched));});
    assert(untouched.planes_sha256()==prior);
    assert(undrawn.cell(0,1,0).word==original_cell);

    // Pixel conversion is part of the same commit boundary.
    auto corrupt_colors=colors;
    corrupt_colors[1U+4U*768U+64U*3U]=64;
    rejects([&]{static_cast<void>(compose_native_source_scene_rgb6(
        undrawn,material,media,sprites,sintab,masks,palettes,corrupt_colors,
        0,0,{wall,sprite},untouched));});
    assert(untouched.planes_sha256()==prior);
    assert(undrawn.cell(0,1,0).word==original_cell);

    auto badmask=masks;
    badmask.original_code2_sha256="bad";
    rejects([&]{static_cast<void>(compose_native_source_scene_rgb6(
        undrawn,material,media,sprites,sintab,badmask,palettes,colors,
        0,0,{wall,sprite},untouched));});
    rejects([&]{static_cast<void>(compose_native_source_scene_rgb6(
        undrawn,material,media,sprites,sintab,masks,palettes,colors,
        32,0,{wall,sprite},untouched));});
    rejects([&]{static_cast<void>(compose_native_source_scene_rgb6(
        undrawn,material,media,sprites,sintab,masks,palettes,colors,
        0,0,{},untouched));});
    auto invalid_wall=wall;
    invalid_wall.wall.projection.height_numerator_255e0=0x100000U;
    rejects([&]{static_cast<void>(compose_native_source_scene_rgb6(
        undrawn,material,media,sprites,sintab,masks,palettes,colors,
        0,0,{invalid_wall,sprite},untouched));});
    assert(undrawn.cell(0,1,0).word==original_cell);
    assert(untouched.planes_sha256()==prior);
}
