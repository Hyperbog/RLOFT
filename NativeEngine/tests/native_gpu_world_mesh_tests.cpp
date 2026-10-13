#include "rloft_native/native_gpu_world_mesh.hpp"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <vector>
using namespace rloft_native;
namespace {
NativeWorldPayload source() {
    NativeWorldPayload world;
    for(unsigned level=0;level<32;++level) {
        NativeLevelPayload map;
        map.id=level;map.width=5;map.height=5;
        map.cell_words.assign(25,0U);
        map.automap.assign(25,0U);
        for(unsigned i=0;i<5U;++i) {
            map.cell_words[i]=1;
            map.cell_words[20+i]=1;
            map.cell_words[i*5]=1;
            map.cell_words[i*5+4]=1;
        }
        map.cell_words[2U*5U+3U]=0x0201U;
        map.original_header[9]=96;
        // Header+5: no ceiling; header+7: original lower surface #3.
        map.original_header[5]=0xff;
        map.original_header[6]=0xff;
        map.original_header[7]=3;
        map.original_header[8]=0;
        for(unsigned code=0;code<64U;++code)
            for(unsigned bank=0;bank<5U;++bank) {
                const unsigned value=bank==4U?0xb5b1U:
                    10U+bank*20U+code;
                const unsigned pos=0x0aU+bank*128U+code*2U;
                map.original_header[pos]=std::uint8_t(value);
                map.original_header[pos+1]=std::uint8_t(value>>8U);
            }
        world.total_cells+=map.cell_words.size();
        world.levels.push_back(std::move(map));
    }
    return world;
}
}
int main() {
    NativeWorldRuntime world(source());
    const auto mesh=build_native_gpu_scene(world,0U);
    assert(mesh.width==5U && mesh.height==5U);
    assert(mesh.original_material_bank_layout &&
           !mesh.original_dos_frame_verified);
    assert(mesh.lower_wall_faces>0U);
    assert(mesh.upper_wall_faces>0U);
    assert(mesh.floor_faces>0U && mesh.ceiling_faces==0U);
        assert(mesh.textured_triangles.count(31U)); // X upper: 10+20+1
        // Distinct X/Y bank IDs are not compass-facing texture substitutes.
    assert(mesh.textured_triangles.count(11U)); // X lower
    assert(mesh.textured_triangles.count(31U)); // X upper
    assert(mesh.textured_triangles.count(71U)); // Y upper
    assert(mesh.textured_triangles.count(71U));
    assert(mesh.textured_triangles.count(603U)); // original floor RES4 class
    assert(mesh.fallback_ceiling.empty()); // -1 means outdoor sky, NOT fake flat roof
    for(const auto& batch:mesh.textured_triangles) {
        assert(!batch.second.empty());
        assert(batch.second.size()%6U==0U);
        for(const auto& v:batch.second) {
            assert(std::isfinite(v.x) && std::isfinite(v.y) &&
                   std::isfinite(v.z) && std::isfinite(v.u) &&
                   std::isfinite(v.v));
            assert(v.x>=0 && v.x<=5 && v.y>=0 && v.y<=5);
        }
    }
    bool rejected=false;
    try {static_cast<void>(build_native_gpu_scene(world,32U));}
    catch(const std::exception&) {rejected=true;}
    assert(rejected);
}
