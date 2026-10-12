#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_sintab_raster.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_type7_scene_shape.hpp"

#include <array>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {
int run(const std::filesystem::path& original_install) {
    try {
        const auto media=rloft_native::NativeMedia::open(original_install);
        const auto source=media.read("SINTAB");
        const auto tables=rloft_native::decode_native_sintab_programs(source);
        const auto original_res3=media.read("RES3");
        const rloft_native::NativeResArchive res3(original_res3);
        constexpr std::array<unsigned,3> scale_samples{0,63,126};
        std::size_t sources=0,descriptors=0,planned=0,nonempty=0;
        for(std::size_t id=0;id<res3.entry_count();++id) {
            const auto shape=rloft_native::decode_native_type7_scene_shape(
                res3.entry(id));
            ++sources;
            for(std::size_t c=0;c<shape.columns.size();++c) {
                for(std::size_t d=0;d<shape.columns[c].descriptors.size();++d) {
                    ++descriptors;
                    for(const auto scale:scale_samples) {
                        const auto plan=rloft_native::resolve_native_eh7_span(
                            shape,tables,scale,
                            static_cast<unsigned>(c),static_cast<unsigned>(d));
                        ++planned;
                        nonempty+=plan.stop_code_offset>plan.entry_code_offset;
                    }
                }
            }
        }
        std::cout<<"{\"schema\":\"rloft-native-sintab-eh7-raster-plans/v1\","
                 <<"\"original_sintab_sha256\":\""
                 <<tables.original_sintab_sha256<<"\","
                 <<"\"original_res3_sha256\":\""
                 <<rloft_native::native_sha256_hex(original_res3)<<"\","
                 <<"\"scale_count\":"<<tables.blocks.size()
                 <<",\"total_scale_payload_bytes\":"
                 <<tables.total_payload_bytes
                 <<",\"total_generated_code_bytes\":"
                 <<tables.total_generated_code_bytes
                 <<",\"original_source_read_opcodes\":"
                 <<tables.total_read_opcodes
                 <<",\"original_destination_store_opcodes\":"
                 <<tables.total_write_opcodes
                 <<",\"eh7_original_entries\":"<<sources
                 <<",\"eh7_descriptors_referenced\":"<<descriptors
                 <<",\"eh7_sample_scale_count\":"<<scale_samples.size()
                 <<",\"eh7_descriptor_scale_plans\":"<<planned
                 <<",\"eh7_nonempty_span_plans\":"<<nonempty
                 <<",\"scale_blocks\":[";
        for(std::size_t n=0;n<tables.blocks.size();++n) {
            if(n)std::cout<<",";
            const auto& p=tables.blocks[n];
            std::cout<<"{\"index\":"<<n
                     <<",\"original_block_sha256\":\""<<p.original_block_sha256
                     <<"\",\"generated_code_sha256\":\""<<p.generated_code_sha256
                     <<"\",\"code_bytes\":"<<p.original_x86_bytes.size()
                     <<",\"first_patch\":"<<p.patch_offsets.front()
                     <<",\"last_patch\":"<<p.patch_offsets.back()
                     <<",\"summed_scale_steps\":"<<p.summed_scale_steps
                     <<",\"instructions\":"<<p.instruction_count
                     <<",\"source_reads\":"<<p.source_reads
                     <<",\"destination_writes\":"<<p.destination_writes
                     <<"}";
        }
        std::cout<<"],\"original_x86_code_executed\":false,"
                 <<"\"safe_opcode_interpreter_implemented\":true,"
                 <<"\"all_127_original_opcode_streams_structurally_validated\":true,"
                 <<"\"original_view_raycast_verified\":false,"
                 <<"\"original_pixel_frame_parity_verified\":false}\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"Original SINTAB/EH7 raster plan FAILED: "<<e.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_sintab_raster_audit <original install>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#else
int main(int argc,char** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_sintab_raster_audit <original install>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#endif
