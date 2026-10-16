#ifdef _WIN32
#define NOMINMAX
#include "rloft/win32_gpu_scene.hpp"
#include <GL/glu.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace rloft {
namespace {
constexpr double pi=3.14159265358979323846;
std::uint8_t expand6(std::uint8_t c) {
    return static_cast<std::uint8_t>((static_cast<unsigned>(c)*255U+31U)/63U);
}
unsigned next_power_two(unsigned n) {
    unsigned out=1U;
    while(out<n && out<1024U)out*=2U;
    if(out<n)throw std::invalid_argument("RES3 image exceeds native GPU limit");
    return out;
}
} // namespace

Win32GpuScene::Win32GpuScene(HWND hwnd,const RavenloftBootAssets& assets,
                              const Palette6& palette)
    :hwnd_(hwnd),assets_(assets),res4_(assets.source()),palette_(palette) {
    dc_=GetDC(hwnd_);
    if(!dc_)throw std::runtime_error("cannot acquire original game GPU HDC");
    PIXELFORMATDESCRIPTOR pfd{};
    pfd.nSize=sizeof(pfd);pfd.nVersion=1U;
    pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
    pfd.iPixelType=PFD_TYPE_RGBA;pfd.cColorBits=32U;
    pfd.cDepthBits=24U;pfd.cAlphaBits=8U;pfd.iLayerType=PFD_MAIN_PLANE;
    const int format=ChoosePixelFormat(dc_,&pfd);
    if(!format || !SetPixelFormat(dc_,format,&pfd))
        throw std::runtime_error("Ravenloft OpenGL pixel format unavailable");
    context_=wglCreateContext(dc_);
    if(!context_)throw std::runtime_error("Ravenloft OpenGL context unavailable");
    make_current();
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1,&canvas_);
    if(!canvas_)throw std::runtime_error("Ravenloft GPU UI texture allocation failed");
    glBindTexture(GL_TEXTURE_2D,canvas_);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    canvas_pixels_.assign(320U*400U*4U,0U);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,512,512,0,GL_RGBA,
                 GL_UNSIGNED_BYTE,nullptr);
    wglMakeCurrent(nullptr,nullptr);
}
Win32GpuScene::~Win32GpuScene() {
    if(context_) {
        if(wglMakeCurrent(dc_,context_)) {
            destroy_textures();
            if(canvas_)glDeleteTextures(1,&canvas_);
            wglMakeCurrent(nullptr,nullptr);
        }
        wglDeleteContext(context_);
    }
    if(dc_&&hwnd_&&IsWindow(hwnd_))ReleaseDC(hwnd_,dc_);
}
void Win32GpuScene::make_current() const {
    if(!context_ || !wglMakeCurrent(dc_,context_))
        throw std::runtime_error("unable to select Ravenloft GPU context");
}
void Win32GpuScene::destroy_textures() noexcept {
    for(const auto& item:materials_)glDeleteTextures(1,&item.second);
    for(const auto& item:sprites_)glDeleteTextures(1,&item.second);
    materials_.clear();sprites_.clear();sprite_uv_.clear();
}
std::uint64_t Win32GpuScene::fingerprint(const LevelState& level) const {
    auto hash=std::uint64_t{14695981039346656037ULL};
    auto mix=[&hash](std::uint8_t b) {
        hash^=b;hash*=1099511628211ULL;
    };
    for(auto word:level.cell_words) {mix(static_cast<std::uint8_t>(word));
                                     mix(static_cast<std::uint8_t>(word>>8U));}
    for(const auto& record:level.group2)
        for(const auto b:record.raw)mix(b);
    for(const auto b:level.header)mix(b);
    return hash;
}
void Win32GpuScene::bind(const ExplorationSessionState& session,
                          const LevelWorldState& world,
                          const std::vector<WorldSceneRecord>& records) {
    session_=&session;world_=&world;records_=records;
    const auto hash=fingerprint(session.level);
    if(!mesh_initialized_ || current_level_!=static_cast<int>(session.level_id) ||
       world_fingerprint_!=hash) {
        // The mutable CODE.2 gameplay map is the sole geometry authority.
        // Rebuild only on real world changes (door/opening, transition).
        auto next=EnhancedGpuWorldMeshModel::build(session.level);
        mesh_=std::move(next);
        mesh_initialized_=true;
        world_failed_=false;
        current_level_=static_cast<int>(session.level_id);
        world_fingerprint_=hash;
    }
}
GLuint Win32GpuScene::material(unsigned id) {
    const auto found=materials_.find(id);
    if(found!=materials_.end())return found->second;
    const auto decoded=res4_.texture(static_cast<int>(id));
    if(decoded.width!=32 && decoded.width!=64)
        throw std::runtime_error("invalid original GPU RES4 texture width");
    std::vector<std::uint8_t> rgb(
        static_cast<std::size_t>(decoded.width)*128U*3U);
    for(int y=0;y<128;++y)for(int x=0;x<decoded.width;++x) {
        const auto index=static_cast<std::size_t>(decoded.texel(x,y))*3U;
        const auto dest=(static_cast<std::size_t>(y)*decoded.width+
                         static_cast<std::size_t>(x))*3U;
        for(unsigned n=0;n<3U;++n)rgb[dest+n]=expand6(palette_.rgb[index+n]);
    }
    GLuint id_gl{};
    glGenTextures(1,&id_gl);
    if(!id_gl)throw std::runtime_error("OpenGL RES4 texture allocation failed");
    glBindTexture(GL_TEXTURE_2D,id_gl);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    const GLint result=gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,
        decoded.width,128,GL_RGB,GL_UNSIGNED_BYTE,rgb.data());
    if(result!=0) {
        glDeleteTextures(1,&id_gl);
        throw std::runtime_error("OpenGL RES4 mipmap upload failed: "+
                                 std::to_string(id));
    }
    materials_.emplace(id,id_gl);
    return id_gl;
}
GLuint Win32GpuScene::sprite(unsigned id) {
    const auto found=sprites_.find(id);
    if(found!=sprites_.end())return found->second;
    const auto raster=assets_.res3_type7_native_raster(static_cast<int>(id));
    if(raster.width==0 || raster.height!=128U ||
       raster.mask.size()!=raster.pixels.size() ||
       raster.pixels.size()!=static_cast<std::size_t>(raster.width)*128U)
        throw std::runtime_error("invalid live RES3 original sprite raster");
    const unsigned w=next_power_two(raster.width);
    std::vector<std::uint8_t> rgba(
        static_cast<std::size_t>(w)*128U*4U,0U);
    for(unsigned y=0;y<128U;++y)for(unsigned x=0;x<raster.width;++x) {
        const auto at=static_cast<std::size_t>(y)*raster.width+x;
        if(raster.mask[at]==0U)continue;
        const auto pi=static_cast<std::size_t>(raster.pixels[at])*3U;
        const auto dst=(static_cast<std::size_t>(127U-y)*w+x)*4U;
        rgba[dst]=expand6(palette_.rgb[pi]);
        rgba[dst+1]=expand6(palette_.rgb[pi+1U]);
        rgba[dst+2]=expand6(palette_.rgb[pi+2U]);
        rgba[dst+3]=255U;
    }
    GLuint tex{};
    glGenTextures(1,&tex);
    if(!tex)throw std::runtime_error("OpenGL RES3 texture allocation failed");
    glBindTexture(GL_TEXTURE_2D,tex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,static_cast<GLsizei>(w),128,
                 0,GL_RGBA,GL_UNSIGNED_BYTE,rgba.data());
    sprites_.emplace(id,tex);
    sprite_uv_.emplace(id,std::array<float,2>{
        static_cast<float>(raster.width)/static_cast<float>(w),1.f});
    return tex;
}
void Win32GpuScene::draw_sprites(double heading) {
    if(!world_)return;
    const double right_x=-std::sin(heading),right_y=std::cos(heading);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.1f);
    glDisable(GL_BLEND);
    glColor3f(1.f,1.f,1.f);
    for(const auto& record:records_) {
        std::uint32_t raw_x{},raw_y{};
        switch(record.source_class) {
          case 0U:
            if(record.source_index>=world_->npcs.size())continue;
            raw_x=world_->npcs[record.source_index].x_position_16_16;
            raw_y=world_->npcs[record.source_index].y_position_16_16;
            break;
          case 1U:
            if(record.source_index>=world_->placed_objects.size())continue;
            raw_x=world_->placed_objects[record.source_index].x_position_16_16;
            raw_y=world_->placed_objects[record.source_index].y_position_16_16;
            break;
          case 2U:
            if(record.source_index>=world_->loose_items.size())continue;
            raw_x=world_->loose_items[record.source_index].x_position_16_16;
            raw_y=world_->loose_items[record.source_index].y_position_16_16;
            break;
          default:continue;
        }
        const auto x=static_cast<double>(static_cast<std::int32_t>(raw_x))/65536.;
        const auto y=static_cast<double>(static_cast<std::int32_t>(raw_y))/65536.;
        // Source 16.16 coordinates identify the actual LIVE object, not
        // an invented spawn. The billboard size remains an enhanced 3D
        // presentation estimate; exact original scene projection is kept
        // in the reference compositor and source records.
        const double width=0.9,half=width/2.;
        try {
            const auto t=sprite(record.resource_id);
            const auto uv=sprite_uv_.at(record.resource_id);
            glBindTexture(GL_TEXTURE_2D,t);
            glBegin(GL_QUADS);
            glTexCoord2f(0,0);glVertex3d(x-right_x*half,y-right_y*half,0.0);
            glTexCoord2f(uv[0],0);glVertex3d(x+right_x*half,y+right_y*half,0.0);
            glTexCoord2f(uv[0],1);glVertex3d(x+right_x*half,y+right_y*half,2.0);
            glTexCoord2f(0,1);glVertex3d(x-right_x*half,y-right_y*half,2.0);
            glEnd();
        }catch(const std::exception& error) {
            OutputDebugStringA(error.what());
            // One malformed RES3 drawing never hides other source scenes.
        }
    }
    glDisable(GL_ALPHA_TEST);
}
void Win32GpuScene::draw_mesh(double pitch,unsigned filter_mode,double aspect) {
    if(!mesh_initialized_ || !session_)return;
    const auto heading=static_cast<double>(session_->facing)*pi/180.0;
    const auto x=static_cast<double>(
        static_cast<std::int32_t>(session_->coordinate_a))/65536.0;
    const auto y=static_cast<double>(
        static_cast<std::int32_t>(session_->coordinate_b))/65536.0;
    const auto c=std::cos(pitch);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();
    gluPerspective(75.0,aspect,0.025,160.0);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    // Game uses cardinal headings in degrees; the world basis remains
    // identical to the archived GPU world mesh, with no extra party state.
    gluLookAt(x,y,1.,x+std::cos(heading)*c,
              y+std::sin(heading)*c,1.+std::sin(pitch),0.,0.,1.);
    glColor3f(1,1,1);glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    // Retain NativeEngine's batched source mesh draw (not one glBegin per
    // triangle). The original live level is fingerprinted only on mutation.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    for(const auto& item:mesh_.textured) {
        if(item.second.empty())continue;
        glBindTexture(GL_TEXTURE_2D,material(item.first));
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
            filter_mode==0U?GL_NEAREST:
            filter_mode==1U?GL_LINEAR:GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
            filter_mode==0U?GL_NEAREST:GL_LINEAR);
        const auto* p=item.second.data();
        glVertexPointer(3,GL_FLOAT,sizeof(EnhancedGpuVertex),&p[0].x);
        glTexCoordPointer(2,GL_FLOAT,sizeof(EnhancedGpuVertex),&p[0].u);
        glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(item.second.size()));
    }
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    const auto draw_fill=[&](const std::vector<EnhancedGpuVertex>& verts,
                              std::uint8_t index) {
        if(verts.empty())return;
        const auto ci=static_cast<unsigned>(index)*3U;
        glColor3f(static_cast<float>(palette_.rgb[ci])/63.f,
                  static_cast<float>(palette_.rgb[ci+1U])/63.f,
                  static_cast<float>(palette_.rgb[ci+2U])/63.f);
        glVertexPointer(3,GL_FLOAT,sizeof(EnhancedGpuVertex),&verts[0].x);
        glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(verts.size()));
    };
    draw_fill(mesh_.floor,session_->level.header[0x299U]);
    draw_fill(mesh_.ceiling,session_->level.header[0x298U]);
    glDisableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_TEXTURE_2D);
    draw_sprites(heading);
    glDisable(GL_DEPTH_TEST);
}
void Win32GpuScene::paint(const RECT& dst,int cw,int ch,
                          const std::vector<std::uint32_t>& bgra,
                          bool draw_world,unsigned filter_mode,double pitch) {
    if(cw<=0 || ch<=0 || bgra.size()!=320U*400U)
        throw std::invalid_argument("invalid enhanced game GPU screen");
    make_current();
    glViewport(0,0,cw,ch);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    // Preserve original 320x400 viewport surround, portraits, menus,
    // cursors and all UI through the live software presenter. GPU world
    // draws ONLY inside the source 32..287, 4..259 scene rectangle.
    for(unsigned sy=0;sy<400U;++sy)for(unsigned sx=0;sx<320U;++sx) {
        const auto raw=bgra[static_cast<std::size_t>(399U-sy)*320U+sx];
        const auto di=(static_cast<std::size_t>(sy)*320U+sx)*4U;
        canvas_pixels_[di]=static_cast<std::uint8_t>((raw>>16U)&255U);
        canvas_pixels_[di+1U]=static_cast<std::uint8_t>((raw>>8U)&255U);
        canvas_pixels_[di+2U]=static_cast<std::uint8_t>(raw&255U);
        canvas_pixels_[di+3U]=255U;
    }
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,canvas_);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,320,400,GL_RGBA,GL_UNSIGNED_BYTE,
                    canvas_pixels_.data());
    const GLint filter=filter_mode==0U?GL_NEAREST:GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();
    glOrtho(0,cw,0,ch,-1,1);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    glColor3f(1,1,1);
    const auto x=static_cast<double>(dst.left),y=static_cast<double>(ch-dst.bottom);
    const auto w=static_cast<double>(dst.right-dst.left),h=static_cast<double>(dst.bottom-dst.top);
    constexpr float u=320.f/512.f,v=400.f/512.f;
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);glVertex2d(x,y);
    glTexCoord2f(u,0);glVertex2d(x+w,y);
    glTexCoord2f(u,v);glVertex2d(x+w,y+h);
    glTexCoord2f(0,v);glVertex2d(x,y+h);
    glEnd();
    if(draw_world && mesh_initialized_ && !world_failed_) {
        const int left=dst.left+(dst.right-dst.left)*32/320;
        const int right=dst.left+(dst.right-dst.left)*288/320;
        const int top=dst.top+(dst.bottom-dst.top)*4/400;
        const int bottom=dst.top+(dst.bottom-dst.top)*260/400;
        glEnable(GL_SCISSOR_TEST);
        glScissor(left,ch-bottom,right-left,bottom-top);
        glViewport(left,ch-bottom,right-left,bottom-top);
        glClear(GL_DEPTH_BUFFER_BIT);
        try{draw_mesh(pitch,filter_mode,
            static_cast<double>(right-left)/static_cast<double>(bottom-top));}
        catch(const std::exception& error) {
            OutputDebugStringA(error.what());
            // Leave original software scene enabled on subsequent frames:
            // malformed source media is never converted to invented art.
            world_failed_=true;
        }
        glDisable(GL_SCISSOR_TEST);
    }
    glFlush();
    SwapBuffers(dc_);
    wglMakeCurrent(nullptr,nullptr);
}
} // namespace rloft
#endif
