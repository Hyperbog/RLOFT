#ifdef _WIN32
// Windows-first native-engine shell. Separate from DemoV1 and NOT a playable
// game: authentic install reader, immutable source-data inventory, and a
// clearly labeled synthetic VGA presentation-backend self-test.
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <objbase.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "rloft/game_source.hpp"
#include "rloft_native/cpu_reference_presenter.hpp"
#include "rloft_native/install_probe.hpp"

#ifndef RLOFT_NATIVE_SOURCE_SHA
#define RLOFT_NATIVE_SOURCE_SHA "local-unversioned"
#endif

namespace {
constexpr int kSelectId = 1001;
constexpr int kFrameWidth = 320;
constexpr int kFrameHeight = 400;

std::wstring widen(std::string_view source) {
    if (source.empty()) return {};
    const auto bytes = static_cast<int>(source.size());
    const int n = MultiByteToWideChar(CP_UTF8, 0, source.data(), bytes, nullptr, 0);
    if (n <= 0) return L"(message encoding unavailable)";
    std::wstring out(static_cast<std::size_t>(n), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, source.data(), bytes, out.data(), n);
    return out;
}

std::optional<std::filesystem::path> browse_install(HWND owner) {
    BROWSEINFOW info{};
    info.hwndOwner = owner;
    info.lpszTitle = L"Select your original Ravenloft: Strahd's Possession installation";
    info.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    PIDLIST_ABSOLUTE id = SHBrowseForFolderW(&info);
    if (!id) return std::nullopt;
    // Legacy SHGetPathFromIDListW has MAX_PATH output contract. Limit this
    // folder picker to the standard path buffer; command-line --install
    // supports paths supplied through the native Unicode filesystem API.
    std::array<wchar_t, MAX_PATH> pathname{};
    const bool ok = SHGetPathFromIDListW(id, pathname.data()) != FALSE;
    CoTaskMemFree(id);
    if (!ok) return std::nullopt;
    return std::filesystem::path(pathname.data());
}

struct State final {
    HWND window{};
    HWND select_button{};
    rloft_native::IndexedFrame diagnostic;
    std::vector<std::uint8_t> bgra;
    std::wstring status{L"No game installation selected.\nChoose the folder containing your owned original game."};
    std::wstring media_kind{L"Not selected"};
    std::wstring selected_folder{L"(none)"};
    bool signature_ok{};
};

void prepare_diagnostic(State& state) {
    state.diagnostic.pixels.resize(kFrameWidth * kFrameHeight);
    // Synthetic, unmistakable color-grid diagnostic. NEVER original art.
    for (unsigned y = 0; y < static_cast<unsigned>(kFrameHeight); ++y)
        for (unsigned x = 0; x < static_cast<unsigned>(kFrameWidth); ++x)
            state.diagnostic.pixels[y * kFrameWidth + x] =
                static_cast<std::uint8_t>(((x / 40U) + 2U * (y / 50U)) % 16U);
    for (unsigned i = 0; i < 256; ++i) {
        state.diagnostic.dac6[i * 3U]     = static_cast<std::uint8_t>((i * 13U) % 64U);
        state.diagnostic.dac6[i * 3U + 1] = static_cast<std::uint8_t>((i * 23U) % 64U);
        state.diagnostic.dac6[i * 3U + 2] = static_cast<std::uint8_t>((i * 37U) % 64U);
    }
    rloft_native::CpuReferencePresenter cpu;
    cpu.present(state.diagnostic);
    state.bgra.resize(cpu.rgba().size());
    const auto& rgba = cpu.rgba();
    for (std::size_t i = 0; i < rgba.size(); i += 4) {
        state.bgra[i] = rgba[i + 2];
        state.bgra[i + 1] = rgba[i + 1];
        state.bgra[i + 2] = rgba[i];
        state.bgra[i + 3] = 255;
    }
}

void inspect(State& state, const rloft::GameSource& original) {
    const auto report = rloft_native::inspect_original_install(
        [&original](std::string_view file) { return original.contains(file); },
        [&original](std::string_view file) { return original.read(file); });
    state.signature_ok = report.minimum_media_present;
    state.media_kind = widen(rloft::to_string(original.kind()));
    state.selected_folder = original.location().install_root.wstring();
    std::wostringstream out;
    out << L"Original installation (READ ONLY)\n"
        << L"Version/patch: UNVERIFIED\n"
        << L"Minimum media signature: "
        << (report.minimum_media_present ? L"PASS" : L"INCOMPLETE")
        << L"\n\n";
    for (const auto& file : report.files) {
        out << (file.present ? L"[+] " : L"[-] ") << widen(file.filename);
        if (file.required_for_probe) out << L"  (minimum signature)";
        out << L"\n";
    }
    out << L"\nThis is a native ENGINE BOOTSTRAP, not a playable game.\n"
        << L"No game data or saves have been written.";
    state.status = out.str();
}

void choose(State& state, const std::filesystem::path& path) {
    state.signature_ok = false;
    try {
        auto media = rloft::GameSource::open(path);
        inspect(state, media);
    } catch (const std::exception& error) {
        state.selected_folder = path.wstring();
        state.media_kind = L"Unsupported or missing installation";
        state.status = L"Original media could not be read:\n" + widen(error.what())
                     + L"\n\nNo original files were modified.";
    }
    if (state.window) InvalidateRect(state.window, nullptr, TRUE);
}

void text_at(HDC dc, const std::wstring& str, RECT rect, unsigned flags = DT_WORDBREAK) {
    DrawTextW(dc, str.c_str(), -1, &rect, flags);
}

void paint(State& state, HDC dc, const RECT& client) {
    HBRUSH background = CreateSolidBrush(RGB(24, 27, 35));
    FillRect(dc, &client, background);
    DeleteObject(background);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(235, 238, 245));
    RECT title{20, 18, client.right - 20, 64};
    text_at(dc, L"RAVENLOFT NATIVE ENGINE — WINDOWS BOOTSTRAP", title, DT_SINGLELINE);
    SetTextColor(dc, RGB(177, 187, 205));
    RECT subtitle{20, 48, client.right - 20, 95};
    text_at(dc, L"Original installation reader | Revision " + widen(RLOFT_NATIVE_SOURCE_SHA) +
        L" | NOT PLAYABLE YET", subtitle, DT_SINGLELINE | DT_END_ELLIPSIS);
    const int split = std::max(350, static_cast<int>(client.right / 2));
    const int left_width = std::max(1, split - 40);
    const int left_height = std::max(1, static_cast<int>(client.bottom - 174));
    const auto box = rloft_native::fit_original_aspect(
        static_cast<unsigned>(left_width), static_cast<unsigned>(left_height));
    const int x = 20 + static_cast<int>(box.x);
    const int y = 128 + static_cast<int>(box.y);
    BITMAPINFO bits{};
    bits.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bits.bmiHeader.biWidth = kFrameWidth;
    bits.bmiHeader.biHeight = -kFrameHeight;
    bits.bmiHeader.biPlanes = 1;
    bits.bmiHeader.biBitCount = 32;
    bits.bmiHeader.biCompression = BI_RGB;
    SetStretchBltMode(dc, COLORONCOLOR);
    StretchDIBits(dc, x, y, static_cast<int>(box.width), static_cast<int>(box.height),
                  0, 0, kFrameWidth, kFrameHeight, state.bgra.data(),
                  &bits, DIB_RGB_COLORS, SRCCOPY);
    SetTextColor(dc, RGB(240, 183, 125));
    RECT caption{20, client.bottom - 38, split - 8, client.bottom - 8};
    text_at(dc, L"Synthetic indexed-frame diagnostic — NOT game graphics",
            caption, DT_SINGLELINE | DT_END_ELLIPSIS);
    SetTextColor(dc, RGB(226, 230, 236));
    RECT right{split + 10, 136, client.right - 14, client.bottom - 12};
    std::wstring report = L"Media: " + state.media_kind +
        L"\nSource: " + state.selected_folder + L"\n\n" + state.status;
    text_at(dc, report, right, DT_WORDBREAK | DT_NOPREFIX);
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    State* state = reinterpret_cast<State*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        const auto* create = reinterpret_cast<const CREATESTRUCTW*>(lparam);
        state = static_cast<State*>(create->lpCreateParams);
        state->window = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
        return TRUE;
    }
    if (!state) return DefWindowProcW(hwnd, message, wparam, lparam);
    switch (message) {
        case WM_CREATE:
            state->select_button = CreateWindowExW(
                0, L"BUTTON", L"Select original install folder...",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                20, 88, 280, 32, hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kSelectId)),
                reinterpret_cast<LPCREATESTRUCTW>(lparam)->hInstance, nullptr);
            return state->select_button ? 0 : -1;
        case WM_COMMAND:
            if (LOWORD(wparam) == kSelectId && HIWORD(wparam) == BN_CLICKED) {
                if (auto selected = browse_install(hwnd)) choose(*state, *selected);
                return 0;
            }
            break;
        case WM_SIZE:
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC dc = BeginPaint(hwnd, &ps);
            RECT rect{};
            GetClientRect(hwnd, &rect);
            paint(*state, dc, rect);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default: break;
    }
    return DefWindowProcW(hwnd, message, wparam, lparam);
}

