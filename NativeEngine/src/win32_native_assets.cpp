#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <objbase.h>
#include <wrl/client.h>

#include "rloft_native/native_media.hpp"
#include "rloft_native/native_assets.hpp"

#include <algorithm>
#include <filesystem>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#ifndef RLOFT_NATIVE_SOURCE_SHA
#define RLOFT_NATIVE_SOURCE_SHA "unversioned"
#endif

namespace {
namespace fs = std::filesystem;
using rloft_native::NativeMedia;
using rloft_native::NativeAssetReport;
using rloft_native::NativePalette6;

constexpr int kFolderButton = 4201;
constexpr int kResults = 4202;

std::wstring unicode(std::string_view utf8) {
    const auto n=MultiByteToWideChar(CP_UTF8, 0, utf8.data(),
                                    static_cast<int>(utf8.size()),nullptr,0);
    if(n <= 0) return L"(invalid message)";
    std::wstring out(static_cast<std::size_t>(n),L'\0');
    MultiByteToWideChar(CP_UTF8,0,utf8.data(),static_cast<int>(utf8.size()),
                        out.data(),n);
    return out;
}

fs::path select_original_install(HWND owner) {
    Microsoft::WRL::ComPtr<IFileOpenDialog> dialog;
    if(FAILED(CoCreateInstance(CLSID_FileOpenDialog,nullptr,CLSCTX_INPROC_SERVER,
                               IID_PPV_ARGS(dialog.GetAddressOf()))))
        throw std::runtime_error("unable to create Windows original-install picker");
    DWORD flags=0;
    if(FAILED(dialog->GetOptions(&flags))||
       FAILED(dialog->SetOptions(flags|FOS_PICKFOLDERS|FOS_FORCEFILESYSTEM|
                                FOS_PATHMUSTEXIST))||
       FAILED(dialog->SetTitle(L"Select owned Ravenloft original install folder")))
        throw std::runtime_error("unable to configure original-install picker");
    const HRESULT shown=dialog->Show(owner);
    if(shown==HRESULT_FROM_WIN32(ERROR_CANCELLED))return {};
    if(FAILED(shown))throw std::runtime_error("Windows install selection failed");
    Microsoft::WRL::ComPtr<IShellItem> item;
    if(FAILED(dialog->GetResult(item.GetAddressOf())))
        throw std::runtime_error("original-install folder not returned");
    PWSTR pathname=nullptr;
    if(FAILED(item->GetDisplayName(SIGDN_FILESYSPATH,&pathname))||!pathname)
        throw std::runtime_error("selected item is not a filesystem path");
    const fs::path path(pathname);
    CoTaskMemFree(pathname);
    return path;
}

struct State {
    HWND window{};
    HWND button{};
    HWND report_widget{};
    std::unique_ptr<NativeMedia> media;
    NativeAssetReport audit;
    NativePalette6 palette;
    bool has_palette{};
    std::wstring status{
        L"Select the directory containing your owned original game.gog, "
        L"or the loose DOS INIT/RES0/T files.\r\n"
        L"This engine opens original media directly; it does not extract "
        L"the installation or execute the original DOS program.\r\n"};
};

std::wstring format_report(const NativeMedia& media,
                           const NativeAssetReport& audit) {
    std::wostringstream out;
    out << L"Original media (READ ONLY)\r\n";
    out << L"Path: " << media.source_path().wstring() << L"\r\n";
    switch(media.kind()) {
        case rloft_native::NativeMediaKind::LooseDirectory:
            out << L"Format: Loose original DOS game files\r\n"; break;
        case rloft_native::NativeMediaKind::GogIso2048:
            out << L"Format: GOG ISO 2048-byte sectors\r\n"; break;
        case rloft_native::NativeMediaKind::GogIso2352_16:
            out << L"Format: GOG raw CD 2352-byte sectors (offset 16)\r\n"; break;
        case rloft_native::NativeMediaKind::GogIso2352_24:
            out << L"Format: GOG raw CD 2352-byte sectors (offset 24)\r\n"; break;
    }
    out << L"Indexed files: " << audit.indexed_files << L"\r\n";
    out << L"Known resources: " << audit.valid << L" validated, "
        << audit.missing << L" missing, "
        << audit.invalid << L" invalid\r\n\r\n";
    for(const auto& a:audit.checks) {
        out << (a.validated?L"[OK]  ":(a.present?L"[BAD] ":L"[--]  "))
            << unicode(a.name);
        if(a.present) {
            out << L"  " << a.bytes << L" bytes";
            if(a.res_entries) out << L", " << a.res_entries << L" RES records";
            if(!a.issue.empty()) out << L"  |  " << unicode(a.issue);
        }
        out << L"\r\n";
    }
    out << L"\r\nNative resource index only. Original files stay unchanged.\r\n";
    out << L"Rendering, gameplay, and original DOS parity are NOT yet established.\r\n";
    return out.str();
}

void install(State& state,const fs::path& path) {
    try {
        auto media=std::make_unique<NativeMedia>(NativeMedia::open(path));
        auto report=rloft_native::inspect_native_assets(*media);
        NativePalette6 palette{};
        bool palette_ok=false;
        if(media->contains("COLORS")) {
            try {
                palette=rloft_native::decode_native_palette(
                    media->read("COLORS"),12);
                palette_ok=true;
            } catch(const std::exception&) {}
        }
        // Commit the new source only after all indexing/reporting succeeded.
        const auto output=format_report(*media,report);
        state.media=std::move(media);
        state.audit=std::move(report);
        state.palette=palette;
        state.has_palette=palette_ok;
        state.status=output;
    } catch(const std::exception& error) {
        // Do not show another install's palette after a failed selection.
        state.media.reset();
        state.has_palette=false;
        state.status=L"Cannot read selected original game media:\r\n"+
                     unicode(error.what())+
                     L"\r\nNo original files were written or extracted.";
    }
    if(state.report_widget)
        SetWindowTextW(state.report_widget,state.status.c_str());
    if(state.window)InvalidateRect(state.window,nullptr,TRUE);
}

void layout(State& state) {
    if(!state.window)return;
    RECT r{};GetClientRect(state.window,&r);
    if(state.button) MoveWindow(state.button,24,82,300,34,TRUE);
    if(state.report_widget) {
        const int left=24,top=132;
        const int width=std::max(200, int(r.right)-370);
        const int height=std::max(150, int(r.bottom)-166);
        MoveWindow(state.report_widget,left,top,width,height,TRUE);
    }
}

void draw(State& state,HDC dc) {
    RECT rect{};GetClientRect(state.window,&rect);
    auto brush=CreateSolidBrush(RGB(22,27,35));
    FillRect(dc,&rect,brush);DeleteObject(brush);
    SetBkMode(dc,TRANSPARENT);
    SetTextColor(dc,RGB(235,240,248));
    RECT title{24,19,rect.right-20,58};
    DrawTextW(dc,L"RAVENLOFT — NATIVE ASSET ENGINE", -1,&title,DT_SINGLELINE);
    SetTextColor(dc,RGB(169,185,207));
    std::wstring subtitle=L"Original media: lazy, read-only  |  Source "+unicode(RLOFT_NATIVE_SOURCE_SHA);
    RECT sub{24,48,rect.right-20,75};
    DrawTextW(dc,subtitle.c_str(),-1,&sub,DT_SINGLELINE|DT_END_ELLIPSIS);
    const int x=std::max(245, int(rect.right)-320);
    RECT label{x,148,rect.right-18,186};
    SetTextColor(dc,RGB(241,207,137));
    DrawTextW(dc,L"ORIGINAL COLORS PALETTE 12", -1,&label,DT_SINGLELINE);
    if(state.has_palette) {
        constexpr int swatch=15;
        for(int index=0;index<256;++index) {
            const auto pos=3*index;
            const auto expand=[](std::uint8_t c) {
                return int((unsigned(c)*255U+31U)/63U);
            };
            auto color=CreateSolidBrush(RGB(expand(state.palette.rgb[pos]),
                                           expand(state.palette.rgb[pos+1]),
                                           expand(state.palette.rgb[pos+2])));
            RECT cell{x+(index%16)*swatch,188+(index/16)*swatch,
                      x+(index%16+1)*swatch,188+(index/16+1)*swatch};
            FillRect(dc,&cell,color);
            DeleteObject(color);
        }
    } else {
        RECT empty{x,190,rect.right-18,286};
        SetTextColor(dc,RGB(175,184,193));
        DrawTextW(dc,L"Original palette not loaded.\nSelect a supported "
                     L"installation to inspect real asset data.",
                  -1,&empty,DT_WORDBREAK);
    }
    RECT explanation{x,465,rect.right-14,rect.bottom-12};
    SetTextColor(dc,RGB(183,192,211));
    DrawTextW(dc,
        L"Read original RES archives and COLORS directly.\n"
        L"No bulk extraction.\nNo DOS executable execution.\n"
        L"No modified save files.\n\n"
        L"ENGINE STAGE: native asset validation\n"
        L"Gameplay and rendering integration follow.",
        -1,&explanation,DT_WORDBREAK);
}

LRESULT CALLBACK proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
    auto* state=reinterpret_cast<State*>(GetWindowLongPtrW(hwnd,GWLP_USERDATA));
    if(msg==WM_NCCREATE) {
        auto* create=reinterpret_cast<CREATESTRUCTW*>(lparam);
        state=static_cast<State*>(create->lpCreateParams);
        state->window=hwnd;
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(state));
        return TRUE;
    }
    if(!state)return DefWindowProcW(hwnd,msg,wparam,lparam);
    switch(msg) {
        case WM_CREATE: {
            const auto instance=reinterpret_cast<CREATESTRUCTW*>(lparam)->hInstance;
            state->button=CreateWindowExW(0,L"BUTTON",L"Open original installation...",
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,
                24,82,300,34,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kFolderButton)),instance,nullptr);
            state->report_widget=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",state->status.c_str(),
                WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_READONLY|
                ES_AUTOVSCROLL,24,132,640,500,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kResults)),instance,nullptr);
            if(!state->button||!state->report_widget)return -1;
            return 0;
        }
        case WM_SIZE:
            layout(*state);InvalidateRect(hwnd,nullptr,TRUE);return 0;
        case WM_COMMAND:
            if(LOWORD(wparam)==kFolderButton && HIWORD(wparam)==BN_CLICKED) {
                try {
                    const auto path=select_original_install(hwnd);
                    if(!path.empty())install(*state,path);
                } catch(const std::exception& error) {
                    state->status=L"Install dialog failed: "+unicode(error.what());
                    SetWindowTextW(state->report_widget,state->status.c_str());
                }
                return 0;
            }
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            auto dc=BeginPaint(hwnd,&ps);
            draw(*state,dc);
            EndPaint(hwnd,&ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);return 0;
        default:break;
    }
    return DefWindowProcW(hwnd,msg,wparam,lparam);
}

