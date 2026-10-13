#ifdef _WIN32
#define NOMINMAX
#include <windows.h>

#include "rloft_native/native_win32_exploration.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_scene_view.hpp"
#include "rloft_native/native_probe_original_textures.hpp"
#include "rloft_native/native_world_payload.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
constexpr unsigned kWidth=320;
constexpr unsigned kRayWidth=256;
constexpr unsigned kHeight=400;
constexpr double kDosPixelAspect=2.0; // 320x400 indexed VGA shown as 640x400

constexpr double kPi=3.14159265358979323846;
constexpr double kMouseSensitivity=0.0025;
constexpr UINT_PTR kFrameTimer=1;
constexpr double kMoveSpeed=1.8;
constexpr double kTurnSpeed=1.8;

struct Preview {
    NativeWorldRuntime world;
    NativeProbeCamera camera;
    std::vector<std::uint8_t> pixels_bgra;
    std::unique_ptr<NativeProbeOriginalTextures> source_textures;
    unsigned last_source_texels{};
    std::size_t wall_hits{};
    std::size_t outside_rays{};
    bool last_step_blocked{};
    bool mouse_look{};
    bool show_full_frame{};
    bool smooth_filter{};
    bool show_automap{};
    unsigned automap_zoom{6U};
    std::map<unsigned,std::vector<std::uint8_t>> revealed;
    ULONGLONG previous_tick_ms{};

    explicit Preview(NativeWorldPayload original,const NativeMedia& media)
        :world(std::move(original)),
         source_textures(std::make_unique<NativeProbeOriginalTextures>(media)) {
        choose_level(0U);
    }

    void choose_level(unsigned level) {
        if(level>=world.original().levels.size())
            throw std::out_of_range("world preview level outside original archive");
        const auto& map=world.original().level(level);
        bool found=false;
        unsigned best_x=0,best_y=0,best_neighbours=0;
        std::uint64_t best_center=std::numeric_limits<std::uint64_t>::max();
        // Explicit preview spawn, NOT the original game's saved party
        // position. All 32 original maps can be inspected independently.
        for(unsigned y=0;y<map.height;++y) {
            for(unsigned x=0;x<map.width;++x) {
                if(world.normal_collision(level,x,y)!=NativeCollision::Passable)
                    continue;
                unsigned n=0;
                if(x>0 && world.normal_collision(level,x-1,y)==NativeCollision::Passable)++n;
                if(y>0 && world.normal_collision(level,x,y-1)==NativeCollision::Passable)++n;
                if(x+1<map.width && world.normal_collision(level,x+1,y)==NativeCollision::Passable)++n;
                if(y+1<map.height && world.normal_collision(level,x,y+1)==NativeCollision::Passable)++n;
                const auto dx=std::int64_t(x)*2+1-std::int64_t(map.width);
                const auto dy=std::int64_t(y)*2+1-std::int64_t(map.height);
                const auto center=static_cast<std::uint64_t>(dx*dx+dy*dy);
                if(!found || n>best_neighbours ||
                   (n==best_neighbours && center<best_center)) {
                    found=true;best_x=x;best_y=y;
                    best_neighbours=n;best_center=center;
                }
            }
        }
        if(!found)
            throw std::runtime_error("selected original level has no passable preview cell");
        NativeProbeCamera candidate{};
        candidate.level=level;
        candidate.column_position=double(best_x)+0.5;
        candidate.row_position=double(best_y)+0.5;
        candidate.heading_radians=-kPi/2;
        candidate.field_of_view_radians=kPi/2; // host physical-aspect horizontal FOV
        // Verify the new pose and framebuffer before changing the
        // currently displayed level; bad source files fail closed.
        const auto view=trace_native_probe_view(world,candidate,kRayWidth,256);
        const auto textured=source_textures->render(world,view,kWidth,kHeight);
        note_visibility(view);
        std::vector<std::uint8_t> staged=textured.bgra;
        world.select_level(level);
        camera=candidate;
        wall_hits=view.rays_that_hit;
        outside_rays=view.rays_that_left_archive;
        pixels_bgra=std::move(staged);
        last_source_texels=textured.original_texel_samples;
        last_step_blocked=false;
    }

