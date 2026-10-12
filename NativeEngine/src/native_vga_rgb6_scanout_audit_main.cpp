#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_type6_column_sampler.hpp"
#include "rloft_native/native_type6_texture.hpp"
#include "rloft_native/native_vga_rgb6_scanout.hpp"

#include <array>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
int run(const std::filesystem::path& root) {
    try {
        const auto media=rloft_native::NativeMedia::open(root);
        const auto code=media.read("CODE.2");
        const auto colors=media.read("COLORS");
        const auto res4_raw=media.read("RES4");
        const auto levels=rloft_native::decode_native_original_level_palettes(code);
        const rloft_native::NativeResArchive res4(res4_raw);
        constexpr unsigned texture_id=62;
        if(res4.entry_count()!=667 || colors.size()!=11521 ||
           levels.original_display_triples[0][0]!=4 ||
           levels.original_display_triples[29][0]!=0)
            throw std::runtime_error("original COLORS/source RES4 cardinalities changed");
        const auto texture=rloft_native::decode_native_type6_texture(
            res4.entry(texture_id));
        if(texture.width!=64 || texture.height!=128)
            throw std::runtime_error("original RES4 texture diagnostic dimensions changed");
        rloft_native::NativeIndexedVgaPlanes planar;
        const auto draw=rloft_native::draw_native_type6_columns({
            {&texture,3,9,128,5,0,0},
            {&texture,42,10,64,7,0,0},
            {&texture,0,11,32,3,1,144}},planar);
        if(draw.source_texel_reads!=224 || draw.plane_writes!=224)
            throw std::runtime_error("original RGB6 source diagnostic strip writes changed");
        // The original four world palettes (banks 0,2,3,4) vary in
        // their last 8 authored color slots (246..253). Add eight
        // explicitly DIAGNOSTIC indexed pixels to test that exact
        // original palette distinction, not to imply gameplay frames.
        for(unsigned x=0;x<8;++x)
            planar.masked_write(80U+x/4U,
                static_cast<std::uint8_t>(1U<<(x&3U)),
                static_cast<std::uint8_t>(246U+x));
        std::array<unsigned,15> histogram{};
        std::cout<<"{\"schema\":\"rloft-original-code2-colors-vga-rgb6/v1\","
                 <<"\"original_code2_sha256\":\""
                 <<rloft_native::native_sha256_hex(code)<<"\","
                 <<"\"original_colors_sha256\":\""
                 <<rloft_native::native_sha256_hex(colors)<<"\","
                 <<"\"original_res4_sha256\":\""
                 <<rloft_native::native_sha256_hex(res4_raw)<<"\","
                 <<"\"original_level_table_sha256\":\""
                 <<levels.original_triplets_sha256<<"\","
                 <<"\"original_level_count\":32,"
                 <<"\"original_palette_bank_count\":15,"
                 <<"\"original_level_display_triplets\":[";
        for(unsigned level=0;level<32;++level) {
            if(level)std::cout<<",";
            const auto& a=levels.original_display_triples[level];
            ++histogram[a[0]];
            std::cout<<"["<<unsigned(a[0])<<","<<unsigned(a[1])
                     <<","<<unsigned(a[2])<<"]";
        }
        std::cout<<"],\"level_palette_histogram\":[";
        for(unsigned i=0;i<15;++i){
            if(i)std::cout<<",";
            std::cout<<histogram[i];
        }
        std::cout<<"],\"diagnostic_res4_id\":"<<texture_id
                 <<",\"diagnostic_source_reads\":"<<draw.source_texel_reads
                 <<",\"diagnostic_palette_probe_indexes\":[246,247,248,249,250,251,252,253]"
                 <<",\"diagnostic_probe_pixel_writes\":8"
                 <<",\"diagnostic_four_plane_sha256\":\""
                 <<planar.planes_sha256()<<"\","
                 <<"\"original_rgb6_frame_dimensions\":[320,400],"
                 <<"\"original_rgb6_frame_level_page0_sha256\":[";
        for(unsigned level=0;level<32;++level){
            const auto result=rloft_native::scanout_native_world_vga_rgb6(
                planar,levels,colors,level,0,320,400);
            if(level)std::cout<<",";
            std::cout<<"\""<<result.rgb6_sha256<<"\"";
        }
        const auto page1=rloft_native::scanout_native_world_vga_rgb6(
            planar,levels,colors,0,1,320,400);
        std::cout<<"],\"original_rgb6_level0_page1_sha256\":\""
                 <<page1.rgb6_sha256<<"\","
                 <<"\"original_dac6_palette_values_preserved\":true,"
                 <<"\"original_vga_plane_addressing_implemented\":true,"
                 <<"\"original_video_page0_and_page1_supported\":true,"
                 <<"\"original_live_dac_updates_verified\":false,"
                 <<"\"original_dos_frame_pixel_parity_verified\":false,"
                 <<"\"original_raycaster_camera_verified\":false}\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr<<"Original CODE.2 COLORS RGB6 scanout audit FAILED: "
                 <<e.what()<<"\n";
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
