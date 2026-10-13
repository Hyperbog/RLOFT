#include "rloft_native/native_media.hpp"
#include "rloft_native/native_probe_original_textures.hpp"
#include "rloft_native/native_scene_view.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_world_payload.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
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
        std::vector<std::uint8_t> frames;
        unsigned hit_rays=0,texels=0,nonempty_levels=0;
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
            NativeProbeCamera camera{};
            camera.level=level;
            camera.column_position=double(selected_x)+0.5;
            camera.row_position=double(selected_y)+0.5;
            camera.heading_radians=-3.14159265358979323846/2.0;
            camera.field_of_view_radians=3.14159265358979323846/3.0;
            const auto view=trace_native_probe_view(world,camera,256U,256U);
            const auto frame=materials.render(world,view,320U,400U);
            if(!frame.original_code2_sampler_used ||
               !frame.source_quadrant_state_used ||
               !frame.original_code2_backdrop_used)
                throw std::runtime_error("original CODE.2 sampler/quadrant inactive");
            if(frame.bgra.size()!=320U*400U*4U)
                throw std::runtime_error("original textured frame dimensions wrong");
            frames.insert(frames.end(),frame.bgra.begin(),frame.bgra.end());
            hit_rays+=frame.wall_columns;
            texels+=frame.original_texel_samples;
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
           materials.cache_size()==0)
            throw std::runtime_error("original source preview emitted no authentic walls");
        std::cout<<"{\"schema\":\"native-real-original-interactive-preview-v1\","
                 <<"\"levels_loaded\":32,"
                 <<"\"levels_with_original_walls\":"<<nonempty_levels<<","
                 <<"\"original_res4_wall_columns\":"<<hit_rays<<","
                 <<"\"original_res4_texel_samples\":"<<texels<<","
                 <<"\"unique_eh6_textures_decoded\":"<<materials.cache_size()<<","
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
