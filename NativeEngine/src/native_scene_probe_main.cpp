#include "rloft_native/native_media.hpp"
#include "rloft_native/native_scene_view.hpp"
#include "rloft_native/native_sha256.hpp"
#include "rloft_native/native_world_payload.hpp"

#include <cmath>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
int run(const std::filesystem::path& original_install,
        const std::filesystem::path& optional_ppm) {
    try {
        const auto media=rloft_native::NativeMedia::open(original_install);
        const auto original=media.read("LEVELS.DAT");
        auto runtime=rloft_native::NativeWorldRuntime(
            rloft_native::decode_native_world_payload(original));
        const unsigned selected_level=22;
        runtime.select_level(selected_level);
        const auto& level=runtime.original().level(selected_level);

        // Find a reproducible, fully passable source cell with as many
        // open cardinal neighbors as possible; this is NOT a proved
        // starting position from the original game's SAVE.DAT.
        unsigned sx=0,sy=0,open_neighbours=0;
        bool have=false;
        for(unsigned row=1;row+1<level.height;++row)
            for(unsigned col=1;col+1<level.width;++col) {
                if(runtime.normal_collision(selected_level,col,row)!=
                   rloft_native::NativeCollision::Passable)continue;
                unsigned score=0;
                for(const auto [dx,dy]:
                    {std::pair<int,int>{0,-1},{1,0},{0,1},{-1,0}}) {
                    if(runtime.normal_collision(
                       selected_level,static_cast<unsigned>(int(col)+dx),
                       static_cast<unsigned>(int(row)+dy))==
                       rloft_native::NativeCollision::Passable)++score;
                }
                if(!have || score>open_neighbours) {
                    sx=col;sy=row;open_neighbours=score;have=true;
                }
            }
        if(!have)throw std::runtime_error(
            "no candidate passable interior original cell for probe");
        rloft_native::NativeProbeCamera camera;
        camera.level=selected_level;
        camera.column_position=double(sx)+0.5;
        camera.row_position=double(sy)+0.5;
        camera.heading_radians=0.0;
        camera.field_of_view_radians=1.0471975511965976;
        const auto view=rloft_native::trace_native_probe_view(
            runtime,camera,320,256);
        rloft_native::apply_native_probe_visibility(runtime,view);

        if(!optional_ppm.empty()) {
            const unsigned width=320,height=200;
            const auto rgb=rloft_native::render_native_probe_rgb(
                view,width,height);
            std::ofstream output(optional_ppm,std::ios::binary|std::ios::trunc);
            if(!output)throw std::runtime_error(
                "cannot open requested original-free PPM output");
            output<<"P6\n"<<width<<" "<<height<<"\n255\n";
            output.write(reinterpret_cast<const char*>(rgb.data()),
                         static_cast<std::streamsize>(rgb.size()));
            if(!output)throw std::runtime_error("probe PPM write failed");
        }
        // Whole original header retention is covered by per-level hashes;
        // no mapping-table original value is copied into the report.
        std::cout<<"{\"schema\":\"rloft-native-scene-probe/v1\","
                 <<"\"input_levels_sha256\":\""
                 <<rloft_native::native_sha256_hex(original)<<"\","
                 <<"\"level\":"<<selected_level
                 <<",\"start_column\":"<<sx
                 <<",\"start_row\":"<<sy
                 <<",\"open_neighbors\":"<<open_neighbours
                 <<",\"rays\":"<<view.columns.size()
                 <<",\"wall_hits\":"<<view.rays_that_hit
                 <<",\"rays_out_of_map\":"<<view.rays_that_left_archive
                 <<",\"total_cells_walked\":"<<view.walked_cells
                 <<",\"original_level_headers_retained\":true,"
                 <<"\"source_cell_semantics_used\":true,"
                 <<"\"renderer_is_provisional_host_probe\":true,"
                 <<"\"original_dos_raycast_verified\":false,"
                 <<"\"original_projection_verified\":false,"
                 <<"\"original_palette_or_textures_verified\":false,"
                 <<"\"original_event_vm_verified\":false,"
                 <<"\"native_gameplay_parity_verified\":false}\n";
        return 0;
    } catch(const std::exception& error) {
        std::cerr<<"Native scene probe FAILED: "<<error.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    if(argc!=2 && argc!=3) {
        std::cerr<<"Usage: rloft_native_scene_probe <original install> [debug.ppm]\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]),
               argc==3?std::filesystem::path(argv[2]):std::filesystem::path());
}
#else
int main(int argc,char** argv) {
    if(argc!=2 && argc!=3) {
        std::cerr<<"Usage: rloft_native_scene_probe <original install> [debug.ppm]\n";
        return 64;
    }
    return run(std::filesystem::path(argv[1]),
               argc==3?std::filesystem::path(argv[2]):std::filesystem::path());
}
#endif
