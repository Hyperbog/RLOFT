#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_lzss.hpp"
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
        // Exact original-source smoke: first EH7 source column's first
        // descriptor at SINTAB scale 126. The caller supplies a
        // diagnostic ECX=0, destination=0, sequencer mask=15; these are
        // NOT original 3D camera/shade/view constants.
        const auto first_entry=res3.entry(0);
        const auto first_shape=rloft_native::decode_native_type7_scene_shape(
            first_entry);
        if(first_shape.columns.empty() ||
           first_shape.columns.front().descriptors.empty())
            throw std::runtime_error("original RES3 shape 0 has no sample descriptor");
        if(first_entry.size()<7)
            throw std::runtime_error("original RES3 shape 0 truncated");
        const auto size=std::uint16_t(first_entry[5]) |
                        (std::uint16_t(first_entry[6])<<8U);
        const auto decompressed=rloft_native::native_lzss4k_decode(
            first_entry.data()+7,first_entry.size()-7,size);
        if(decompressed.consumed!=first_entry.size()-7)
            throw std::runtime_error("original RES3 smoke input not consumed");
        const auto sample_plan=rloft_native::resolve_native_eh7_span(
            first_shape,tables,126,0,0);
        rloft_native::NativeIndexedVgaPlanes source_probe;
        const auto source_execution=rloft_native::draw_native_eh7_span(
            tables,sample_plan,decompressed.bytes,
            0,0,0,15,source_probe);

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
                 <<",\"original_source_isolated_span_execution\":true,"
                 <<"\"source_sample_shape\":0,"
                 <<"\"source_sample_column\":0,"
                 <<"\"source_sample_descriptor\":0,"
                 <<"\"source_sample_scale\":126,"
                 <<"\"source_sample_ecx_diagnostic\":0,"
                 <<"\"source_sample_mask_diagnostic\":15,"
                 <<"\"source_sample_entry_selector\":"
                 <<sample_plan.entry_selector
                 <<",\"source_sample_stop_selector\":"
                 <<sample_plan.stop_selector
                 <<",\"source_sample_signed_adjustment\":"
                 <<sample_plan.source_adjustment
                 <<",\"source_sample_read_opcodes_executed\":"
                 <<source_execution.source_reads
                 <<",\"source_sample_masked_stores_executed\":"
                 <<source_execution.masked_destination_writes
                 <<",\"source_sample_plane_sha256\":\""
                 <<source_probe.planes_sha256()
                 <<"\",\"source_sample_frame_parity_verified\":false,"
                 <<"\"scale_blocks\":[";
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
