#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <deque>
#include <exception>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include "rloft/anm_cinematic.hpp"
#include "rloft/boot_flow.hpp"
#include "rloft/character_creation_flow.hpp"
#include "rloft/character_creation_presenter.hpp"
#include "rloft/character_stats.hpp"
#include "rloft/character_transfer.hpp"
#include "rloft/exploration_legacy_input.hpp"
#include "rloft/exploration_presenter.hpp"
#include "rloft/exploration_runtime.hpp"
#include "rloft/top_edge_menu_presenter.hpp"
#include "rloft/game_source.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"
#include "rloft/sound_config.hpp"
#include "rloft/voc_decoder.hpp"
#include "rloft/win32_audio.hpp"
#include "rloft/win32_midi.hpp"
#include "rloft/xmidi.hpp"

namespace {

using Clock = std::chrono::steady_clock;

// VGA mode timing used by Ravenloft's 320x400 planar presentation path.
// CODE.1 AF64 programs the CRTC start address and then AF34 waits for the
// next vertical retrace. The same retrace cadence drives DAC burst pacing.
constexpr double kVgaRefreshHz = 70.0863;

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
    ~Host() {
        if (frame_timer_) {
            CloseHandle(frame_timer_);
        }
    }

    bool create(HINSTANCE instance) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = &Host::window_proc;
        wc.hInstance = instance;
        // CODE.2/CODE.1 render their own RES0 cursor into the framebuffer.
        // Leave the client cursor unmanaged by the window class so the host
        // never paints a second Windows arrow over the game.
        wc.hCursor = nullptr;
        wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        wc.lpszClassName = L"RavenloftDemoV1Window";
        RegisterClassW(&wc);

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
        // CODE.1 synchronizes every page switch to VGA retrace. Use a
        // high-resolution waitable timer so the native host can preserve the
        // ~14.27 ms retrace period without 1 ms polling/scheduler jitter.
        frame_timer_ = CreateWaitableTimerExW(
            nullptr,
            nullptr,
            CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
            TIMER_ALL_ACCESS);
        if (!frame_timer_) {
            frame_timer_ = CreateWaitableTimerW(
                nullptr,
                TRUE,
                nullptr);
        }

        // The native framebuffer is 320x400, but the VGA mode was viewed on
        // a 4:3 display with non-square pixels. Present it at 4:3 rather than
        // treating framebuffer samples as square host pixels.
        RECT rect{0, 0, 640, 480};
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
        hwnd_ = CreateWindowExW(
            0,
            wc.lpszClassName,
            L"Ravenloft: Strahd's Possession - Demo Build v1",
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

    void set_game_cursor(rloft::Type1Raster cursor) {
        if (cursor.count <= 0 || cursor.width <= 0 || cursor.height <= 0) {
            throw std::runtime_error(
                "Ravenloft cursor resource has invalid dimensions");
        }
        game_cursor_ = std::move(cursor);
        refresh_pointer_overlay();
    }

    void present(const rloft::IndexedFramebuffer& framebuffer,
                 const rloft::Palette6& palette) {
        base_indices_ = framebuffer.pixels();
        current_palette_ = palette;
        rebuild_bgra();

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
        using HundredNanoseconds =
            std::chrono::duration<LONGLONG, std::ratio<1, 10000000>>;

        for (;;) {
            if (!pump()) return false;
            if (allow_escape && consume_escape()) return false;

            const auto now = Clock::now();
            if (now >= deadline) return true;

            if (!frame_timer_) {
                std::this_thread::yield();
                continue;
            }

            const auto remaining =
                std::chrono::duration_cast<HundredNanoseconds>(
                    deadline - now);
            LARGE_INTEGER due{};
            due.QuadPart = -std::max<LONGLONG>(
                1,
                remaining.count());

            if (!SetWaitableTimer(
                    frame_timer_,
                    &due,
                    0,
                    nullptr,
                    nullptr,
                    FALSE)) {
                std::this_thread::yield();
                continue;
            }

            const HANDLE handles[] = {frame_timer_};
            const DWORD result = MsgWaitForMultipleObjectsEx(
                1,
                handles,
                INFINITE,
                QS_ALLINPUT,
                MWMO_INPUTAVAILABLE);

            if (result == WAIT_FAILED) {
                std::this_thread::yield();
            }
            // WAIT_OBJECT_0 means the high-resolution deadline fired.
            // WAIT_OBJECT_0 + 1 means window/input messages arrived; loop,
            // pump them, and arm the remaining interval again.
        }
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
            if (!character_mode_) character_pointer_.reset();
        } else {
            update_pointer_from_system_cursor();
        }
        refresh_pointer_overlay();
    }

    void set_character_mode(bool enabled) {
        character_mode_ = enabled;
        if (!enabled) {
            character_click_.reset();
            if (!menu_mode_) character_pointer_.reset();
        } else {
            update_pointer_from_system_cursor();
        }
        refresh_pointer_overlay();
    }

    void set_exploration_input(
        rloft::ExplorationLegacyInputController* controller) noexcept {
        if (exploration_input_ && exploration_input_ != controller) {
            exploration_input_->clear();
        }
        exploration_input_ = controller;
        exploration_click_.reset();

        if (!exploration_input_) {
            if (!menu_mode_ && !character_mode_) {
                character_pointer_.reset();
            }
            refresh_pointer_overlay();
            return;
        }

        exploration_input_->clear();
        update_pointer_from_system_cursor();
        refresh_pointer_overlay();
    }

    int selection() const noexcept { return selection_; }

    std::optional<int> consume_click() {
        auto result = clicked_;
        clicked_.reset();
        return result;
    }

    struct NativePoint {
        int row{};
        int column{};
    };

    std::optional<NativePoint> consume_character_click() {
        auto result = character_click_;
        character_click_.reset();
        return result;
    }

    void clear_character_click() noexcept {
        character_click_.reset();
    }

    [[nodiscard]] std::optional<NativePoint> character_pointer() const {
        return character_pointer_;
    }

    [[nodiscard]] std::optional<NativePoint> exploration_pointer() const {
        return character_pointer_;
    }

    std::optional<NativePoint> consume_exploration_click() {
        auto result = exploration_click_;
        exploration_click_.reset();
        return result;
    }

    std::optional<int> consume_key_event() {
        if (key_events_.empty()) return std::nullopt;
        const int value = key_events_.front();
        key_events_.pop_front();
        return value;
    }

    void clear_key_events() {
        key_events_.clear();
    }

    bool quit_requested() const noexcept { return quit_; }
    HWND hwnd() const noexcept { return hwnd_; }

private:
    static std::optional<std::uint8_t> legacy_scan_from_vk(
        WPARAM key) noexcept {
        using Input = rloft::ExplorationLegacyInputController;
        switch (key) {
            case VK_HOME:
            case VK_NUMPAD7:
                return Input::kHome;
            case VK_UP:
            case VK_NUMPAD8:
                return Input::kUp;
            case VK_PRIOR:
            case VK_NUMPAD9:
                return Input::kPageUp;
            case VK_LEFT:
            case VK_NUMPAD4:
                return Input::kLeft;
            case VK_CLEAR:
            case VK_NUMPAD5:
                return Input::kCenter;
            case VK_RIGHT:
            case VK_NUMPAD6:
                return Input::kRight;
            case VK_END:
            case VK_NUMPAD1:
                return Input::kEnd;
            case VK_DOWN:
            case VK_NUMPAD2:
                return Input::kDown;
            case VK_NEXT:
            case VK_NUMPAD3:
                return Input::kPageDown;
            default:
                return std::nullopt;
        }
    }

    static std::uint8_t expand6(std::uint8_t value) {
        return static_cast<std::uint8_t>(
            (static_cast<unsigned>(value) * 255U + 31U) / 63U);
    }

