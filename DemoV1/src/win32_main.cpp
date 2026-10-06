#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include "rloft/boot_flow.hpp"
#include "rloft/character_creation_presenter.hpp"
#include "rloft/game_source.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace {

using Clock = std::chrono::steady_clock;

std::wstring widen(std::string_view text) {
    if (text.empty()) return {};
    const int count = MultiByteToWideChar(
        CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    std::wstring out(static_cast<std::size_t>(count), L'\0');
    MultiByteToWideChar(
        CP_UTF8, 0, text.data(), static_cast<int>(text.size()), out.data(), count);
    return out;
}

class Host final {
public:
    bool create(HINSTANCE instance) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = &Host::window_proc;
        wc.hInstance = instance;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        wc.lpszClassName = L"RavenloftDemoV1Window";
        RegisterClassW(&wc);

        RECT rect{0, 0, 640, 800};
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
        hwnd_ = CreateWindowExW(
            0,
            wc.lpszClassName,
            L"Ravenloft: Strahd's Possession — Demo Build v1",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr,
            nullptr,
            instance,
            this);
        if (!hwnd_) return false;

        ShowWindow(hwnd_, SW_SHOW);
        UpdateWindow(hwnd_);
        return true;
    }

    void present(const rloft::IndexedFramebuffer& framebuffer,
                 const rloft::Palette6& palette) {
        const auto& src = framebuffer.pixels();
        bgra_.resize(src.size());

        for (std::size_t i = 0; i < src.size(); ++i) {
            const auto index = static_cast<std::size_t>(src[i]) * 3U;
            const auto r = expand6(palette.rgb[index]);
            const auto g = expand6(palette.rgb[index + 1U]);
            const auto b = expand6(palette.rgb[index + 2U]);
            bgra_[i] = static_cast<std::uint32_t>(b) |
                       (static_cast<std::uint32_t>(g) << 8U) |
                       (static_cast<std::uint32_t>(r) << 16U);
        }

        InvalidateRect(hwnd_, nullptr, FALSE);
        UpdateWindow(hwnd_);
    }

    bool pump() {
        MSG msg{};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit_ = true;
                return false;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        return !quit_;
    }

    bool wait_until(Clock::time_point deadline, bool allow_escape) {
        while (Clock::now() < deadline) {
            if (!pump()) return false;
            if (allow_escape && consume_escape()) return false;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return true;
    }

    bool pause_frame(std::chrono::duration<double> duration) {
        return wait_until(
            Clock::now() + std::chrono::duration_cast<Clock::duration>(duration),
            false);
    }

    bool consume_escape() {
        const bool result = escape_pressed_;
        escape_pressed_ = false;
        return result;
    }

    void set_menu_mode(bool enabled) {
        menu_mode_ = enabled;
        if (!enabled) {
            selection_ = -1;
            clicked_.reset();
        }
    }

    int selection() const noexcept { return selection_; }

    std::optional<int> consume_click() {
        auto result = clicked_;
        clicked_.reset();
        return result;
    }

    bool quit_requested() const noexcept { return quit_; }
    HWND hwnd() const noexcept { return hwnd_; }

private:
    static std::uint8_t expand6(std::uint8_t value) {
        return static_cast<std::uint8_t>(
            (static_cast<unsigned>(value) * 255U + 31U) / 63U);
    }

    RECT render_rect() const {
        RECT client{};
        GetClientRect(hwnd_, &client);
        const int cw = client.right - client.left;
        const int ch = client.bottom - client.top;
        const double scale = std::min(cw / 320.0, ch / 400.0);
        const int width = std::max(1, static_cast<int>(320.0 * scale));
        const int height = std::max(1, static_cast<int>(400.0 * scale));
        const int left = (cw - width) / 2;
        const int top = (ch - height) / 2;
        return RECT{left, top, left + width, top + height};
    }

    int native_y_from_client(int client_y) const {
        const auto dst = render_rect();
        if (client_y < dst.top || client_y >= dst.bottom) return -1;
        const auto height = std::max<LONG>(1, dst.bottom - dst.top);
        return (client_y - dst.top) * 400 / height;
    }

    void paint() {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(hwnd_, &ps);
        RECT client{};
        GetClientRect(hwnd_, &client);
        FillRect(dc, &client, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

        if (!bgra_.empty()) {
            BITMAPINFO info{};
            info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            info.bmiHeader.biWidth = 320;
            info.bmiHeader.biHeight = -400;
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;

            const auto dst = render_rect();
            SetStretchBltMode(dc, COLORONCOLOR);
            StretchDIBits(
                dc,
                dst.left,
                dst.top,
                dst.right - dst.left,
                dst.bottom - dst.top,
                0,
                0,
                320,
                400,
                bgra_.data(),
                &info,
                DIB_RGB_COLORS,
                SRCCOPY);
        }

        EndPaint(hwnd_, &ps);
    }

    LRESULT handle(UINT msg, WPARAM wparam, LPARAM lparam) {
        switch (msg) {
            case WM_PAINT:
                paint();
                return 0;

            case WM_ERASEBKGND:
                return 1;

            case WM_KEYDOWN:
                if (wparam == VK_ESCAPE) escape_pressed_ = true;
                return 0;

            case WM_MOUSEMOVE:
                if (menu_mode_) {
                    const int y = native_y_from_client(GET_Y_LPARAM(lparam));
                    selection_ =
                        rloft::StartupPresenter::menu_selection_from_y(y);
                }
                return 0;

            case WM_LBUTTONDOWN:
                if (menu_mode_ && selection_ >= 0) {
                    clicked_ = selection_;
                }
                return 0;

            case WM_CLOSE:
                DestroyWindow(hwnd_);
                return 0;

            case WM_DESTROY:
                quit_ = true;
                PostQuitMessage(0);
                return 0;
        }

        return DefWindowProcW(hwnd_, msg, wparam, lparam);
    }

    static LRESULT CALLBACK window_proc(
        HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        Host* self =
            reinterpret_cast<Host*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

        if (msg == WM_NCCREATE) {
            const auto* create =
                reinterpret_cast<const CREATESTRUCTW*>(lparam);
            self = static_cast<Host*>(create->lpCreateParams);
            self->hwnd_ = hwnd;
            SetWindowLongPtrW(
                hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }

        return self
            ? self->handle(msg, wparam, lparam)
            : DefWindowProcW(hwnd, msg, wparam, lparam);
    }

    HWND hwnd_{};
    std::vector<std::uint32_t> bgra_;
    bool quit_{false};
    bool escape_pressed_{false};
    bool menu_mode_{false};
    int selection_{-1};
    std::optional<int> clicked_{};
};

bool fade_in(Host& host,
             rloft::StartupPresenter& presenter,
             const rloft::Palette6& palette) {
    // CODE.2 0x322EC applies factors 1..8 with shift 3.
    for (int factor = 1; factor <= 8; ++factor) {
        host.present(
            presenter.framebuffer(),
            rloft::StartupPresenter::fade_factor(palette, factor));
        if (!host.pause_frame(std::chrono::duration<double>(1.0 / 70.0))) {
            return false;
        }
    }
    return true;
}

bool fade_out(Host& host,
              rloft::StartupPresenter& presenter,
              const rloft::Palette6& palette) {
    // CODE.2 0x32220 applies factors 7..0 with shift 3.
    for (int factor = 7; factor >= 0; --factor) {
        host.present(
            presenter.framebuffer(),
            rloft::StartupPresenter::fade_factor(palette, factor));
        if (!host.pause_frame(std::chrono::duration<double>(1.0 / 70.0))) {
            return false;
        }
    }
    return true;
}

Clock::duration pit_ticks(int ticks) {
    return std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>(
            ticks / rloft::StartupPresenter::kPitHz));
}

std::optional<std::filesystem::path> command_line_source() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return std::nullopt;

    std::optional<std::filesystem::path> result;
    if (argc >= 2) result = std::filesystem::path(argv[1]);
    LocalFree(argv);
    return result;
}

int run(HINSTANCE instance) {
    rloft::GameSource source;

    if (const auto explicit_path = command_line_source()) {
        source = rloft::GameSource::open(*explicit_path);
    } else {
        auto found = rloft::GameSource::auto_locate();
        if (!found) {
            MessageBoxW(
                nullptr,
                L"Ravenloft was not located automatically.\n\n"
                L"Install an owned copy, set RLOFT_GAME_DIR, or pass the "
                L"installation directory/game.gog as the first command-line "
                L"argument.",
                L"Ravenloft Demo Build v1",
                MB_OK | MB_ICONERROR);
            return 2;
        }
        source = std::move(*found);
    }

    const bool chars_present = source.runtime_file("CHARS").has_value();
    if (chars_present) {
        MessageBoxW(
            nullptr,
            L"A legacy CHARS transfer file is present. The CHARS-present "
            L"CODE.2 startup branch is not implemented in Demo v1 yet.\n\n"
            L"The file has not been modified or deleted.",
            L"Ravenloft Demo Build v1",
            MB_OK | MB_ICONWARNING);
        return 3;
    }

    rloft::RavenloftBootAssets assets(std::move(source));
    rloft::StartupPresenter presenter(assets);
    rloft::BootFlow flow;
    flow.boot();
    flow.finish_code2_startup(false, false);

    Host host;
    if (!host.create(instance)) {
        throw std::runtime_error("Unable to create presentation window");
    }

    rloft::Palette6 black{};
    host.present(presenter.framebuffer(), black);

    bool skipped = false;
    const auto palette14 = presenter.palette14();
    const auto palette9 = presenter.palette9();
    const auto palette12 = presenter.palette12();

    auto deadline = Clock::now() + pit_ticks(120);
    presenter.first_logo();
    if (!fade_in(host, presenter, palette14)) return 0;
    if (!host.wait_until(deadline, true)) {
        if (host.quit_requested()) return 0;
        skipped = true;
    }

    if (!skipped) {
        if (!fade_out(host, presenter, palette14)) return 0;
        deadline = Clock::now() + pit_ticks(106);
        presenter.second_logo();
        if (!fade_in(host, presenter, palette14)) return 0;
        if (!host.wait_until(deadline, true)) {
            if (host.quit_requested()) return 0;
            skipped = true;
        }
    }

    if (!skipped) {
        if (!fade_out(host, presenter, palette14)) return 0;
        deadline = Clock::now() + pit_ticks(121);
        presenter.third_logo();
        if (!fade_in(host, presenter, palette9)) return 0;
        if (!host.wait_until(deadline, true)) {
            if (host.quit_requested()) return 0;
            skipped = true;
        }
    }

    if (!skipped) {
        if (!fade_out(host, presenter, palette9)) return 0;
        deadline = Clock::now() + pit_ticks(55);
        presenter.title_background();
        if (!fade_in(host, presenter, palette12)) return 0;
        if (!host.wait_until(deadline, true)) {
            if (host.quit_requested()) return 0;
            skipped = true;
        }
    }

    if (!skipped) {
        // CODE.2 0x191C0 runs steps 1..14. The original loop is driven by
        // page presentation rather than the IRQ0 wait routine; the host uses
        // one nominal VGA 70 Hz presentation interval per visible step.
        for (int step = 1; step <= 14; ++step) {
            presenter.menu_animation_frame(step);
            host.present(presenter.framebuffer(), palette12);
            if (!host.pause_frame(
                    std::chrono::duration<double>(1.0 / 70.0))) {
                return 0;
            }
        }
        flow.finish_cold_start_presentation();
    } else {
        flow.skip_cold_start_presentation();
    }

    presenter.menu_frame(-1);
    host.present(presenter.framebuffer(), palette12);
    host.set_menu_mode(true);
    int drawn_selection = -1;

    while (host.pump()) {
        const int selection = host.selection();
        if (selection != drawn_selection) {
            drawn_selection = selection;
            presenter.menu_frame(selection);
            host.present(presenter.framebuffer(), palette12);
        }

        if (const auto click = host.consume_click()) {
            flow.select_startup_menu(*click);

            if (*click == rloft::BootFlow::kCharacterWorkflowMenuIndex) {
                if (!fade_out(host, presenter, palette12)) return 0;
                flow.supervisor_dispatch();

                host.set_menu_mode(false);

                rloft::CharacterCreationPresenter character_presenter(assets);
                character_presenter.first_screen();
                host.present(
                    character_presenter.framebuffer(),
                    character_presenter.palette());

                SetWindowTextW(
                    host.hwnd(),
                    L"Ravenloft: Strahd's Possession — Character Creation");

                // Demo-v1 milestone: stay on CODE.1 state 0. The next unit
                // restores the observable 0x45FC opening animation and then
                // wires the original state-0 input/hover behavior.
                while (host.pump()) {
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(1));
                }
                return 0;
            }

            if (*click == 3) {
                if (!fade_out(host, presenter, palette12)) return 0;
                flow.supervisor_dispatch();
                return 0;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    try {
        return run(instance);
    } catch (const std::exception& exc) {
        const auto text = widen(exc.what());
        MessageBoxW(
            nullptr,
            text.c_str(),
            L"Ravenloft Demo Build v1",
            MB_OK | MB_ICONERROR);
        return 1;
    }
}
#endif