    void note_visibility(const NativeProbeView& view) {
        // v0.7-style session discovery, but reuses the already traced
        // native original-cell rays: no second expensive 192-ray scan.
        const auto& map=world.original().level(view.camera.level);
        auto& cells=revealed[view.camera.level];
        if(cells.size()!=std::size_t(map.width)*map.height)
            cells.assign(std::size_t(map.width)*map.height,0U);
        const auto mark=[&](unsigned x,unsigned y) {
            if(x<map.width && y<map.height)
                cells[std::size_t(y)*map.width+x]=1U;
        };
        mark(unsigned(view.camera.column_position),
             unsigned(view.camera.row_position));
        for(const auto& ray:view.columns)
            for(const auto& pos:ray.reached)
                mark(pos.first,pos.second);
    }
    void redraw() {
        const auto view=trace_native_probe_view(world,camera,kRayWidth,256);
        const auto textured=source_textures->render(world,view,kWidth,kHeight);
        note_visibility(view);
        pixels_bgra=textured.bgra;
        last_source_texels=textured.original_texel_samples;
        wall_hits=view.rays_that_hit;
        outside_rays=view.rays_that_left_archive;
    }
    void move(double forward,double strafe) {
        // This moves through original passability data with a bounded
        // host sweep. Movement physics/actor footprint are NOT original.
        const double a=camera.heading_radians;
        const auto dx=forward*std::cos(a)-strafe*std::sin(a);
        const auto dy=forward*std::sin(a)+strafe*std::cos(a);
        const auto moved=move_native_probe_camera_clearance(
            world,camera,dx,dy);
        last_step_blocked=moved.blocked;
        // When the camera is in contact with a wall, do not redraw
        // a fake advancing view: unchanged position -> unchanged pixels.
        // Paint once on WM_TIMER instead of rendering on every OS
        // key repeat or mouse message.
        (void)moved;
    }
    void look(double mouse_dx,double mouse_dy) {
        camera.heading_radians+=mouse_dx*kMouseSensitivity;
        camera.heading_radians=std::remainder(camera.heading_radians,2*kPi);
        camera.pitch_radians=std::clamp(
            camera.pitch_radians-mouse_dy*kMouseSensitivity,-0.8,0.8);
        last_step_blocked=false;
    }
    void turn(double radians) {
        camera.heading_radians+=radians;
        if(camera.heading_radians>kPi)camera.heading_radians-=2*kPi;
        if(camera.heading_radians<-kPi)camera.heading_radians+=2*kPi;
        last_step_blocked=false;
    }
};

std::wstring label(const Preview& state) {
    const auto x=static_cast<unsigned>(state.camera.column_position);
    const auto y=static_cast<unsigned>(state.camera.row_position);
    return L"Original LEVELS.DAT level " + std::to_wstring(state.camera.level) +
           L" / 31  |  Cell (" + std::to_wstring(x) + L", " +
           std::to_wstring(y) + L")  |  Walls " +
           std::to_wstring(state.wall_hits) + L"/256  |  RES4 wall texels " +
           std::to_wstring(state.last_source_texels) +
           (state.mouse_look?L"  |  MOUSE LOOK ON":L"  |  KEYBOARD LOOK") +
           (state.show_full_frame?L"  |  FULL DOS FRAME":L"  |  VIEW ONLY") +
           (state.show_automap?L"  |  AUTOMAP ON":L"") +
           (state.last_step_blocked?L"  |  BLOCKED":L"");
}