    std::uint32_t palette_bgra(std::uint8_t value) const {
        const auto index = static_cast<std::size_t>(value) * 3U;
        const auto r = expand6(current_palette_.rgb[index]);
        const auto g = expand6(current_palette_.rgb[index + 1U]);
        const auto b = expand6(current_palette_.rgb[index + 2U]);
        return static_cast<std::uint32_t>(b) |
               (static_cast<std::uint32_t>(g) << 8U) |
               (static_cast<std::uint32_t>(r) << 16U);
    }

    void rebuild_bgra() {
        bgra_.resize(base_indices_.size());
        for (std::size_t i = 0; i < base_indices_.size(); ++i) {
            bgra_[i] = palette_bgra(base_indices_[i]);
        }

        if (!(menu_mode_ || character_mode_ || exploration_input_) ||
            !character_pointer_ ||
            !game_cursor_ ||
            game_cursor_->count <= 0) {
            return;
        }

        const auto& cursor = *game_cursor_;
        const auto frame_stride =
            static_cast<std::size_t>(cursor.width) * cursor.height;
        const auto frame_at = 0U * frame_stride;
        for (int sy = 0; sy < cursor.height; ++sy) {
            const int row = character_pointer_->row + sy;
            if (row < 0 || row >= 400) continue;
            for (int sx = 0; sx < cursor.width; ++sx) {
                const int column = character_pointer_->column + sx;
                if (column < 0 || column >= 320) continue;

                const auto value = cursor.pixels[
                    frame_at +
                    static_cast<std::size_t>(sy) * cursor.width +
                    static_cast<std::size_t>(sx)];
                if (cursor.transparent_index != 0xFF &&
                    value == cursor.transparent_index) {
                    continue;
                }

                const auto at =
                    static_cast<std::size_t>(row) * 320U +
                    static_cast<std::size_t>(column);
                if (at < bgra_.size()) {
                    bgra_[at] = palette_bgra(value);
                }
            }
        }
    }

    void refresh_pointer_overlay() {
        if (base_indices_.empty()) return;
        rebuild_bgra();

        // Mouse movement can generate a very dense WM_MOUSEMOVE stream.
        // Queue one repaint and let the normal message pump coalesce it rather
        // than forcing a synchronous full-window repaint for every pixel.
        InvalidateRect(hwnd_, nullptr, FALSE);
    }

    void update_pointer_from_system_cursor() {
        if (!hwnd_) return;
        POINT point{};
        if (!GetCursorPos(&point) || !ScreenToClient(hwnd_, &point)) {
            return;
        }

        const int row = native_y_from_client(point.y);
        const int column = native_x_from_client(point.x);
        if (row >= 0 && column >= 0) {
            character_pointer_ = NativePoint{row, column};
        } else {
            character_pointer_.reset();
        }
    }

