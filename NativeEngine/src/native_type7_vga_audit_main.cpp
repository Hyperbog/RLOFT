#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_lzss.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_sintab_raster.hpp"
#include "rloft_native/native_type7_scene_shape.hpp"
#include "rloft_native/native_type7_vga_dispatch.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
void emit_run(const char* title,
              const rloft_native::NativeType7VgaDispatch& dispatch,
              const rloft_native::NativeType7VgaDraw& drawn) {
    std::cout<<"\""<<title<<"\":{\"individual_columns\":"
             <<(dispatch.individual_columns?"true":"false")
             <<",\"original_page\":"<<unsigned(dispatch.original_video_page)
             <<",\"plane_passes\":"<<dispatch.calls.size()
             <<",\"lookup_after_dispatch\":"
             <<dispatch.lookup_after_dispatch
             <<",\"descriptor_invocations\":"
             <<drawn.descriptor_invocations
             <<",\"raster_call_invocations\":"
             <<drawn.raster_call_invocations
             <<",\"indexed_source_reads\":"
             <<drawn.indexed_source_reads
             <<",\"masked_vga_writes\":"
             <<drawn.masked_vga_writes
             <<",\"individual_plane_writes\":"
             <<drawn.individual_plane_writes
             <<",\"four_plane_sha256\":\""
             <<drawn.resulting_four_plane_sha256
             <<"\",\"original_frame_parity_verified\":false}";
}
int run(const std::filesystem::path& root) {
    try {
        const auto media=rloft_native::NativeMedia::open(root);
        const auto original_code2=media.read("CODE.2");
        const auto masks=rloft_native::decode_native_type7_vga_masks(
            original_code2);
        const auto original_sintab=media.read("SINTAB");
        const auto tables=rloft_native::decode_native_sintab_programs(
            original_sintab);
        const auto original_res3=media.read("RES3");
        const rloft_native::NativeResArchive archive(original_res3);
        const auto raw=archive.entry(0);
        const auto shape=rloft_native::decode_native_type7_scene_shape(raw);
        if(raw.size()<8)
            throw std::runtime_error("original RES3 scene shape 0 truncated");
        const auto size=std::uint16_t(raw[5]) |
                        (std::uint16_t(raw[6])<<8U);
        const auto decoded=rloft_native::native_lzss4k_decode(
            raw.data()+7,raw.size()-7,size);
        if(decoded.consumed!=raw.size()-7)
            throw std::runtime_error("original RES3 scene shape 0 overread");
        constexpr unsigned start=6, width=7,scale=126;
        constexpr std::uint32_t ecx=0U; // Explicit diagnostic lookup
        const auto grouped=rloft_native::plan_native_type7_vga_dispatch(
            masks,start,width,masks.initial_video_page,
            0,0,ecx,false);
        const auto singles=rloft_native::plan_native_type7_vga_dispatch(
            masks,start,width,masks.initial_video_page,
            0,0,ecx,true);
        rloft_native::NativeIndexedVgaPlanes grouped_video,individual_video;
        const auto group_render=rloft_native::draw_native_type7_vga_dispatch(
            tables,shape,scale,0,decoded.bytes,0,grouped,grouped_video);
        const auto single_render=rloft_native::draw_native_type7_vga_dispatch(
            tables,shape,scale,0,decoded.bytes,0,singles,individual_video);
        std::cout<<"{\"schema\":\"rloft-native-type7-vga-dispatch/v1\","
                 <<"\"original_code2_sha256\":\""
                 <<masks.original_code2_sha256<<"\","
                 <<"\"original_sintab_sha256\":\""
                 <<tables.original_sintab_sha256<<"\","
                 <<"\"original_res3_sha256\":\""
                 <<rloft_native::native_sha256_hex(original_res3)<<"\","
                 <<"\"original_code2_initial_individual\":"
                 <<(masks.initial_individual_columns?"true":"false")<<","
                 <<"\"original_code2_initial_page\":"
                 <<unsigned(masks.initial_video_page)<<","
                 <<"\"original_mask_table_runs_verified\":28,"
                 <<"\"original_vga_mask_triplets\":[";
        for(unsigned phase=0;phase<4;++phase)
            for(unsigned n=1;n<=7;++n) {
                if(phase || n>1)std::cout<<",";
                const auto masks_triplet=masks.for_run(phase,
                    static_cast<std::uint8_t>(n));
                std::cout<<"["<<unsigned(masks_triplet[0])<<","
                         <<unsigned(masks_triplet[1])<<","
                         <<unsigned(masks_triplet[2])<<"]";
            }
        std::cout<<"],\"sample_shape_id\":0,"
                 <<"\"sample_source_column_index\":0,"
                 <<"\"sample_sintab_scale_index\":"<<scale<<","
                 <<"\"sample_start_viewport_column\":"<<start<<","
                 <<"\"sample_viewport_run_width\":"<<width<<","
                 <<"\"sample_renderer_row\":0,"
                 <<"\"sample_renderer_row_byte_offset\":0,"
                 <<"\"sample_source_base_offset\":0,"
                 <<"\"sample_ecx_lookup_diagnostic\":"<<ecx<<",";
        emit_run("grouped_mode",grouped,group_render);
        std::cout<<",";
        emit_run("individual_mode",singles,single_render);
        std::cout<<",\"original_16bit_x86_executed\":false,"
                 <<"\"original_camera_projection_verified\":false,"
                 <<"\"original_screen_frame_parity_verified\":false}\n";
        return 0;
    }catch(const std::exception& error) {
        std::cerr<<"Original TYPE7 VGA dispatch original-data audit FAILED: "
                 <<error.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_type7_vga_audit <original media root>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#else
int main(int argc,char** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_type7_vga_audit <original media root>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#endif
