#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "rloft_native/native_win32_gpu_exploration.hpp"
#include "rloft_native/native_win32_gl_target.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_gpu_world_mesh.hpp"
#include "rloft_native/native_scene_view.hpp"
#include "rloft_native/native_type6_texture.hpp"
#include "rloft_native/native_palette_context.hpp"
#include "rloft_native/native_code2_backdrop.hpp"
#include "rloft_native/native_raster_colors.hpp"
#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_scene_instances.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <utility>
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
constexpr double kRunSpeed=3.5;

struct State {
    NativeWorldRuntime world;
    // Null only for the explicitly separate technical viewport preview.
    std::optional<NativePartyStart> party;
    std::optional<NativeAuthoredSceneDirectory> authored_scenes;
    bool show_authored_markers{true};
    NativeGpuScene mesh;
    NativeProbeCamera camera;
    NativeResArchive res4;
    NativeCode2Backdrop backdrop;
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
    bool automap{true};
    bool show_help{true};
    bool fullscreen{};
    bool noclip{};
    WINDOWPLACEMENT saved_placement{sizeof(WINDOWPLACEMENT)};
    LONG_PTR saved_style{};
    unsigned filter_mode{};
    unsigned aa_samples{};
    unsigned map_panel_size{210U};
    GLuint hud_font{};
    NativeGlViewportTarget render_target;
    bool blocked{};
    unsigned level{};
    unsigned zoom{6U};
    unsigned face_count{};
    unsigned resolution_index{};
    bool visibility_initialized{};
    double last_visibility_x{},last_visibility_y{},last_visibility_heading{};
    ULONGLONG previous_time{};
    std::map<unsigned,std::vector<std::uint8_t>> explored;

