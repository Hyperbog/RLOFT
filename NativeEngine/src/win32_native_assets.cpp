#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <objbase.h>
#include <wrl/client.h>

#include "rloft_native/native_media.hpp"
#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_raster_browser.hpp"
#include "rloft_native/native_raster_colors.hpp"
#include "rloft_native/native_palette_context.hpp"
#include "rloft_native/native_win32_exploration.hpp"
#include "rloft_native/native_win32_gpu_exploration.hpp"
#include "rloft/boot_flow.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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
constexpr int kArchiveSelect = 4203;
constexpr int kRasterIndex = 4204;
constexpr int kRasterGo = 4205;
constexpr int kRasterPrevious = 4206;
constexpr int kRasterNext = 4207;
constexpr int kPaletteSelect = 4208;
constexpr int kTitleCursor = 4209;
constexpr int kWorldPreview = 4210;
constexpr int kGpuWorldPreview = 4211;

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

struct NativeCursorPreview {
    int width{};
    int height{};
    std::size_t res0_id{};
    std::vector<std::uint8_t> bgra;
};

NativeCursorPreview make_raster_preview(
    const rloft_native::NativeType1Raster& image,
    std::size_t id, const NativePalette6& palette,
    rloft_native::NativeRasterColorMode mode =
        rloft_native::NativeRasterColorMode::RawIndices) {
    if (image.width > 2048 || image.height > 2048)
        throw std::runtime_error("original raster exceeds inspector viewport");
    auto converted =
        rloft_native::colorize_native_raster(image,palette,0,mode);
    NativeCursorPreview output;
    output.width = converted.width;
    output.height = converted.height;
    output.res0_id = id;
    output.bgra = std::move(converted.bgra);
    return output;
}

NativeCursorPreview make_cursor_preview(
    const NativeMedia& media, const NativePalette6& palette) {
    auto resource = rloft_native::load_native_default_cursor(media);
    return make_raster_preview(
        resource.image,resource.res0_id,palette,
        rloft_native::NativeRasterColorMode::Code2TitleCursor);
}

NativeCursorPreview load_browser_preview(
    const NativeMedia& media, const NativePalette6& palette,
    const std::string& archive, std::size_t record, int direction,
    std::size_t& actual_record) {
    const auto result = direction == 0 ?
        rloft_native::load_native_raster_record(media,archive,record) :
        rloft_native::seek_native_raster_record(
            media,archive,record,direction > 0);
    actual_record = result.record_index;
    return make_raster_preview(result.raster,actual_record,palette);
}

