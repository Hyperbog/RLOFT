#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "rloft_native/native_win32_gpu_exploration.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_gpu_world_mesh.hpp"
#include "rloft_native/native_scene_view.hpp"
#include "rloft_native/native_type6_texture.hpp"
#include "rloft_native/native_palette_context.hpp"
#include "rloft_native/native_raster_colors.hpp"
#include "rloft_native/native_assets.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft_native {
namespace {
constexpr double kPi=3.14159265358979323846;
constexpr UINT_PTR kTimer=101;
constexpr double kMouseSensitivity=0.0020;
constexpr double kMoveSpeed=2.05;
constexpr double kTurnSpeed=1.9;

struct State {
    NativeWorldRuntime world;
    NativeGpuScene mesh;
    NativeProbeCamera camera;
    NativeResArchive res4;
    std::vector<std::uint8_t> original_colors;
    NativePalette6 palette;
    unsigned palette_bank{};
    std::map<unsigned,GLuint> gpu_textures;
    HDC dc{};
    HGLRC context{};
    HWND hwnd{};
    bool owns_window{};
    bool mouse_look{};
    bool smooth_filter{};
    bool automap{};
    bool blocked{};
    unsigned level{};
    unsigned zoom{6U};
    unsigned face_count{};
    ULONGLONG previous_time{};
    std::map<unsigned,std::vector<std::uint8_t>> explored;

