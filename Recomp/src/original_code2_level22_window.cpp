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
#include <cstdlib>
#include <iterator>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

namespace {
namespace source=rloft::original_code2_session;
namespace machine=rloft::canonical_code2_runtime;
using Owner=std::unique_ptr<source::Opaque,void(*)(source::Opaque*)>;
static constexpr UINT kSourceFrameReady=WM_APP+1u;
static constexpr auto kFrameInterval=std::chrono::milliseconds(120);
static constexpr unsigned kGameWidth=320u,kGameHeight=400u;

struct LiveWindow final {
    Owner game{nullptr,&source::destroy};
    HWND hwnd=nullptr;
    std::vector<std::uint32_t> scanout_rgba;
    // The native UI only reads an atomically published frame and input bits.
    // CODE.2 guest execution remains on its single original worker owner.
    std::mutex frame_lock;
    std::thread gameplay_worker;
    std::atomic<std::uint32_t> key_mask{0u};
    std::atomic<bool> quit_requested{false};
    std::string posted_title;
    std::string posted_error;
    bool error_shown=false;
    unsigned ticks=0u;
    unsigned pose_changes=0u;
    unsigned source_displacements=0u;
    unsigned source_rotations=0u;
    bool failed=false;
    std::string error;

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
            // MSVC std::getenv reads the CRT-owned _environ array.
            // SetEnvironmentVariableA alone does not synchronize it.
            if(_putenv_s(name,"1")!=0 || !std::getenv(name) ||
               std::strcmp(std::getenv(name),"1")!=0)
                throw std::runtime_error(std::string("Failed to enable original-source CRT mode: ")+name);
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
        const auto held=key_mask.load(std::memory_order_relaxed);
        for(unsigned i=0u;i<6u;++i)
            if((held&(1u<<i))!=0u)return scans[i];
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
        std::vector<std::uint32_t> complete(indexed.size());
        for(std::size_t i=0;i<indexed.size();++i) {
            const auto c=vga.palette(indexed[i]);
            const auto expand=[](unsigned channel)->std::uint32_t {
                return (channel<<2u)|(channel>>4u);
            };
            complete[i]=(expand(c.r)<<16u) |
                        (expand(c.g)<<8u) |
                         expand(c.b);
        }
        // Publish the completed original planar-VGA scanout, not a
        // partially written frame or half-updated palette.
        std::lock_guard<std::mutex> guard(frame_lock);
        scanout_rgba.swap(complete);
    }
    bool audited_portal_roundtrip(const machine::Evidence& proof) const noexcept {
        // A stale 320x400 buffer is not proof of a post-portal frame.
        // This single gate applies to both the archived 2D3D8 probe and
        // original 07938 saved-game starting-state probe.
        return !failed && ticks==6u &&
            code2::load_le32(guest_bytes()+0xC6C8u)==23u &&
            scanout_rgba.size()==kGameWidth*kGameHeight &&
            proof.original_live_level_transitions==3u &&
            proof.original_live_portal_full_cycle_frames==3u &&
            proof.original_live_pending_portal_level==0xFFFFFFFFu &&
            proof.original_1fc14_source_scene_returns==
                proof.original_1fc14_post_scene_cycles &&
            proof.original_1fc14_page_handoffs==
                proof.original_1fc14_post_scene_cycles;
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
                    "Ravenloft CODE.2 - Level %u [EXPERIMENTAL] | x:%ld y:%ld heading:%u | ticks:%u moves:%u turns:%u",
                    static_cast<unsigned>(code2::load_le32(mem+0xC6C8u)),
                    static_cast<long>(static_cast<std::int32_t>(nx)>>16u),
                    static_cast<long>(static_cast<std::int32_t>(ny)>>16u),
                    static_cast<unsigned>(nh),ticks,
                    source_displacements,source_rotations);
                {
                    std::lock_guard<std::mutex> guard(frame_lock);
                    posted_title=title;
                }
                if(!quit_requested.load(std::memory_order_acquire))
                    PostMessageA(hwnd,kSourceFrameReady,0,0);
            }
        }catch(const std::exception& e){
            failed=true;
            error=e.what();
            if(hwnd) {
                {
                    std::lock_guard<std::mutex> guard(frame_lock);
                    posted_error=error;
                }
                if(!quit_requested.load(std::memory_order_acquire))
                    PostMessageA(hwnd,kSourceFrameReady,0,0);
            }
            std::cerr<<"RLOFT_LEVEL22_LIVE_SOURCE_STOP="<<error<<"\n";
        }
    }
    // The Win32 message pump NEVER executes original 1FC14 frames. Long
    // source actor/raster operations cannot block keyboard or window paint.
    // The 120ms minimum is a host-input throttle, not a synthesized IRQ.
    void worker_loop() {
        while(!quit_requested.load(std::memory_order_acquire) && !failed) {
            const auto started=std::chrono::steady_clock::now();
            tick(held_scan());
            while(!failed && !quit_requested.load(std::memory_order_acquire) &&
                  std::chrono::steady_clock::now()-started<kFrameInterval)
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
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
            if(const auto slot=key_slot(wp);slot<6u)
                self->key_mask.fetch_or(1u<<slot,std::memory_order_release);
            return 0;
        case WM_KEYUP:case WM_SYSKEYUP:
            if(const auto slot=key_slot(wp);slot<6u)
                self->key_mask.fetch_and(~(1u<<slot),std::memory_order_release);
            return 0;
        case WM_KILLFOCUS:
            self->key_mask.store(0u,std::memory_order_release);
            return 0;
        case kSourceFrameReady: {
            std::string title,error;
            {
                std::lock_guard<std::mutex> guard(self->frame_lock);
                title=self->posted_title;
                error=self->posted_error;
            }
            if(!title.empty())SetWindowTextA(window,title.c_str());
            InvalidateRect(window,nullptr,FALSE);
            if(!error.empty() && !self->error_shown) {
                self->error_shown=true;
                SetWindowTextA(window,
                    "Ravenloft CODE.2 - stopped at unsupported source event");
                MessageBoxA(window,error.c_str(),
                    "Original CODE.2 stopped (no substituted behavior)",
                    MB_OK|MB_ICONWARNING);
            }
            return 0;
        }
        case WM_ERASEBKGND:return 1;
        case WM_PAINT: {
            PAINTSTRUCT paint{};
            HDC dc=BeginPaint(window,&paint);
            RECT area{};GetClientRect(window,&area);
            {
                std::lock_guard<std::mutex> guard(self->frame_lock);
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
            }
            EndPaint(window,&paint);
            return 0;
        }
        case WM_DESTROY:
            self->quit_requested.store(true,std::memory_order_release);
            self->key_mask.store(0u,std::memory_order_release);
            if(self->gameplay_worker.joinable())self->gameplay_worker.join();
            PostQuitMessage(0);
            return 0;
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
        gameplay_worker=std::thread([this]{worker_loop();});
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
        const bool portal_smoke=argc==6 &&
            std::string(argv[5])=="--smoke-portal";
        const bool ordinary_smoke=argc==6 &&
            std::string(argv[5])=="--smoke";
        const bool saved_smoke=argc==7 &&
            std::string(argv[5])=="--smoke-save23";
        const bool saved_interactive=argc==7 &&
            std::string(argv[5])=="--load-save23";
        const bool saved_start=saved_smoke || saved_interactive;
        if((argc!=5 && argc!=6 && argc!=7) ||
           std::string(argv[1])!="--root" ||
           std::string(argv[3])!="--cd-root" ||
           (argc==6 && !ordinary_smoke && !portal_smoke) ||
           (argc==7 && !saved_start))
            throw std::invalid_argument(
                "usage: rloft_code2_level22_window --root ORIGINAL_RLOFT --cd-root ORIGINAL_CD_RLOFT [--smoke|--smoke-portal|--load-save23 SLOT_DIR|--smoke-save23 SLOT_DIR]");
        const auto root=std::filesystem::absolute(argv[2]);
        const auto cd=std::filesystem::absolute(argv[4]);
        if(!std::filesystem::is_regular_file(root/"CODE.2"))
            throw std::runtime_error("Original CODE.2 not found");
        if(saved_start) {
            const auto saved=std::filesystem::absolute(argv[6]);
            if(!std::filesystem::is_regular_file(saved/"SAVE.DAT") ||
               !std::filesystem::is_regular_file(saved/"LEV023.DAT"))
                throw std::runtime_error("Original saved Level23 slot must contain SAVE.DAT and LEV023.DAT");
            if(_putenv_s("RLOFT_RECOMP_ORIGINAL_LEVEL23_SAVE_DIR",saved.string().c_str())!=0 ||
               _putenv_s("RLOFT_RECOMP_EXPERIMENTAL_LEVEL23_SAVE_RESTORE","1")!=0)
                throw std::runtime_error("Cannot configure private original 07938 save restore");
        }
        if(portal_smoke || saved_start) {
            // Explicit diagnostic mode. These values are consumed by
            // retained original 30B40 source worker, never by a fake
            // host player engine.
            if(_putenv_s("RLOFT_RECOMP_EXPERIMENTAL_LIVE_LEVEL_TRANSITIONS","1")!=0 ||
               _putenv_s("RLOFT_RECOMP_EXPERIMENTAL_PORTAL_SOURCE_PROBE","1")!=0)
                throw std::runtime_error("Cannot enable owner-local authentic portal diagnostic");
        }
        LiveWindow live;
        live.open_original_guest(root,cd);
        if(saved_start) {
            // FFFA is an owner-only request into compiled source 07938,
            // never a retail keyboard code or invented Level23 camera.
            // The original saved LEV023 is read from the private overlay.
            // Subsequent frame generation must still run to 30B40.
            live.tick(0xFFFAu);
            if(live.failed ||
               code2::load_le32(live.guest_bytes()+0xC6C8u)!=23u)
                throw std::runtime_error("Retained 07938 Level23 save restore or first native 1FC14 frame failed");
            // A successful source 07938/181B4 load is not an indexed
            // frame. Demand the complete 1FC14 audit and next real 30B40
            // suspension on this same retained game stack first.
            const auto first=machine::inspect(live.game->canonical.get());
            if(live.ticks!=1u ||
               live.scanout_rgba.size()!=kGameWidth*kGameHeight ||
               first.original_live_level_transitions!=1u ||
               first.original_live_portal_full_cycle_frames!=1u ||
               first.original_live_pending_portal_level!=0xFFFFFFFFu ||
               first.original_1fc14_source_scene_returns!=
                   first.original_1fc14_post_scene_cycles ||
               first.original_1fc14_page_handoffs!=
                   first.original_1fc14_post_scene_cycles)
                throw std::runtime_error(
                    "Retained original Level23 07938 to 30B40 full source 1FC14 frame NOT_VERIFIED");
            std::cout<<"CODE2_07938_RETAINED_LEVEL23_1FC14_30B40_FULL_FRAME_PASS"
                     <<" source_saved_restore=YES original_level=23"
                     <<" transitions="<<first.original_live_level_transitions
                     <<" audited_frames="<<first.original_live_portal_full_cycle_frames
                     <<" source_page_handoffs="<<first.original_1fc14_page_handoffs
                     <<" owner_original_media=READ_ONLY\n"<<std::flush;
            if(saved_smoke) {
                for(const auto scan:{0u,0xFFFCu,0u,0xFFFBu,0u})
                    live.tick(scan);
                const auto proof=machine::inspect(live.game->canonical.get());
                if(!live.audited_portal_roundtrip(proof))
                    throw std::runtime_error(
                        "Original 07938 saved Level23 to archived 26/23 native full-cycle frame gate failed");
                std::cout<<"CODE2_07938_RETAINED_LEVEL23_PORTAL_26_23_FULL_CYCLE_PASS"
                         <<" original_07938=YES original_2D3D8=YES"
                         <<" transitions="<<proof.original_live_level_transitions
                         <<" audited_frames="<<proof.original_live_portal_full_cycle_frames
                         <<" live_guest=UNCHANGED owner_files=READ_ONLY"
                         <<" independent_DOS_frame_parity=OPEN\n"<<std::flush;
                return 0;
            }
        }
        if(portal_smoke) {
            // 0xFFFD selects a SOURCE 181B4(23) starting level. This is
            // NOT a retail Level22 portal. FFFC/FFFB enter authentic
            // original archived Level23->26 and Level26->23 portals via
            // compiled 2D3D8, retaining one live guest and its VGA.
            for(const auto scan:{0xFFFDu,0u,0xFFFCu,0u,0xFFFBu,0u})
                live.tick(scan);
            const auto proof=machine::inspect(live.game->canonical.get());
            if(!live.audited_portal_roundtrip(proof))
                throw std::runtime_error(
                    "Authentic CODE2 23-26-23 portal continuation or VGA scanout failed");
            std::cout<<"RLOFT_2D3D8_AUTHENTIC_PORTAL_NATIVE_SMOKE_PASS"
                     <<" source_levels=23_26_23"
                     <<" full_1FC14_ticks="<<live.ticks
                     <<" original_live_level_transitions="<<proof.original_live_level_transitions
                     <<" audited_post_portal_frames="<<proof.original_live_portal_full_cycle_frames
                     <<" original_1FC14_scene_returns="<<proof.original_1fc14_source_scene_returns
                     <<" original_1FC14_post_cycles="<<proof.original_1fc14_post_scene_cycles
                     <<" native_scanout_pixels="<<live.scanout_rgba.size()
                     <<" no_fabricated_level22_portal=YES"
                     <<" physical_DOS_retail_frame_parity=OPEN\n"<<std::flush;
            return 0;
        }
        if(ordinary_smoke) {
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
        if(argc!=6 ||
           (std::string(argv[5])!="--smoke" &&
            std::string(argv[5])!="--smoke-portal"))
            MessageBoxA(nullptr,e.what(),"Ravenloft experimental source host",MB_OK|MB_ICONERROR);
        return 1;
    }
}