struct State {
    HWND window{};
    HWND button{};
    HWND report_widget{};
    HWND archive_control{};
    HWND index_control{};
    HWND go_control{};
    HWND previous_control{};
    HWND next_control{};
    HWND palette_control{};
    HWND title_cursor_control{};
    HWND world_preview_button{};
    HWND gpu_world_preview_button{};
    unsigned palette_bank{12};
    bool auto_palette{true};
    bool title_cursor_active{true};
    std::string browsed_archive{"RES0"};
    std::size_t browsed_record{};
    std::unique_ptr<NativeMedia> media;
    rloft::BootFlow boot_flow;
    bool boot_route_ready{};
    NativeAssetReport audit;
    NativePalette6 palette;
    bool has_palette{};
    NativeCursorPreview cursor;
    std::wstring cursor_error;
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
        NativeCursorPreview preview;
        std::wstring cursor_info;
        if (palette_ok) {
            try {
                preview = make_cursor_preview(*media, palette);
                cursor_info = L"CODE.2 title cursor with original index remap (RES0 ID " +
                              std::to_wstring(preview.res0_id) + L", " +
                              std::to_wstring(preview.width) + L"x" +
                              std::to_wstring(preview.height) + L").";
            } catch (const std::exception& error) {
                cursor_info = L"Native EH type-1 cursor preview: " +
                              unicode(error.what());
            }
        } else {
            cursor_info = L"Native EH type-1 cursor preview requires valid COLORS.";
        }
        // Use the exact original CODE.2 supervisor route from the neutral
        // Shared/BootFlow module. This classifies source startup intent;
        // it DOES NOT import CHARS, run the movies, or enter gameplay.
        rloft::BootFlow resolved_boot;
        const bool boot_resources=media->contains("CODE.1") &&
            media->contains("CODE.2") && media->contains("INIT");
        std::wstring startup_info;
        if(boot_resources) {
            resolved_boot.boot();
            const bool chars_present=media->contains("CHARS");
            resolved_boot.finish_code2_startup(chars_present,false);
            startup_info=L"Original supervisor: "+unicode(
                rloft::to_string(resolved_boot.snapshot().stage))+
                L" (source CHARS "+(chars_present?std::wstring(L"present"):
                std::wstring(L"absent"))+L").\\r\\n"+
                L"Shared boot routing is wired; CODE.1/ANM GUI and party import remain pending.";
        } else {
            startup_info=L"Original supervisor unavailable: CODE.1/CODE.2/INIT required.";
        }
        // Commit the source and image only after the reader + audit succeeds.
        const auto output = format_report(*media, report) +
                            L"\r\n" + cursor_info + L"\r\n" +
                            startup_info + L"\r\n";
        state.boot_flow=std::move(resolved_boot);
        state.boot_route_ready=boot_resources;
        state.media=std::move(media);
        state.audit=std::move(report);
        state.palette=palette;
        state.has_palette=palette_ok;
        state.cursor=std::move(preview);
        state.browsed_archive="RES0";
        state.browsed_record=state.cursor.res0_id;
        state.title_cursor_active=true;
        state.palette_bank=12;
        state.auto_palette=true;
        if (state.palette_control)
            SendMessageW(state.palette_control,CB_SETCURSEL,0,0);
        state.cursor_error=cursor_info;
        if (state.index_control)
            SetWindowTextW(state.index_control,
                           std::to_wstring(state.browsed_record).c_str());
        state.status=output;
    } catch(const std::exception& error) {
        // Do not show another install's palette after a failed selection.
        state.boot_flow=rloft::BootFlow{};
        state.boot_route_ready=false;
        state.media.reset();
        state.has_palette=false;
        state.cursor=NativeCursorPreview{};
        state.title_cursor_active=false;
        state.cursor_error.clear();
        state.status=L"Cannot read selected original game media:\r\n"+
                     unicode(error.what())+
                     L"\r\nNo original files were written or extracted.";
    }
    if(state.report_widget)
        SetWindowTextW(state.report_widget,state.status.c_str());
    if(state.window)InvalidateRect(state.window,nullptr,TRUE);
}

// Original-art inspection: keep the source installation immutable and
// display typed data only after successfully decoding a complete EH1 record.
void browse(State& state, std::string archive, std::size_t index, int direction) {
    if (!state.media || !state.has_palette) return;
    try {
        // Decode the original indexed resource ONCE, then select a scene-
        // or callsite-appropriate palette. No RES archive-wide assumption.
        const auto resource=direction==0 ?
            rloft_native::load_native_raster_record(
                *state.media,archive,index) :
            rloft_native::seek_native_raster_record(
                *state.media,archive,index,direction>0);
        auto palette=state.palette;
        unsigned bank=state.palette_bank;
        std::wstring provenance=L"Manual COLORS bank " + std::to_wstring(bank);
        if(state.auto_palette) {
            const auto rule=rloft_native::resolve_native_palette(
                resource.archive,resource.record_index);
            if(rule.bank) {
                bank=*rule.bank;
                palette=rloft_native::decode_native_palette(
                    state.media->read("COLORS"),bank);
            }
            const wchar_t* quality=L"UNRESOLVED";
            switch(rule.evidence) {
                case rloft_native::NativePaletteEvidence::ExactCallSite:
                    quality=L"PROVEN ORIGINAL CALLSITE";break;
                case rloft_native::NativePaletteEvidence::LevelContext:
                    quality=L"ACTIVE LEVEL CONTEXT";break;
                case rloft_native::NativePaletteEvidence::ContextualPreview:
                    quality=L"CONTEXTUAL PREVIEW ONLY";break;
                case rloft_native::NativePaletteEvidence::Unresolved:
                    quality=L"UNRESOLVED — previous bank retained";break;
            }
            provenance=L"AUTO: " + std::wstring(quality) +
                L"; " + unicode(rule.reason);
        }
        auto preview=make_raster_preview(
            resource.raster,resource.record_index,palette);
        state.cursor=std::move(preview);
        state.palette=palette;
        state.palette_bank=bank;
        state.title_cursor_active=false;
        state.browsed_archive=resource.archive;
        state.browsed_record=resource.record_index;
        state.cursor_error=
            L"Original " + unicode(state.browsed_archive) +
            L" #" + std::to_wstring(state.browsed_record) +
            L" (" + std::to_wstring(state.cursor.width) +
            L"x" + std::to_wstring(state.cursor.height) +
            (resource.resource_type==6 ? L", EH6 world texture" :
                                         L", EH1 indexed raster") +
            L"), COLORS " + std::to_wstring(bank) +
            L". " + provenance + L".";
        if(state.index_control)
            SetWindowTextW(state.index_control,
                           std::to_wstring(resource.record_index).c_str());
    } catch(const std::exception& error) {
        state.cursor=NativeCursorPreview{};
        state.cursor_error=L"Original RES raster decode: "+unicode(error.what());
    }
    state.status=format_report(*state.media,state.audit) +
                 L"\r\n" + state.cursor_error + L"\r\n";
    if(state.report_widget)
        SetWindowTextW(state.report_widget,state.status.c_str());
    if(state.window)InvalidateRect(state.window,nullptr,TRUE);
}