    explicit State(NativeWorldPayload world_source,const NativeMedia& media)
        :world(std::move(world_source)),
         res4(media.read("RES4")),
         original_colors(media.read("COLORS")) {
        camera.field_of_view_radians=75.0*kPi/180.0;
    }
    void initialize_level(unsigned id) {
        if(id>=32U)throw std::out_of_range("GPU level out of range");
        const auto scene=build_native_gpu_scene(world,id);
        const auto& map=world.original().level(id);
        unsigned best_x=0,best_y=0,neighbors=0;
        double best_score=-1e30;
        bool found=false;
        for(unsigned y=0;y<map.height;++y)for(unsigned x=0;x<map.width;++x) {
            if(world.normal_collision(id,x,y)!=NativeCollision::Passable)continue;
            unsigned n=0;
            if(x>0 && world.normal_collision(id,x-1,y)==NativeCollision::Passable)++n;
            if(y>0 && world.normal_collision(id,x,y-1)==NativeCollision::Passable)++n;
            if(x+1<map.width && world.normal_collision(id,x+1,y)==NativeCollision::Passable)++n;
            if(y+1<map.height && world.normal_collision(id,x,y+1)==NativeCollision::Passable)++n;
            const auto dx=double(x)+0.5-double(map.width)*0.5;
            const auto dy=double(y)+0.5-double(map.height)*0.5;
            const auto score=100.0*double(n)-dx*dx-dy*dy;
            if(!found || score>best_score) {
                found=true;best_score=score;best_x=x;best_y=y;neighbors=n;
            }
        }
        if(!found)throw std::runtime_error("no passable cell in original source level");
        const auto new_palette_bank=native_world_palette_bank(id);
        const auto new_palette=decode_native_palette(original_colors,new_palette_bank);
        // Commit the scene and current-level source palette together.
        if(context) {
            wglMakeCurrent(dc,context);
            for(const auto& tex:gpu_textures)glDeleteTextures(1,&tex.second);
        }
        gpu_textures.clear();
        palette=new_palette;
        palette_bank=new_palette_bank;
        world.select_level(id);
        mesh=scene;
        level=id;
        camera.level=id;
        camera.column_position=double(best_x)+0.5;
        camera.row_position=double(best_y)+0.5;
        camera.heading_radians=-kPi/2;
        camera.pitch_radians=0.0;
        blocked=false;
        previous_time=0;
        face_count=mesh.lower_wall_faces+mesh.upper_wall_faces;
        auto& seen=explored[level];
        if(seen.size()!=std::size_t(map.width)*map.height)
            seen.assign(std::size_t(map.width)*map.height,0);
        seen[std::size_t(best_y)*map.width+best_x]=1;
    }
    void release_graphics() {
        if(context) {
            wglMakeCurrent(dc,context);
            for(const auto& tex:gpu_textures)glDeleteTextures(1,&tex.second);
            gpu_textures.clear();
            wglMakeCurrent(nullptr,nullptr);
            wglDeleteContext(context);
            context=nullptr;
        }
        if(dc) {
            ReleaseDC(hwnd,dc);
            dc=nullptr;
        }
    }
    void make_graphics() {
        dc=GetDC(hwnd);
        if(!dc)throw std::runtime_error("GPU window HDC unavailable");
        PIXELFORMATDESCRIPTOR fmt{};
        fmt.nSize=sizeof(fmt);
        fmt.nVersion=1;
        fmt.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
        fmt.iPixelType=PFD_TYPE_RGBA;
        fmt.cColorBits=32;
        fmt.cDepthBits=24;
        fmt.iLayerType=PFD_MAIN_PLANE;
        const int pix=ChoosePixelFormat(dc,&fmt);
        if(!pix || !SetPixelFormat(dc,pix,&fmt))
            throw std::runtime_error("OpenGL pixel format unavailable");
        context=wglCreateContext(dc);
        if(!context || !wglMakeCurrent(dc,context))
            throw std::runtime_error("OpenGL context unavailable");
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_CULL_FACE); // original authored faces have both windings
        glClearDepth(1.0);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    }
    GLuint original_texture(unsigned id) {
        const auto it=gpu_textures.find(id);
        if(it!=gpu_textures.end())return it->second;
        if(id>=res4.entry_count())
            throw std::runtime_error("GPU source texture ID outside original RES4");
        const auto decoded=decode_native_type6_texture(res4.entry(id));
        if(decoded.frames!=1 || decoded.height!=128 ||
           (decoded.width!=32 && decoded.width!=64))
            throw std::runtime_error("original GPU EH6 texture shape incorrect");
        std::vector<std::uint8_t> rgb(decoded.pixels.size()*3U);
        for(std::size_t i=0;i<decoded.pixels.size();++i) {
            const unsigned ci=decoded.pixels[i];
            const std::size_t from=ci*3U;
            const std::size_t to=i*3U;
            for(unsigned k=0;k<3U;++k)
                rgb[to+k]=static_cast<std::uint8_t>(
                    (unsigned(palette.rgb[from+k])*255U+31U)/63U);
        }
        GLuint tex=0;
        glGenTextures(1,&tex);
        if(!tex)throw std::runtime_error("failed to allocate native GPU texture");
        glBindTexture(GL_TEXTURE_2D,tex);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        const auto filter=smooth_filter?GL_LINEAR:GL_NEAREST;
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter);
        // EH6 decoder already transposed the ORIGINAL 128-byte columns.
        // Keep top source row at v=0: wall UV=0 at upper edge, =1 bottom.
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,decoded.width,decoded.height,
            0,GL_RGB,GL_UNSIGNED_BYTE,rgb.data());
        gpu_textures.emplace(id,tex);
        return tex;
    }
    void update_filter() {
        if(!context)return;
        wglMakeCurrent(dc,context);
        const auto filter=smooth_filter?GL_LINEAR:GL_NEAREST;
        for(const auto& kv:gpu_textures) {
            glBindTexture(GL_TEXTURE_2D,kv.second);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter);
        }
    }
    void mark_seen() {
        const auto& map=world.original().level(level);
        auto& grid=explored[level];
        const int cx=int(camera.column_position),cy=int(camera.row_position);
        for(int y=std::max(0,cy-6);y<std::min(int(map.height),cy+7);++y)
            for(int x=std::max(0,cx-6);x<std::min(int(map.width),cx+7);++x) {
                const double a=std::atan2(double(y)+0.5-camera.row_position,
                                         double(x)+0.5-camera.column_position);
                // Only reveal forward cells; old v0.7 showed similar
                // session-only explored topology, not DOS map semantics.
                const double relative=std::remainder(a-camera.heading_radians,2*kPi);
                if(std::abs(relative)>1.25)continue;
                const auto ix=unsigned(x),iy=unsigned(y);
                // Intervening source collision cells must block map sight.
                bool visible=true;
                const double vx=double(x)+0.5-camera.column_position;
                const double vy=double(y)+0.5-camera.row_position;
                const int steps=std::max(1,int(std::hypot(vx,vy)*8.0));
                for(int s=1;s<steps;++s) {
                    const double px=camera.column_position+vx*s/double(steps);
                    const double py=camera.row_position+vy*s/double(steps);
                    if(world.normal_collision(level,unsigned(px),unsigned(py))!=
                       NativeCollision::Passable) {visible=false;break;}
                }
                if(visible)grid[std::size_t(iy)*map.width+ix]=1;
            }
        if(cx>=0 && cy>=0 && cx<int(map.width) && cy<int(map.height))
            grid[std::size_t(cy)*map.width+unsigned(cx)]=1;
    }
    void step(double dt) {
        const auto held=[](int vk){return (GetAsyncKeyState(vk)&0x8000)!=0;};
        if(mouse_look && GetFocus()==hwnd) {
            RECT bounds{};GetClientRect(hwnd,&bounds);
            POINT center{bounds.right/2,bounds.bottom/2};
            if(ClientToScreen(hwnd,&center)) {
                POINT pos{};
                if(GetCursorPos(&pos)) {
                    const int dx=pos.x-center.x,dy=pos.y-center.y;
                    if(dx||dy) {
                        SetCursorPos(center.x,center.y);
                        camera.heading_radians=std::remainder(
                            camera.heading_radians+double(dx)*kMouseSensitivity,
                            2*kPi);
                        camera.pitch_radians=std::clamp(
                            camera.pitch_radians-double(dy)*kMouseSensitivity,
                            -1.42,1.42);
                    }
                }
            }
        }
        const auto yaw=int(held('E'))-int(held('Q'));
        if(yaw)camera.heading_radians=std::remainder(
            camera.heading_radians+double(yaw)*kTurnSpeed*dt,2*kPi);
        const int forward=int(held('W'))-int(held('S'));
        const int strafe=int(held('D'))-int(held('A'));
        if(forward||strafe) {
            const double norm=std::max(1.0,std::hypot(double(forward),double(strafe)));
            const double walk=kMoveSpeed*dt/norm;
            const double ca=std::cos(camera.heading_radians);
            const double sa=std::sin(camera.heading_radians);
            const double dx=(double(forward)*ca-double(strafe)*sa)*walk;
            const double dy=(double(forward)*sa+double(strafe)*ca)*walk;
            const auto move=move_native_probe_camera_clearance(world,camera,dx,dy,.22);
            blocked=move.blocked;
        } else blocked=false;
        mark_seen();
    }
    void automap_draw(int width,int height) {
        if(!automap)return;
        const auto& map=world.original().level(level);
        const auto& revealed=explored.at(level);
        const int size=std::min(210,std::max(110,width/4));
        const int margin=15;
        const int ax=std::max(0,width-size-margin),ay=15;
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();glLoadIdentity();
        glOrtho(0,double(width),double(height),0,-1,1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();glLoadIdentity();
        glColor3f(.05f,.08f,.11f);
        glBegin(GL_QUADS);
        glVertex2i(ax,ay);glVertex2i(ax+size,ay);
        glVertex2i(ax+size,ay+size);glVertex2i(ax,ay+size);
        glEnd();
        const int z=int(zoom),cx=int(camera.column_position),cy=int(camera.row_position);
        for(int y=std::max(0,cy-size/(2*z)-1);
             y<std::min(int(map.height),cy+size/(2*z)+2);++y) {
            for(int x=std::max(0,cx-size/(2*z)-1);
                 x<std::min(int(map.width),cx+size/(2*z)+2);++x) {
                if(!revealed[std::size_t(y)*map.width+unsigned(x)])continue;
                const int sx=ax+size/2+(x-cx)*z;
                const int sy=ay+size/2+(y-cy)*z;
                if(sx<ax || sy<ay || sx+z>ax+size || sy+z>ay+size)continue;
                if(world.normal_collision(level,unsigned(x),unsigned(y))==
                   NativeCollision::Passable)glColor3f(.24f,.30f,.30f);
                else glColor3f(.56f,.55f,.57f);
                glBegin(GL_QUADS);
                glVertex2i(sx,sy);glVertex2i(sx+z,sy);
                glVertex2i(sx+z,sy+z);glVertex2i(sx,sy+z);
                glEnd();
            }
        }
        const float mx=float(ax+size/2),my=float(ay+size/2);
        glColor3f(1.f,.76f,.22f);
        glBegin(GL_LINES);
        glVertex2f(mx,my);
        glVertex2f(mx+float(12*std::cos(camera.heading_radians)),
                   my+float(12*std::sin(camera.heading_radians)));
        glEnd();
        glMatrixMode(GL_PROJECTION);glPopMatrix();
        glMatrixMode(GL_MODELVIEW);glPopMatrix();
        glEnable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);
    }
    void paint() {
        if(!context || !dc)return;
        wglMakeCurrent(dc,context);
        RECT rect{};GetClientRect(hwnd,&rect);
        const int width=std::max(1,int(rect.right-rect.left));
        const int height=std::max(1,int(rect.bottom-rect.top));
        glViewport(0,0,width,height);
        // Missing original outdoor sky is a level-authored COLORS fallback.
        const auto& map=world.original().level(level);
        const unsigned ci=map.original_header[0x298U];
        const unsigned color_base=ci*3U;
        glClearColor(float(palette.rgb[color_base])/63.f,
                     float(palette.rgb[color_base+1U])/63.f,
                     float(palette.rgb[color_base+2U])/63.f,1.f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);glLoadIdentity();
        gluPerspective(75.0,double(width)/double(height),0.025,160.0);
        glMatrixMode(GL_MODELVIEW);glLoadIdentity();
        const double c=std::cos(camera.pitch_radians);
        const double dx=std::cos(camera.heading_radians)*c;
        const double dy=std::sin(camera.heading_radians)*c;
        const double dz=std::sin(camera.pitch_radians);
        gluLookAt(camera.column_position,camera.row_position,1.0,
                  camera.column_position+dx,camera.row_position+dy,1.0+dz,
                  0,0,1);
        glColor3f(1,1,1);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        for(const auto& batch:mesh.textured_triangles) {
            if(batch.second.empty())continue;
            glBindTexture(GL_TEXTURE_2D,original_texture(batch.first));
            const auto* p=batch.second.data();
            glVertexPointer(3,GL_FLOAT,sizeof(NativeGpuVertex),&p[0].x);
            glTexCoordPointer(2,GL_FLOAT,sizeof(NativeGpuVertex),&p[0].u);
            glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(batch.second.size()));
        }
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
        const auto draw_fill=[&](const std::vector<NativeGpuVertex>& batch,unsigned idx) {
            if(batch.empty())return;
            const auto at=idx*3U;
            glColor3f(float(palette.rgb[at])/63.f,
                      float(palette.rgb[at+1U])/63.f,
                      float(palette.rgb[at+2U])/63.f);
            glVertexPointer(3,GL_FLOAT,sizeof(NativeGpuVertex),&batch[0].x);
            glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(batch.size()));
        };
        draw_fill(mesh.fallback_floor,map.original_header[0x299U]);
        draw_fill(mesh.fallback_ceiling,map.original_header[0x298U]);
        glDisableClientState(GL_VERTEX_ARRAY);
        glEnable(GL_TEXTURE_2D);
        automap_draw(width,height);
        glFlush();
        SwapBuffers(dc);
    }
};