int inspect_headless(const fs::path& source) {
    try {
        const auto media=NativeMedia::open(source);
        const auto report=rloft_native::inspect_native_assets(media);
        bool init=false,res0=false,t=false,colors=false;
        for(const auto& check:report.checks) {
            if(check.name=="INIT") init=check.validated;
            if(check.name=="RES0") res0=check.validated;
            if(check.name=="T") t=check.validated;
            if(check.name=="COLORS") colors=check.validated;
        }
        return init&&res0&&t&&colors ? 0 : 3;
    } catch(...) {return 2;}
}

int run(HINSTANCE instance) {
    int argc=0;
    LPWSTR* argv=CommandLineToArgvW(GetCommandLineW(),&argc);
    if(!argv)return 2;
    std::wstring option=argc>1?argv[1]:L"";
    fs::path source=argc>2?fs::path(argv[2]):fs::path{};
    const bool headless=argc==3&&option==L"--verify-assets";
    const bool from_arg=argc==3&&option==L"--install";
    const bool invalid=argc!=1&&!headless&&!from_arg;
    LocalFree(argv);
    if(headless)return inspect_headless(source);
    if(invalid)return 64;

    const HRESULT initialized=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);
    State state;
    WNDCLASSW klass{};
    klass.lpfnWndProc=proc;
    klass.hInstance=instance;
    klass.hCursor=LoadCursorW(nullptr,IDC_ARROW);
    klass.lpszClassName=L"RloftIndependentNativeAssetsWindow";
    if(!RegisterClassW(&klass)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS) {
        if(SUCCEEDED(initialized))CoUninitialize();
        return 2;
    }
    auto hwnd=CreateWindowExW(0,klass.lpszClassName,
        L"Ravenloft: Strahd's Possession — Native Asset Engine",
        WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,
        1160,770,nullptr,nullptr,instance,&state);
    if(!hwnd) {
        if(SUCCEEDED(initialized))CoUninitialize();
        return 2;
    }
    if(from_arg)install(state,source);
    ShowWindow(hwnd,SW_SHOW);
    UpdateWindow(hwnd);
    MSG msg{};
    while(GetMessageW(&msg,nullptr,0,0)>0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if(SUCCEEDED(initialized))CoUninitialize();
    return int(msg.wParam);
}
}
int WINAPI wWinMain(HINSTANCE h,HINSTANCE,PWSTR,int) {
    try{return run(h);}
    catch(const std::exception& error) {
        MessageBoxW(nullptr,unicode(error.what()).c_str(),
                    L"Native Asset Engine",MB_OK|MB_ICONERROR);
        return 2;
    }
}
#endif
