#include "rloft_native/native_code2_wall_emission.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_world_payload.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
std::string span_sha(
    const std::vector<std::uint8_t>& file,std::size_t start,
    std::size_t stop) {
    const auto offset=std::size_t(0x3AF24U);
    if(start>=stop || stop>0x3A852U ||
       offset+stop>file.size())
        throw std::runtime_error("original CODE.2 executable segment incomplete");
    const std::vector<std::uint8_t> span(
        file.begin()+static_cast<std::ptrdiff_t>(offset+start),
        file.begin()+static_cast<std::ptrdiff_t>(offset+stop));
    return rloft_native::native_sha256_hex(span);
}
int run(const std::filesystem::path& root) {
    try {
        const auto media=rloft_native::NativeMedia::open(root);
        const auto code=media.read("CODE.2");
        const auto levels=media.read("LEVELS.DAT");
        const auto init=media.read("INIT");
        const auto source_res4=media.read("RES4");
        const auto front_sha=span_sha(code,0x32F8CU,0x33122U);
        const auto side_sha=span_sha(code,0x33124U,0x332B3U);
        if(front_sha!="57fe0d8c39e1b19e52b567db39df43c04131f3d3ab50ef8d0c0fb72a684825ae" ||
           side_sha!="a3f6583594908a90514a3f33ccd54f72b9660a464b7e13d2a2f202432b438a7f")
            throw std::runtime_error("original CODE.2 source wall-emission bytes differ");
        if(init.size()<16)throw std::runtime_error("original INIT texture boundary missing");
        const unsigned split=unsigned(init[14])|(unsigned(init[15])<<8U);
        const rloft_native::NativeResArchive archive(source_res4);
        const auto world_source=rloft_native::decode_native_world_payload(levels);
        const auto catalogue=rloft_native::decode_native_world_material_catalogue(
            world_source,archive,split);
        rloft_native::NativeWorldRuntime world(world_source);
        const unsigned level=0,col=6,row=0;
        if(world.cell(level,col,row).code!=1U)
            throw std::runtime_error("original CODE.2 wall scene test cell changed");
        rloft_native::NativeIndexedVgaPlanes frame;
        std::size_t total_reads=0,total_writes=0;
        std::cout<<"{\"schema\":\"rloft-native-code2-wall-record-emission/v1\","
                 <<"\"original_code2_sha256\":\""
                 <<rloft_native::native_sha256_hex(code)<<"\","
                 <<"\"original_levels_sha256\":\""
                 <<rloft_native::native_sha256_hex(levels)<<"\","
                 <<"\"original_res4_sha256\":\""
                 <<rloft_native::native_sha256_hex(source_res4)<<"\","
                 <<"\"original_front_wall_path_sha256\":\""
                 <<front_sha<<"\","
                 <<"\"original_side_wall_path_sha256\":\""
                 <<side_sha<<"\","
                 <<"\"original_material_candidates_validated\":"
                 <<catalogue.candidate_slots<<","
                 <<"\"original_unique_wide_textures_validated\":"
                 <<catalogue.unique_wide_textures<<","
                 <<"\"sample_level\":0,\"sample_cell\":[6,0],"
                 <<"\"sample_phase_dword\":21845,"
                 <<"\"sample_eh6_draws\":[";
        unsigned n=0;
        for(const auto path:{
            rloft_native::NativeCode2WallPath::Banks3And2_32F8C,
            rloft_native::NativeCode2WallPath::Banks1And0_33124}) {
            for(const bool sentinel:{false,true}) {
                const auto record=rloft_native::emit_native_code2_wall_record(
                    world,catalogue,level,col,row,path,0x5555U,sentinel,77U,123U);
                for(unsigned slot=0;slot<2;++slot) {
                    const unsigned x=8U+n;
                    const auto drawn=rloft_native::draw_native_code2_wall_record_column(
                        world,catalogue,archive,record,slot,x,128,5,0,0,frame);
                    if(n)std::cout<<",";
                    std::cout<<"{\"path\":"<<unsigned(path==rloft_native::NativeCode2WallPath::Banks3And2_32F8C?0U:1U)
                             <<",\"sentinel\":"<<(sentinel?"true":"false")
                             <<",\"slot\":"<<slot
                             <<",\"bank\":"<<(slot?record.second_source_bank:record.first_source_bank)
                             <<",\"reflected\":"<<(record.reflected_source_column?"true":"false")
                             <<",\"source_column\":"<<record.source_texture_column
                             <<",\"selected_id\":"<<drawn.selected_original_res4_id
                             <<",\"shade_class\":"<<unsigned(record.original_shade_class)
                             <<",\"reads\":"<<drawn.raster.source_texel_reads
                             <<",\"writes\":"<<drawn.raster.plane_writes<<"}";
                    total_reads+=drawn.raster.source_texel_reads;
                    total_writes+=drawn.raster.plane_writes;
                    ++n;
                }
            }
        }
        std::cout<<"],\"sample_draw_count\":"<<n
                 <<",\"sample_source_reads\":"<<total_reads
                 <<",\"sample_plane_writes\":"<<total_writes
                 <<",\"sample_full_four_plane_sha256\":\""
                 <<frame.planes_sha256()<<"\","
                 <<"\"sample_original_visibility_0440\":"
                 <<(((world.cell(level,col,row).word&0x0440U)==0x0440U)?"true":"false")
                 <<",\"original_material_direction_verified\":false,"
                 <<"\"original_full_scene_projection_verified\":false,"
                 <<"\"original_dos_frame_parity_verified\":false}\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"CODE.2 wall original-data audit FAILED: "<<e.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    return argc==2?run(std::filesystem::path(argv[1])):64;
}
#else
int main(int argc,char** argv) {
    return argc==2?run(std::filesystem::path(argv[1])):64;
}
#endif