    explicit State(NativeWorldPayload world_source,const NativeMedia& media,
                   std::optional<NativePartyStart> startup={})
        :world(std::move(world_source)),
         party(std::move(startup)),
         res4(media.read("RES4")),
         backdrop(media.read("CODE.2")),
         original_colors(media.read("COLORS")) {
        camera.field_of_view_radians=75.0*kPi/180.0;
        if(party) {
            // Consume existing independently verified original RES3 / NPCS /
            // INIT / OBJECTS compact-placement ledger. No provisional shape
            // frame, fake actor or world item is instantiated by this step.
            const auto res3_count=
                NativeResArchive(media.read("RES3")).entry_count();
            const auto banks=decode_native_res3_scene_banks(
                media.read("INIT"),res3_count);
            const auto definitions=decode_native_scene_definitions(
                banks,media.read("NPCS.DAT"),media.read("OBJECTS.DAT"));
            authored_scenes=derive_native_authored_scene_instances(
                world.original(),definitions);
            const auto& first=authored_scenes->levels.at(party->level);
            if(party->level==22U && first.size()!=111U)
                throw std::runtime_error(
                    "Level22 original 1+110+0 scene placement inventory changed");
        }
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
        if(party) {
            // CODE.2 normal first level, after original CODE.1 CHARS transfer.
            // Fail rather than invent a replacement spawn when source conflicts.
            if(id!=party->level)
                throw std::logic_error("active party cannot teleport to arbitrary map");
            best_x=unsigned(party->column);
            best_y=unsigned(party->row);
            if(best_x>=map.width||best_y>=map.height||
               world.normal_collision(id,best_x,best_y)!=NativeCollision::Passable)
                throw std::runtime_error(
                    "original Level22 party start is not source-passable");
        }
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
        camera.column_position=party?party->column:double(best_x)+0.5;
        camera.row_position=party?party->row:double(best_y)+0.5;
        camera.heading_radians=party?
            double(party->heading_degrees)*kPi/180.0:-kPi/2;
        camera.pitch_radians=0.0;
        blocked=false;
        previous_time=0;
        face_count=mesh.lower_wall_faces+mesh.upper_wall_faces;
        visibility_initialized=false;
        auto& seen=explored[level];
        if(seen.size()!=std::size_t(map.width)*map.height)
            seen.assign(std::size_t(map.width)*map.height,0);
        seen[std::size_t(best_y)*map.width+best_x]=1;
    }
    void release_graphics() {
        if(context) {
            if(hud_font){glDeleteLists(hud_font,96);hud_font=0;}
            render_target.release();
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
        render_target.initialize();
        HFONT font=CreateFontW(-17,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
            ANTIALIASED_QUALITY,FIXED_PITCH|FF_MODERN,L"Consolas");
        if(font){
            const HGDIOBJ old=SelectObject(dc,font);
            hud_font=glGenLists(96);
            if(hud_font&&!wglUseFontBitmapsW(dc,32,96,hud_font)){
                glDeleteLists(hud_font,96);hud_font=0;
            }
            SelectObject(dc,old);DeleteObject(font);
        }
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
        // F3 modes mirror the legacy viewer's nearest / filtered /
        // distant-mipmap tradeoff. Mipmaps are PRESENTATION derivatives
        // only; original game asset pixels are never modified.
        const GLint mip=gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,
            decoded.width,decoded.height,GL_RGB,GL_UNSIGNED_BYTE,rgb.data());
        if(mip!=0) {
            glDeleteTextures(1,&tex);
            throw std::runtime_error("OpenGL/GLU mipmap upload failed for original RES4 resource "+
                std::to_string(id)+", GLU="+std::to_string(mip));
        }
        const GLenum min_filter=filter_mode==0U?GL_NEAREST:
            (filter_mode==1U?GL_LINEAR:GL_LINEAR_MIPMAP_LINEAR);
        const GLenum mag_filter=filter_mode==0U?GL_NEAREST:GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,min_filter);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,mag_filter);
        gpu_textures.emplace(id,tex);
        return tex;
    }
    void update_filter() {
        if(!context)return;
        wglMakeCurrent(dc,context);
        const auto min_filter=filter_mode==0U?GL_NEAREST:
            (filter_mode==1U?GL_LINEAR:GL_LINEAR_MIPMAP_LINEAR);
        const auto mag_filter=filter_mode==0U?GL_NEAREST:GL_LINEAR;
        for(const auto& kv:gpu_textures) {
            glBindTexture(GL_TEXTURE_2D,kv.second);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,mag_filter);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,min_filter);
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
                            camera.heading_radians-double(dx)*kMouseSensitivity,
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
            const auto intended=native_relative_movement(
                camera.heading_radians,double(forward),double(strafe),
                held(VK_SHIFT)?kRunSpeed:kMoveSpeed,dt);
            const double dx=intended.delta_columns;
            const double dy=intended.delta_rows;
            if(noclip) {
                camera.column_position=std::clamp(camera.column_position+dx,
                    .22,double(world.original().level(level).width)-.22);
                camera.row_position=std::clamp(camera.row_position+dy,
                    .22,double(world.original().level(level).height)-.22);
                blocked=false;
            } else {
                const auto move=move_native_probe_camera_clearance(world,camera,dx,dy,.22);
                blocked=move.blocked;
            }
        } else blocked=false;
        // Visibility is a session automap operation, not the render
        // loop. Never repeat a ~200-ray cell traversal 60 times per
        // second while stationary; that caused input jitter in the
        // old CPU implementation on large levels.
        if(!visibility_initialized ||
           std::hypot(camera.column_position-last_visibility_x,
                      camera.row_position-last_visibility_y)>0.12 ||
           std::abs(std::remainder(camera.heading_radians-
                      last_visibility_heading,2*kPi))>0.10) {
            mark_seen();
            last_visibility_x=camera.column_position;
            last_visibility_y=camera.row_position;
            last_visibility_heading=camera.heading_radians;
            visibility_initialized=true;
        }
    }
    void automap_draw(int width,int height) {
        if(!automap)return;
        const auto& map=world.original().level(level);
        const auto& revealed=explored.at(level);
        const int size=int(map_panel_size);
        const int margin=15;
        const int ax=margin,ay=15;
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
        // Diagnostic-only placement markers from the source-exact
        // compact 11-byte NPC/INIT/OBJECTS ledger. Show only discovered
        // cells; preserve original 16.16 subcell coordinates. Markers are
        // not simulated actors, pickups, EH7 view frames or encounter logic.
        if(party&&authored_scenes&&show_authored_markers) {
            const auto present=native_explored_scene_instance_candidates(
                world,*authored_scenes,level,revealed,32768U);
            glPointSize(float(std::max(3,z/2)));
            glBegin(GL_POINTS);
            for(const auto& item:present) {
                const double px=double(item.x_position_16_16)/65536.;
                const double py=double(item.y_position_16_16)/65536.;
                const float sx=float(ax+size/2)+
                    float(px-double(cx))*float(z);
                const float sy=float(ay+size/2)+
                    float(py-double(cy))*float(z);
                if(sx<ax+2||sx>ax+size-2||sy<ay+2||sy>ay+size-2)
                    continue;
                if(item.source_class==0U)glColor3f(.95f,.31f,.29f);
                else if(item.source_class==1U)glColor3f(.32f,.85f,.94f);
                else glColor3f(.89f,.77f,.32f);
                glVertex2f(sx,sy);
            }
            glEnd();
            glPointSize(1.f);
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
    void draw_hud(int width,int height) {
        if(!hud_font)return; // OpenGL window still renders if GDI fonts fail.
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
        glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();
        glOrtho(0,width,height,0,-1,1);
        glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
        const auto line=[&](int x,int y,const std::string& str) {
            glColor3f(.94f,.90f,.80f);
            glRasterPos2i(x,y);
            glListBase(hud_font-32);
            glCallLists(GLsizei(str.size()),GL_UNSIGNED_BYTE,str.c_str());
        };
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        const int box_height=show_help?208:55;
        glColor4f(.055f,.07f,.09f,.84f);
        glBegin(GL_QUADS);
        glVertex2i(0,height-box_height);glVertex2i(std::min(width,1200),height-box_height);
        glVertex2i(std::min(width,1200),height);glVertex2i(0,height);
        glEnd();
        glDisable(GL_BLEND);
        constexpr const char* modes[]{"Native","HD 720p","Full HD 1080p","QHD 1440p","4K UHD"};
        const auto* mode=modes[resolution_index];
        const std::string resolution=std::to_string(render_target.width())+"x"+
                                     std::to_string(render_target.height());
        const std::string actor_label=party?
            (" | "+party->actors[0].name+" / "+party->actors[1].name):
            std::string{};
        line(12,height-box_height+22,
            std::string(party?"RAVENLOFT PARTY / CODE.2 ENTRY":"RAVENLOFT 3D GPU")+
            actor_label+" | Level "+std::to_string(level)+
            " | "+std::string(mode)+" "+resolution+" internal | AA "+
            std::to_string(render_target.actual_samples())+"x"+
            (mouse_look?" | Mouse look":" | Q/E keyboard")+
            (blocked?" | BLOCKED":"")+(noclip?" | NOCLIP":""));
        line(12,height-box_height+44,
            "RES4 / LEVELS.DAT | F3: "+std::string(filter_mode==0U?"Nearest":filter_mode==1U?"Smooth":"Mipmap")+
            " | F1: "+std::string(show_help?"Hide":"Show")+" controls");
        if(show_help) {
            int y=height-box_height+69;
            constexpr int dy=23;
            line(12,y,"W/A/S/D move | Shift sprint | Q/E turn | Mouse: M/right-click to capture");y+=dy;
            line(12,y,"F2 resolution (Auto > 720p > 1080p > 1440p > 4K) | F3 filtering");y+=dy;
            line(12,y,"F4 AA (Off / 2x / 4x) | F11 fullscreen | F12 screenshot");y+=dy;
            line(12,y,party?
                "Tab automap | +/- zoom | F5 source placement markers | F8 map size":
                "Tab automap | +/- map zoom | F8 map size | N noclip diagnostic");y+=dy;
            line(12,y,party?
                "Level22 first-party state | R reset | Esc release mouse / close":
                "Page Up/Down original level | R reset | Esc release mouse / close");y+=dy;
            line(12,y,party?
                "SOURCE PLACEMENTS ARE DIAGNOSTIC DOTS / RES3 FRAMES, EVENTS PENDING":
                "SOURCE-BASED GEOMETRY / PROVISIONAL SCENERY & ACTOR RENDERING");
        }
        glMatrixMode(GL_PROJECTION);glPopMatrix();
        glMatrixMode(GL_MODELVIEW);glPopMatrix();
        glEnable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);
    }
    void save_screenshot() {
        if(!context || !dc)
            throw std::runtime_error("OpenGL screenshot requires an active context");
        wglMakeCurrent(dc,context);
        RECT rect{};GetClientRect(hwnd,&rect);
        const int w=std::max(1,int(rect.right)),h=std::max(1,int(rect.bottom));
        if(w>7680 || h>4320)
            throw std::runtime_error("GPU screenshot dimensions beyond safe limit");
        const std::size_t pitch=(std::size_t(w)*3U+3U)&~std::size_t(3U);
        std::vector<std::uint8_t> rgb(std::size_t(w)*h*3U);
        std::vector<std::uint8_t> bgr(pitch*std::size_t(h),0);
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadBuffer(GL_FRONT);
        glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,rgb.data());
        if(glGetError()!=GL_NO_ERROR)
            throw std::runtime_error("OpenGL screenshot readback failed");
        for(int y=0;y<h;++y)for(int x=0;x<w;++x){
            const auto src=(std::size_t(y)*w+unsigned(x))*3U;
            const auto dest=std::size_t(y)*pitch+std::size_t(x)*3U;
            bgr[dest]=rgb[src+2U];bgr[dest+1U]=rgb[src+1U];bgr[dest+2U]=rgb[src];
        }
        SYSTEMTIME when{};GetLocalTime(&when);
        wchar_t name[160]{};
        wsprintfW(name,L"RLOFT_GPU_L%02u_%04u%02u%02u_%02u%02u%02u.bmp",
            level,when.wYear,when.wMonth,when.wDay,
            when.wHour,when.wMinute,when.wSecond);
        const HANDLE file=CreateFileW(name,GENERIC_WRITE,0,nullptr,CREATE_NEW,
            FILE_ATTRIBUTE_NORMAL,nullptr);
        if(file==INVALID_HANDLE_VALUE)
            throw std::runtime_error("Cannot create screenshot BMP in current directory");
        BITMAPFILEHEADER head{};BITMAPINFOHEADER info{};
        head.bfType=0x4D42;
        head.bfOffBits=sizeof(head)+sizeof(info);
        head.bfSize=head.bfOffBits+DWORD(bgr.size());
        info.biSize=sizeof(info);info.biWidth=w;info.biHeight=h;
        info.biPlanes=1;info.biBitCount=24;info.biCompression=BI_RGB;
        info.biSizeImage=DWORD(bgr.size());
        DWORD wrote=0;
        const bool ok=
            WriteFile(file,&head,sizeof(head),&wrote,nullptr)&&wrote==sizeof(head)&&
            WriteFile(file,&info,sizeof(info),&wrote,nullptr)&&wrote==sizeof(info)&&
            WriteFile(file,bgr.data(),DWORD(bgr.size()),&wrote,nullptr)&&wrote==bgr.size();
        CloseHandle(file);
        if(!ok){DeleteFileW(name);throw std::runtime_error("Screenshot was not completely written");}
        MessageBoxW(hwnd,name,L"GPU screenshot saved",MB_OK|MB_ICONINFORMATION);
    }
    void paint() {
        if(!context || !dc)return;
        wglMakeCurrent(dc,context);
        RECT rect{};GetClientRect(hwnd,&rect);
        const int window_width=std::max(1,int(rect.right-rect.left));
        const int window_height=std::max(1,int(rect.bottom-rect.top));
        render_target.begin(window_width,window_height,resolution_index,aa_samples);
        const int width=render_target.width();
        const int height=render_target.height();
        // Missing original outdoor sky is a level-authored COLORS fallback.
        const auto& map=world.original().level(level);
        const unsigned ci=map.original_header[0x298U];
        const unsigned color_base=ci*3U;
        glClearColor(float(palette.rgb[color_base])/63.f,
                     float(palette.rgb[color_base+1U])/63.f,
                     float(palette.rgb[color_base+2U])/63.f,1.f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        // Authentic CODE.2 33E28/33DE0 level-authored environment
        // shading is visible behind geometry when header +0x05 is -1.
        // A -1 outdoor ceiling MUST NOT become a solid fake roof.
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
        glMatrixMode(GL_PROJECTION);glLoadIdentity();
        glOrtho(0,1,0,1,-1,1);
        glMatrixMode(GL_MODELVIEW);glLoadIdentity();
        const int pitch_px=std::clamp(
            int(std::lround(camera.pitch_radians*100.0)),-96,96);
        glBegin(GL_QUADS);
        for(unsigned row=0;row<256U;++row) {
            const auto ci=unsigned(backdrop.source_index(map,row,pitch_px));
            const unsigned at=ci*3U;
            glColor3f(float(palette.rgb[at])/63.f,
                      float(palette.rgb[at+1U])/63.f,
                      float(palette.rgb[at+2U])/63.f);
            const float high=1.f-float(row)/256.f;
            const float low=1.f-float(row+1U)/256.f;
            glVertex2f(0.f,low);glVertex2f(1.f,low);
            glVertex2f(1.f,high);glVertex2f(0.f,high);
        }
        glEnd();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1,1,1);
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
        render_target.present(window_width,window_height);
        automap_draw(window_width,window_height);
        draw_hud(window_width,window_height);
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
    const auto name=std::wstring(s.party?
        L"Ravenloft — Original CHARS Party / CODE.2 Level ":
        L"Ravenloft — Original 3D GPU Preview | Level ")+
        std::to_wstring(s.level)+L"  ["+
        std::to_wstring(unsigned(s.camera.column_position))+L","+
        std::to_wstring(unsigned(s.camera.row_position))+L"] | "+
        (s.mouse_look?L"MOUSE LOOK":L"Q/E KEYBOARD LOOK")+
        L" | F1 controls, F2 native-to-4K, F3 filter, F4 AA, F8 map size"+
        (s.blocked?L" | BLOCKED":L"");
    const auto label=s.party?(
        L" | "+std::wstring(s.party->actors[0].name.begin(),
                              s.party->actors[0].name.end())+
        L" / "+std::wstring(s.party->actors[1].name.begin(),
                            s.party->actors[1].name.end())
        ):std::wstring{};
    SetWindowTextW(hwnd,(name+label).c_str());
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
            s->initialize_level(
                s->party?s->party->level:0U);
            if(!SetTimer(hwnd,kTimer,16U,nullptr))
                throw std::runtime_error("GPU frame timer unavailable");
            title(hwnd,*s);
            return 0;
        } catch(const std::exception& error) {
            MessageBoxA(hwnd,error.what(),
                "GPU initialization failure",MB_OK|MB_ICONERROR);
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
            else if(wp==VK_F1 && !(lp&(1L<<30))) {
                s->show_help=!s->show_help;
            } else if(wp==VK_F2 && !(lp&(1L<<30))) {
                const unsigned new_mode=(s->resolution_index+1U)%
                    NativeGlViewportTarget::preset_count();
                RECT r{};GetClientRect(hwnd,&r);
                try {
                    s->render_target.begin(std::max(1,int(r.right)),
                        std::max(1,int(r.bottom)),new_mode,s->aa_samples);
                    s->resolution_index=new_mode;
                } catch(const std::exception& error) {
                    MessageBoxA(hwnd,error.what(),
                        "Internal render resolution not supported",
                        MB_OK|MB_ICONINFORMATION);
                }
            } else if(wp==VK_F3 && !(lp&(1L<<30))) {
                s->filter_mode=(s->filter_mode+1U)%3U;
                s->smooth_filter=s->filter_mode!=0U;
                s->update_filter();
            } else if(wp==VK_F4 && !(lp&(1L<<30))) {
                const unsigned samples=s->aa_samples==0U?2U:
                                        s->aa_samples==2U?4U:0U;
                RECT r{};GetClientRect(hwnd,&r);
                try {
                    s->render_target.begin(std::max(1,int(r.right)),
                        std::max(1,int(r.bottom)),s->resolution_index,samples);
                    s->aa_samples=samples;
                } catch(const std::exception& error) {
                    MessageBoxA(hwnd,error.what(),
                        "MSAA unavailable on this GPU",MB_OK|MB_ICONINFORMATION);
                }
            } else if(wp==VK_F12 && !(lp&(1L<<30))) {
                s->save_screenshot();
            } else if(wp==VK_F5 && s->party && !(lp&(1L<<30))) {
                s->show_authored_markers=!s->show_authored_markers;
            } else if(wp==VK_F8 && !(lp&(1L<<30))) {
                s->map_panel_size=s->map_panel_size==150U?210U:
                                  s->map_panel_size==210U?300U:150U;
            } else if(wp==VK_F11 && !(lp&(1L<<30))) {
                // Legacy viewer F11 changes the display WINDOW only;
                // F2 retains its independent original internal buffer.
                if(!s->fullscreen) {
                    s->saved_style=GetWindowLongPtrW(hwnd,GWL_STYLE);
                    s->saved_placement.length=sizeof(WINDOWPLACEMENT);
                    GetWindowPlacement(hwnd,&s->saved_placement);
                    MONITORINFO info{sizeof(MONITORINFO)};
                    if(GetMonitorInfoW(MonitorFromWindow(hwnd,MONITOR_DEFAULTTONEAREST),&info)) {
                        const auto& box=info.rcMonitor;
                        SetWindowLongPtrW(hwnd,GWL_STYLE,WS_POPUP|WS_VISIBLE);
                        SetWindowPos(hwnd,HWND_TOP,box.left,box.top,
                            box.right-box.left,box.bottom-box.top,
                            SWP_FRAMECHANGED|SWP_SHOWWINDOW);
                        s->fullscreen=true;
                    }
                } else {
                    SetWindowLongPtrW(hwnd,GWL_STYLE,s->saved_style);
                    SetWindowPlacement(hwnd,&s->saved_placement);
                    SetWindowPos(hwnd,nullptr,0,0,0,0,
                        SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
                    s->fullscreen=false;
                }
            } else if(wp=='N' && !s->party && !(lp&(1L<<30))) {
                s->noclip=!s->noclip;
            } else if(wp==VK_TAB && !(lp&(1L<<30)))s->automap=!s->automap;
            else if(wp==VK_OEM_PLUS || wp==VK_ADD)
                s->zoom=std::min(16U,s->zoom+1U);
            else if(wp==VK_OEM_MINUS || wp==VK_SUBTRACT)
                s->zoom=std::max(2U,s->zoom-1U);
            else if(wp==VK_PRIOR && !s->party) {
                s->initialize_level((s->level+1U)%32U);
                title(hwnd,*s);
            } else if(wp==VK_NEXT && !s->party) {
                s->initialize_level((s->level+31U)%32U);
                title(hwnd,*s);
            } else if(wp=='R') {
                s->initialize_level(s->level);title(hwnd,*s);
            } else if(wp==VK_ESCAPE) {
                if(s->mouse_look)mouse_mode(hwnd,*s,false);
                else DestroyWindow(hwnd);
            }
        } catch(const std::exception& error) {
            MessageBoxA(hwnd,error.what(),
                "GPU control/original resource failure",MB_OK|MB_ICONWARNING);
        }
        return 0;
    case WM_TIMER:
        if(wp==kTimer && GetFocus()==hwnd) {
            const auto now=GetTickCount64();
            const double dt=s->previous_time?
                std::clamp(double(now-s->previous_time)/1000.0,0.0,.06):.016;
            s->previous_time=now;
            try {
                s->step(dt);s->paint();
                // Windows SetWindowText can invalidate the nonclient area.
                // Never force an extra WM_PAINT for every 16-ms tick.
                if((now/500U)!=((now-static_cast<ULONGLONG>(dt*1000))/500U))
                    title(hwnd,*s);
            } catch(const std::exception& error) {
                KillTimer(hwnd,kTimer);
                if(s->mouse_look)mouse_mode(hwnd,*s,false);
                OutputDebugStringA(error.what());
                MessageBoxA(hwnd,error.what(),
                    "GPU runtime error (source game unchanged)",MB_OK|MB_ICONERROR);
            }
            return 0;
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        BeginPaint(hwnd,&ps);
        try{s->paint();}
        catch(const std::exception& error) {
            KillTimer(hwnd,kTimer);
            OutputDebugStringA(error.what());
            MessageBoxA(hwnd,error.what(),"GPU paint failure",MB_OK|MB_ICONERROR);
        }
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
HWND open_native_gpu_window(HWND owner,std::unique_ptr<State> state) {
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
    return window;
}

void open_native_gpu_world_preview(HWND owner,const NativeMedia& media) {
    open_native_gpu_window(owner,
        std::make_unique<State>(load_native_world_payload(media),media));
}
HWND open_native_gpu_world_session(
    HWND owner,const NativeMedia& media,const NativeCharsImage& chars) {
    // Import and validate the exact shared CODE.1 output before allocating
    // a graphics context or modifying any mutable world state.
    auto party=decode_native_party_start(chars);
    return open_native_gpu_window(owner,
        std::make_unique<State>(load_native_world_payload(media),
                                media,std::move(party)));
}
} // namespace rloft_native
#endif