int self_test() {
    State state;
    prepare_diagnostic(state);
    const auto id = rloft_native::frame_identity(state.diagnostic);
    return state.bgra.size() == 320U * 400U * 4U &&
           id.pixels_fnv1a64 != 0 && id.palette_fnv1a64 != 0
           ? 0 : 1;
}

int run(HINSTANCE instance) {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return 2;
    if (argc == 2 && std::wstring_view(argv[1]) == L"--self-test") {
        LocalFree(argv);
        return self_test();
    }
    std::optional<std::filesystem::path> initial;
    if (argc == 3 && std::wstring_view(argv[1]) == L"--install")
        initial = std::filesystem::path(argv[2]);
    else if (argc != 1) {
        LocalFree(argv);
        MessageBoxW(nullptr, L"Usage: rloft_native_bootstrap.exe [--install <folder>]\n"
                    L"Or: rloft_native_bootstrap.exe --self-test",
                    L"Native engine bootstrap arguments", MB_ICONERROR | MB_OK);
        return 64;
    }
    LocalFree(argv);
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    State state;
    prepare_diagnostic(state);
    const wchar_t* klass = L"RloftNativeBootstrapWindow";
    WNDCLASSW window{};
    window.lpfnWndProc = window_proc;
    window.hInstance = instance;
    window.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    window.lpszClassName = klass;
    if (!RegisterClassW(&window) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        return 2;
    HWND handle = CreateWindowExW(0, klass,
        L"Ravenloft Native Engine — Windows Bootstrap (not gameplay)",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 1080, 750,
        nullptr, nullptr, instance, &state);
    if (!handle) {
        if (SUCCEEDED(com)) CoUninitialize();
        return 2;
    }
    if (initial) choose(state, *initial);
    else if (auto found = rloft::GameSource::auto_locate()) inspect(state, *found);
    ShowWindow(handle, SW_SHOW);
    UpdateWindow(handle);
    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    if (SUCCEEDED(com)) CoUninitialize();
    return static_cast<int>(message.wParam);
}
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    try { return run(instance); }
    catch (const std::exception& error) {
        MessageBoxW(nullptr, widen(error.what()).c_str(),
                    L"Native engine bootstrap error", MB_OK | MB_ICONERROR);
        return 2;
    }
}
#endif
