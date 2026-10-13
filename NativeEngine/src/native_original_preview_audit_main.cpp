#include "rloft_native/native_media.hpp"
#include "rloft_native/native_probe_original_textures.hpp"
#include "rloft_native/native_scene_view.hpp"
#include "rloft_native/native_gpu_world_mesh.hpp"
#include "rloft_native/native_type6_texture.hpp"
#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_world_payload.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <set>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc,char** argv) {
    using namespace rloft_native;
    try {
        if(argc!=2) {
            std::cerr<<"usage: rloft_native_original_preview_audit <original-directory>\n";
            return 64;
        }
        const auto media=NativeMedia::open(argv[1]);
        auto payload=load_native_world_payload(media);
        NativeWorldRuntime world(std::move(payload));
        NativeProbeOriginalTextures materials(media);
        const NativeResArchive original_gpu_textures(media.read("RES4"));
        std::set<unsigned> gpu_decoded_ids;
        std::vector<std::uint8_t> frames;
        unsigned hit_rays=0,texels=0,nonempty_levels=0;
        unsigned floors=0,ceilings=0,upper_columns=0;
        unsigned gpu_lower=0,gpu_upper=0,gpu_floor=0,gpu_ceiling=0;
        unsigned gpu_unique_level_materials=0;
        // Exact source-placed material regression from the field
        // screenshot: original LEVELS.DAT level 1 (14,10), code 0x2c
        // with +0x0200 upper tier, four authored wall resource IDs.
        // This would have exposed the preview's old opposite-face/
        // upper-tier mixup even when arbitrary 32-level draws passed.
        const auto& screenshot_map=world.original().level(1U);
        if(screenshot_map.cell(14U,10U)!=0x022cU ||
           screenshot_map.original_header[9U]!=96U ||
           screenshot_map.resource_lookup(0U,44U)!=497U ||
           screenshot_map.resource_lookup(1U,44U)!=62U ||
           screenshot_map.resource_lookup(2U,44U)!=499U ||
           screenshot_map.resource_lookup(3U,44U)!=62U)
            throw std::runtime_error(
                "authored level-1 cell 14,10 wall axes/upper art changed");
        for(unsigned level=0;level<32;++level) {
            const auto& map=world.original().level(level);
            bool found=false;
            unsigned selected_x=0,selected_y=0;
            std::uint64_t best_dist=std::numeric_limits<std::uint64_t>::max();
            for(unsigned y=0;y<map.height;++y) {
                for(unsigned x=0;x<map.width;++x) {
                    if(world.normal_collision(level,x,y)!=NativeCollision::Passable)
                        continue;
                    const auto dx=std::int64_t(x)*2+1-std::int64_t(map.width);
                    const auto dy=std::int64_t(y)*2+1-std::int64_t(map.height);
                    const auto dist=std::uint64_t(dx*dx+dy*dy);
                    if(!found || dist<best_dist) {
                        found=true;selected_x=x;selected_y=y;best_dist=dist;
                    }
                }
            }
            if(!found)throw std::runtime_error("original level has no passable camera cell");
            world.select_level(level);
            // The accelerated viewport is checked against the actual
            // owner-source LEVELS.DAT, not only a synthetic map fixture.
            const auto gpu=build_native_gpu_scene(world,level);
            // Level 22 is an ORIGINAL outdoor special case: all 86
            // blocking cell codes map to boundary texture ID 147,
            // which the v0.7 owner viewer independently establishes
            // as a deliberate NO-DRAW sentinel. Drawing a wall there
            // would be a source-data error. Other maps must show
            // original wall geometry.
            const bool outdoor_no_draw=level==22U;
            if(!gpu.original_material_bank_layout ||
               gpu.original_dos_frame_verified ||
               gpu.floor_faces==0U || gpu.textured_triangles.empty() ||
               (outdoor_no_draw ?
                   (gpu.lower_wall_faces!=0U ||
                    gpu.skipped_boundary_faces==0U):
                   (gpu.lower_wall_faces==0U)))
                throw std::runtime_error(
                    "authored GPU source geometry or outdoor boundary invalid at level "+
                    std::to_string(level));
            gpu_lower+=gpu.lower_wall_faces;
            gpu_upper+=gpu.upper_wall_faces;
            gpu_floor+=gpu.floor_faces;
            gpu_ceiling+=gpu.ceiling_faces;
            gpu_unique_level_materials+=unsigned(gpu.textured_triangles.size());
            // Exercise exactly what the WGL texture uploader expects.
            // A source-batch ID which is not an EH6 32/64x128 single
            // frame must fail the original-data gate, not wait for the
            // user to see a generic GPU dialog.
            for(const auto& batch:gpu.textured_triangles) {
                const auto id=batch.first;
                if(!gpu_decoded_ids.insert(id).second)continue;
                try {
                    const auto raster=decode_native_type6_texture(
                        original_gpu_textures.entry(id));
                    if(raster.frames!=1 || raster.height!=128 ||
                       (raster.width!=32 && raster.width!=64) ||
                       raster.pixels.size()!=
                           std::size_t(raster.width)*raster.height)
                        throw std::runtime_error("invalid WGL EH6 raster dimensions");
                } catch(const std::exception& error) {
                    throw std::runtime_error("GPU source RES4 ID "+
                        std::to_string(id)+": "+error.what());
                }
            }
            NativeProbeCamera camera{};
            camera.level=level;
            camera.column_position=double(selected_x)+0.5;
            camera.row_position=double(selected_y)+0.5;
            camera.heading_radians=-3.14159265358979323846/2.0;
            camera.field_of_view_radians=3.14159265358979323846/2.0;
            if(level==0U) {
                auto pitch=camera;
                pitch.pitch_radians=0.20;
                const auto shifted_pitch=trace_native_probe_view(world,pitch,256U,256U);
                const auto pitched=materials.render(world,shifted_pitch,320U,400U);
                const auto flat=trace_native_probe_view(world,camera,256U,256U);
                const auto level_frame=materials.render(world,flat,320U,400U);
                if(pitched.bgra==level_frame.bgra)
                    throw std::runtime_error("mouse pitch does not affect source-backed image");
            }
            const auto view=trace_native_probe_view(world,camera,256U,256U);
            const auto frame=materials.render(world,view,320U,400U);
            if(!frame.original_code2_sampler_used ||
               !frame.source_quadrant_state_used ||
               !frame.original_code2_backdrop_used ||
               !frame.original_environment_surface_source_verified)
                throw std::runtime_error("original CODE.2 sampler/quadrant inactive");
            if(frame.bgra.size()!=320U*400U*4U)
                throw std::runtime_error("original textured frame dimensions wrong");
            frames.insert(frames.end(),frame.bgra.begin(),frame.bgra.end());
            hit_rays+=frame.wall_columns;
            texels+=frame.original_texel_samples;
            floors+=frame.preview_floor_texels;
            ceilings+=frame.preview_ceiling_texels;
            upper_columns+=frame.preview_upper_wall_columns;
            nonempty_levels+=frame.wall_columns>0U;
            // Verify the *native* camera input bridge on real source data:
            // changing direction must produce a valid second source frame.
            camera.heading_radians+=0.12;
            const auto turned=trace_native_probe_view(world,camera,256U,256U);
            const auto rotated=materials.render(world,turned,320U,400U);
            if(rotated.bgra.size()!=frame.bgra.size())
                throw std::runtime_error("native preview turn changed frame shape");
            // Try a bounded native move without pretending it is original
            // DOS movement. Either safely advances or reports a collision.
            const auto delta_x=std::cos(camera.heading_radians)*0.1;
            const auto delta_y=std::sin(camera.heading_radians)*0.1;
            static_cast<void>(move_native_probe_camera(
                world,camera,delta_x,delta_y));
            const auto moved=trace_native_probe_view(world,camera,256U,256U);
            const auto shifted=materials.render(world,moved,320U,400U);
            if(shifted.bgra.size()!=frame.bgra.size())
                throw std::runtime_error("native preview movement changed frame shape");
        }
        if(nonempty_levels!=32 || hit_rays==0 || texels==0 ||
           materials.cache_size()==0 || floors==0 || ceilings==0 ||
           upper_columns==0 || gpu_lower==0 || gpu_upper==0 ||
           gpu_floor==0 || gpu_unique_level_materials==0)
            throw std::runtime_error("original source preview emitted no authentic walls");
        std::cout<<"{\"schema\":\"native-real-original-interactive-preview-v1\","
                 <<"\"levels_loaded\":32,"
                 <<"\"levels_with_original_walls\":"<<nonempty_levels<<","
                 <<"\"original_res4_wall_columns\":"<<hit_rays<<","
                 <<"\"original_res4_texel_samples\":"<<texels<<","
                 <<"\"unique_eh6_textures_decoded\":"<<materials.cache_size()<<","
                 <<"\"source_upper_wall_columns\":"<<upper_columns<<","
                 <<"\"source_authored_gpu_lower_faces\":"<<gpu_lower<<","
                 <<"\"source_authored_gpu_upper_faces\":"<<gpu_upper<<","
                 <<"\"source_authored_gpu_floor_faces\":"<<gpu_floor<<","
                 <<"\"source_authored_gpu_ceiling_faces\":"<<gpu_ceiling<<","
                 <<"\"source_gpu_level_material_batches\":"<<gpu_unique_level_materials<<","
                 <<"\"source_gpu_res4_textures_verified\":"<<gpu_decoded_ids.size()<<","
                 <<"\"source_floor_preview_texels\":"<<floors<<","
                 <<"\"source_ceiling_preview_texels\":"<<ceilings<<","
                 <<"\"frames_rgb6_bgra_sha256\":\""<<native_sha256_hex(frames)<<"\","
                 <<"\"source_media_read_only\":true,"
                 <<"\"original_camera_parity_verified\":false,"
                 <<"\"original_dos_frame_verified\":false,"
                 <<"\"original_gameplay_verified\":false}\n";
        return 0;
    } catch(const std::exception& error) {
        std::cerr<<"original interactive preview audit FAILED: "<<error.what()<<"\n";
        return 2;
    }
}