void paint(HWND window,const Preview& state,HDC dc) {
    RECT bounds{};
    GetClientRect(window,&bounds);
    HBRUSH black=static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    FillRect(dc,&bounds,black);
    SetBkMode(dc,TRANSPARENT);
    SetTextColor(dc,RGB(236,241,248));
    RECT title{20,14,bounds.right-20,39};
    DrawTextW(dc,L"RAVENLOFT — ORIGINAL WORLD INTERACTIVE PREVIEW",
              -1,&title,DT_SINGLELINE|DT_END_ELLIPSIS);

    SetTextColor(dc,RGB(245,189,103));
    RECT warning{20,41,bounds.right-20,68};
    DrawTextW(dc,L"ORIGINAL RES4 PIXELS | HOST WALL/FLOOR PROJECTION (NOT DOS PARITY)",
              -1,&warning,DT_SINGLELINE|DT_END_ELLIPSIS);

    const int available_width=std::max(1,int(bounds.right)-40);
    const int available_height=std::max(1,int(bounds.bottom)-175);
    // CODE.2 320x400 VGA is displayed in a 640x400 DOS-shaped
    // presentation: source pixels are twice as wide. Alpha 0.8 used
    // 320x400 square host pixels, distorting the original view.
    const int source_w=state.show_full_frame?int(kWidth):256;
    const int source_h=state.show_full_frame?int(kHeight):256;
    const int source_x=state.show_full_frame?0:32;
    const int source_y=state.show_full_frame?0:4;
    const double scale=std::min(double(available_width)/(source_w*kDosPixelAspect),
                                double(available_height)/source_h);
    const int w=std::max(1,int(source_w*kDosPixelAspect*scale));
    const int h=std::max(1,int(source_h*scale));
    const int left=(int(bounds.right)-w)/2;
    const int top=78+(available_height-h)/2;
    if(state.pixels_bgra.size()==std::size_t(kWidth)*kHeight*4U) {
        BITMAPINFO info{};
        info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        // GDI StretchDIBits cropping from a top-down 320x400 DIB
        // varies across drivers (the reported screenshot displayed
        // only a thin strip with the remaining viewport black).
        // Explicitly materialize the exact authored viewport into
        // a tightly packed top-down DIB before applying its aspect.
        std::vector<std::uint8_t> cropped(
            std::size_t(source_w)*source_h*4U);
        for(int y=0;y<source_h;++y) {
            const auto src=(std::size_t(y+source_y)*kWidth+
                            unsigned(source_x))*4U;
            const auto dst=std::size_t(y)*unsigned(source_w)*4U;
            std::copy_n(state.pixels_bgra.begin()+src,
                        std::size_t(source_w)*4U,
                        cropped.begin()+dst);
        }
        info.bmiHeader.biWidth=source_w;
        info.bmiHeader.biHeight=-static_cast<LONG>(source_h);
        info.bmiHeader.biPlanes=1;
        info.bmiHeader.biBitCount=32;
        info.bmiHeader.biCompression=BI_RGB;
        SetStretchBltMode(dc,state.smooth_filter?HALFTONE:COLORONCOLOR);
        if(state.smooth_filter)SetBrushOrgEx(dc,0,0,nullptr);
        StretchDIBits(dc,left,top,w,h,0,0,source_w,source_h,
                      cropped.data(),&info,DIB_RGB_COLORS,SRCCOPY);
    }
    if(state.show_automap) {
        // Read-only native exploration overlay inspired by v0.7.
        // Grid positions are exact original LEVELS.DAT cells, while
        // revealed cells are session-only HOST camera visibility.
        const auto& map=state.world.original().level(state.camera.level);
        const auto found=state.revealed.find(state.camera.level);
        const int size=178;
        const int margin=10;
        const int mx=std::max(0,int(bounds.right)-size-margin);
        const int my=84;
        const RECT backing{mx,my,mx+size,my+size};
        HBRUSH bg=CreateSolidBrush(RGB(15,21,30));
        HBRUSH wall=CreateSolidBrush(RGB(140,146,156));
        HBRUSH empty=CreateSolidBrush(RGB(52,68,72));
        HBRUSH player=CreateSolidBrush(RGB(248,201,76));
        FillRect(dc,&backing,bg);
        const int z=int(state.automap_zoom);
        const int cx=int(state.camera.column_position);
        const int cy=int(state.camera.row_position);
        const int tile_offset=size/2;
        if(found!=state.revealed.end()) {
            for(int gy=std::max(0,cy-size/(2*z)-1);
                gy<std::min(int(map.height),cy+size/(2*z)+2);++gy) {
                for(int gx=std::max(0,cx-size/(2*z)-1);
                    gx<std::min(int(map.width),cx+size/(2*z)+2);++gx) {
                    if(!found->second[std::size_t(gy)*map.width+gx])
                        continue;
                    const int px=mx+tile_offset+(gx-cx)*z;
                    const int py=my+tile_offset+(gy-cy)*z;
                    RECT r{std::max(mx,px),std::max(my,py),
                           std::min(mx+size,px+z),std::min(my+size,py+z)};
                    if(r.right<=r.left||r.bottom<=r.top)continue;
                    FillRect(dc,&r,
                        state.world.normal_collision(state.camera.level,
                            unsigned(gx),unsigned(gy))==
                            NativeCollision::Passable?empty:wall);
                }
            }
        }
        RECT marker{mx+tile_offset-3,my+tile_offset-3,
                    mx+tile_offset+4,my+tile_offset+4};
        FillRect(dc,&marker,player);
        HPEN heading=CreatePen(PS_SOLID,2,RGB(251,209,72));
        const auto previous=SelectObject(dc,heading);
        MoveToEx(dc,mx+tile_offset,my+tile_offset,nullptr);
        LineTo(dc,mx+tile_offset+int(12*std::cos(state.camera.heading_radians)),
                my+tile_offset+int(12*std::sin(state.camera.heading_radians)));
        SelectObject(dc,previous);
        DeleteObject(heading);
        SetTextColor(dc,RGB(240,227,191));
        RECT map_label{mx+6,my+4,mx+size-6,my+22};
        DrawTextW(dc,L"EXPLORATION MAP  [+ / -]",-1,&map_label,
                  DT_SINGLELINE|DT_END_ELLIPSIS);
        DeleteObject(bg);DeleteObject(wall);
        DeleteObject(empty);DeleteObject(player);
    }
    SetTextColor(dc,RGB(219,229,239));
    const auto info=label(state);
    RECT status{20,bounds.bottom-92,bounds.right-20,bounds.bottom-65};
    DrawTextW(dc,info.c_str(),-1,&status,DT_SINGLELINE|DT_END_ELLIPSIS);
    SetTextColor(dc,RGB(166,185,207));
    RECT controls{20,bounds.bottom-65,bounds.right-20,bounds.bottom-12};
    DrawTextW(dc,
      L"W/S = forward/back    A/D = strafe    Q/E = turn left/right\n"
      L"M/right-click = look  Tab = automap  +/- = map zoom  N = filter\n"
      L"R = respawn    Esc = close    Original media read-only.",
      -1,&controls,DT_WORDBREAK);
}

