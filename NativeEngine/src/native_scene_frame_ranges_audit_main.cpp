#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_scene_frame_ranges.hpp"
#include "rloft_native/native_scene_instances.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_world_payload.hpp"

#include <cstddef>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
int run(const std::filesystem::path& root) {
    try {
        const auto media=rloft_native::NativeMedia::open(root);
        const auto init=media.read("INIT");
        const auto npcs=media.read("NPCS.DAT");
        const auto objects=media.read("OBJECTS.DAT");
        const auto levels=media.read("LEVELS.DAT");
        const auto res3_data=media.read("RES3");
        const rloft_native::NativeResArchive res3(res3_data);
        const auto banks=rloft_native::decode_native_res3_scene_banks(
            init,res3.entry_count());
        const auto defs=rloft_native::decode_native_scene_definitions(
            banks,npcs,objects);
        const auto world=rloft_native::decode_native_world_payload(levels);
        const auto authored=rloft_native::derive_native_authored_scene_instances(
            world,defs);
        const auto frames=rloft_native::derive_native_authored_frame_ranges(
            authored,defs);

        // Verify a real original typed EH7 shape at EVERY legal authored
        // source-allocation slot, not just the archived base frame.
        // Decode any particular original RES3 entry at most once.
        std::vector<std::uint8_t> seen(res3.entry_count(),0);
        std::size_t unique=0, checked=0;
        std::size_t width64=0,width96=0,width128=0;
        for(const auto& level:frames.levels)
            for(const auto& record:level) {
                for(unsigned state=0;state<record.state_variant_count;++state) {
                    const unsigned count=record.source_class==0?1U:
                        record.source_shape_stride==0?1U:
                        record.source_shape_stride;
                    for(unsigned sub=0;sub<count;++sub) {
                        const auto id=rloft_native::select_native_authored_res3_frame(
                            record,banks,state,sub);
                        if(id>=seen.size())
                            throw std::runtime_error("original RES3 chosen ID outside archive");
                        ++checked;
                        if(seen[id])continue;
                        seen[id]=1;
                        ++unique;
                        const auto shape=rloft_native::decode_native_type7_scene_shape(
                            res3.entry(id));
                        if(!shape.structure_verified)
                            throw std::runtime_error("original authored variant EH7 not valid");
                        if(shape.source_width==64)++width64;
                        else if(shape.source_width==96)++width96;
                        else if(shape.source_width==128)++width128;
                        else throw std::runtime_error(
                            "original authored variant EH7 width unexpected");
                    }
                }
            }
        if(checked!=frames.bounded_resource_slots ||
           unique!=width64+width96+width128)
            throw std::runtime_error("original authored EH7 frame totals disagree");
        auto json=rloft_native::native_authored_frame_ranges_json(frames);
        if(json.size()<2 || json[json.size()-2]!='}')
            throw std::runtime_error("native frame range JSON malformed");
        json.pop_back();json.pop_back();
        json+=",\"original_levels_sha256\":\""+
              rloft_native::native_sha256_hex(levels)+"\"";
        json+=",\"original_init_sha256\":\""+
              rloft_native::native_sha256_hex(init)+"\"";
        json+=",\"original_npcs_sha256\":\""+
              rloft_native::native_sha256_hex(npcs)+"\"";
        json+=",\"original_objects_sha256\":\""+
              rloft_native::native_sha256_hex(objects)+"\"";
        json+=",\"original_res3_sha256\":\""+
              rloft_native::native_sha256_hex(res3_data)+"\"";
        json+=",\"unique_referenced_eh7_shapes_decoded\":"+
              std::to_string(unique);
        json+=",\"all_source_legal_frame_references_checked\":"+
              std::to_string(checked);
        json+=",\"unique_width64\":"+std::to_string(width64);
        json+=",\"unique_width96\":"+std::to_string(width96);
        json+=",\"unique_width128\":"+std::to_string(width128);
        json+=",\"original_live_npc_animation_verified\":false,"
              "\"original_perspective_frame_choice_verified\":false,"
              "\"original_dos_scene_sort_verified\":false,"
              "\"original_frame_pixels_verified\":false}\n";
        std::cout<<json;
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"Original authored frame bounds audit FAILED: "
                 <<e.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_scene_frame_ranges_audit <original game directory>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#else
int main(int argc,char** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_scene_frame_ranges_audit <original game directory>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#endif
