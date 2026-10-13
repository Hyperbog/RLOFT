#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include "rloft_native/native_win32_startup.hpp"
#include "rloft_native/native_startup_visuals.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft/startup_presenter.hpp"
#include "rloft/anm_cinematic.hpp"
#include "rloft/boot_flow.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
namespace rloft_native {
namespace {
constexpr UINT_PTR kTick=31;
constexpr unsigned kWidth=320,kHeight=400;
constexpr int kMenu=18;
constexpr std::array<unsigned,4> kLogoWaitMs{
    120U*1000U/18U,106U*1000U/18U,
    121U*1000U/18U,55U*1000U/18U};
struct Preview {
    NativeStartupVisualSource source;
    rloft::StartupPresenter presenter;
    rloft::BootFlow boot;
    std::unique_ptr<rloft::AnmCinematic> cine00;
    std::unique_ptr<rloft::AnmCinematic> cine12;
    unsigned cinematic_wait_ms{14U};
    unsigned cinematic_audio_events{};
    HWND hwnd{};
    bool ownership_transferred{};
    int phase{};
    int selection{-1};
    ULONGLONG phase_started{};
    rloft::Palette6 palette{};
    std::vector<std::uint8_t> bgra;
    explicit Preview(const NativeMedia& media)
        :source(media),presenter(source),bgra(std::size_t(kWidth)*kHeight*4U) {
        // This window inspects the original no-CHARS cold presentation
        // only. It never creates a fabricated party or bypasses CODE.1.
        boot.boot();
        boot.finish_code2_startup(false,false);
        presenter.first_logo();
        palette=presenter.palette14();
        // The shipped CD source stores ANM files inside game.gog or the
        // extracted CD RLOFT folder. A loose DOS-only installation may not.
        if(media.contains("CINE00.ANM") && media.contains("CINE12.ANM")) {
            cine00=std::make_unique<rloft::AnmCinematic>(
                media.read("CINE00.ANM"),0,source);
            cine12=std::make_unique<rloft::AnmCinematic>(
                media.read("CINE12.ANM"),12,source);
            phase=-2;
            render_cinematic_frame();
        } else refresh();
    }
    void refresh() {
        const auto& pixels=phase<0?
            (phase==-2?cine00->framebuffer().pixels():cine12->framebuffer().pixels())
            :presenter.framebuffer().pixels();
        if(pixels.size()!=std::size_t(kWidth)*kHeight)
            throw std::runtime_error("original CODE.2 menu framebuffer size invalid");
        for(std::size_t i=0;i<pixels.size();++i) {
            const auto at=std::size_t(pixels[i])*3U;
            const auto out=i*4U;
            const auto expand=[](std::uint8_t v) {
                return std::uint8_t((unsigned(v)*255U+31U)/63U);
            };
            bgra[out+0U]=expand(palette.rgb[at+2U]);
            bgra[out+1U]=expand(palette.rgb[at+1U]);
            bgra[out+2U]=expand(palette.rgb[at+0U]);
            bgra[out+3U]=255U;
        }
        if(hwnd)InvalidateRect(hwnd,nullptr,FALSE);
    }
    void render_cinematic_frame() {
        auto& current=phase==-2?*cine00:*cine12;
        if(current.finished()) {
            if(phase==-2) {
                phase=-1;
                cine12->reset();
                render_cinematic_frame();
            } else {
                select_phase(0);
            }
            return;
        }
        const auto frame=current.render_next();
        cinematic_audio_events+=unsigned(frame.audio.size());
        // Original DOS BIOS PIT 1193182/65536 = approximately 18.2065 Hz.
        // ANM type-3/8 frame overrides are already computed by the shared
        // source decoder. Music/VOC playback is a separate open A1 item.
        cinematic_wait_ms=frame.wait_ticks>0?
            unsigned((double(frame.wait_ticks)*1000.0)/
                rloft::StartupPresenter::kPitHz+0.5):14U;
        phase_started=GetTickCount64();
        palette=current.palette();
        refresh();
    }
    void select_phase(int next) {
        phase=next;
        phase_started=GetTickCount64();
        if(phase==0){presenter.first_logo();palette=presenter.palette14();}
        else if(phase==1){presenter.second_logo();palette=presenter.palette14();}
        else if(phase==2){presenter.third_logo();palette=presenter.palette9();}
        else if(phase==3){presenter.title_background();palette=presenter.palette12();}
        else if(phase>=4 && phase<kMenu) {
            presenter.menu_animation_frame(phase-3);
            palette=presenter.palette12();
        } else if(phase==kMenu) {
            if(boot.snapshot().stage==rloft::BootStage::ColdStartPresentation)
                boot.finish_cold_start_presentation();
            presenter.menu_frame(selection);palette=presenter.palette12();
        } else throw std::runtime_error("original startup preview phase outside bounded range");
        refresh();
    }
    void advance() {
        if(phase>=kMenu)return;
        const auto elapsed=GetTickCount64()-phase_started;
        if(phase<0) {
            if(elapsed>=cinematic_wait_ms)render_cinematic_frame();
            return;
        }
        const unsigned wait=phase<4?kLogoWaitMs[std::size_t(phase)]:14U;
        if(elapsed>=wait)select_phase(phase+1);
    }
    void skip_to_menu() {
        if(phase<kMenu && boot.snapshot().stage==rloft::BootStage::ColdStartPresentation)
            boot.skip_cold_start_presentation();
        select_phase(kMenu);
    }
};
RECT image_rect(HWND hwnd) {
    RECT client{};GetClientRect(hwnd,&client);
    const int width=std::max(1,int(client.right));
    const int height=std::max(1,int(client.bottom)-66);
    const int scale=std::max(1,std::min(width/int(kWidth),height/int(kHeight)));
    const int w=int(kWidth)*scale,h=int(kHeight)*scale;
    return {(width-w)/2,24+(height-h)/2,(width+w)/2,24+(height+h)/2};
}
void report_failure(HWND hwnd,const char* what) {
    MessageBoxA(hwnd,what,
        "Original CODE.2 startup visual error (game data untouched)",
        MB_OK|MB_ICONERROR);
}
LRESULT CALLBACK wnd_proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    auto* session=reinterpret_cast<Preview*>(GetWindowLongPtrW(hwnd,GWLP_USERDATA));
    if(msg==WM_NCCREATE) {
        auto* create=reinterpret_cast<CREATESTRUCTW*>(lp);
        session=static_cast<Preview*>(create->lpCreateParams);
        if(!session)return FALSE;
        session->hwnd=hwnd;
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(session));
        return TRUE;
    }
    if(!session)return DefWindowProcW(hwnd,msg,wp,lp);
    switch(msg) {
    case WM_CREATE:
        session->phase_started=GetTickCount64();
        if(!SetTimer(hwnd,kTick,14U,nullptr))return -1;
        return 0;
    case WM_TIMER:
        if(wp==kTick) {
            try {session->advance();}
            catch(const std::exception& e) {
                KillTimer(hwnd,kTick);report_failure(hwnd,e.what());
            }
            return 0;
        }
        break;
    case WM_KEYDOWN:
        if(wp==VK_ESCAPE){DestroyWindow(hwnd);return 0;}
        if(wp==VK_SPACE || wp==VK_RETURN) {
            if(session->phase<kMenu) {
                try{session->skip_to_menu();}
                catch(const std::exception& e){report_failure(hwnd,e.what());}
            }
            return 0;
        }
        break;
    case WM_MOUSEMOVE:
        if(session->phase==kMenu) {
            const RECT bounds=image_rect(hwnd);
            const int x=static_cast<short>(LOWORD(lp));
            const int y=static_cast<short>(HIWORD(lp));
            int next=-1;
            if(x>=bounds.left && x<bounds.right &&
               y>=bounds.top && y<bounds.bottom)
                next=rloft::StartupPresenter::menu_selection_from_y(
                    (y-bounds.top)*int(kHeight)/(bounds.bottom-bounds.top));
            if(next!=session->selection) {
                session->selection=next;
                try {session->presenter.menu_frame(next);session->refresh();}
                catch(const std::exception& e){report_failure(hwnd,e.what());}
            }
        }
        return 0;
    case WM_LBUTTONDOWN:
        if(session->phase==kMenu && session->selection>=0) {
            try {
                const int selected=session->selection;
                session->boot.select_startup_menu(selected);
                if(selected==rloft::BootFlow::kCharacterWorkflowMenuIndex) {
                    session->boot.supervisor_dispatch();
                    MessageBoxW(hwnd,
                        L"Original CODE.2 menu selection routed into CODE.1.\n"
                        L"The original interactive character presenter and CHARS\n"
                        L"transfer are the next Alpha A1 integration step.\n"
                        L"No party or save was fabricated.",
                        L"CODE.1 original handoff — not yet playable",
                        MB_OK|MB_ICONINFORMATION);
                } else {
                    MessageBoxW(hwnd,
                        L"The original CODE.2 menu selection has been accepted\n"
                        L"by the shared supervisor. Its game-facing handler is\n"
                        L"not yet implemented in the native executable.",
                        L"Original menu route — implementation pending",
                        MB_OK|MB_ICONINFORMATION);
                }
            } catch(const std::exception& e){report_failure(hwnd,e.what());}
            return 0;
        }
        return 0;
    case WM_ERASEBKGND:return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps{};HDC dc=BeginPaint(hwnd,&ps);
        RECT whole{};GetClientRect(hwnd,&whole);
        auto bg=CreateSolidBrush(RGB(13,17,25));
        FillRect(dc,&whole,bg);DeleteObject(bg);
        const auto rect=image_rect(hwnd);
        BITMAPINFO info{};
        info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth=LONG(kWidth);
        info.bmiHeader.biHeight=-LONG(kHeight);
        info.bmiHeader.biPlanes=1;
        info.bmiHeader.biBitCount=32;
        info.bmiHeader.biCompression=BI_RGB;
        SetStretchBltMode(dc,COLORONCOLOR);
        StretchDIBits(dc,rect.left,rect.top,rect.right-rect.left,
            rect.bottom-rect.top,0,0,int(kWidth),int(kHeight),
            session->bgra.data(),&info,DIB_RGB_COLORS,SRCCOPY);
        SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB(230,233,240));
        RECT help{14,whole.bottom-35,whole.right-10,whole.bottom-8};
        DrawTextW(dc,
            session->phase==kMenu?
                L"Original CODE.2 title menu | Select an item | Esc closes"
                :(session->phase<0?
                    L"Original CINE00/CINE12 indexed ANM (sound pending) | Space skips | Esc closes"
                    :L"Original RES0 startup presentation | Space skips to menu | Esc closes"),
            -1,&help,DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
        EndPaint(hwnd,&ps);return 0;
    }
    case WM_CLOSE:DestroyWindow(hwnd);return 0;
    case WM_NCDESTROY:
        KillTimer(hwnd,kTick);
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,0);
        if(session->ownership_transferred)delete session;
        return DefWindowProcW(hwnd,msg,wp,lp);
    default:break;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
} // namespace
void open_native_startup(HWND owner,const NativeMedia& media) {
    auto session=std::make_unique<Preview>(media);
    WNDCLASSW wc{};
    wc.lpfnWndProc=wnd_proc;
    wc.hInstance=GetModuleHandleW(nullptr);
    wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
    wc.lpszClassName=L"RloftNativeSourceStartup";
    if(!RegisterClassW(&wc) && GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)
        throw std::runtime_error("unable to register original CODE.2 startup window");
    HWND hwnd=CreateWindowExW(0,wc.lpszClassName,
        L"Ravenloft — Original CODE.2 Startup (Alpha A1)",
        WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,
        1020,920,owner,nullptr,wc.hInstance,session.get());
    if(!hwnd)throw std::runtime_error("unable to create original startup window");
    session->ownership_transferred=true;
    session.release();
    ShowWindow(hwnd,SW_SHOW);
    UpdateWindow(hwnd);
    SetFocus(hwnd);
}
} // namespace rloft_native
#endif
