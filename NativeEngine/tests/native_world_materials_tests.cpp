#include "rloft_native/native_world_materials.hpp"
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
int main(){
    const auto resource=archive();
    const auto original=world_source();
    const auto dir=decode_native_world_material_catalogue(original,resource,2);
    assert(dir.levels.size()==32 && dir.candidate_slots==8192);
    assert(dir.nonzero_candidate_slots==4096);
    assert(dir.unique_wide_textures==2 && dir.original_fifth_bank_word==0xB5B1);
    assert(dir.original_lookup_sha256.size()==64);
    assert(dir.levels[0].original_lookup_sha256.size()==64);
    NativeWorldRuntime world(original);
    NativeIndexedVgaPlanes video;
    const auto first=draw_native_world_surface_column(
        world,dir,resource,0,1,0,0,3,9,128,5,0,0,video);
    assert(first.selected_original_res4_id==1);
    assert(first.original_tile_code==1 && first.effective_tile_code==1);
    assert(first.raster.source_texel_reads==128);
    assert(video.indexed_pixel(9,64)==54);
    assert(video.indexed_pixel(9,65)==55);
    const auto digest=video.planes_sha256();
    const auto second=draw_native_world_surface_column(
        world,dir,resource,0,1,0,1,3,10,128,5,0,0,video);
    assert(second.selected_original_res4_id==0);
    assert(video.indexed_pixel(10,64)==4);
    assert(video.planes_sha256()!=digest);
    const auto safe=video.planes_sha256();
    rejects([&]{static_cast<void>(draw_native_world_surface_column(
        world,dir,resource,0,1,0,4,3,11,128,0,0,0,video));});
    rejects([&]{static_cast<void>(draw_native_world_surface_column(
        world,dir,resource,0,1,0,0,64,11,128,0,0,0,video));});
    rejects([&]{static_cast<void>(draw_native_world_surface_column(
        world,dir,resource,0,1,0,0,3,11,257,0,0,0,video));});
    assert(video.planes_sha256()==safe);
    auto invalid=original;
    invalid.levels[3].original_header[0xA]=3;
    rejects([&]{static_cast<void>(
        decode_native_world_material_catalogue(invalid,resource,2));});
    invalid=original;
    invalid.levels[31].original_header[0xA+4*128]=0;
    rejects([&]{static_cast<void>(
        decode_native_world_material_catalogue(invalid,resource,2));});
}
