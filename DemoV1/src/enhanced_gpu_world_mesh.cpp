#include "rloft/enhanced_gpu_world_mesh.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {
using V=EnhancedGpuVertex;
void quad(std::vector<V>& out,V a,V b,V c,V d) {
    out.push_back(a);out.push_back(b);out.push_back(c);
    out.push_back(a);out.push_back(c);out.push_back(d);
}
int signed_word(const LevelState& level,unsigned offset) {
    const auto raw=static_cast<unsigned>(level.header.at(offset)) |
                   (static_cast<unsigned>(level.header.at(offset+1U))<<8U);
    return raw>=0x8000U?static_cast<int>(raw)-65536:static_cast<int>(raw);
}
unsigned resource(const LevelState& l,unsigned bank,unsigned code) {
    if(bank>=5U || code>=64U)
        throw std::out_of_range("invalid original RES4 bank/selector");
    const auto index=0x0AU+128U*bank+2U*code;
    return static_cast<unsigned>(l.header[index]) |
           (static_cast<unsigned>(l.header[index+1U])<<8U);
}
bool passable(const LevelState& l,int x,int y) {
    const auto word=l.cell_words[static_cast<std::size_t>(y*l.width+x)];
    if((word&0x0080U)!=0U) {
        // CODE.2 dynamic kind-1 control supplies the open position.
        // Unknown/missing controls stay blocked, never become fake paths.
        for(const auto& rec:l.group2)
            if(rec.kind()==1U && rec.column()==x && rec.row()==y)
                return rec.raw[6]==0U;
        return false;
    }
    return (word&0x003FU)==0U;
}
void face(std::vector<V>& dst,int cx,int cy,int direction,
          float low,float high,float vb,float vt) {
    const float x=static_cast<float>(cx),y=static_cast<float>(cy);
    if(direction==0)
        quad(dst,{x,y,low,0,vb},{x,y+1,low,1,vb},
             {x,y+1,high,1,vt},{x,y,high,0,vt});
    else if(direction==1)
        quad(dst,{x+1,y+1,low,0,vb},{x+1,y,low,1,vb},
             {x+1,y,high,1,vt},{x+1,y+1,high,0,vt});
    else if(direction==2)
        quad(dst,{x+1,y,low,0,vb},{x,y,low,1,vb},
             {x,y,high,1,vt},{x+1,y,high,0,vt});
    else
        quad(dst,{x,y+1,low,0,vb},{x+1,y+1,low,1,vb},
             {x+1,y+1,high,1,vt},{x,y+1,high,0,vt});
}
} // namespace

EnhancedGpuWorldMesh EnhancedGpuWorldMeshModel::build(
    const LevelState& l,unsigned wide,unsigned sentinel) {
    if(l.width<=0 || l.height<=0 || l.width>128 || l.height>128 ||
       static_cast<std::size_t>(l.width)*l.height!=l.cell_words.size() ||
       wide==0U)
        throw std::invalid_argument("invalid live GPU level geometry");
    const unsigned upper_rows=l.header[9U];
    if(upper_rows>128U)
        throw std::invalid_argument("original upper wall rows exceed 128");
    const float upper_height=2.f*static_cast<float>(upper_rows)/128.f;
    EnhancedGpuWorldMesh scene{};scene.width=l.width;scene.height=l.height;
    bool upper_present=false;
    for(const auto word:l.cell_words)
        if((word&0x0200U)!=0U && (word&0x3FU)!=0U)
            upper_present=true;
    const float ceiling_z=upper_present?2.f+upper_height:2.f;
    const int floor_id=signed_word(l,7U);
    const int ceiling_id=signed_word(l,5U);
    const auto narrow=[wide](int id)->unsigned {
        if(id<0 || id>=67)
            throw std::invalid_argument("RES4 narrow material outside original bank");
        return wide+static_cast<unsigned>(id);
    };
    if(floor_id>=0) (void)narrow(floor_id);
    if(ceiling_id>=0) (void)narrow(ceiling_id);
    for(int y=0;y<l.height;++y)for(int x=0;x<l.width;++x) {
        const auto word=l.cell_words[static_cast<std::size_t>(y*l.width+x)];
        if(passable(l,x,y)) {
            auto& floor=floor_id<0?scene.floor:scene.textured[narrow(floor_id)];
            const float fx=static_cast<float>(x),fy=static_cast<float>(y);
            quad(floor,{fx,fy,0,fx,fy},{fx+1,fy,0,fx+1,fy},
                 {fx+1,fy+1,0,fx+1,fy+1},{fx,fy+1,0,fx,fy+1});
            ++scene.floor_faces;
            if(ceiling_id>=0) {
                auto& ceiling=scene.textured[narrow(ceiling_id)];
                quad(ceiling,{fx,fy,ceiling_z,fx,fy},
                     {fx,fy+1,ceiling_z,fx,fy+1},
                     {fx+1,fy+1,ceiling_z,fx+1,fy+1},
                     {fx+1,fy,ceiling_z,fx+1,fy});
                ++scene.ceiling_faces;
            }
            continue;
        }
        const auto code=static_cast<unsigned>(word&0x003FU);
        if(code==0U)continue; // special dynamic frame without wall art
        const bool upper=(word&0x0200U)!=0U;
        const float height=2.f+(upper?upper_height:0.f);
        constexpr int dx[]{-1,1,0,0},dy[]{0,0,-1,1};
        for(int n=0;n<4;++n) {
            const int nx=x+dx[n],ny=y+dy[n];
            float neighbor=0.f;
            if(nx>=0 && ny>=0 && nx<l.width && ny<l.height &&
               !passable(l,nx,ny)) {
                const auto w=l.cell_words[static_cast<std::size_t>(ny*l.width+nx)];
                neighbor=2.f+((w&0x0200U)!=0U?upper_height:0.f);
            }
            const unsigned bank=n<2?0U:2U;
            const float low=std::max(0.f,neighbor);
            if(low<2.f) {
                const unsigned id=resource(l,bank,code);
                if(id>=wide)throw std::runtime_error("wide RES4 selector outside authored bank");
                if(id==sentinel)++scene.skipped_boundary_faces;
                else {
                    face(scene.textured[id],x,y,n,low,2.f,(2.f-low)/2.f,0.f);
                    ++scene.lower_faces;
                }
            }
            if(upper && height>std::max(2.f,neighbor)) {
                const float base=std::max(2.f,neighbor);
                const unsigned id=resource(l,bank+1U,code);
                if(id>=wide)throw std::runtime_error("upper RES4 selector outside bank");
                if(id==sentinel)++scene.skipped_boundary_faces;
                else {
                    face(scene.textured[id],x,y,n,base,height,
                         (height-base)/2.f,0.f);
                    ++scene.upper_faces;
                }
            }
        }
    }
    return scene;
}
} // namespace rloft
