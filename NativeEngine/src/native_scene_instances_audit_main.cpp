#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_scene_instances.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_world_payload.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {
int run(const std::filesystem::path& root) {
    try {
        const auto media=rloft_native::NativeMedia::open(root);
        const auto init=media.read("INIT");
        const auto npcs=media.read("NPCS.DAT");
        const auto objects=media.read("OBJECTS.DAT");
        const auto levels=media.read("LEVELS.DAT");
        // The retail RES3 entry cardinality (4234) is research-proven.
        // If RES3 is available, prefer the true original archive count.
        // If absent, make the unverified RES3 identity explicit.
        const bool res3_present=media.contains("RES3");
        const auto res3_count=res3_present?
            rloft_native::NativeResArchive(media.read("RES3")).entry_count()
            :std::size_t(4234);
        const auto banks=rloft_native::decode_native_res3_scene_banks(
            init,res3_count);
        const auto defs=rloft_native::decode_native_scene_definitions(
            banks,npcs,objects);
        const auto world=rloft_native::decode_native_world_payload(levels);
        const auto instances=rloft_native::derive_native_authored_scene_instances(
            world,defs);
        std::cout<<"{\"schema\":\"rloft-native-authored-scene-instances/v1\","
                 <<"\"input_levels_sha256\":\""
                 <<rloft_native::native_sha256_hex(levels)<<"\","
                 <<"\"input_init_sha256\":\""
                 <<rloft_native::native_sha256_hex(init)<<"\","
                 <<"\"input_npcs_sha256\":\""
                 <<rloft_native::native_sha256_hex(npcs)<<"\","
                 <<"\"input_objects_sha256\":\""
                 <<rloft_native::native_sha256_hex(objects)<<"\","
                 <<"\"original_res3_archive_present\":"
                 <<(res3_present?"true":"false")<<","
                 <<"\"res3_archive_count_source\":\""
                 <<(res3_present?"actual_archive":"known_retail_directory_cardinality")
                 <<"\",\"res3_entries\":"<<res3_count<<","
                 <<"\"map_count\":"<<instances.levels.size()<<","
                 <<"\"authored_npc_instances\":"
                 <<instances.original_npc_instances<<","
                 <<"\"authored_placed_instances\":"
                 <<instances.original_placed_instances<<","
                 <<"\"authored_loose_instances\":"
                 <<instances.original_loose_instances<<","
                 <<"\"legacy_object_type_aliases\":"
                 <<instances.original_legacy_object_aliases<<","
                 <<"\"compact_scene_instance_source_sha256\":\""
                 <<instances.original_compact_families_sha256<<"\","
                 <<"\"original_view_frame_selection_verified\":false,"
                 <<"\"original_dos_rasterizer_executed\":false,"
                 <<"\"gameplay_parity_verified\":false}\n";
        return 0;
    }catch(const std::exception& error) {
        std::cerr<<"Native original authored scene instance audit FAILED: "
                 <<error.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_scene_instances_audit <original directory/game.gog>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#else
int main(int argc,char** argv) {
    if(argc!=2) {
        std::cerr<<"Usage: rloft_native_scene_instances_audit <original directory>\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]));
}
#endif