// Explicit opt-in mouse capture. WM_MOUSEMOVE relative-to-client-center
// works consistently with both physical and virtual mouse devices,
// including remoting where WM_INPUT is not delivered. No game input
// fires unless this window owns focus and the user enabled mouse look.
POINT mouse_anchor(HWND hwnd) {
    RECT rc{};
    GetClientRect(hwnd,&rc);
    POINT pt{(rc.left+rc.right)/2,(rc.top+rc.bottom)/2};
    return pt;
}
bool warp_mouse_center(HWND hwnd) {
    POINT center=mouse_anchor(hwnd);
    return ClientToScreen(hwnd,&center) &&
           SetCursorPos(center.x,center.y);
}
void set_mouse_look(HWND hwnd,Preview& state,bool enabled) {
    if(state.mouse_look==enabled)return;
    if(enabled) {
        SetFocus(hwnd);
        SetForegroundWindow(hwnd);
        if(!warp_mouse_center(hwnd))
            throw std::runtime_error("unable to center mouse for free-look");
        state.mouse_look=true;
        SetCapture(hwnd);
        ShowCursor(FALSE);
    } else {
        state.mouse_look=false;
        if(GetCapture()==hwnd)ReleaseCapture();
        ShowCursor(TRUE);
        state.camera.pitch_radians=0.0;
        state.redraw();
    }
}

