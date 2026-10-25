// Experimental native Level 22 player window.
// The only world renderer and movement engine are the ORIGINAL reconstructed
// CODE.2 functions, executing on the retained original guest session.
// No original commercial media, palettes or frames are embedded in this exe.
// Windows-only host presentation and translated key sampling are not DOS IRQ9
// nor independently synchronized retail-DOS indexed VGA parity.
#ifndef _WIN32
#error Native Level22 window requires Windows
#endif
#define RLOFT_BUNDLE_NO_MAIN
#include "code2_original_res_startup.cpp"
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

namespace {
namespace source=rloft::original_code2_session;
namespace machine=rloft::canonical_code2_runtime;
using Owner=std::unique_ptr<source::Opaque,void(*)(source::Opaque*)>;
static constexpr UINT kFrameTimer=1u;
static constexpr unsigned kGameWidth=320u,kGameHeight=400u;

struct LiveWindow final {
    Owner game{nullptr,&source::destroy};
    HWND hwnd=nullptr;
    std::vector<std::uint32_t> scanout_rgba;
    unsigned ticks=0u;
    unsigned pose_changes=0u;
    unsigned source_displacements=0u;
    unsigned source_rotations=0u;
    bool failed=false;
    std::string error;
    bool keys[6]{};

    static void enable_experimental_source() {
        for(const char* name:{
           "RLOFT_RECOMP_EXPERIMENTAL_HUD",
           "RLOFT_RECOMP_EXPERIMENTAL_EXTERIOR",
           "RLOFT_RECOMP_EXPERIMENTAL_FRAME_ENTRY",
           "RLOFT_RECOMP_EXPERIMENTAL_FULL_COMPOSITOR",
           "RLOFT_RECOMP_EXPERIMENTAL_POLYGON_FRONTIER",
           "RLOFT_RECOMP_EXPERIMENTAL_POLYGON_SOURCE",
           "RLOFT_RECOMP_EXPERIMENTAL_1FC14_PAGE_HANDOFF",
           "RLOFT_RECOMP_EXPERIMENTAL_1FC14_SCENE_SOURCE",
           "RLOFT_RECOMP_EXPERIMENTAL_1FC14_MOVEMENT_SOURCE"}) {
            if(!SetEnvironmentVariableA(name,"1"))
                throw std::runtime_error(std::string("Failed to enable original-source mode: ")+name);
        }
        // Do not opt into digital SFX automatically. Its native driver
        // acceptance boundary remains distinct from rendering/playback.
    }
    void open_original_guest(const std::filesystem::path& root,
                             const std::filesystem::path& cd) {
        enable_experimental_source();
        game.reset(source::create(root,cd,5u));
        // The source layer itself requires 11 authentic RES archives,
        // INIT/INFO/NPCS/OBJECTS, original DOS mouse callbacks and
        // the unmodified original 290F4 -> 29930 video mode.
        source::dispatch_pending(game.get());
        if(source::test_advance_through_original_video_mode(game.get())!=10u ||
           source::test_advance_past_original_second_init(game.get())!=11u ||
           machine::pending_host_service(game->canonical.get())!=11u)
            throw std::runtime_error("Original CODE.2 did not reach live Level22 30B40 input");
        if(!game->guest || !game->guest->m.data ||
           game->guest->memory.data()!=game->guest->m.data)
            throw std::runtime_error("Original CODE.2 guest memory not retained");
        capture_source_vga();
    }
    const std::uint8_t* guest_bytes() const {
        return game->guest->memory.data();
    }
    static std::uint32_t key_scan(int vkey) noexcept {
        switch(vkey){
        case 'W':case VK_UP:return 0x148u;
        case 'S':case VK_DOWN:return 0x150u;
        case 'A':return 0x14Bu;
        case 'D':return 0x14Du;
        case 'J':case VK_LEFT:return 0x147u;
        case 'L':case VK_RIGHT:return 0x149u;
        default:return 0u;
        }
    }
    static unsigned key_slot(WPARAM key) noexcept {
        switch(key) {
        case 'W':case VK_UP:return 0u;
        case 'S':case VK_DOWN:return 1u;
        case 'A':return 2u;
        case 'D':return 3u;
        case 'J':case VK_LEFT:return 4u;
        case 'L':case VK_RIGHT:return 5u;
        default:return 6u;
        }
    }
    std::uint32_t held_scan() const noexcept {
        // Deterministic priority. A single original BIOS-style key is
        // delivered per source 30B40 poll; simultaneous keys are not
        // fabricated as a new combined command.
        static const std::uint32_t scans[6]={
            0x148u,0x150u,0x14Bu,0x14Du,0x147u,0x149u};
        for(unsigned i=0u;i<6u;++i)if(keys[i])return scans[i];
        return 0u;
    }
    void capture_source_vga() {
        if(!game || !game->guest || machine::pending_host_service(
                  game->canonical.get())!=11u)
            throw std::runtime_error("VGA scanout requires paused original 30B40 guest");
        const auto& vga=game->guest->video;
        if(vga.bios_mode()!=0x13u || vga.repeat_scanlines()!=1u)
            throw std::runtime_error("Original CODE.2 320x400 VGA mode changed");
        const auto indexed=vga.scanout_indices();
        if(indexed.size()!=kGameWidth*kGameHeight)
            throw std::runtime_error("Original CODE.2 scanout dimensions changed");
        scanout_rgba.resize(indexed.size());
        for(std::size_t i=0;i<indexed.size();++i) {
            const auto c=vga.palette(indexed[i]);
            const auto expand=[](unsigned channel)->std::uint32_t {
                return (channel<<2u)|(channel>>4u);
            };
            scanout_rgba[i]=(expand(c.r)<<16u) |
                            (expand(c.g)<<8u) |
                             expand(c.b);
        }
    }
    void tick(std::uint32_t scan) {
        if(failed || !game)return;
        try {
            const auto* mem=guest_bytes();
            const auto x=code2::load_le32(mem+0x255D0u);
            const auto y=code2::load_le32(mem+0x255D4u);
            const auto heading=code2::load_le32(mem+0xC658u);
            const auto base=game->guest->memory.data();
            const auto next=machine::answer_original_keyboard_event(
                game->canonical.get(),scan);
            if(next!=11u || base!=game->guest->memory.data())
                throw std::runtime_error("Original 1FC14 guest did not return to same 30B40 poll");
            const auto nx=code2::load_le32(mem+0x255D0u);
            const auto ny=code2::load_le32(mem+0x255D4u);
            const auto nh=code2::load_le32(mem+0xC658u);
            ++ticks;
            if(x!=nx||y!=ny)++source_displacements;
            if(heading!=nh)++source_rotations;
            if(x!=nx||y!=ny||heading!=nh)++pose_changes;
            capture_source_vga();
            if(hwnd) {
                char title[256]{};
                std::snprintf(title,sizeof(title),
                    "Ravenloft CODE.2 - Level 22 [EXPERIMENTAL] | x:%ld y:%ld heading:%u | ticks:%u moves:%u turns:%u",
                    static_cast<long>(static_cast<std::int32_t>(nx)>>16u),
                    static_cast<long>(static_cast<std::int32_t>(ny)>>16u),
                    static_cast<unsigned>(nh),ticks,
                    source_displacements,source_rotations);
                SetWindowTextA(hwnd,title);
                InvalidateRect(hwnd,nullptr,FALSE);
            }
        }catch(const std::exception& e){
            failed=true;
            error=e.what();
            if(hwnd) {
                KillTimer(hwnd,kFrameTimer);
                SetWindowTextA(hwnd,"Ravenloft CODE.2 - stopped at unsupported source event");
                MessageBoxA(hwnd,error.c_str(),
                    "Original CODE.2 stopped (no substituted behavior)",MB_OK|MB_ICONWARNING);
            }
            std::cerr<<"RLOFT_LEVEL22_LIVE_SOURCE_STOP="<<error<<"\n";
        }
    }
    static LRESULT CALLBACK wnd_proc(HWND window,UINT msg,WPARAM wp,LPARAM lp){
        auto* self=reinterpret_cast<LiveWindow*>(
            GetWindowLongPtrA(window,GWLP_USERDATA));
        if(msg==WM_NCCREATE) {
            auto* cs=reinterpret_cast<CREATESTRUCTA*>(lp);
            SetWindowLongPtrA(window,GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return TRUE;
        }
        if(!self)return DefWindowProcA(window,msg,wp,lp);
        switch(msg){
        case WM_KEYDOWN:case WM_SYSKEYDOWN:
            if(wp==VK_ESCAPE){DestroyWindow(window);return 0;}
            if(const auto slot=key_slot(wp);slot<6u)self->keys[slot]=true;
            return 0;
        case WM_KEYUP:case WM_SYSKEYUP:
            if(const auto slot=key_slot(wp);slot<6u)self->keys[slot]=false;
            return 0;
        case WM_KILLFOCUS:
            std::fill(std::begin(self->keys),std::end(self->keys),false);
            return 0;
        case WM_TIMER:
            if(wp==kFrameTimer)self->tick(self->held_scan());
            return 0;
        case WM_ERASEBKGND:return 1;
        case WM_PAINT: {
            PAINTSTRUCT paint{};
            HDC dc=BeginPaint(window,&paint);
            RECT area{};GetClientRect(window,&area);
            if(self->scanout_rgba.size()==kGameWidth*kGameHeight){
                BITMAPINFO info{};
                info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
                info.bmiHeader.biWidth=kGameWidth;
                info.bmiHeader.biHeight=-static_cast<LONG>(kGameHeight);
                info.bmiHeader.biPlanes=1;
                info.bmiHeader.biBitCount=32;
                info.bmiHeader.biCompression=BI_RGB;
                SetStretchBltMode(dc,COLORONCOLOR);
                StretchDIBits(dc,0,0,area.right,area.bottom,
                    0,0,kGameWidth,kGameHeight,self->scanout_rgba.data(),
                    &info,DIB_RGB_COLORS,SRCCOPY);
            }
            EndPaint(window,&paint);
            return 0;
        }
        case WM_DESTROY:KillTimer(window,kFrameTimer);PostQuitMessage(0);return 0;
        default:return DefWindowProcA(window,msg,wp,lp);
        }
    }
    int show(HINSTANCE instance) {
        WNDCLASSA cls{};
        cls.hInstance=instance;
        cls.lpszClassName="RLOFT_SOURCE_LEVEL22_VGA_WINDOW";
        cls.lpfnWndProc=&wnd_proc;
        cls.hCursor=LoadCursorA(nullptr,IDC_ARROW);
        cls.hbrBackground=static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        if(!RegisterClassA(&cls))throw std::runtime_error("Could not register original VGA window");
        DWORD style=WS_OVERLAPPEDWINDOW;
        RECT rect{0,0,640,800};
        AdjustWindowRect(&rect,style,FALSE);
        hwnd=CreateWindowExA(0,cls.lpszClassName,
           "Ravenloft CODE.2 - Level 22 [EXPERIMENTAL]",
           style,CW_USEDEFAULT,CW_USEDEFAULT,
           rect.right-rect.left,rect.bottom-rect.top,
           nullptr,nullptr,instance,this);
        if(!hwnd)throw std::runtime_error("Could not create original VGA window");
        ShowWindow(hwnd,SW_SHOWNORMAL);
        UpdateWindow(hwnd);
        SetTimer(hwnd,kFrameTimer,120u,nullptr);
        MSG msg{};
        while(GetMessageA(&msg,nullptr,0,0)>0){
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        std::cerr<<"RLOFT_LEVEL22_WINDOW_END ticks="<<ticks
                 <<" camera_displacements="<<source_displacements
                 <<" heading_rotations="<<source_rotations
                 <<" stopped="<<failed
                 <<" source=ORIGINAL_1FC14_2E120_2DBAC"
                 <<" dos_irq9=NOT_CERTIFIED"
                 <<" retail_indexed_frame_parity=OPEN\n";
        return failed?2:0;
    }
};
} // namespace

int main(int argc,char** argv){
    try {
        if((argc!=5 && argc!=6) || std::string(argv[1])!="--root" ||
           std::string(argv[3])!="--cd-root" ||
           (argc==6 && std::string(argv[5])!="--smoke"))
            throw std::invalid_argument(
                "usage: rloft_code2_level22_window --root ORIGINAL_RLOFT --cd-root ORIGINAL_CD_RLOFT [--smoke]");
        const auto root=std::filesystem::absolute(argv[2]);
        const auto cd=std::filesystem::absolute(argv[4]);
        if(!std::filesystem::is_regular_file(root/"CODE.2"))
            throw std::runtime_error("Original CODE.2 not found");
        LiveWindow live;
        live.open_original_guest(root,cd);
        if(argc==6) {
            if(live.scanout_rgba.size()!=kGameWidth*kGameHeight)
                throw std::runtime_error("No source VGA framebuffer");
            const std::uint32_t tests[]={
                0x148u,0x148u,0x148u,0x147u,0x147u,0x149u,0x148u};
            for(const auto scan:tests)live.tick(scan);
            if(live.failed || live.ticks!=7u ||
               (live.source_displacements==0u &&
                live.source_rotations==0u))
                throw std::runtime_error(
                    "Source-key Level22 tick did not change real guest camera/heading");
            std::cout<<"RLOFT_LEVEL22_NATIVE_HOST_SMOKE_PASS"
                <<" source_keyboard_ticks="<<live.ticks
                <<" camera_displacements="<<live.source_displacements
                <<" heading_rotations="<<live.source_rotations
                <<" scanout_pixels="<<live.scanout_rgba.size()
                <<" original_source=1FC14_2E120_2DBAC_290F4"
                <<" native_window=NOT_OPENED_IN_SMOKE"
                <<" retail_frame_parity=OPEN\n"<<std::flush;
            return 0;
        }
        return live.show(GetModuleHandleA(nullptr));
    }catch(const std::exception& e){
        std::cerr<<"RLOFT_LEVEL22_NATIVE_HOST_FAIL: "<<e.what()<<"\n";
        if(argc!=6 || std::string(argv[5])!="--smoke")
            MessageBoxA(nullptr,e.what(),"Ravenloft experimental source host",MB_OK|MB_ICONERROR);
        return 1;
    }
}
