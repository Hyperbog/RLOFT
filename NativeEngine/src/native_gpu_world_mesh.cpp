#include "rloft_native/native_gpu_world_mesh.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
namespace rloft_native {
namespace {
using V=NativeGpuVertex;
void quad(std::vector<V>& dst,
          V a,V b,V c,V d) {
    dst.push_back(a);dst.push_back(b);dst.push_back(c);
    dst.push_back(a);dst.push_back(c);dst.push_back(d);
}
std::int16_t signed_word(const NativeLevelPayload& level,unsigned p) {
    return static_cast<std::int16_t>(std::uint16_t(level.original_header.at(p))|
        (std::uint16_t(level.original_header.at(p+1U))<<8U));
}
void textured_face(std::vector<V>& dst,
                   int cell_x,int cell_y,int direction,
                   float low,float high,float vbottom,float vtop) {
    const auto x=float(cell_x),y=float(cell_y);
    if(direction==0) {
        quad(dst,{x,y,low,0,vbottom},{x,y+1,low,1,vbottom},
                 {x,y+1,high,1,vtop},{x,y,high,0,vtop});
    } else if(direction==1) {
        quad(dst,{x+1,y+1,low,0,vbottom},{x+1,y,low,1,vbottom},
                 {x+1,y,high,1,vtop},{x+1,y+1,high,0,vtop});
    } else if(direction==2) {
        quad(dst,{x+1,y,low,0,vbottom},{x,y,low,1,vbottom},
                 {x,y,high,1,vtop},{x+1,y,high,0,vtop});
    } else {
        quad(dst,{x,y+1,low,0,vbottom},{x+1,y+1,low,1,vbottom},
                 {x+1,y+1,high,1,vtop},{x,y+1,high,0,vtop});
    }
}
} // namespace
NativeGpuScene build_native_gpu_scene(const NativeWorldRuntime& world,
                                       unsigned level,unsigned wide_count,
                                       unsigned sentinel) {
    if(level>=32U || wide_count==0U)
        throw std::invalid_argument("original mesh source parameters invalid");
    const auto& source=world.original().level(level);
    NativeGpuScene scene{};
    scene.level=level;scene.width=source.width;scene.height=source.height;
    const unsigned rows=source.original_header[9U];
    if(rows>128U)throw std::runtime_error("original upper wall rows >128");
    const float upper_height=2.0f*float(rows)/128.0f;
    bool any_upper=false;
    for(unsigned y=0;y<source.height;++y)for(unsigned x=0;x<source.width;++x) {
        const auto cell=world.cell(level,x,y);
        if((cell.word&0x0200U) && cell.code!=0U)any_upper=true;
    }
    const float ceiling_z=any_upper?2.0f+upper_height:2.0f;
    const auto ceil_id=signed_word(source,5U);
    const auto floor_id=signed_word(source,7U);
    const auto tex_id=[&](int16_t id)->unsigned {
        const unsigned result=wide_count+unsigned(id);
        if(result>=wide_count+67U)
            throw std::runtime_error("source GPU surface ID outside original 67 narrow RES4 entries");
        return result;
    };
    for(unsigned y=0;y<source.height;++y)for(unsigned x=0;x<source.width;++x) {
        const auto cell=world.cell(level,x,y);
        if(cell.normal_collision==NativeCollision::Passable) {
            auto& floor=floor_id<0?scene.fallback_floor:
                                scene.textured_triangles[tex_id(floor_id)];
            quad(floor,{float(x),float(y),0,float(x),float(y)},
                       {float(x+1),float(y),0,float(x+1),float(y)},
                       {float(x+1),float(y+1),0,float(x+1),float(y+1)},
                       {float(x),float(y+1),0,float(x),float(y+1)});
            ++scene.floor_faces;
            auto& ceil=ceil_id<0?scene.fallback_ceiling:
                               scene.textured_triangles[tex_id(ceil_id)];
            quad(ceil,{float(x),float(y),ceiling_z,float(x),float(y)},
                      {float(x),float(y+1),ceiling_z,float(x),float(y+1)},
                      {float(x+1),float(y+1),ceiling_z,float(x+1),float(y+1)},
                      {float(x+1),float(y),ceiling_z,float(x+1),float(y)});
            ++scene.ceiling_faces;
            continue;
        }
        const unsigned code=cell.code;
        if(code>=64U)throw std::runtime_error("invalid six-bit level wall source code");
        const bool upper=(cell.word&0x0200U)!=0U;
        const float height=2.0f+(upper?upper_height:0.f);
        constexpr int dx[]{-1,1,0,0},dy[]{0,0,-1,1};
        for(int face=0;face<4;++face) {
            const int nx=int(x)+dx[face],ny=int(y)+dy[face];
            float neighbor_height=0.f;
            if(nx>=0 && ny>=0 && nx<int(source.width) && ny<int(source.height)) {
                const auto nc=world.cell(level,unsigned(nx),unsigned(ny));
                if(nc.normal_collision!=NativeCollision::Passable)
                    neighbor_height=2.0f+((nc.word&0x0200U)?upper_height:0.f);
            }
            const unsigned lower_bank=face<2?0U:2U;
            const unsigned upper_bank=lower_bank+1U;
            // An exposed lower face is exactly the portion not
            // occluded by a solid adjacent cell. Do not stretch
            // the texture or draw the hidden interior wall.
            const float lower_low=std::max(0.f,neighbor_height);
            if(lower_low<2.0f) {
                const unsigned id=source.resource_lookup(lower_bank,code);
                if(id>=wide_count)throw std::runtime_error("original authored wall ID outside wide source bank");
                if(id==sentinel)++scene.skipped_boundary_faces;
                else {
                    textured_face(scene.textured_triangles[id],
                        int(x),int(y),face,lower_low,2.f,
                        (2.f-lower_low)/2.f,0.f);
                    ++scene.lower_wall_faces;
                }
            }
            if(upper && height>std::max(2.0f,neighbor_height)) {
                const float low=std::max(2.f,neighbor_height);
                const unsigned id=source.resource_lookup(upper_bank,code);
                if(id>=wide_count)throw std::runtime_error("original upper wall ID outside wide source bank");
                if(id==sentinel)++scene.skipped_boundary_faces;
                else {
                    textured_face(scene.textured_triangles[id],
                        int(x),int(y),face,low,height,
                        (height-low)/2.f,0.f);
                    ++scene.upper_wall_faces;
                }
            }
        }
    }
    return scene;
}
} // namespace rloft_native