void mouse_mode(HWND hwnd,State& s,bool enabled) {
    if(s.mouse_look==enabled)return;
    if(enabled) {
        SetFocus(hwnd);
        RECT rect{};GetClientRect(hwnd,&rect);
        POINT center{rect.right/2,rect.bottom/2};
        if(!ClientToScreen(hwnd,&center) ||
           !SetCursorPos(center.x,center.y))
            throw std::runtime_error("unable to enable mouse look");
        SetCapture(hwnd);
        ShowCursor(FALSE);
        s.mouse_look=true;
    } else {
        s.mouse_look=false;
        if(GetCapture()==hwnd)ReleaseCapture();
        ShowCursor(TRUE);
        s.camera.pitch_radians=0.0; // optional only
    }
}
void title(HWND hwnd,const State& s) {
    const auto name=std::wstring(L"Ravenloft — Original 3D GPU Preview | Level ")+
        std::to_wstring(s.level)+L"  ["+
        std::to_wstring(unsigned(s.camera.column_position))+L","+
        std::to_wstring(unsigned(s.camera.row_position))+L"] | "+
        (s.mouse_look?L"MOUSE LOOK":L"Q/E KEYBOARD LOOK")+
        (s.blocked?L" | BLOCKED":L"");
    SetWindowTextW(hwnd,name.c_str());
}
LRESULT CALLBACK gpu_proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    auto* s=reinterpret_cast<State*>(GetWindowLongPtrW(hwnd,GWLP_USERDATA));
    if(msg==WM_NCCREATE) {
        const auto* create=reinterpret_cast<const CREATESTRUCTW*>(lp);
        s=static_cast<State*>(create->lpCreateParams);
        if(!s)return FALSE;
        s->hwnd=hwnd;
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(s));
        return TRUE;
    }
    if(!s)return DefWindowProcW(hwnd,msg,wp,lp);
    switch(msg) {
    case WM_CREATE:
        try {
            s->make_graphics();
            s->initialize_level(0);
            if(!SetTimer(hwnd,kTimer,16U,nullptr))
                throw std::runtime_error("GPU frame timer unavailable");
            title(hwnd,*s);
            return 0;
        } catch(const std::exception&) {
            s->release_graphics();
            return -1;
        }
    case WM_ERASEBKGND:return 1;
    case WM_SIZE:
        InvalidateRect(hwnd,nullptr,FALSE);return 0;
    case WM_LBUTTONDOWN:SetFocus(hwnd);return 0;
    case WM_RBUTTONDOWN:
        try {mouse_mode(hwnd,*s,!s->mouse_look);}
        catch(const std::exception&) {MessageBoxW(hwnd,L"Unable to capture mouse",
            L"GPU viewport",MB_OK|MB_ICONWARNING);}
        return 0;
    case WM_MOUSEMOVE:
        // Deliberately never recenter or repaint on individual mouse
        // messages; fixed tick owns mouse deltas and rendering.
        return 0;
    case WM_KILLFOCUS:
        s->previous_time=0;
        if(s->mouse_look)mouse_mode(hwnd,*s,false);
        return 0;
    case WM_KEYDOWN:
        try {
            if(wp=='M' && !(lp&(1L<<30)))mouse_mode(hwnd,*s,!s->mouse_look);
            else if(wp=='N' && !(lp&(1L<<30))) {
                s->smooth_filter=!s->smooth_filter;s->update_filter();
            } else if(wp==VK_TAB && !(lp&(1L<<30)))s->automap=!s->automap;
            else if(wp==VK_OEM_PLUS || wp==VK_ADD)
                s->zoom=std::min(16U,s->zoom+1U);
            else if(wp==VK_OEM_MINUS || wp==VK_SUBTRACT)
                s->zoom=std::max(2U,s->zoom-1U);
            else if(wp==VK_PRIOR) {
                s->initialize_level((s->level+1U)%32U);
                title(hwnd,*s);
            } else if(wp==VK_NEXT) {
                s->initialize_level((s->level+31U)%32U);
                title(hwnd,*s);
            } else if(wp=='R') {
                s->initialize_level(s->level);title(hwnd,*s);
            } else if(wp==VK_ESCAPE)DestroyWindow(hwnd);
        } catch(const std::exception&) {
            MessageBoxW(hwnd,L"Unable to change original world scene",
                        L"GPU preview",MB_OK|MB_ICONWARNING);
        }
        return 0;
    case WM_TIMER:
        if(wp==kTimer && GetFocus()==hwnd) {
            const auto now=GetTickCount64();
            const double dt=s->previous_time?
                std::clamp(double(now-s->previous_time)/1000.0,0.0,.06):.016;
            s->previous_time=now;
            try {s->step(dt);s->paint();title(hwnd,*s);}
            catch(const std::exception&) {
                KillTimer(hwnd,kTimer);
                if(s->mouse_look)mouse_mode(hwnd,*s,false);
                MessageBoxW(hwnd,L"An original asset or GPU viewport call failed. "
                    L"The original game files were not modified.",
                    L"GPU preview",MB_OK|MB_ICONWARNING);
            }
            return 0;
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        BeginPaint(hwnd,&ps);
        try{s->paint();}
        catch(const std::exception&) {}
        EndPaint(hwnd,&ps);
        return 0;
    }
    case WM_CLOSE:DestroyWindow(hwnd);return 0;
    case WM_NCDESTROY:
        KillTimer(hwnd,kTimer);
        if(s->mouse_look)mouse_mode(hwnd,*s,false);
        s->release_graphics();
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,0);
        if(s->owns_window)delete s;
        return DefWindowProcW(hwnd,msg,wp,lp);
    default:break;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
} // namespace
void open_native_gpu_world_preview(HWND owner,const NativeMedia& media) {
    auto state=std::make_unique<State>(load_native_world_payload(media),media);
    WNDCLASSW type{};
    type.lpfnWndProc=gpu_proc;
    type.hInstance=GetModuleHandleW(nullptr);
    type.hCursor=LoadCursorW(nullptr,IDC_ARROW);
    type.lpszClassName=L"RloftNativeOpenGLWorld";
    type.style=CS_OWNDC;
    if(!RegisterClassW(&type) && GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)
        throw std::runtime_error("unable to register native GPU viewport class");
    HWND window=CreateWindowExW(0,type.lpszClassName,
        L"Ravenloft — Original-file GPU World | Q/E turn, M mouse, N filter, Tab map",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,
        1260,840,owner,nullptr,type.hInstance,state.get());
    if(!window)throw std::runtime_error("unable to initialize accelerated world preview");
    state->owns_window=true;
    state.release();
    ShowWindow(window,SW_SHOW);
    UpdateWindow(window);
    SetFocus(window);
}
} // namespace rloft_native
#endif
