#ifdef _WIN32
#define NOMINMAX
#include <windows.h>

#include "rloft_native/native_win32_exploration.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_scene_view.hpp"
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
constexpr unsigned kHeight=200;
constexpr double kStep=0.18;
constexpr double kTurn=0.12;
constexpr double kPi=3.14159265358979323846;

struct Preview {
    NativeWorldRuntime world;
    NativeProbeCamera camera;
    std::vector<std::uint8_t> pixels_bgra;
    std::size_t wall_hits{};
    std::size_t outside_rays{};
    bool last_step_blocked{};

    explicit Preview(NativeWorldPayload original)
        :world(std::move(original)) {
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
        const auto view=trace_native_probe_view(world,candidate,kWidth,256);
        std::vector<std::uint8_t> staged=convert(view);
        world.select_level(level);
        camera=candidate;
        wall_hits=view.rays_that_hit;
        outside_rays=view.rays_that_left_archive;
        pixels_bgra=std::move(staged);
        last_step_blocked=false;
    }

    static std::vector<std::uint8_t> convert(const NativeProbeView& view) {
        const auto rgb=render_native_probe_rgb(view,kWidth,kHeight);
        if(rgb.size()!=std::size_t(kWidth)*kHeight*3U)
            throw std::runtime_error("native preview source-frame size mismatch");
        std::vector<std::uint8_t> bgra(std::size_t(kWidth)*kHeight*4U);
        for(std::size_t i=0;i<std::size_t(kWidth)*kHeight;++i) {
            bgra[4U*i]=rgb[3U*i+2U];
            bgra[4U*i+1U]=rgb[3U*i+1U];
            bgra[4U*i+2U]=rgb[3U*i];
            bgra[4U*i+3U]=255U;
        }
        return bgra;
    }

    void redraw() {
        const auto view=trace_native_probe_view(world,camera,kWidth,256);
        auto staged=convert(view);
        pixels_bgra=std::move(staged);
        wall_hits=view.rays_that_hit;
        outside_rays=view.rays_that_left_archive;
    }
    void move(double forward,double strafe) {
        // This moves through original passability data with a bounded
        // host sweep. Movement physics/actor footprint are NOT original.
        const double a=camera.heading_radians;
        const auto dx=forward*std::cos(a)-strafe*std::sin(a);
        const auto dy=forward*std::sin(a)+strafe*std::cos(a);
        last_step_blocked=!move_native_probe_camera(world,camera,dx,dy);
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
           std::to_wstring(state.wall_hits) + L"/320" +
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
    DrawTextW(dc,L"PROVISIONAL HOST RAYCAST / SYNTHETIC COLORS — NOT DOS GRAPHICS",
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
      L"W/S or Up/Down = step    A/D = strafe    Left/Right = turn\n"
      L"PageUp/PageDown = other original level    R = preview respawn    Esc = close\n"
      L"Read-only archive. No party, animation, events, combat or save writes.",
      -1,&controls,DT_WORDBREAK);
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
    case WM_KEYDOWN:
        try {
            bool consumed=true;
            switch(wp) {
            case 'W':case VK_UP:state->move(kStep,0);break;
            case 'S':case VK_DOWN:state->move(-kStep,0);break;
            case 'A':state->move(0,-kStep);break;
            case 'D':state->move(0,kStep);break;
            case VK_LEFT:state->turn(-kTurn);break;
            case VK_RIGHT:state->turn(kTurn);break;
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
        load_native_world_payload(original_media));
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
