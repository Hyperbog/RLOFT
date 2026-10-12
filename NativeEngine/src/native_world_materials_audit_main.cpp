#include "rloft_native/native_media.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_world_materials.hpp"
#include "rloft_native/native_world_payload.hpp"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
int run(const std::filesystem::path& root){
    try{
        const auto media=rloft_native::NativeMedia::open(root);
        const auto levels=media.read("LEVELS.DAT");
        const auto res4_data=media.read("RES4");
        const auto init=media.read("INIT");
        if(init.size()<16U)throw std::runtime_error("original INIT source split truncated");
        const unsigned wide=unsigned(init[14])|(unsigned(init[15])<<8U);
        const rloft_native::NativeResArchive res4(res4_data);
        const auto source=rloft_native::decode_native_world_payload(levels);
        const auto catalogue=rloft_native::decode_native_world_material_catalogue(
            source,res4,wide);
        rloft_native::NativeWorldRuntime runtime(source);
        // Level 0 (6,0) is an original source code-1 wall cell.
        // The candidate bank is an explicit diagnostic input, NOT a
        // DOS view-direction or material-branch attestation.
        const unsigned test_x=6U,test_y=0U,bank=0U;
        if(runtime.cell(0,test_x,test_y).code!=1U)
            throw std::runtime_error("original diagnostic world cell changed");
        rloft_native::NativeIndexedVgaPlanes pixels;
        const auto sample=rloft_native::draw_native_world_surface_column(
            runtime,catalogue,res4,0,test_x,test_y,bank,
            3U,9U,128U,5U,0U,0U,pixels);
        std::cout<<"{\"schema\":\"rloft-native-original-world-res4-materials/v1\","
                 <<"\"original_levels_sha256\":\""
                 <<rloft_native::native_sha256_hex(levels)<<"\","
                 <<"\"original_res4_sha256\":\""
                 <<rloft_native::native_sha256_hex(res4_data)<<"\","
                 <<"\"original_init_sha256\":\""
                 <<rloft_native::native_sha256_hex(init)<<"\","
                 <<"\"level_count\":"<<catalogue.levels.size()<<","
                 <<"\"world_candidate_entries\":"<<catalogue.candidate_slots<<","
                 <<"\"nonzero_candidate_entries\":"
                 <<catalogue.nonzero_candidate_slots<<","
                 <<"\"unique_wide_eh6_materials_decoded\":"
                 <<catalogue.unique_wide_textures<<","
                 <<"\"original_wide_res4_split\":"
                 <<catalogue.wide_texture_count<<","
                 <<"\"fifth_level_lookup_marker\":"
                 <<catalogue.original_fifth_bank_word<<","
                 <<"\"world_materials_sha256\":\""
                 <<catalogue.original_lookup_sha256<<"\","
                 <<"\"per_level_material_sha256\":[";
        for(std::size_t i=0;i<catalogue.levels.size();++i){
            if(i)std::cout<<",";
            std::cout<<"\""<<catalogue.levels[i].original_lookup_sha256<<"\"";
        }
        std::cout<<"],\"sample_cell\":[0,"<<test_x<<","<<test_y<<"],"
                 <<"\"sample_selected_bank\":"<<bank<<","
                 <<"\"sample_res4_id\":"<<sample.selected_original_res4_id<<","
                 <<"\"sample_raster_source_reads\":"
                 <<sample.raster.source_texel_reads<<","
                 <<"\"sample_raster_plane_writes\":"
                 <<sample.raster.plane_writes<<","
                 <<"\"sample_four_plane_sha256\":\""
                 <<sample.raster.resulting_four_plane_sha256<<"\","
                 <<"\"original_dos_material_direction_verified\":false,"
                 <<"\"original_camera_projection_verified\":false,"
                 <<"\"original_full_frame_parity_verified\":false}\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"Original world-to-RES4 material audit FAILED: "<<e.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv){
    return argc==2?run(std::filesystem::path(argv[1])):64;
}
#else
int main(int argc,char** argv){
    return argc==2?run(std::filesystem::path(argv[1])):64;
}
#endif
