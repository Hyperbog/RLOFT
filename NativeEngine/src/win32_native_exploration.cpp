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
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
constexpr unsigned kWidth=320;
constexpr unsigned kRayWidth=256;
constexpr unsigned kHeight=400;
constexpr double kStep=0.18;
constexpr double kTurn=0.12;
constexpr double kPi=3.14159265358979323846;
constexpr double kMouseSensitivity=0.0025;

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
        candidate.field_of_view_radians=kPi/3;
        // Verify the new pose and framebuffer before changing the
        // currently displayed level; bad source files fail closed.
        const auto view=trace_native_probe_view(world,candidate,kRayWidth,256);
        const auto textured=source_textures->render(world,view,kWidth,kHeight);
        std::vector<std::uint8_t> staged=textured.bgra;
        world.select_level(level);
        camera=candidate;
        wall_hits=view.rays_that_hit;
        outside_rays=view.rays_that_left_archive;
        pixels_bgra=std::move(staged);
        last_source_texels=textured.original_texel_samples;
        last_step_blocked=false;
    }

    void redraw() {
        const auto view=trace_native_probe_view(world,camera,kRayWidth,256);
        const auto textured=source_textures->render(world,view,kWidth,kHeight);
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
        if(moved.moved)redraw();
    }
    void look(double mouse_dx,double mouse_dy) {
        camera.heading_radians+=mouse_dx*kMouseSensitivity;
        camera.heading_radians=std::remainder(camera.heading_radians,2*kPi);
        camera.pitch_radians=std::clamp(
            camera.pitch_radians-mouse_dy*kMouseSensitivity,-0.8,0.8);
        last_step_blocked=false;
        redraw();
    }
    void turn(double radians) {
        camera.heading_radians+=radians;
        if(camera.heading_radians>kPi)camera.heading_radians-=2*kPi;
        if(camera.heading_radians<-kPi)camera.heading_radians+=2*kPi;
        last_step_blocked=false;
        redraw();
    }
};

std::wstring label(const Preview& state) {
    const auto x=static_cast<unsigned>(state.camera.column_position);
    const auto y=static_cast<unsigned>(state.camera.row_position);
    return L"Original LEVELS.DAT level " + std::to_wstring(state.camera.level) +
           L" / 31  |  Cell (" + std::to_wstring(x) + L", " +
           std::to_wstring(y) + L")  |  Walls " +
           std::to_wstring(state.wall_hits) + L"/320  |  RES4 texels " +
           std::to_wstring(state.last_source_texels) +
           (state.mouse_look?L"  |  MOUSE LOOK ON":L"  |  KEYBOARD LOOK") +
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
    DrawTextW(dc,L"ORIGINAL CODE.2 ENVIRONMENT + WALL VGA / HOST CAMERA",
              -1,&warning,DT_SINGLELINE|DT_END_ELLIPSIS);

    const int available_width=std::max(1,int(bounds.right)-40);
    const int available_height=std::max(1,int(bounds.bottom)-175);
    const double scale=std::min(double(available_width)/kWidth,
                                double(available_height)/kHeight);
    const int w=std::max(1,int(kWidth*scale));
    const int h=std::max(1,int(kHeight*scale));
    const int left=(int(bounds.right)-w)/2;
    const int top=78+(available_height-h)/2;
    if(state.pixels_bgra.size()==std::size_t(kWidth)*kHeight*4U) {
        BITMAPINFO info{};
        info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth=kWidth;
        info.bmiHeader.biHeight=-static_cast<LONG>(kHeight);
        info.bmiHeader.biPlanes=1;
        info.bmiHeader.biBitCount=32;
        info.bmiHeader.biCompression=BI_RGB;
        SetStretchBltMode(dc,COLORONCOLOR);
        StretchDIBits(dc,left,top,w,h,0,0,kWidth,kHeight,
                      state.pixels_bgra.data(),&info,DIB_RGB_COLORS,SRCCOPY);
    }
    SetTextColor(dc,RGB(219,229,239));
    const auto info=label(state);
    RECT status{20,bounds.bottom-92,bounds.right-20,bounds.bottom-65};
    DrawTextW(dc,info.c_str(),-1,&status,DT_SINGLELINE|DT_END_ELLIPSIS);
    SetTextColor(dc,RGB(166,185,207));
    RECT controls{20,bounds.bottom-65,bounds.right-20,bounds.bottom-12};
    DrawTextW(dc,
      L"W/S = forward/back    A/D = strafe    Q/E = turn left/right\n"
      L"M = optional mouse-look (yaw + pitch)    PageUp/Down = level\n"
      L"R = respawn    Esc = close    Read-only source; host camera not DOS.",
      -1,&controls,DT_WORDBREAK);
}

