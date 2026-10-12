#include "rloft_native/native_source_scene_compositor.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_world_payload.hpp"

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
        const auto res4_original=media.read("RES4");
        const auto res3_original=media.read("RES3");
        const auto sintab_original=media.read("SINTAB");
        const auto code=media.read("CODE.2");
        const auto colors=media.read("COLORS");
        const auto init=media.read("INIT");
        if(init.size()<16)
            throw std::runtime_error("original INIT RES4 class boundary missing");
        const auto source=rloft_native::decode_native_world_payload(levels);
        rloft_native::NativeWorldRuntime world(source);
        const rloft_native::NativeResArchive res4(res4_original);
        const rloft_native::NativeResArchive res3(res3_original);
        const auto wide=unsigned(init[14])|(unsigned(init[15])<<8U);
        const auto catalogue=rloft_native::decode_native_world_material_catalogue(
            source,res4,wide);
        const auto sintab=rloft_native::decode_native_sintab_programs(
            sintab_original);
        const auto masks=rloft_native::decode_native_type7_vga_masks(code);
        const auto palette=rloft_native::decode_native_original_level_palettes(code);
        if(world.cell(0,5,0).code!=6 ||
           catalogue.levels[0].source_lookups[6][3]!=70U ||
           res3.entry_count()!=4234 ||
           catalogue.candidate_slots!=8192)
            throw std::runtime_error("original diagnostic Level0 source changed");

        rloft_native::NativeSceneDrawCommand wall,sprite;
        wall.kind=rloft_native::NativeSceneDrawKind::Wall;
        wall.wall.cell_column=5;wall.wall.cell_row=0;
        wall.wall.path=rloft_native::NativeCode2WallPath::Banks3And2_32F8C;
        wall.wall.original_phase=0x5555U;
        wall.wall.original_sign_sentinel=true;
        wall.wall.resource_word_slot=0;
        wall.wall.viewport_column=9;
        wall.wall.original_palette_shade=5;
        wall.wall.projection.live_x_255f0=0x20000U;
        wall.wall.projection.direction_x_255f8=0x8000U;
        wall.wall.projection.height_numerator_255e0=0x8000U;
        sprite.kind=rloft_native::NativeSceneDrawKind::Sprite;
        sprite.sprite.original_res3_id=0;
        sprite.sprite.original_sintab_scale=126;
        sprite.sprite.source_column=0;
        sprite.sprite.viewport_start_x=9;
        sprite.sprite.destination_width=3;
        sprite.sprite.renderer_row=64;
        sprite.sprite.original_ecx_lookup=0;
        // Original CODE.2 live view-order/camera are not assumed here.
        rloft_native::NativeIndexedVgaPlanes video;
        const auto forward=rloft_native::compose_native_source_scene_rgb6(
            world,catalogue,res4,res3,sintab,masks,palette,colors,
            0,0,{wall,sprite},video);
        rloft_native::NativeWorldRuntime reverse_world(source);
        rloft_native::NativeIndexedVgaPlanes reverse_video;
        const auto reverse=rloft_native::compose_native_source_scene_rgb6(
            reverse_world,catalogue,res4,res3,sintab,masks,palette,colors,
            0,0,{sprite,wall},reverse_video);
        if(forward.ordered_commands!=2 || forward.wall_commands!=1 ||
           forward.sprite_commands!=1 || forward.texture_source_reads!=128 ||
           forward.scanout.bank!=4 || forward.scanout.rgb6.size()!=384000 ||
           reverse.scanout.rgb6.size()!=384000 ||
           (world.cell(0,5,0).word&0x0440U)!=0x0440U)
            throw std::runtime_error("original frame compositor accounting mismatch");
        // Fail closed on a late invalid source; no visibility or pixel writes
        // may reach the original world/video after an aborted composite.
        auto fail_world=rloft_native::NativeWorldRuntime(source);
        rloft_native::NativeIndexedVgaPlanes fail_video;
        const auto prior=fail_video.planes_sha256();
        const auto prior_word=fail_world.cell(0,5,0).word;
        auto invalid=sprite;invalid.sprite.original_res3_id=4234;
        bool rejected=false;
        try{
            static_cast<void>(rloft_native::compose_native_source_scene_rgb6(
                fail_world,catalogue,res4,res3,sintab,masks,palette,colors,
                0,0,{wall,invalid},fail_video));
        }catch(const std::exception&){rejected=true;}
        if(!rejected || fail_video.planes_sha256()!=prior ||
           fail_world.cell(0,5,0).word!=prior_word)
            throw std::runtime_error("original compositor rollback mismatch");
        std::cout<<"{\"schema\":\"rloft-native-original-eh6-eh7-rgb6-compositor/v1\","
                 <<"\"original_levels_sha256\":\""
                 <<rloft_native::native_sha256_hex(levels)<<"\","
                 <<"\"original_res4_sha256\":\""
                 <<rloft_native::native_sha256_hex(res4_original)<<"\","
                 <<"\"original_res3_sha256\":\""
                 <<rloft_native::native_sha256_hex(res3_original)<<"\","
                 <<"\"original_sintab_sha256\":\""
                 <<rloft_native::native_sha256_hex(sintab_original)<<"\","
                 <<"\"original_code2_sha256\":\""
                 <<rloft_native::native_sha256_hex(code)<<"\","
                 <<"\"original_colors_sha256\":\""
                 <<rloft_native::native_sha256_hex(colors)<<"\","
                 <<"\"original_init_sha256\":\""
                 <<rloft_native::native_sha256_hex(init)<<"\","
                 <<"\"original_world_level\":0,"
                 <<"\"source_cell\":[5,0],"
                 <<"\"original_res4_wall_id\":70,"
                 <<"\"original_res3_sprite_id\":0,"
                 <<"\"original_sintab_scale\":126,"
                 <<"\"original_level_palette_bank\":"<<forward.scanout.bank<<","
                 <<"\"frame_dimensions\":[320,400],"
                 <<"\"source_owned_command_count\":"<<forward.ordered_commands<<","
                 <<"\"forward_wall_source_reads\":"<<forward.texture_source_reads<<","
                 <<"\"forward_sprite_source_reads\":"<<forward.eh7_source_reads<<","
                 <<"\"forward_indexed_plane_writes\":"<<forward.indexed_plane_writes<<","
                 <<"\"forward_eh7_descriptors\":"<<forward.total_eh7_descriptors<<","
                 <<"\"forward_four_plane_sha256\":\""
                 <<forward.final_four_plane_sha256<<"\","
                 <<"\"forward_rgb6_frame_sha256\":\""
                 <<forward.scanout.rgb6_sha256<<"\","
                 <<"\"reverse_four_plane_sha256\":\""
                 <<reverse.final_four_plane_sha256<<"\","
                 <<"\"reverse_rgb6_frame_sha256\":\""
                 <<reverse.scanout.rgb6_sha256<<"\","
                 <<"\"rollback_world_and_frame_verified\":true,"
                 <<"\"original_live_draw_order_verified\":false,"
                 <<"\"original_camera_ray_traversal_verified\":false,"
                 <<"\"original_dos_frame_parity_verified\":false}\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"Original EH6/EH7/RGB6 frame compositor audit FAILED: "
                 <<e.what()<<"\n";
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