void change_palette(State& state, unsigned bank) {
    if (!state.media || bank >= 15) return;
    try {
        const auto selected =
            rloft_native::decode_native_palette(state.media->read("COLORS"),bank);
        NativeCursorPreview preview;
        if (state.title_cursor_active) {
            preview=make_cursor_preview(*state.media,selected);
        } else {
            std::size_t actual{};
            preview=load_browser_preview(*state.media,selected,
                state.browsed_archive,state.browsed_record,0,actual);
        }
        state.palette=selected;
        state.palette_bank=bank;
        state.auto_palette=false;
        state.has_palette=true;
        state.cursor=std::move(preview);
        state.cursor_error=L"MANUAL bank override. Original indexed pixels with COLORS bank " +
            std::to_wstring(bank) +
            (state.title_cursor_active ?
             L" and CODE.2 title cursor remap." : L" (raw RES indices).");
    } catch (const std::exception& error) {
        state.cursor=NativeCursorPreview{};
        state.cursor_error=L"Palette/render error: "+unicode(error.what());
    }
    state.status=format_report(*state.media,state.audit) +
                 L"\r\n" + state.cursor_error + L"\r\n";
    if (state.report_widget)
        SetWindowTextW(state.report_widget,state.status.c_str());
    if (state.window) InvalidateRect(state.window,nullptr,TRUE);
}

void show_title_cursor(State& state) {
    if (!state.media || !state.has_palette) return;
    try {
        // Title cursor is an original CODE.2 menu callsite, not an unknown
        // resource-level auto palette. In auto mode its scene bank is 12.
        if(state.auto_palette && state.palette_bank!=12) {
            state.palette=rloft_native::decode_native_palette(
                state.media->read("COLORS"),12);
            state.palette_bank=12;
        }
        auto preview=make_cursor_preview(*state.media,state.palette);
        state.browsed_archive="RES0";
        state.browsed_record=preview.res0_id;
        state.title_cursor_active=true;
        state.cursor=std::move(preview);
        state.cursor_error=
            L"Original CODE.2 title cursor: remapped raster indices; "
            L"COLORS bank " + std::to_wstring(state.palette_bank) + L".";
        if(state.index_control)
            SetWindowTextW(state.index_control,
                std::to_wstring(state.browsed_record).c_str());
        if(state.archive_control)
            SendMessageW(state.archive_control,CB_SETCURSEL,0,0);
    } catch(const std::exception& error) {
        state.cursor=NativeCursorPreview{};
        state.cursor_error=L"Original menu cursor: "+unicode(error.what());
    }
    state.status=format_report(*state.media,state.audit) +
                 L"\r\n" + state.cursor_error + L"\r\n";
    if(state.report_widget)
        SetWindowTextW(state.report_widget,state.status.c_str());
    if(state.window)InvalidateRect(state.window,nullptr,TRUE);
}

