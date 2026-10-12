#include "rloft_native/native_code2_wall_emission.hpp"
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
    auto original=world_source();
    // Original shade-class 2, tile code 1 in the second source cell.
    original.levels[0].cell_words[1]=0x8001U;
    const auto dir=decode_native_world_material_catalogue(original,resource,2);
    NativeWorldRuntime world(original);
    NativeIndexedVgaPlanes output;
    const auto input_phase=0x00005555U;
    // CODE.2 32F93: (0x5555>>4)&0xFC0 = 0x540, source column 21.
    const auto a=emit_native_code2_wall_record(
        world,dir,0,1,0,NativeCode2WallPath::Banks3And2_32F8C,
        input_phase,false,77,123);
    assert(a.first_source_bank==3 && a.second_source_bank==2);
    assert(a.first_original_res4_id==0 && a.second_original_res4_id==1);
    assert(a.original_cell_code==1 && a.effective_cell_code==1);
    assert(a.original_shade_class==2);
    assert(a.original_state_variant_offset==-1);
    assert(!a.reflected_source_column && !a.sentinel_counter_increment);
    assert(a.unreflected_column_selector==0x540);
    assert(a.selected_column_selector==0x540);
    assert(a.source_texture_column==21);
    assert(a.caller_depth==77 && a.caller_screen_position==123);
    assert((world.cell(0,1,0).word&0x0440U)==0x0440U);
    const auto reflected_a=emit_native_code2_wall_record(
        world,dir,0,1,0,NativeCode2WallPath::Banks3And2_32F8C,
        input_phase,true,77,123);
    assert(reflected_a.reflected_source_column);
    assert(reflected_a.sentinel_counter_increment);
    assert(reflected_a.selected_column_selector==0xA80);
    assert(reflected_a.source_texture_column==42);
    const auto reflected_b=emit_native_code2_wall_record(
        world,dir,0,1,0,NativeCode2WallPath::Banks1And0_33124,
        input_phase,false,88,456);
    assert(reflected_b.first_source_bank==1 && reflected_b.second_source_bank==0);
    assert(reflected_b.first_original_res4_id==0 && reflected_b.second_original_res4_id==1);
    assert(reflected_b.reflected_source_column && !reflected_b.sentinel_counter_increment);
    assert(reflected_b.source_texture_column==42);
    const auto unreflected_b=emit_native_code2_wall_record(
        world,dir,0,1,0,NativeCode2WallPath::Banks1And0_33124,
        input_phase,true,88,456);
    assert(!unreflected_b.reflected_source_column);
    assert(unreflected_b.sentinel_counter_increment);
    assert(unreflected_b.source_texture_column==21);
    assert(unreflected_b.caller_depth==88 && unreflected_b.caller_screen_position==456);

    // Actual original source-id pair -> decoded EH6 -> two VGA pixel lanes.
    const auto first=draw_native_code2_wall_record_column(
        world,dir,resource,reflected_a,0,12,128,2,0,0,output);
    assert(first.selected_original_res4_id==0 && first.raster.plane_writes==128);
    assert(output.indexed_pixel(12,64)==124); // (42*3)-2
    const auto second=draw_native_code2_wall_record_column(
        world,dir,resource,reflected_a,1,13,128,2,0,0,output);
    assert(second.selected_original_res4_id==1 && second.raster.plane_writes==128);
    assert(output.indexed_pixel(13,64)==174); // 50+(42*3)-2
    const auto prior=output.planes_sha256();
    rejects([&]{static_cast<void>(draw_native_code2_wall_record_column(
        world,dir,resource,reflected_a,2,14,128,2,0,0,output));});
    rejects([&]{static_cast<void>(draw_native_code2_wall_record_column(
        world,dir,resource,reflected_a,0,14,257,2,0,0,output));});
    assert(output.planes_sha256()==prior);

    // Delayed emission with a stale source-code record must be rejected.
    world.replace_cell_code(0,1,0,6);
    rejects([&]{static_cast<void>(draw_native_code2_wall_record_column(
        world,dir,resource,reflected_a,0,14,128,2,0,0,output));});
    assert(output.planes_sha256()==prior);
    world.replace_cell_code(0,1,0,1);

    // Full original WORD-level source phase range, both reflected paths.
    for(unsigned index=0;index<65536U;index+=17U) {
        for(const auto path:{
            NativeCode2WallPath::Banks3And2_32F8C,
            NativeCode2WallPath::Banks1And0_33124}) {
            for(const bool sentinel:{false,true}){
                auto record=emit_native_code2_wall_record(
                    world,dir,0,1,0,path,index,sentinel,0,0);
                const unsigned raw=(index>>4U)&0xFC0U;
                const bool mirror=path==
                    NativeCode2WallPath::Banks3And2_32F8C?sentinel:!sentinel;
                assert(record.source_texture_column==
                       (mirror?(63U-(raw>>6U)):(raw>>6U)));
                assert(record.reflected_source_column==mirror);
            }
        }
    }
    rejects([&]{static_cast<void>(emit_native_code2_wall_record(
        world,dir,0,1,0,static_cast<NativeCode2WallPath>(7),0,false,0,0));});
    assert(!a.original_complete_frame_verified &&
           !a.original_camera_depth_verified &&
           !a.original_viewpoint_choice_verified);
}