// Raw Windows mouse deltas (no pointer recentering or acceleration).
// Capture is OPTIONAL and OFF by default. Native Q/E remains available.
void set_mouse_look(HWND hwnd,Preview& state,bool enabled) {
    if(state.mouse_look==enabled)return;
    RAWINPUTDEVICE device{};
    device.usUsagePage=0x01;
    device.usUsage=0x02; // Generic desktop mouse
    device.dwFlags=enabled?0U:RIDEV_REMOVE;
    device.hwndTarget=enabled?hwnd:nullptr;
    if(!RegisterRawInputDevices(&device,1U,sizeof(device)))
        throw std::runtime_error("unable to toggle Windows raw mouse input");
    state.mouse_look=enabled;
    if(enabled) {
        SetCapture(hwnd);
        ShowCursor(FALSE);
    } else {
        if(GetCapture()==hwnd)ReleaseCapture();
        ShowCursor(TRUE);
        state.camera.pitch_radians=0.0; // restore native keyboard-only horizon
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
    case WM_INPUT:
        if(state->mouse_look && GetForegroundWindow()==hwnd) {
            UINT bytes=0;
            if(GetRawInputData(reinterpret_cast<HRAWINPUT>(lp),
                RID_INPUT,nullptr,&bytes,sizeof(RAWINPUTHEADER))==0U &&
                bytes>=sizeof(RAWINPUT) && bytes<=4096U) {
                std::vector<std::uint8_t> buffer(bytes);
                if(GetRawInputData(reinterpret_cast<HRAWINPUT>(lp),
                    RID_INPUT,buffer.data(),&bytes,sizeof(RAWINPUTHEADER))==bytes) {
                    const auto* input=reinterpret_cast<const RAWINPUT*>(buffer.data());
                    if(input->header.dwType==RIM_TYPEMOUSE &&
                       !(input->data.mouse.usFlags&MOUSE_MOVE_ABSOLUTE)) {
                        const auto dx=input->data.mouse.lLastX;
                        const auto dy=input->data.mouse.lLastY;
                        if(dx||dy) {
                            try {
                                state->look(double(dx),double(dy));
                                InvalidateRect(hwnd,nullptr,FALSE);
                            } catch(const std::exception&) {
                                set_mouse_look(hwnd,*state,false);
                            }
                        }
                    }
                }
            }
        }
        return DefWindowProcW(hwnd,msg,wp,lp);
    case WM_KILLFOCUS:
        if(state->mouse_look)set_mouse_look(hwnd,*state,false);
        InvalidateRect(hwnd,nullptr,FALSE);
        return 0;
    case WM_KEYDOWN:
        try {
            bool consumed=true;
            switch(wp) {
            case 'W':case VK_UP:state->move(kStep,0);break;
            case 'S':case VK_DOWN:state->move(-kStep,0);break;
            case 'A':state->move(0,-kStep);break;
            case 'D':state->move(0,kStep);break;
            case 'Q':state->turn(-kTurn);break;
            case 'E':state->turn(kTurn);break;
            case 'M':set_mouse_look(hwnd,*state,!state->mouse_look);break;
            case VK_PRIOR:
                state->choose_level((state->camera.level+1U)%32U);break;
            case VK_NEXT:
                state->choose_level((state->camera.level+31U)%32U);break;
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
    ShowWindow(window,SW_SHOW);
    UpdateWindow(window);
}
} // namespace rloft_native
#endif