    RECT render_rect() const {
        RECT client{};
        GetClientRect(hwnd_, &client);
        const int cw = client.right - client.left;
        const int ch = client.bottom - client.top;

        // Preserve the physical 4:3 VGA presentation aspect. The game's
        // 320x400 framebuffer is a memory geometry, not a square-pixel aspect.
        const double scale = std::min(cw / 4.0, ch / 3.0);
        const int width = std::max(1, static_cast<int>(4.0 * scale));
        const int height = std::max(1, static_cast<int>(3.0 * scale));
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

    int native_x_from_client(int client_x) const {
        const auto dst = render_rect();
        if (client_x < dst.left || client_x >= dst.right) return -1;
        const auto width = std::max<LONG>(1, dst.right - dst.left);
        return (client_x - dst.left) * 320 / width;
    }

    void paint() {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(hwnd_, &ps);
        RECT client{};
        GetClientRect(hwnd_, &client);
        const int width = std::max<LONG>(1, client.right - client.left);
        const int height = std::max<LONG>(1, client.bottom - client.top);

        // Render the complete client image off-screen and publish it with one
        // BitBlt. Direct FillRect + StretchDIBits to the window could expose
        // the cleared background between those operations and visibly flash.
        HDC back_dc = CreateCompatibleDC(dc);
        HBITMAP back_bitmap = CreateCompatibleBitmap(dc, width, height);
        HGDIOBJ old_bitmap =
            back_dc && back_bitmap ? SelectObject(back_dc, back_bitmap) : nullptr;

        HDC target = (back_dc && back_bitmap && old_bitmap) ? back_dc : dc;
        FillRect(
            target,
            &client,
            static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

        if (!bgra_.empty()) {
            BITMAPINFO info{};
            info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            info.bmiHeader.biWidth = 320;
            info.bmiHeader.biHeight = -400;
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;

            const auto dst = render_rect();
            SetStretchBltMode(target, COLORONCOLOR);
            StretchDIBits(
                target,
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

        if (target == back_dc) {
            BitBlt(dc, 0, 0, width, height, back_dc, 0, 0, SRCCOPY);
        }

        if (old_bitmap) SelectObject(back_dc, old_bitmap);
        if (back_bitmap) DeleteObject(back_bitmap);
        if (back_dc) DeleteDC(back_dc);
        EndPaint(hwnd_, &ps);
    }

    LRESULT handle(UINT msg, WPARAM wparam, LPARAM lparam) {
        switch (msg) {
            case WM_PAINT:
                paint();
                return 0;

            case WM_SETCURSOR:
                if (LOWORD(lparam) == HTCLIENT) {
                    SetCursor(nullptr);
                    return TRUE;
                }
                break;

            case WM_ERASEBKGND:
                return 1;

            case WM_KEYDOWN:
                if (wparam == VK_ESCAPE) {
                    escape_pressed_ = true;
                    return 0;
                }

                if (exploration_input_) {
                    if (const auto scan = legacy_scan_from_vk(wparam)) {
                        exploration_input_->press(*scan);
                    }
                    return 0;
                }

                if (character_mode_) {
                    if (wparam >= 'A' && wparam <= 'Z') {
                        key_events_.push_back(static_cast<int>(wparam));
                    } else if (wparam >= '0' && wparam <= '9') {
                        key_events_.push_back(static_cast<int>(wparam));
                    } else if (wparam == VK_SPACE) {
                        key_events_.push_back(' ');
                    } else if (wparam == VK_BACK) {
                        key_events_.push_back(8);
                    } else if (wparam == VK_RETURN) {
                        key_events_.push_back(0x0D);
                    } else if (wparam == VK_ADD) {
                        key_events_.push_back(0x14E);
                    } else if (wparam == VK_SUBTRACT) {
                        key_events_.push_back(0x14A);
                    }
                }
                return 0;

            case WM_KEYUP:
                if (exploration_input_) {
                    if (const auto scan = legacy_scan_from_vk(wparam)) {
                        exploration_input_->release(*scan);
                    }
                    return 0;
                }
                break;

            case WM_KILLFOCUS:
                if (exploration_input_) {
                    exploration_input_->clear();
                }
                break;

            case WM_MOUSEMOVE: {
                if (menu_mode_) {
                    const int y = native_y_from_client(GET_Y_LPARAM(lparam));
                    selection_ =
                        rloft::StartupPresenter::menu_selection_from_y(y);
                }

                if (menu_mode_ || character_mode_ || exploration_input_) {
                    const auto old_pointer = character_pointer_;
                    const int row =
                        native_y_from_client(GET_Y_LPARAM(lparam));
                    const int column =
                        native_x_from_client(GET_X_LPARAM(lparam));
                    if (row >= 0 && column >= 0) {
                        character_pointer_ = NativePoint{row, column};
                    } else {
                        character_pointer_.reset();
                    }

                    if ((!old_pointer && character_pointer_) ||
                        (old_pointer && !character_pointer_) ||
                        (old_pointer && character_pointer_ &&
                         (old_pointer->row != character_pointer_->row ||
                          old_pointer->column !=
                              character_pointer_->column))) {
                        refresh_pointer_overlay();
                    }

                    TRACKMOUSEEVENT leave{};
                    leave.cbSize = sizeof(leave);
                    leave.dwFlags = TME_LEAVE;
                    leave.hwndTrack = hwnd_;
                    TrackMouseEvent(&leave);
                }
                return 0;
            }

            case WM_MOUSELEAVE:
                if (menu_mode_ || character_mode_ || exploration_input_) {
                    character_pointer_.reset();
                    refresh_pointer_overlay();
                }
                return 0;

            case WM_LBUTTONDOWN:
                if (menu_mode_ && selection_ >= 0) {
                    clicked_ = selection_;
                }
                if (character_mode_) {
                    const int row =
                        native_y_from_client(GET_Y_LPARAM(lparam));
                    const int column =
                        native_x_from_client(GET_X_LPARAM(lparam));
                    if (row >= 0 && column >= 0) {
                        character_pointer_ = NativePoint{row, column};
                        character_click_ = *character_pointer_;
                    }
                }
                if (exploration_input_) {
                    const int row =
                        native_y_from_client(GET_Y_LPARAM(lparam));
                    const int column =
                        native_x_from_client(GET_X_LPARAM(lparam));
                    if (row >= 0 && column >= 0) {
                        character_pointer_ = NativePoint{row, column};
                        exploration_click_ = *character_pointer_;
                    }
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
    HANDLE frame_timer_{};
    std::vector<std::uint8_t> base_indices_;
    rloft::Palette6 current_palette_{};
    std::vector<std::uint32_t> bgra_;
    std::optional<rloft::Type1Raster> game_cursor_;
    bool quit_{false};
    bool escape_pressed_{false};
    bool menu_mode_{false};
    bool character_mode_{false};
    rloft::ExplorationLegacyInputController* exploration_input_{nullptr};
    int selection_{-1};
    std::optional<int> clicked_{};
    std::optional<NativePoint> character_click_{};
    std::optional<NativePoint> exploration_click_{};
    std::optional<NativePoint> character_pointer_{};
    std::deque<int> key_events_{};
};

bool fade_in(Host& host,
             rloft::StartupPresenter& presenter,
             const rloft::Palette6& palette) {
    // CODE.2 0x322EC applies factors 1..8 with shift 3.
    for (int factor = 1; factor <= 8; ++factor) {
        host.present(
            presenter.framebuffer(),
            rloft::StartupPresenter::fade_factor(palette, factor));
        if (!host.pause_frame(std::chrono::duration<double>(1.0 / kVgaRefreshHz))) {
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
        if (!host.pause_frame(std::chrono::duration<double>(1.0 / kVgaRefreshHz))) {
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

Clock::duration code1_ticks(int ticks) {
    // CODE.1 hooks INT 8 before Miles AIL. AIL later DDA-chains that prior
    // vector at the BIOS 54,925-us cadence even when a sound driver requests
    // a faster service interrupt. 87D8(2,0) therefore waits two BIOS ticks.
    return std::chrono::duration_cast<Clock::duration>(
        std::chrono::microseconds(54925LL * ticks));
}

std::uint32_t code1_tick_count(Clock::time_point epoch) {
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(
            Clock::now() - epoch);
    return static_cast<std::uint32_t>(
        elapsed.count() / 54925LL);
}

int run_exploration(
    Host& host,
    rloft::RavenloftBootAssets& assets,
    const std::vector<std::uint8_t>& chars) {
    auto runtime =
        rloft::ExplorationRuntime::load_fresh(
            assets.source(), chars);
    rloft::ExplorationPresenter presenter(assets);
    rloft::TopEdgeMenuPresenter menu_presenter(assets);
    rloft::ExplorationLegacyInputController input;

    host.set_character_mode(false);
    host.set_menu_mode(false);
    host.set_exploration_input(&input);
    SetWindowTextW(
        host.hwnd(),
        L"Ravenloft: Strahd's Possession - Exploration");

    // Native host cadence follows the same nominal VGA retrace pacing already
    // used by the reconstructed startup/character presenters. Input axes are
    // persistent, so every retrace advances the original 1FC14 -> 2E120 path
    // even when no new Windows message arrived.
    auto next_frame = Clock::now();

    while (host.pump()) {
        if (host.consume_escape()) {
            break;
        }

        int hovered_menu = -1;
        if (const auto pointer =
                host.exploration_pointer()) {
            hovered_menu =
                rloft::TopEdgeMenuModel::
                    selection_from_mouse(
                        pointer->column,
                        pointer->row);
        }

        int pressed_menu = -1;
        if (const auto click =
                host.consume_exploration_click()) {
            pressed_menu =
                rloft::TopEdgeMenuModel::
                    selection_from_mouse(
                        click->column,
                        click->row);
        }

        // CODE.2 080AC is modal while the pointer remains in the top
        // 64-pixel band (D680=3). Do not advance 1FC14/2E120 locomotion
        // while that dispatcher owns input.
        if (hovered_menu < 0) {
            const auto motion_inputs =
                input.tick(false);
            (void)runtime.tick_motion(
                motion_inputs);
        }

        const auto frame = runtime.build_frame();
        const auto presented = presenter.present(
            frame,
            runtime.renderer_tables());

        if (hovered_menu >= 0) {
            menu_presenter.draw(
                presenter.framebuffer(),
                hovered_menu,
                pressed_menu);
        }

        host.present(
            presenter.framebuffer(),
            presented.palette);

        next_frame += std::chrono::duration_cast<Clock::duration>(
            std::chrono::duration<double>(
                1.0 / kVgaRefreshHz));
        if (!host.wait_until(next_frame, false)) {
            break;
        }

        // Do not let a stalled/debugged host accumulate an arbitrarily large
        // catch-up burst. CODE.2 is real-time driven; resume from the current
        // retrace phase instead.
        const auto now = Clock::now();
        if (next_frame + std::chrono::milliseconds(100) < now) {
            next_frame = now;
        }
    }

    host.set_exploration_input(nullptr);
    return 0;
}

bool code1_palette_reveal(
    Host& host,
    rloft::CharacterCreationPresenter& presenter,
    const rloft::Palette6& source) {
    // BFE0 performs factors 1..8. 6618 -> A010 uploads the 768-byte DAC bank
    // in four 0xC0-byte bursts, and A010 waits for vertical retrace before
    // each burst. Preserve the partial-bank progression instead of collapsing
    // each factor to one host frame.
    rloft::Palette6 current{};

    for (int factor = 1; factor <= 8; ++factor) {
        const auto target =
            rloft::StartupPresenter::fade_factor(source, factor);

        for (int burst = 0; burst < 4; ++burst) {
            const auto first =
                static_cast<std::size_t>(burst) * 0xC0U;
            std::copy_n(
                target.rgb.begin() + static_cast<std::ptrdiff_t>(first),
                0xC0,
                current.rgb.begin() + static_cast<std::ptrdiff_t>(first));

            host.present(presenter.framebuffer(), current);
            if (!host.pause_frame(
                    std::chrono::duration<double>(
                        1.0 / kVgaRefreshHz))) {
                return false;
            }
        }
    }
    return true;
}

enum class CinematicResult {
    Completed,
    Escaped,
    Quit,
};

CinematicResult play_cinematic(
    Host& host,
    rloft::AnmCinematic& cinematic,
    const rloft::RavenloftBootAssets& assets,
    rloft::Win32AudioPlayer& audio_player,
    rloft::Win32MidiPlayer& midi_player) {
    cinematic.reset();

    while (!cinematic.finished()) {
        const auto frame = cinematic.render_next();

        // Apply the sampled-audio commands at the same logical ANM frame.
        // Type 5 music commands remain separately owned by the XMIDI bridge.
        for (const auto& event : frame.audio) {
            if (event.command == rloft::AnmAudioCommand::QueuedSfx ||
                event.command == rloft::AnmAudioCommand::DirectSfx) {
                const auto voc =
                    assets.sampled_audio(event.archive, event.resource);
                audio_player.play(rloft::decode_creative_voice(voc));
            } else if (event.command == rloft::AnmAudioCommand::MusicSequence) {
                const auto xmi = assets.music_sequence(event.resource);
                const int sequence_index = event.second + 1;
                midi_player.play(
                    rloft::parse_xmidi_sequence(xmi, sequence_index));
            }
        }

        host.present(cinematic.framebuffer(), cinematic.palette());

        if (frame.wait_ticks > 0) {
            if (!host.wait_until(
                    Clock::now() + pit_ticks(frame.wait_ticks),
                    true)) {
                if (host.quit_requested()) return CinematicResult::Quit;
                return CinematicResult::Escaped;
            }
        } else {
            if (!host.pump()) return CinematicResult::Quit;
            if (host.consume_escape()) return CinematicResult::Escaped;
        }
    }

    return CinematicResult::Completed;
}


std::optional<std::filesystem::path> choose_install_folder(HWND owner) {
    BROWSEINFOW bi{};
    bi.hwndOwner = owner;
    bi.lpszTitle =
        L"Select your original Ravenloft: Strahd's Possession installation folder";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_USENEWUI;

    PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);
    if (!pidl) return std::nullopt;

    std::array<wchar_t, MAX_PATH> path{};
    const bool ok = SHGetPathFromIDListW(pidl, path.data()) != FALSE;
    CoTaskMemFree(pidl);
    if (!ok) return std::nullopt;
    return std::filesystem::path(path.data());
}

void save_preferred_install(const std::filesystem::path& path) {
    HKEY key = nullptr;
    if (RegCreateKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Hyperbog\\RLOFT Demo V1",
            0,
            nullptr,
            0,
            KEY_SET_VALUE,
            nullptr,
            &key,
            nullptr) != ERROR_SUCCESS) {
        return;
    }

    const auto value = path.wstring();
    RegSetValueExW(
        key,
        L"GamePath",
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(value.c_str()),
        static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(key);
}

std::optional<std::filesystem::path> preferred_install() {
    HKEY key = nullptr;
    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Hyperbog\\RLOFT Demo V1",
            0,
            KEY_QUERY_VALUE,
            &key) != ERROR_SUCCESS) {
        return std::nullopt;
    }

    std::array<wchar_t, 32768> value{};
    DWORD type = 0;
    DWORD bytes = static_cast<DWORD>(value.size() * sizeof(wchar_t));
    const auto status = RegQueryValueExW(
        key,
        L"GamePath",
        nullptr,
        &type,
        reinterpret_cast<BYTE*>(value.data()),
        &bytes);
    RegCloseKey(key);

    if (status != ERROR_SUCCESS ||
        (type != REG_SZ && type != REG_EXPAND_SZ) ||
        value[0] == L'\0') {
        return std::nullopt;
    }
    return std::filesystem::path(value.data());
}

std::optional<rloft::GameSource> choose_valid_install(HWND owner) {
    for (;;) {
        const auto selected = choose_install_folder(owner);
        if (!selected) return std::nullopt;

        try {
            auto source = rloft::GameSource::open(*selected);
            if (!source.contains("RES0") ||
                !source.contains("INIT") ||
                !source.contains("T")) {
                throw std::runtime_error(
                    "The selected folder does not contain a valid Ravenloft installation.");
            }
            save_preferred_install(*selected);
            return source;
        } catch (const std::exception& exc) {
            const auto message =
                widen(std::string(exc.what()) +
                      "\n\nChoose the Ravenloft installation folder that contains "
                      "the original game files or game.gog.");
            const int result = MessageBoxW(
                owner,
                message.c_str(),
                L"Ravenloft installation not found",
                MB_RETRYCANCEL | MB_ICONWARNING);
            if (result != IDRETRY) return std::nullopt;
        }
    }
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
        save_preferred_install(source.location().install_root);
    } else {
        bool resolved = false;

        if (const auto saved = preferred_install()) {
            try {
                source = rloft::GameSource::open(*saved);
                resolved = true;
            } catch (...) {
                // A moved/uninstalled copy is not fatal; continue discovery.
            }
        }

        if (!resolved) {
            if (auto found = rloft::GameSource::auto_locate()) {
                source = std::move(*found);
                save_preferred_install(source.location().install_root);
                resolved = true;
            }
        }

        if (!resolved) {
            const int result = MessageBoxW(
                nullptr,
                L"Ravenloft was not located automatically.\n\n"
                L"Choose the folder where your owned copy is installed.",
                L"Locate Ravenloft installation",
                MB_OKCANCEL | MB_ICONINFORMATION);
            if (result != IDOK) return 2;

            auto chosen = choose_valid_install(nullptr);
            if (!chosen) return 2;
            source = std::move(*chosen);
        }
    }

    // CODE.2 204B3 checks the writable runtime CHARS transfer before the
    // cold-start presentation. A valid existing party enters the gameplay
    // import/level-load path directly; CHARS-absent startup continues through
    // the cinematic/menu/character workflow below.
    auto startup_chars = source.read_runtime("CHARS");

    rloft::RavenloftBootAssets assets(std::move(source));
    Host host;

    // CODE.2 1A0B0 and CODE.1 52AC both resolve the default mouse cursor as
    // RES0[INIT word 0 + 2Fh]. CODE.2 19378 then recolors record zero for
    // the title menu; CODE.1 later uses the raw resource again.
    const auto raw_cursor =
        assets.res0_type1(
            static_cast<int>(assets.init_u16(0x00)) + 0x2F);
    const auto menu_cursor =
        rloft::StartupPresenter::code2_menu_cursor(raw_cursor);
    host.set_game_cursor(raw_cursor);

    rloft::Win32AudioPlayer audio_player;
    rloft::Win32MidiPlayer midi_player;
    rloft::StartupPresenter presenter(assets);
    rloft::BootFlow flow;
    flow.boot();
    flow.finish_code2_startup(
        startup_chars.has_value(),
        false);

    if (!host.create(instance)) {
        throw std::runtime_error("Unable to create presentation window");
    }

    rloft::Palette6 black{};
    host.present(presenter.framebuffer(), black);

    if (startup_chars) {
        if (flow.snapshot().stage !=
                rloft::BootStage::Exploration ||
            flow.snapshot().module !=
                rloft::LegacyModule::Code2) {
            throw std::logic_error(
                "CODE.2 CHARS-present startup did not enter exploration");
        }
        return run_exploration(
            host,
            assets,
            *startup_chars);
    }

    bool skipped = false;

    // CODE.2 19888 begins with numbered cinematic event 0, then event 12.
    // 13EF8 maps those numbers directly to CINE00.ANM and CINE12.ANM.
    {
        rloft::AnmCinematic cine00(
            assets.source().read("CINE00.ANM"), 0, assets);
        const auto result =
            play_cinematic(
                host, cine00, assets, audio_player, midi_player);
        audio_player.stop_all();
        midi_player.stop();
        if (result == CinematicResult::Quit) return 0;
        if (result == CinematicResult::Escaped) skipped = true;
    }

    if (!skipped) {
        rloft::AnmCinematic cine12(
            assets.source().read("CINE12.ANM"), 12, assets);
        const auto result =
            play_cinematic(
                host, cine12, assets, audio_player, midi_player);
        audio_player.stop_all();
        midi_player.stop();
        if (result == CinematicResult::Quit) return 0;
        if (result == CinematicResult::Escaped) skipped = true;
    }

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
                    std::chrono::duration<double>(1.0 / kVgaRefreshHz))) {
                return 0;
            }
        }
        flow.finish_cold_start_presentation();
    } else {
        flow.skip_cold_start_presentation();
    }

    // CODE.2 19464 executes 1A0B0 -> 19378 before the first menu cursor
    // draw. Use the identically translated record-zero cursor only here.
    host.set_game_cursor(menu_cursor);
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
                // The CODE.2 title translation is an in-place mutation of its
                // private cursor copy. CODE.1 starts from the raw RES0 cursor.
                host.set_game_cursor(raw_cursor);

                rloft::CharacterCreationPresenter character_presenter(assets);
                const auto character_palette = character_presenter.palette();
                rloft::Palette6 black{};
                const auto code1_epoch = Clock::now();

                SetWindowTextW(
                    host.hwnd(),
                    L"Ravenloft: Strahd's Possession - Character Creation");

                // CODE.1 4CB0 reads INIT word 11 into 712C and C650(6)
                // selects RES6 for the C4E0 one-shot sound path.
                const int character_sfx_base =
                    assets.init_u16(0x16);

                int character_music_relative_volume = 100;
                if (const auto sound_cfg =
                        assets.source().read_runtime("SOUND.CFG")) {
                    character_music_relative_volume =
                        rloft::code1_music_relative_volume(
                            *sound_cfg,
                            100);
                }

                // CODE.1 45FC..4976: 70 displayed iterations. Each 87D8(2,0)
                // wait is two chained BIOS INT-8 ticks. The first frame is
                // submitted while the inherited CODE.2 palette is black; only
                // after that wait does BFE0 reveal COLORS palette 8.
                for (int frame = 0;
                     frame < rloft::CharacterCreationPresenter::kOpeningFrameCount;
                     ++frame) {
                    character_presenter.opening_frame(frame);
                    host.present(
                        character_presenter.framebuffer(),
                        frame == 0 ? black : character_palette);

                    if (!host.wait_until(
                            Clock::now() + code1_ticks(2), false)) {
                        return 0;
                    }

                    if (frame == 0) {
                        // 45FC: BC1C(18h,1,9C4h). CODE.1's dispatcher maps
                        // ED29 to AIL_set_relative_volume; static audio state
                        // is volume=100, tempo=100, enabled=1 and 2B88=0.
                        // B824 starts RES5:24 sequence 1 at volume zero, then
                        // ramps the sequence-relative volume to 100 over
                        // 2500 ms before the palette reveal begins.
                        midi_player.play(
                            rloft::parse_xmidi_sequence(
                                assets.music_sequence(0x18),
                                1),
                            0,
                            character_music_relative_volume,
                            0x9C4);

                        if (!code1_palette_reveal(
                                host,
                                character_presenter,
                                character_palette)) {
                            return 0;
                        }
                    }

                    // 45FC calls C4E0(base+0Dh) on frame 1 of each 10-frame
                    // group (logical frames 1 and 36), and C4E0(base+0Ch)
                    // on group 0's second-loop frame 15 (logical frame 25).
                    // C4E0 first invokes C57C, so stop the previous one-shot
                    // before submitting the new RES6 Creative Voice record.
                    int sound_id = -1;
                    if (frame == 1 || frame == 36) {
                        sound_id = character_sfx_base + 0x0D;
                    } else if (frame == 25) {
                        sound_id = character_sfx_base + 0x0C;
                    }

                    if (sound_id >= 0) {
                        // CODE.1 C4E0 -> C57C only finalizes the previous
                        // Miles sample when its status is SMP_DONE. It does
                        // not explicitly stop a sample that is still playing.
                        // Win32AudioPlayer::play() already reaps completed
                        // voices, so do not truncate an active one-shot here.
                        audio_player.play(
                            rloft::decode_creative_voice(
                                assets.sampled_audio(6, sound_id)));
                    }
                }

                rloft::CharacterCreationFlow character_flow;
                rloft::CharacterStatsModel character_stats(assets);
                const auto transfer_tables =
                    rloft::CharacterTransferTables::from_assets(assets);
                std::array<std::optional<rloft::CharacterStatsModel>, 2>
                    actor_stats{};
                bool stats_ready = false;
                bool stats_editor = false;
                bool final_decision = false;
                bool name_entry = false;
                bool existing_actor_decision = false;
                int existing_actor = -1;
                int selected_stat = -1;
                int portrait_hover_slot = -1;
                std::string pending_name;
                bool name_cursor_on = true;
                auto name_blink_deadline =
                    code1_tick_count(code1_epoch) + 9U;

                auto render_character = [&]() {
                    if (name_entry) {
                        character_presenter.name_entry_screen(
                            character_flow,
                            character_stats,
                            pending_name,
                            name_cursor_on);
                    } else if (final_decision) {
                        character_presenter.final_decision_screen(
                            character_flow,
                            character_stats);
                    } else if (existing_actor_decision) {
                        if (existing_actor < 0 || existing_actor >= 2 ||
                            !actor_stats[
                                static_cast<std::size_t>(existing_actor)]) {
                            throw std::logic_error(
                                "existing actor decision lacks saved stats");
                        }
                        character_presenter.existing_actor_decision_screen(
                            character_flow,
                            existing_actor,
                            *actor_stats[
                                static_cast<std::size_t>(existing_actor)]);
                    } else {
                        character_presenter.screen(
                            character_flow,
                            stats_ready ? &character_stats : nullptr,
                            selected_stat,
                            stats_editor,
                            portrait_hover_slot);
                    }
                    host.present(
                        character_presenter.framebuffer(),
                        character_palette);
                };

                const auto character_animation_period =
                    std::chrono::duration_cast<Clock::duration>(
                        std::chrono::duration<double>(
                            1.0 / kVgaRefreshHz));
                Clock::time_point character_animation_deadline{};

                auto present_character_animation_frame = [&]() {
                    host.present(
                        character_presenter.framebuffer(),
                        character_palette);

                    // CODE.1 53E4/65A4 waits for the next VGA retrace on
                    // every page switch. If native rendering misses a target
                    // retrace, the original waits for a later retrace; it
                    // never shortens the following frame to "catch up".
                    const auto now = Clock::now();
                    if (character_animation_deadline ==
                            Clock::time_point{} ||
                        now >= character_animation_deadline) {
                        character_animation_deadline = now;
                    }
                    character_animation_deadline +=
                        character_animation_period;

                    const bool ok = host.wait_until(
                        character_animation_deadline,
                        false);

                    // 10A4/07E4/15B0 do not accept a mouse selection while
                    // their page-flip animation is running. CODE.1's 043C
                    // input path also waits for any held button to be released
                    // before accepting the next hit. Do not carry a Windows
                    // WM_LBUTTONDOWN from an animation frame into the newly
                    // interactive screen.
                    host.clear_character_click();
                    return ok;
                };

                auto animate_15b0 = [&](
                    const rloft::CharacterCreationFlow& before,
                    const rloft::CharacterStatsModel* stats,
                    int selected_slot,
                    int target_row,
                    int target_column,
                    int final_stage) {
                    const int frames =
                        rloft::CharacterCreationPresenter::
                            transition_15b0_frame_count(selected_slot);
                    for (int frame = 0; frame < frames; ++frame) {
                        character_presenter.transition_15b0_frame(
                            before,
                            stats,
                            &character_flow,
                            selected_slot,
                            target_row,
                            target_column,
                            final_stage,
                            frame);
                        if (!present_character_animation_frame()) {
                            return false;
                        }
                    }
                    return true;
                };

                auto animate_24d4 = [&](
                    const rloft::CharacterCreationFlow& before,
                    int selected_slot,
                    int target_row,
                    int target_column) {
                    for (int frame = 0; frame <= 8; ++frame) {
                        character_presenter.transition_24d4_frame(
                            before,
                            selected_slot,
                            target_row,
                            target_column,
                            frame);
                        if (!present_character_animation_frame()) {
                            return false;
                        }
                    }
                    return true;
                };

                auto animate_07e4 = [&](
                    const rloft::CharacterCreationFlow& before,
                    const rloft::CharacterCreationFlow& after,
                    const rloft::CharacterStatsModel* before_stats,
                    const rloft::CharacterStatsModel* after_stats,
                    int first_slot,
                    int last_slot) {
                    const int frames =
                        rloft::CharacterCreationPresenter::
                            transition_07e4_frame_count(
                                first_slot,
                                last_slot);
                    for (int frame = 0; frame < frames; ++frame) {
                        character_presenter.transition_07e4_frame(
                            before,
                            after,
                            before_stats,
                            after_stats,
                            first_slot,
                            last_slot,
                            frame);
                        if (!present_character_animation_frame()) {
                            return false;
                        }
                    }
                    return true;
                };

                auto animate_controller_entry = [&](
                    const rloft::CharacterCreationFlow& target,
                    const rloft::CharacterStatsModel* stats) {
                    const int frames =
                        character_presenter.controller_entry_frame_count(
                            target);
                    for (int frame = 0; frame < frames; ++frame) {
                        character_presenter.controller_entry_frame(
                            target,
                            stats,
                            frame);
                        if (!present_character_animation_frame()) {
                            return false;
                        }
                    }
                    return true;
                };

                auto animate_existing_actor_decision = [&](
                    const rloft::CharacterCreationFlow& before,
                    const rloft::CharacterCreationFlow& after,
                    int actor,
                    const rloft::CharacterStatsModel& stats,
                    bool entering) {
                    const int frames =
                        rloft::CharacterCreationPresenter::
                            existing_actor_transition_frame_count(
                                entering);
                    for (int frame = 0; frame < frames; ++frame) {
                        character_presenter.
                            existing_actor_transition_frame(
                                before,
                                after,
                                actor,
                                stats,
                                entering,
                                frame);
                        if (!present_character_animation_frame()) {
                            return false;
                        }
                    }
                    return true;
                };

                auto animate_final_decision_entry = [&]() {
                    const int frames =
                        rloft::CharacterCreationPresenter::
                            final_decision_entry_frame_count();
                    for (int frame = 0; frame < frames; ++frame) {
                        character_presenter.final_decision_entry_frame(
                            character_flow,
                            character_stats,
                            frame);
                        if (!present_character_animation_frame()) {
                            return false;
                        }
                    }
                    return true;
                };

                auto animate_final_decision_dissolve = [&]() {
                    for (int frame = 0; frame < 10; ++frame) {
                        character_presenter.final_decision_dissolve_frame(
                            character_flow,
                            character_stats,
                            frame);
                        if (!present_character_animation_frame()) {
                            return false;
                        }
                    }
                    return true;
                };

                auto animate_name_entry = [&]() {
                    const int frames =
                        rloft::CharacterCreationPresenter::
                            name_entry_entry_frame_count();
                    for (int frame = 0; frame < frames; ++frame) {
                        character_presenter.name_entry_entry_frame(
                            character_flow,
                            character_stats,
                            frame);
                        if (!present_character_animation_frame()) {
                            return false;
                        }
                    }
                    return true;
                };

                auto animate_name_dissolve = [&](
                    const std::string& name) {
                    for (int frame = 0; frame < 10; ++frame) {
                        character_presenter.name_entry_dissolve_frame(
                            character_flow,
                            character_stats,
                            name,
                            frame);
                        if (!present_character_animation_frame()) {
                            return false;
                        }
                    }
                    return true;
                };

                auto animate_stats_editor = [&](bool entering) {
                    const int frames =
                        rloft::CharacterCreationPresenter::
                            stats_editor_transition_frame_count(
                                entering);
                    for (int frame = 0; frame < frames; ++frame) {
                        character_presenter.stats_editor_transition_frame(
                            character_flow,
                            character_stats,
                            entering,
                            frame);
                        if (!present_character_animation_frame()) {
                            return false;
                        }
                    }
                    return true;
                };

                const auto name_font = assets.res0_font(0);
                auto name_width = [&](const std::string& value) {
                    int width = 0;
                    for (const unsigned char ch : value) {
                        if (ch >= name_font.widths.size()) {
                            throw std::runtime_error(
                                "CODE.1 name uses glyph outside font table");
                        }
                        width += name_font.widths[ch];
                    }
                    return width;
                };

                host.set_character_mode(true);
                if (!animate_controller_entry(
                        character_flow,
                        nullptr)) {
                    return 0;
                }
                render_character();

                // CODE.1 043C/035C input path. Mode 6 changes the scan range
                // from slots 0..5 to 6..8; its event 9 opens the 3C58 editor.
                while (host.pump()) {
                    // function_2A64 is the only character controller with
                    // continuous mouseover rendering. Resting portraits use
                    // 5AF8; the usable portrait under the pointer is redrawn
                    // raw until the pointer moves away.
                    if (!name_entry &&
                        !final_decision &&
                        !existing_actor_decision &&
                        character_flow.state() ==
                            rloft::CharacterCreationState::PortraitChoice) {
                        int hover = -1;
                        if (const auto pointer =
                                host.character_pointer()) {
                            const int hit =
                                rloft::CharacterCreationFlow::hit_test(
                                    rloft::CharacterCreationState::
                                        PortraitChoice,
                                    pointer->row,
                                    pointer->column);
                            if (hit >= 1 && hit <= 6) {
                                const auto available =
                                    character_flow.available_choices();
                                if (available[
                                        static_cast<std::size_t>(
                                            hit - 1)]) {
                                    hover = hit - 1;
                                }
                            }
                        }

                        if (hover != portrait_hover_slot) {
                            portrait_hover_slot = hover;
                            render_character();
                        }
                    } else if (portrait_hover_slot != -1) {
                        portrait_hover_slot = -1;
                    }

                    // function_043C polls 831C only while mode == 6.
                    // Keypad '+' maps to event 7; keypad '-' maps to event 9.
                    // 831C is a peek, so unrelated queued keys are consumed
                    // here only as host input noise, never as controller events.
                    if (!name_entry &&
                        !final_decision &&
                        !existing_actor_decision &&
                        character_flow.state() ==
                            rloft::CharacterCreationState::StatsReview &&
                        stats_ready) {
                        if (const auto key = host.consume_key_event()) {
                            int key_event = -1;
                            if (*key == 0x14E) key_event = 7;
                            if (*key == 0x14A) key_event = 9;

                            if (key_event != -1) {
                                if (stats_editor) {
                                    if (selected_stat != -1) {
                                        character_stats.adjust(
                                            character_flow.draft(),
                                            selected_stat,
                                            key_event == 7);
                                        render_character();
                                    }
                                } else if (key_event == 7) {
                                    const auto old_stats =
                                        character_stats;
                                    character_stats.generate(
                                        character_flow.draft());
                                    if (!animate_07e4(
                                            character_flow,
                                            character_flow,
                                            &old_stats,
                                            &character_stats,
                                            7,
                                            8)) {
                                        return 0;
                                    }
                                    render_character();
                                } else {
                                    if (!animate_stats_editor(true)) {
                                        return 0;
                                    }
                                    stats_editor = true;
                                    selected_stat = -1;
                                    render_character();
                                }
                                continue;
                            }
                        }
                    }

                    if (name_entry) {
                        const auto tick = code1_tick_count(code1_epoch);
                        if (tick > name_blink_deadline) {
                            name_blink_deadline = tick + 9U;
                            name_cursor_on = !name_cursor_on;
                            render_character();
                        }

                        if (const auto key = host.consume_key_event()) {
                            if (*key == 0x0D) {
                                const int actor =
                                    character_flow.draft().actor;
                                if (actor < 0 || actor >= 2) {
                                    throw std::logic_error(
                                        "name entry lost selected actor");
                                }

                                if (!animate_name_dissolve(
                                        pending_name)) {
                                    return 0;
                                }

                                actor_stats[
                                    static_cast<std::size_t>(actor)] =
                                    character_stats;
                                character_flow.finish_current_actor(
                                    true,
                                    pending_name);
                                stats_ready = false;
                                stats_editor = false;
                                final_decision = false;
                                name_entry = false;
                                selected_stat = -1;
                                pending_name.clear();
                                host.clear_key_events();

                                // 4978 converts mode 7 back to mode 0 and
                                // immediately calls 1CD0/1464 again.
                                if (!animate_controller_entry(
                                        character_flow,
                                        nullptr)) {
                                    return 0;
                                }
                                render_character();
                                continue;
                            }

                            if (*key == 8) {
                                if (!pending_name.empty()) {
                                    pending_name.pop_back();
                                    name_cursor_on = true;
                                    name_blink_deadline =
                                        code1_tick_count(code1_epoch) + 9U;
                                    render_character();
                                }
                                continue;
                            }

                            const bool valid =
                                (*key >= 'A' && *key <= 'Z') ||
                                (*key >= '0' && *key <= '9') ||
                                *key == ' ';
                            if (valid &&
                                pending_name.size() < 15U) {
                                std::string candidate = pending_name;
                                candidate.push_back(
                                    static_cast<char>(*key));
                                // function_2DFC accepts only when the new
                                // rendered width remains strictly below 30h.
                                if (name_width(candidate) < 0x30) {
                                    pending_name = std::move(candidate);
                                    name_cursor_on = true;
                                    name_blink_deadline =
                                        code1_tick_count(code1_epoch) + 9U;
                                    render_character();
                                }
                            }
                        }

                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(1));
                        continue;
                    }

                    if (final_decision) {
                        if (const auto point =
                                host.consume_character_click()) {
                            // function_3138 reuses slots 2/3, which are the
                            // same rectangles as the mode-0 actor cards.
                            const int event =
                                rloft::CharacterCreationFlow::hit_test(
                                    rloft::CharacterCreationState::ActorChoice,
                                    point->row,
                                    point->column);

                            if (event == 3) {
                                if (!animate_final_decision_dissolve()) {
                                    return 0;
                                }
                                if (!animate_name_entry()) {
                                    return 0;
                                }

                                pending_name.clear();
                                name_entry = true;
                                name_cursor_on = true;
                                name_blink_deadline =
                                    code1_tick_count(code1_epoch) + 9U;
                                host.clear_key_events();
                                render_character();
                            } else if (event == 4) {
                                if (!animate_final_decision_dissolve()) {
                                    return 0;
                                }

                                character_flow.finish_current_actor(false);
                                stats_ready = false;
                                stats_editor = false;
                                final_decision = false;
                                selected_stat = -1;
                                pending_name.clear();

                                // Delete also returns through mode 7 -> 0.
                                if (!animate_controller_entry(
                                        character_flow,
                                        nullptr)) {
                                    return 0;
                                }
                                render_character();
                            }
                        }

                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(1));
                        continue;
                    }

                    if (existing_actor_decision) {
                        if (const auto point =
                                host.consume_character_click()) {
                            const int event =
                                rloft::CharacterCreationFlow::hit_test(
                                    rloft::CharacterCreationState::ActorChoice,
                                    point->row,
                                    point->column);

                            if (event == 3 || event == 4) {
                                const int actor = existing_actor;
                                const auto before = character_flow;
                                character_flow.resolve_existing_actor(
                                    actor,
                                    event == 3);
                                const auto after = character_flow;

                                if (!animate_existing_actor_decision(
                                        before,
                                        after,
                                        actor,
                                        *actor_stats[
                                            static_cast<std::size_t>(
                                                actor)],
                                        false)) {
                                    return 0;
                                }

                                if (event == 4) {
                                    actor_stats[
                                        static_cast<std::size_t>(
                                            actor)].reset();
                                }
                                existing_actor_decision = false;
                                existing_actor = -1;
                                render_character();
                            }
                        }

                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(1));
                        continue;
                    }

                    if (const auto point = host.consume_character_click()) {
                        const auto state = character_flow.state();
                        const int event =
                            rloft::CharacterCreationFlow::hit_test(
                                state,
                                point->row,
                                point->column);

                        if (state ==
                                rloft::CharacterCreationState::StatsReview &&
                            stats_ready) {
                            if (stats_editor) {
                                if (event == 0) {
                                    if (!animate_stats_editor(false)) {
                                        return 0;
                                    }
                                    stats_editor = false;
                                    selected_stat = -1;
                                    render_character();
                                } else if (event == 8) {
                                    const int q =
                                        (point->row - 0x60) / 10;
                                    selected_stat =
                                        (q >= 0 && (q < 6 || q == 10))
                                            ? q
                                            : -1;
                                    render_character();
                                } else if (event == 7 &&
                                           selected_stat != -1) {
                                    character_stats.adjust(
                                        character_flow.draft(),
                                        selected_stat,
                                        true);
                                    render_character();
                                } else if (event == 9 &&
                                           selected_stat != -1) {
                                    character_stats.adjust(
                                        character_flow.draft(),
                                        selected_stat,
                                        false);
                                    render_character();
                                }
                            } else {
                                if (event == 0) {
                                    const auto before = character_flow;
                                    if (character_flow.apply_event(0)) {
                                        if (!animate_15b0(
                                                before,
                                                &character_stats,
                                                -1,
                                                -1,
                                                -1,
                                                0)) {
                                            return 0;
                                        }
                                        stats_ready = false;
                                        selected_stat = -1;
                                        if (!animate_controller_entry(
                                                character_flow,
                                                nullptr)) {
                                            return 0;
                                        }
                                        render_character();
                                    }
                                } else if (event == 7) {
                                    // function_40FC rerolls through 3884 without
                                    // reseeding the 618A generator, then 07E4
                                    // replaces only slot 7.
                                    const auto old_stats = character_stats;
                                    character_stats.generate(
                                        character_flow.draft());
                                    if (!animate_07e4(
                                            character_flow,
                                            character_flow,
                                            &old_stats,
                                            &character_stats,
                                            7,
                                            8)) {
                                        return 0;
                                    }
                                    render_character();
                                } else if (event == 9) {
                                    if (!animate_stats_editor(true)) {
                                        return 0;
                                    }
                                    stats_editor = true;
                                    selected_stat = -1;
                                    render_character();
                                } else if (event == 8) {
                                    // function_40FC event 8 dissolves mode 6
                                    // through 15B0 before entering 3138.
                                    const auto before = character_flow;
                                    if (!animate_15b0(
                                            before,
                                            &character_stats,
                                            -1,
                                            -1,
                                            -1,
                                            0)) {
                                        return 0;
                                    }
                                    if (!animate_final_decision_entry()) {
                                        return 0;
                                    }
                                    final_decision = true;
                                    stats_editor = false;
                                    selected_stat = -1;
                                    host.clear_key_events();
                                    render_character();
                                }
                            }
                            continue;
                        }

                        if (state ==
                                rloft::CharacterCreationState::ActorChoice &&
                            (event == 3 || event == 4)) {
                            const int actor = event - 3;
                            if (character_flow.actor_present(actor)) {
                                if (!actor_stats[
                                        static_cast<std::size_t>(actor)]) {
                                    throw std::logic_error(
                                        "populated actor lacks retained stats");
                                }
                                const auto before =
                                    character_flow;
                                if (!animate_existing_actor_decision(
                                        before,
                                        before,
                                        actor,
                                        *actor_stats[
                                            static_cast<std::size_t>(
                                                actor)],
                                        true)) {
                                    return 0;
                                }
                                existing_actor_decision = true;
                                existing_actor = actor;
                                render_character();
                                continue;
                            }
                        }

                        if (event >= 0) {
                            const auto before = character_flow;
                            const auto before_state = before.state();

                            if (!character_flow.apply_event(event)) {
                                continue;
                            }

                            bool transition_ok = true;
                            switch (before_state) {
                                case rloft::CharacterCreationState::ActorChoice:
                                    if (event == 3 || event == 4) {
                                        // 1CD0 dissolves the actor-choice
                                        // controller before entering mode 1.
                                        transition_ok = animate_15b0(
                                            before,
                                            nullptr,
                                            -1,
                                            -1,
                                            -1,
                                            0);
                                    }
                                    break;

                                case rloft::CharacterCreationState::SexChoice:
                                    transition_ok = animate_15b0(
                                        before,
                                        nullptr,
                                        event == 0 ? -1 : event - 1,
                                        0x114,
                                        3,
                                        event == 0 ? 0 : 1);
                                    break;

                                case rloft::CharacterCreationState::RaceChoice:
                                    transition_ok = animate_15b0(
                                        before,
                                        nullptr,
                                        event == 0 ? -1 : event - 1,
                                        0x114,
                                        0x38,
                                        event == 0 ? 0 : 2);
                                    break;

                                case rloft::CharacterCreationState::ClassChoice:
                                    if (event == 0 &&
                                        character_flow.state() ==
                                            rloft::CharacterCreationState::
                                                ClassChoice) {
                                        // 266C substage cancel clears the
                                        // class tuple and uses 07E4 to restore
                                        // the stage-0 six-choice set.
                                        transition_ok = animate_07e4(
                                            before,
                                            character_flow,
                                            nullptr,
                                            nullptr,
                                            0,
                                            6);
                                    } else if (event >= 1 && event <= 6 &&
                                               character_flow.state() ==
                                                   rloft::
                                                       CharacterCreationState::
                                                           ClassChoice) {
                                        // A further class stage exists:
                                        // 24D4 moves the accepted card, then
                                        // 07E4 replaces the six choices.
                                        transition_ok = animate_24d4(
                                            before,
                                            event - 1,
                                            0x114,
                                            0x6D);
                                        if (transition_ok) {
                                            transition_ok = animate_07e4(
                                                before,
                                                character_flow,
                                                nullptr,
                                                nullptr,
                                                0,
                                                6);
                                        }
                                    } else {
                                        // Stage-0 cancel, explicit FF finish,
                                        // or a class pick with no next stage
                                        // leaves mode 3 through 15B0.
                                        transition_ok = animate_15b0(
                                            before,
                                            nullptr,
                                            (event >= 1 && event <= 6)
                                                ? event - 1
                                                : -1,
                                            0x114,
                                            0x6D,
                                            (event >= 1 && event <= 6)
                                                ? 3
                                                : 0);
                                    }
                                    break;

                                case rloft::CharacterCreationState::OptionChoice:
                                    transition_ok = animate_15b0(
                                        before,
                                        nullptr,
                                        event == 0 ? -1 : event - 1,
                                        0x114,
                                        0xA2,
                                        event == 0 ? 0 : 4);
                                    break;

                                case rloft::CharacterCreationState::PortraitChoice:
                                    if (event != 0xFF) {
                                        transition_ok = animate_15b0(
                                            before,
                                            nullptr,
                                            event == 0 ? -1 : event - 1,
                                            0x114,
                                            0xD7,
                                            event == 0 ? 0 : 5);
                                    }
                                    // FF rebuilds mode 5 through a fresh
                                    // 1464/07E4 entry. That entrance path is
                                    // handled separately from old-page
                                    // replacement below.
                                    break;

                                case rloft::CharacterCreationState::StatsReview:
                                    break;
                            }

                            if (!transition_ok) {
                                return 0;
                            }


                            if (character_flow.aborted()) {
                                // function_5014 begins with BB80(9C4h) and
                                // BCB4: fade active sequence relative volume
                                // to zero over 2500 ms, wait for zero, then
                                // normalize/stop it before module teardown.
                                midi_player.fade_out_and_stop(0x9C4);
                                audio_player.stop_all();

                                flow.finish_character_creation(
                                    rloft::LegacyResult::OrdinaryTermination);
                                flow.supervisor_dispatch();
                                host.set_character_mode(false);
                                return 0;
                            }

                            if (character_flow.completed()) {
                                // 5014 performs the same BB80/BCB4 music
                                // fade before 5F4C/4EF0 serialize CHARS.
                                midi_player.fade_out_and_stop(0x9C4);
                                audio_player.stop_all();

                                std::array<
                                    rloft::CharacterTransferActor,
                                    2> transfer{};

                                for (std::size_t actor = 0;
                                     actor < transfer.size();
                                     ++actor) {
                                    if (!actor_stats[actor]) {
                                        throw std::logic_error(
                                            "completed actor lacks retained "
                                            "CODE.1 stats");
                                    }
                                    transfer[actor].draft =
                                        character_flow.actor(
                                            static_cast<int>(actor));
                                    transfer[actor].stats =
                                        actor_stats[actor]->value();
                                }

                                const auto image =
                                    rloft::build_character_transfer(
                                        transfer,
                                        transfer_tables);
                                const auto path =
                                    assets.source().runtime_output_path(
                                        "CHARS");

                                std::error_code ec;
                                std::filesystem::remove(path, ec);

                                std::ofstream out(
                                    path,
                                    std::ios::binary |
                                        std::ios::trunc);
                                if (!out) {
                                    throw std::runtime_error(
                                        "Unable to create CHARS at: " +
                                        path.string());
                                }
                                out.write(
                                    reinterpret_cast<const char*>(
                                        image.data()),
                                    static_cast<std::streamsize>(
                                        image.size()));
                                out.close();
                                if (!out) {
                                    throw std::runtime_error(
                                        "Unable to write complete "
                                        "0x920-byte CHARS handoff at: " +
                                        path.string());
                                }

                                flow.finish_character_creation(
                                    rloft::LegacyResult::
                                        ContinueModuleCycle);
                                flow.supervisor_dispatch();

                                if (flow.snapshot().module !=
                                        rloft::LegacyModule::Code2 ||
                                    flow.snapshot().stage !=
                                        rloft::BootStage::Code2Startup ||
                                    !flow.snapshot().chars_present) {
                                    throw std::logic_error(
                                        "R.EXE status-10 dispatch did not "
                                        "reach CODE.2 CHARS startup");
                                }

                                // CODE.2 204B3 imports CHARS, then the normal
                                // fresh-party path loads level 22 at 4.5/4.5,
                                // facing 270, time 14:00 and enters gameplay.
                                flow.finish_code2_startup(true, false);
                                if (flow.snapshot().stage !=
                                        rloft::BootStage::Exploration) {
                                    throw std::logic_error(
                                        "CODE.2 CHARS startup did not enter "
                                        "exploration");
                                }

                                host.set_character_mode(false);
                                return run_exploration(
                                    host,
                                    assets,
                                    std::vector<std::uint8_t>(
                                        image.begin(),
                                        image.end()));
                            }

                            if (character_flow.state() ==
                                rloft::CharacterCreationState::StatsReview) {
                                // function_40FC seeds once from the CODE.1
                                // INT-8 tick counter and calls 3884.
                                character_stats.seed(
                                    code1_tick_count(code1_epoch));
                                character_stats.generate(
                                    character_flow.draft());
                                stats_ready = true;
                                stats_editor = false;
                                final_decision = false;
                                name_entry = false;
                                selected_stat = -1;
                            }

                            const bool fresh_controller =
                                before_state != character_flow.state() ||
                                (before_state ==
                                     rloft::CharacterCreationState::
                                         PortraitChoice &&
                                 event == 0xFF);
                            if (fresh_controller) {
                                const auto* entry_stats =
                                    character_flow.state() ==
                                            rloft::
                                                CharacterCreationState::
                                                    StatsReview
                                        ? &character_stats
                                        : nullptr;
                                if (!animate_controller_entry(
                                        character_flow,
                                        entry_stats)) {
                                    return 0;
                                }
                            }
                            render_character();
                        }
                    }

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