LRESULT CALLBACK preview_proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    auto* state=reinterpret_cast<Preview*>(GetWindowLongPtrW(hwnd,GWLP_USERDATA));
    if(msg==WM_NCCREATE) {
        const auto* cs=reinterpret_cast<const CREATESTRUCTW*>(lp);
        state=static_cast<Preview*>(cs->lpCreateParams);
        if(!state)return FALSE;
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(state));
        return TRUE;
    }
    if(!state)return DefWindowProcW(hwnd,msg,wp,lp);
    switch(msg) {
    case WM_ERASEBKGND:
        return 1; // Full viewport is redrawn on WM_PAINT.
    case WM_SIZE:
        InvalidateRect(hwnd,nullptr,FALSE);
        return 0;
    case WM_MOUSEMOVE:
        // Intentionally never recenter or redraw in WM_MOUSEMOVE.
        // Cursor deltas are sampled/coalesced once per simulation tick;
        // this avoids a SetCursorPos -> WM_MOUSEMOVE feedback loop and
        // dozens of heavy original-VGA redraws per physical movement.
        return 0;
    case WM_TIMER:
        if(wp==kFrameTimer && GetFocus()==hwnd) {
            try {
                const auto now=GetTickCount64();
                const double dt=state->previous_tick_ms?
                    std::clamp(double(now-state->previous_tick_ms)/1000.0,
                               0.0,0.06):0.016;
                state->previous_tick_ms=now;
                bool changed=false;
                if(state->mouse_look) {
                    POINT current{};
                    const auto center=mouse_anchor(hwnd);
                    POINT screen=center;
                    if(ClientToScreen(hwnd,&screen) && GetCursorPos(&current)) {
                        const int dx=current.x-screen.x;
                        const int dy=current.y-screen.y;
                        if(dx||dy) {
                            // Sample and recenter exactly ONCE per tick,
                            // equivalent to SDL/pygame mouse.get_rel().
                            SetCursorPos(screen.x,screen.y);
                            state->look(std::clamp(double(dx),-160.0,160.0),
                                        std::clamp(double(dy),-160.0,160.0));
                            changed=true;
                        }
                    }
                }
                // Real-time SDL-style movement replaces OS WM_KEYDOWN
                // autorepeat, preserving constant speed across PCs.
                const auto down=[](int key) {
                    return (GetAsyncKeyState(key)&0x8000)!=0;
                };
                const auto turn=int(down('E'))-int(down('Q'));
                if(turn) {
                    state->turn(double(turn)*kTurnSpeed*dt);
                    changed=true;
                }
                const auto forward=int(down('W'))-int(down('S'));
                const auto strafe=int(down('D'))-int(down('A'));
                if(forward||strafe) {
                    const double norm=std::max(1.0,std::hypot(
                        double(forward),double(strafe)));
                    const auto before_x=state->camera.column_position;
                    const auto before_y=state->camera.row_position;
                    state->move(kMoveSpeed*dt*forward/norm,
                                kMoveSpeed*dt*strafe/norm);
                    changed=changed ||
                        state->camera.column_position!=before_x ||
                        state->camera.row_position!=before_y;
                }
                if(changed) {
                    state->redraw();
                    InvalidateRect(hwnd,nullptr,FALSE);
                } else if(state->last_step_blocked) {
                    // Collision status refresh without falsely advancing
                    // the rendered camera projection.
                    InvalidateRect(hwnd,nullptr,FALSE);
                }
            } catch(const std::exception&) {
                if(state->mouse_look)set_mouse_look(hwnd,*state,false);
            }
            return 0;
        }
        break;
    case WM_LBUTTONDOWN:
        SetFocus(hwnd);
        return 0;
    case WM_RBUTTONDOWN:
        SetFocus(hwnd);
        try {set_mouse_look(hwnd,*state,!state->mouse_look);}
        catch(const std::exception&) {
            MessageBoxW(hwnd,L"Mouse capture is unavailable.",
                L"Optional mouse-look",MB_OK|MB_ICONWARNING);
        }
        InvalidateRect(hwnd,nullptr,FALSE);
        return 0;
    case WM_KILLFOCUS:
        state->previous_tick_ms=0;
        if(state->mouse_look) {
            set_mouse_look(hwnd,*state,false);
            InvalidateRect(hwnd,nullptr,FALSE);
        }
        return 0;
    case WM_KEYDOWN:
        try {
            bool consumed=true;
            switch(wp) {
            case 'W':case 'S':case 'A':case 'D':
            case 'Q':case 'E':
                // Held keys are sampled at uniform WM_TIMER intervals.
                break;
            case 'M':
                // Ignore Windows key-repeat; otherwise holding M can
                // immediately undo the opt-in capture.
                if((lp & (1L<<30))==0)
                    set_mouse_look(hwnd,*state,!state->mouse_look);
                break;
            case VK_PRIOR:
                state->choose_level((state->camera.level+1U)%32U);break;
            case VK_NEXT:
                state->choose_level((state->camera.level+31U)%32U);break;
            case 'V':state->show_full_frame=!state->show_full_frame;break;
            case 'N':state->smooth_filter=!state->smooth_filter;break;
            case VK_TAB:state->show_automap=!state->show_automap;break;
            case VK_OEM_PLUS:case VK_ADD:
                state->automap_zoom=std::min(16U,state->automap_zoom+1U);break;
            case VK_OEM_MINUS:case VK_SUBTRACT:
                state->automap_zoom=std::max(2U,state->automap_zoom-1U);break;
            case 'R':state->choose_level(state->camera.level);break;
            case VK_ESCAPE:DestroyWindow(hwnd);return 0;
            default:consumed=false;break;
            }
            if(consumed) {
                InvalidateRect(hwnd,nullptr,FALSE);
                return 0;
            }
        } catch(const std::exception&) {
            MessageBoxW(hwnd,
                L"World preview could not apply the requested action.\n"
                L"Original installed files have not been changed.",
                L"Read-only native preview error",MB_OK|MB_ICONWARNING);
            return 0;
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        auto dc=BeginPaint(hwnd,&ps);
        paint(hwnd,*state,dc);
        EndPaint(hwnd,&ps);
        return 0;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_NCDESTROY:
        KillTimer(hwnd,kFrameTimer);
        if(state->mouse_look)set_mouse_look(hwnd,*state,false);
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,0);
        delete state; // ownership transferred only after successful CreateWindow
        return DefWindowProcW(hwnd,msg,wp,lp);
    default:break;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
} // namespace