void layout(State& state) {
    if(!state.window)return;
    RECT r{};GetClientRect(state.window,&r);
    if(state.button) MoveWindow(state.button,24,82,300,34,TRUE);
    if(state.gpu_world_preview_button)
        MoveWindow(state.gpu_world_preview_button,343,82,320,34,TRUE);
    if(state.world_preview_button)
        MoveWindow(state.world_preview_button,676,82,248,34,TRUE);
    if(state.report_widget) {
        const int left=24,top=132;
        const int width=std::max(200, int(r.right)-370);
        const int height=std::max(150, int(r.bottom)-166);
        MoveWindow(state.report_widget,left,top,width,height,TRUE);
    }
    const int x=std::max(245,int(r.right)-320);
    if(state.palette_control) MoveWindow(state.palette_control,x,418,120,320,TRUE);
    if(state.title_cursor_control) MoveWindow(state.title_cursor_control,x+132,418,108,27,TRUE);
    if(state.archive_control) MoveWindow(state.archive_control,x,453,97,220,TRUE);
    if(state.index_control) MoveWindow(state.index_control,x+105,453,72,26,TRUE);
    if(state.go_control) MoveWindow(state.go_control,x+184,453,56,26,TRUE);
    if(state.previous_control) MoveWindow(state.previous_control,x,486,114,26,TRUE);
    if(state.next_control) MoveWindow(state.next_control,x+126,486,114,26,TRUE);
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
    const std::wstring palette_label=L"ORIGINAL COLORS BANK " +
        std::to_wstring(state.palette_bank) +
        (state.auto_palette ? L" (AUTO CONTEXT)" : L" (MANUAL)") + L" DAC6";
    DrawTextW(dc,palette_label.c_str(),-1,&label,DT_SINGLELINE);
    if(state.has_palette) {
        constexpr int swatch=14;
        for(int index=0;index<256;++index) {
            const auto pos=3*index;
            const auto expand=[](std::uint8_t c) {
                return int((unsigned(c)*255U+31U)/63U);
            };
            auto color=CreateSolidBrush(RGB(expand(state.palette.rgb[pos]),
                                           expand(state.palette.rgb[pos+1]),
                                           expand(state.palette.rgb[pos+2])));
            RECT cell{x+(index%16)*swatch,181+(index/16)*swatch,
                      x+(index%16+1)*swatch,181+(index/16+1)*swatch};
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
    RECT cursor_title{x,520,rect.right-14,543};
    SetTextColor(dc,RGB(241,207,137));
    const std::wstring caption =
        L"RES " + unicode(state.browsed_archive) +
        L" #" + std::to_wstring(state.browsed_record) +
        (state.title_cursor_active ? L" — CODE.2 CURSOR" :
         state.browsed_archive=="RES4" ? L" — EH6 WORLD TEXTURE" :
         L" — EH1 INDEXED RASTER");
    DrawTextW(dc,caption.c_str(),-1,&cursor_title,DT_SINGLELINE|DT_END_ELLIPSIS);
    if (!state.cursor.bgra.empty()) {
        BITMAPINFO info{};
        info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth=state.cursor.width;
        info.bmiHeader.biHeight=-state.cursor.height;
        info.bmiHeader.biPlanes=1;
        info.bmiHeader.biBitCount=32;
        info.bmiHeader.biCompression=BI_RGB;
        const double scale=std::min({
            8.0,240.0/state.cursor.width,130.0/state.cursor.height});
        const int display_width=std::max(1,int(scale*state.cursor.width));
        const int display_height=std::max(1,int(scale*state.cursor.height));
        const int left=x+(240-display_width)/2;
        const int top=545+(130-display_height)/2;
        SetStretchBltMode(dc,COLORONCOLOR);
        StretchDIBits(dc,left,top,display_width,display_height,
                      0,0,state.cursor.width,state.cursor.height,
                      state.cursor.bgra.data(),&info,DIB_RGB_COLORS,SRCCOPY);
    } else {
        RECT unavailable{x,550,rect.right-20,670};
        SetTextColor(dc,RGB(175,184,193));
        DrawTextW(dc,L"Select a RES archive and record. EH1 pixels appear "
                     L"here; unsupported/non-image types are reported "
                     L"separately from archive structural validation.",
                  -1,&unavailable,DT_WORDBREAK);
    }
    RECT explanation{x,690,rect.right-14,rect.bottom-12};
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
            state->gpu_world_preview_button=CreateWindowExW(0,L"BUTTON",
                L"Explore Original World (3D GPU)",
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,
                343,82,320,34,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kGpuWorldPreview)),
                instance,nullptr);
            state->world_preview_button=CreateWindowExW(0,L"BUTTON",
                L"CODE.2 VGA reference view",
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,
                676,82,248,34,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kWorldPreview)),
                instance,nullptr);
            state->report_widget=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",state->status.c_str(),
                WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_READONLY|
                ES_AUTOVSCROLL,24,132,640,500,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kResults)),instance,nullptr);
            state->archive_control=CreateWindowExW(0,L"COMBOBOX",nullptr,
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL,
                800,453,97,220,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kArchiveSelect)),
                instance,nullptr);
            state->index_control=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"0",
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_NUMBER|ES_AUTOHSCROLL,
                906,453,72,26,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kRasterIndex)),
                instance,nullptr);
            state->go_control=CreateWindowExW(0,L"BUTTON",L"Go",
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,
                980,453,56,26,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kRasterGo)),
                instance,nullptr);
            state->previous_control=CreateWindowExW(0,L"BUTTON",L"Previous",
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,
                800,486,114,26,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kRasterPrevious)),
                instance,nullptr);
            state->next_control=CreateWindowExW(0,L"BUTTON",L"Next image",
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,
                925,486,114,26,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kRasterNext)),
                instance,nullptr);
            state->palette_control=CreateWindowExW(
                0,L"COMBOBOX",nullptr,
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL,
                800,418,120,320,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kPaletteSelect)),
                instance,nullptr);
            state->title_cursor_control=CreateWindowExW(
                0,L"BUTTON",L"Title cursor",
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,
                932,418,108,27,hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kTitleCursor)),
                instance,nullptr);
            if(!state->button||!state->report_widget||!state->archive_control||
               !state->index_control||!state->go_control||
               !state->previous_control||!state->next_control||
               !state->palette_control||!state->title_cursor_control||
               !state->world_preview_button)
                return -1;
            SendMessageW(state->palette_control,CB_ADDSTRING,0,
                reinterpret_cast<LPARAM>(L"AUTO (source context)"));
            for(unsigned bank=0;bank<15;++bank) {
                const std::wstring label=L"COLORS " + std::to_wstring(bank) +
                    (bank==8 ? L" · CODE.1" :
                     bank==12 ? L" · MENU" :
                     bank==9 || bank==14 ? L" · LOGO" : L"");
                SendMessageW(state->palette_control,CB_ADDSTRING,0,
                    reinterpret_cast<LPARAM>(label.c_str()));
            }
            SendMessageW(state->palette_control,CB_SETCURSEL,0,0);
            for(const auto* archive : {
                L"RES0", L"RES1", L"RES2", L"RES3", L"RES4",
                L"RES5", L"RES6", L"RES7", L"RES8", L"RES9", L"RES10"
            }) {
                SendMessageW(state->archive_control,CB_ADDSTRING,0,
                             reinterpret_cast<LPARAM>(archive));
            }
            SendMessageW(state->archive_control,CB_SETCURSEL,0,0);
            return 0;
        }
        case WM_SIZE:
            layout(*state);InvalidateRect(hwnd,nullptr,TRUE);return 0;
        case WM_COMMAND:
            if(LOWORD(wparam)==kGpuWorldPreview && HIWORD(wparam)==BN_CLICKED) {
                if(state->media) {
                    try {rloft_native::open_native_gpu_world_preview(hwnd,*state->media);}
                    catch(const std::exception& error) {
                        MessageBoxW(hwnd,unicode(error.what()).c_str(),
                            L"Accelerated original-world viewport unavailable",
                            MB_OK|MB_ICONWARNING);
                    }
                } else {
                    MessageBoxW(hwnd,L"Open your owned original game installation first.",
                        L"No original installation loaded",MB_OK|MB_ICONINFORMATION);
                }
                return 0;
            }
            if(LOWORD(wparam)==kWorldPreview && HIWORD(wparam)==BN_CLICKED) {
                if(state->media) {
                    try {
                        rloft_native::open_native_world_preview(hwnd,*state->media);
                    } catch(const std::exception& error) {
                        MessageBoxW(hwnd,unicode(error.what()).c_str(),
                            L"Original LEVELS.DAT preview unavailable",
                            MB_OK|MB_ICONWARNING);
                    }
                } else {
                    MessageBoxW(hwnd,L"Open your original Ravenloft installation first.",
                        L"No original installation loaded",MB_OK|MB_ICONINFORMATION);
                }
                return 0;
            }
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
            if (LOWORD(wparam)==kPaletteSelect &&
                HIWORD(wparam)==CBN_SELCHANGE) {
                const auto selection=SendMessageW(
                    state->palette_control,CB_GETCURSEL,0,0);
                if(selection==0) {
                    state->auto_palette=true;
                    if(state->title_cursor_active)
                        show_title_cursor(*state);
                    else
                        browse(*state,state->browsed_archive,
                               state->browsed_record,0);
                } else if(selection>0 && selection<=15) {
                    change_palette(*state,static_cast<unsigned>(selection-1));
                }
                return 0;
            }
            if (LOWORD(wparam)==kTitleCursor &&
                HIWORD(wparam)==BN_CLICKED) {
                show_title_cursor(*state);
                return 0;
            }
            if (LOWORD(wparam)==kRasterGo &&
                HIWORD(wparam)==BN_CLICKED) {
                if (state->media && state->has_palette) {
                    try {
                        wchar_t record_text[32]{};
                        GetWindowTextW(state->index_control,record_text,32);
                        const std::wstring input(record_text);
                        std::size_t position=0;
                        const auto id=std::stoull(input,&position,10);
                        if (input.empty()||position!=input.size())
                            throw std::invalid_argument("raster ID must be numeric");
                        browse(*state,state->browsed_archive,
                               static_cast<std::size_t>(id),0);
                    } catch(const std::exception& error) {
                        state->status=format_report(*state->media,state->audit) +
                            L"\r\nInvalid raster ID: " + unicode(error.what());
                        SetWindowTextW(state->report_widget,state->status.c_str());
                    }
                }
                return 0;
            }
            if ((LOWORD(wparam)==kRasterPrevious ||
                 LOWORD(wparam)==kRasterNext) &&
                 HIWORD(wparam)==BN_CLICKED) {
                if (state->media && state->has_palette) {
                    browse(*state,state->browsed_archive,
                        state->browsed_record,
                        LOWORD(wparam)==kRasterNext ? 1 : -1);
                }
                return 0;
            }
            if (LOWORD(wparam)==kArchiveSelect &&
                HIWORD(wparam)==CBN_SELCHANGE) {
                if(state->media && state->has_palette) {
                    wchar_t name[16]{};
                    const auto selected=SendMessageW(
                        state->archive_control,CB_GETCURSEL,0,0);
                    if (selected>=0) {
                        SendMessageW(state->archive_control,CB_GETLBTEXT,
                            static_cast<WPARAM>(selected),
                            reinterpret_cast<LPARAM>(name));
                        const std::wstring wide(name);
                        const std::string archive(wide.begin(),wide.end());
                        state->browsed_archive=archive;
                        state->browsed_record=0;
                        browse(*state,archive,0,0);
                        if(state->cursor.bgra.empty())
                            browse(*state,archive,0,1);
                    }
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

int inspect_headless(const fs::path& source, bool need_raster) {
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
        if (!(init&&res0&&t&&colors)) return 3;
        if (need_raster) {
            try {
                const auto palette = rloft_native::decode_native_palette(
                    media.read("COLORS"),12);
                static_cast<void>(make_cursor_preview(media,palette));
            } catch (const std::exception&) {
                return 4;
            }
        }
        return 0;
    } catch(...) {return 2;}
}

int run(HINSTANCE instance) {
    int argc=0;
    LPWSTR* argv=CommandLineToArgvW(GetCommandLineW(),&argc);
    if(!argv)return 2;
    std::wstring option=argc>1?argv[1]:L"";
    fs::path source=argc>2?fs::path(argv[2]):fs::path{};
    const bool headless=argc==3&&option==L"--verify-assets";
    const bool raster_test=argc==3&&option==L"--verify-raster";
    const bool from_arg=argc==3&&option==L"--install";
    const bool invalid=argc!=1&&!headless&&!raster_test&&!from_arg;
    LocalFree(argv);
    if(headless||raster_test)return inspect_headless(source,raster_test);
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
