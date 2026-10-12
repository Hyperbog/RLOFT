#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_type6_column_sampler.hpp"
#include "rloft_native/native_type6_texture.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void u16(std::vector<std::uint8_t>& data,std::size_t n) {
    if(n>65535)throw std::runtime_error("original type-6 width over 16 bits");
    data.push_back(static_cast<std::uint8_t>(n));
    data.push_back(static_cast<std::uint8_t>(n>>8U));
}
int run(const std::filesystem::path& root) {
    try {
        const auto media=rloft_native::NativeMedia::open(root);
        const auto original=media.read("RES4");
        const auto init=media.read("INIT");
        const auto colors=media.read("COLORS");
        const rloft_native::NativeResArchive archive(original);
        if(archive.entry_count()!=667 || init.size()<0x10 ||
           (unsigned(init[0x0e])|(unsigned(init[0x0f])<<8U))!=600U)
            throw std::runtime_error("original RES4/INIT source family counts invalid");
        for(unsigned bank=0;bank<15;++bank)
            static_cast<void>(rloft_native::decode_native_palette(colors,bank));
        std::vector<std::uint8_t> identity{
            'R','L','O','F','T','-','n','a','t','i','v','e','-',
            'R','E','S','4','-','s','t','r','i','p','s','-','v','1'};
        std::size_t count32=0,count64=0,total_reads=0,total_writes=0;
        std::cout<<"{\"schema\":\"rloft-native-type6-original-texture-strips/v1\","
                 <<"\"original_res4_sha256\":\""
                 <<rloft_native::native_sha256_hex(original)<<"\","
                 <<"\"original_init_sha256\":\""
                 <<rloft_native::native_sha256_hex(init)<<"\","
                 <<"\"original_colors_sha256\":\""
                 <<rloft_native::native_sha256_hex(colors)<<"\","
                 <<"\"original_res4_entries\":"<<archive.entry_count()
                 <<",\"original_init_narrow_bank_start\":600,"
                 <<"\"validated_palette_banks\":15,"
                 <<"\"textures\":[";
        for(std::size_t i=0;i<archive.entry_count();++i) {
            const auto entry=archive.entry(i);
            const auto texture=rloft_native::decode_native_type6_texture(entry);
            if(i && i<600 && texture.width!=64)
                throw std::runtime_error("original wide RES4 width mismatch");
            if(i>=600 && texture.width!=32)
                throw std::runtime_error("original narrow RES4 width mismatch");
            count32+=texture.width==32;
            count64+=texture.width==64;
            // Four explicit source-column projected heights. The caller
            // diagnoses a host draw, NOT the original live camera.
            const std::vector<rloft_native::NativeType6ProjectedColumn> calls{
                {&texture,0,0,128,0,0,0},
                {&texture,unsigned(texture.width/2U),1,64,7,0,0},
                {&texture,unsigned(texture.width-1U),2,256,255,0,0},
                {&texture,unsigned(texture.width/2U),3,32,3,1,144}};
            rloft_native::NativeIndexedVgaPlanes planes;
            const auto out=rloft_native::draw_native_type6_columns(calls,planes);
            if(out.source_texel_reads!=480 || out.plane_writes!=480 ||
               out.first_scanline!=0 || out.last_scanline!=287)
                throw std::runtime_error("original texture source diagnostic count mismatch");
            total_reads+=out.source_texel_reads;
            total_writes+=out.plane_writes;
            const auto entry_sha=rloft_native::native_sha256_hex(entry);
            const auto row_major_sha=rloft_native::native_sha256_hex(texture.pixels);
            const auto vga_sha=out.resulting_four_plane_sha256;
            if(i)std::cout<<",";
            std::cout<<"{\"id\":"<<i<<",\"width\":"<<texture.width
                     <<",\"height\":128,\"original_entry_sha256\":\""
                     <<entry_sha<<"\",\"decoded_row_major_sha256\":\""
                     <<row_major_sha<<"\",\"diagnostic_four_plane_sha256\":\""
                     <<vga_sha<<"\"}";
            u16(identity,i);
            u16(identity,texture.width);
            for(const auto& digest:{entry_sha,row_major_sha,vga_sha}) {
                identity.insert(identity.end(),digest.begin(),digest.end());
            }
        }
        if(count64!=600 || count32!=67)
            throw std::runtime_error("original RES4 source class dimensions mismatch");
        std::cout<<"],\"wide_64x128_entries\":"<<count64
                 <<",\"narrow_32x128_entries\":"<<count32
                 <<",\"source_texel_reads\":"<<total_reads
                 <<",\"plane_writes\":"<<total_writes
                 <<",\"original_wide_source_base\":0,"
                 <<"\"original_narrow_source_base\":600,"
                 <<"\"combined_diagnostic_sha256\":\""
                 <<rloft_native::native_sha256_hex(identity)<<"\","
                 <<"\"original_x86_executed\":false,"
                 <<"\"original_camera_projection_verified\":false,"
                 <<"\"original_wall_order_verified\":false,"
                 <<"\"original_dos_frame_parity_verified\":false}\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"Original RES4 projected column audit FAILED: "<<e.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_type6_strip_audit <original game dir>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#else
int main(int argc,char** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_type6_strip_audit <original game dir>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#endif