void open_native_world_preview(HWND owner,const NativeMedia& original_media) {
    // No image extraction, save creation or writing into the original
    // installation. The view owns an immutable-source-derived world copy.
    auto session=std::make_unique<Preview>(
        load_native_world_payload(original_media),original_media);
    const HINSTANCE instance=GetModuleHandleW(nullptr);
    WNDCLASSW klass{};
    klass.lpfnWndProc=preview_proc;
    klass.hInstance=instance;
    klass.hCursor=LoadCursorW(nullptr,IDC_ARROW);
    klass.lpszClassName=L"RloftNativeOriginalWorldPreview";
    if(!RegisterClassW(&klass) && GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)
        throw std::runtime_error("unable to register native world preview");
    HWND window=CreateWindowExW(0,klass.lpszClassName,
        L"Ravenloft: Original LEVELS.DAT — Interactive World Preview (Not Gameplay)",
        WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,1024,750,
        owner,nullptr,instance,session.get());
    if(!window)
        throw std::runtime_error("unable to open native world preview");
    session.release();
    if(!SetTimer(window,kFrameTimer,16U,nullptr)) {
        DestroyWindow(window);
        throw std::runtime_error("native preview timer setup failed");
    }
    ShowWindow(window,SW_SHOW);
    UpdateWindow(window);
}
} // namespace rloft_native
#endif
