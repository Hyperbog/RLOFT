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
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "rloft/anm_cinematic.hpp"
#include "rloft/dev_level_diagnostics.hpp"
#include "rloft/dev_session_trace.hpp"
#include "rloft/win32_scan_observation.hpp"
#include "rloft/dev_level_interaction_policy.hpp"
#include "rloft/automap_modal.hpp"
#include "rloft/automap_screen_presenter.hpp"
#include "rloft/level_display.hpp"
#include "rloft/exploration_ui.hpp"
#include "rloft/automap_screen_state.hpp"
#include "rloft/automap_screen_presenter.hpp"
#include "rloft/automap_controls.hpp"
#include "rloft/original_automap_file_export.hpp"
#include "rloft/enhanced_automap_keyboard.hpp"
#include "rloft/boot_flow.hpp"
#include "rloft/title_keyboard.hpp"
#include "rloft/enhanced_game_save.hpp"
#include "rloft/character_creation_flow.hpp"
#include "rloft/character_creation_presenter.hpp"
#include "rloft/character_stats.hpp"
#include "rloft/character_transfer.hpp"
#include "rloft/exploration_legacy_input.hpp"
#include "rloft/exploration_gameplay_cadence.hpp"
#include "rloft/enhanced_world_music.hpp"
#include "rloft/win32_gpu_scene.hpp"
#include "rloft/exploration_presenter.hpp"
#include "rloft/exploration_frame_capture.hpp"
#include "rloft/exploration_preview.hpp"
#include "rloft/exploration_runtime.hpp"
#include "rloft/party_modal_presenter.hpp"
#include "rloft/original_f790_ordinary_slot_event_plan.hpp"
#include "rloft/party_panel_controls.hpp"
#include "rloft/combat_live_hand_request.hpp"
#include "rloft/enhanced_combat_target_arming.hpp"
#include "rloft/quit_confirm.hpp"
#include "rloft/top_edge_menu_presenter.hpp"
#include "rloft/enhanced_options_presenter.hpp"
#include "rloft/game_source.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"
#include "rloft/sound_config.hpp"
#include "rloft/voc_decoder.hpp"
#include "rloft/win32_audio.hpp"
#include "rloft/original_npc_sound.hpp"
#include "rloft/win32_midi.hpp"
#include "rloft/xmidi.hpp"

// Printed in native diagnostics to distinguish a newly compiled EXE from
// the previously published checked-in executable. The workflow stamps
// the exact source checkout; local builds self-identify as unversioned.
#ifndef RLOFT_DEMO_SOURCE_SHA
#define RLOFT_DEMO_SOURCE_SHA "local-unversioned"
#endif

namespace {

using Clock = std::chrono::steady_clock;


// VGA mode timing used by Ravenloft's 320x400 planar presentation path.
// CODE.1 AF64 programs the CRTC start address and then AF34 waits for the
// next vertical retrace. The same retrace cadence drives DAC burst pacing.
constexpr double kVgaRefreshHz = 70.0863;

// Enhanced PC exploration display cadence. Unlike the original 70 Hz
// palette/cinematic/character timing, native gameplay presentation is
// scheduled for a common 60 Hz monitor. Physics/gameplay integration
// remains independently clocked by ExplorationGameplayCadence.
constexpr double kNativeGameplayRefreshHz = 60.0;

std::wstring widen(std::string_view text) {
    if (text.empty()) return {};
    const int count = MultiByteToWideChar(
        CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    std::wstring out(static_cast<std::size_t>(count), L'\0');
    MultiByteToWideChar(
        CP_UTF8, 0, text.data(), static_cast<int>(text.size()), out.data(), count);
    return out;
}

std::wstring executable_source_revision() {
    return widen(std::string_view(RLOFT_DEMO_SOURCE_SHA).substr(0, 12));
}

class Host final {
public:
    ~Host() {
        gpu_scene_.reset();
        release_gdi_backbuffer();
        if (frame_timer_) {
            CloseHandle(frame_timer_);
        }
    }

    bool create(HINSTANCE instance) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = &Host::window_proc;
        // CS_OWNDC stabilizes the one genuine game's WGL context.
        wc.style = CS_OWNDC;
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
            L"Ravenloft: Strahd's Possession - Enhanced PC (Pre-Alpha)",
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

    void begin_gpu_scene(
        const rloft::RavenloftBootAssets& assets,
        const rloft::Palette6& palette) {
        if(gpu_scene_) {
            try {gpu_scene_->set_palette(palette);}
            catch(const std::exception& e){OutputDebugStringA(e.what());}
            return;
        }
        try {
            gpu_scene_=std::make_unique<rloft::Win32GpuScene>(
                hwnd_,assets,palette);
            gpu_world_visible_=false;
        }catch(const std::exception& error) {
            OutputDebugStringA(error.what());
            gpu_scene_.reset(); // original renderer remains usable
        }
    }

    void bind_gpu_frame(const rloft::ExplorationSessionState& session,
                        const rloft::LevelWorldState& world,
                        const std::vector<rloft::WorldSceneRecord>& records,
                        const std::vector<std::uint8_t>& environment_backdrop,
                        const rloft::Palette6& frame_palette) {
        if(!gpu_scene_)return;
        try {
            gpu_scene_->bind(session,world,records,environment_backdrop,
                             frame_palette);
            gpu_world_visible_=gpu_world_enabled_;
        }catch(const std::exception& error) {
            OutputDebugStringA(error.what());
            gpu_world_visible_=false;
        }
    }
    void set_gpu_world_visible(bool enabled) noexcept {
        gpu_world_visible_=enabled && gpu_world_enabled_;
    }
    void set_gpu_menu_overlay(bool enabled) noexcept {
        gpu_menu_overlay_=enabled;
    }
    void end_gpu_scene() noexcept {
        gpu_world_visible_=false;
        gpu_scene_.reset();
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
        if (menu_mode_ != enabled) {
            // CODE.2 19464 title events are mouse-modal. Win32's
            // global Escape latch must not turn a later 13EF8(CINE00)
            // replay into an immediate skip, or leak into CODE.1.
            // This is host-event isolation, NOT guest DOS F572 state.
            escape_pressed_ = false;
            clicked_.reset();
        }
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
        if (character_mode_ != enabled) {
            // CODE.1 043C/2DFC own a different input stream from the
            // CODE.2 title and first gameplay frame. Discard native
            // queued keys/clicks captured in the previous host phase.
            escape_pressed_ = false;
            key_events_.clear();
            character_click_.reset();
        }
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
        if (exploration_input_ != controller) {
            // CODE.2 exploration must not inherit an unconsumed native
            // title/creation Escape flag or character name/roll keys.
            // A fresh physical keypress is required in the new phase.
            escape_pressed_ = false;
            key_events_.clear();
        }
        if (exploration_input_ && exploration_input_ != controller) {
            exploration_input_->clear();
        }
        exploration_input_ = controller;
        if(!controller) {
            stop_mouse_look();
            strafe_left_=strafe_right_=false;
            map_toggle_requested_=false;
        }
        exploration_click_.reset();
        exploration_release_.reset();
        exploration_secondary_click_.reset();

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

    std::optional<NativePoint> consume_exploration_release() {
        auto result=exploration_release_;
        exploration_release_.reset();
        return result;
    }

    std::optional<NativePoint> consume_exploration_secondary_click() {
        auto result = exploration_secondary_click_;
        exploration_secondary_click_.reset();
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

    // CODE.2 full party inventory is modal D680=1. Keypress D is
    // command 0x44 ONLY while this mode owns the event queue;
    // prevent stale gameplay or character-creation D keys from
    // turning into a later world drop.
    void set_inventory_mode(bool enabled) {
        if(inventory_mode_!=enabled) {
            key_events_.clear();
            // Movement keys held before inventory entry must not
            // carry over into a delayed move when the panel closes.
            if(exploration_input_) exploration_input_->clear();
        }
        inventory_mode_=enabled;
        if(enabled) {
            stop_mouse_look();
            strafe_left_=strafe_right_=false;
        }
    }

    // In the automap modal, arrow keys pan the map. They must never
    // reach the exploration movement controller; queued input from
    // before the modal must not execute when the game resumes.
    void set_automap_mode(bool enabled) {
        if (automap_mode_ != enabled) {
            key_events_.clear();
            if (exploration_input_) exploration_input_->clear();
        }
        automap_mode_ = enabled;
        if(enabled) {
            stop_mouse_look();
            strafe_left_=strafe_right_=false;
        }
    }

    // Enhanced Windows controls are active only after genuine CODE.1 party
    // creation, inside the native exploration process.
    [[nodiscard]] bool sprint_held() const noexcept {
        return exploration_input_ && GetFocus()==hwnd_ &&
            !automap_mode_ && !inventory_mode_ &&
            (GetKeyState(VK_SHIFT)&0x8000)!=0;
    }
    [[nodiscard]] int strafe_axis() const noexcept {
        return (strafe_right_?1:0)-(strafe_left_?1:0);
    }
    [[nodiscard]] bool consume_map_toggle() noexcept {
        const bool result=map_toggle_requested_;
        map_toggle_requested_=false;
        return result;
    }
    void stop_mouse_look() {
        if(!mouse_look_)return;
        mouse_look_=false;
        yaw_fraction_=0.0;
        if(hwnd_ && GetCapture()==hwnd_)ReleaseCapture();
        refresh_pointer_overlay();
    }
    [[nodiscard]] int consume_mouse_yaw() {
        if(!mouse_look_ || GetFocus()!=hwnd_ || !exploration_input_ ||
           automap_mode_ || inventory_mode_)return 0;
        RECT r{};
        if(!GetClientRect(hwnd_,&r))return 0;
        POINT center{(r.left+r.right)/2,(r.top+r.bottom)/2};
        if(!ClientToScreen(hwnd_,&center))return 0;
        POINT pos{};
        if(!GetCursorPos(&pos))return 0;
        const int delta=std::clamp(static_cast<int>(pos.x-center.x),-256,256);
        const int vertical=std::clamp(static_cast<int>(pos.y-center.y),-256,256);
        // Modern camera pitch is presentation-only. Original fixed-point
        // CODE.2 facing and collision never receive this extra dimension.
        gpu_pitch_=std::clamp(gpu_pitch_-vertical*0.0032,-1.35,1.35);
        if(pos.x!=center.x || pos.y!=center.y)
            SetCursorPos(center.x,center.y);
        yaw_fraction_+=static_cast<double>(delta)*0.18;
        const int yaw=static_cast<int>(yaw_fraction_);
        yaw_fraction_-=static_cast<double>(yaw);
        return std::clamp(yaw,-46,46);
    }
    void toggle_mouse_look() {
        if(mouse_look_){stop_mouse_look();return;}
        if(!exploration_input_ || automap_mode_ || inventory_mode_)return;
        SetFocus(hwnd_);
        RECT r{};GetClientRect(hwnd_,&r);
        POINT center{(r.left+r.right)/2,(r.top+r.bottom)/2};
        if(!ClientToScreen(hwnd_,&center) ||
           !SetCursorPos(center.x,center.y))return;
        SetCapture(hwnd_);
        mouse_look_=true;
        character_pointer_.reset();
        refresh_pointer_overlay();
    }

    void set_session_trace(rloft::DevSessionTrace* trace) noexcept {
        session_trace_=trace;
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

        if (mouse_look_ ||
            !(menu_mode_ || character_mode_ || exploration_input_) ||
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
            if (menu_mode_) selection_ = -1;
            character_pointer_.reset();
            return;
        }

        const int row = native_y_from_client(point.y);
        const int column = native_x_from_client(point.x);
        if (row >= 0 && column >= 0) {
            character_pointer_ = NativePoint{row, column};
        } else {
            character_pointer_.reset();
        }
        // Initial title entry and CINE00 replay restore the hover
        // without waiting for a new WM_MOUSEMOVE event.
        if (menu_mode_) {
            selection_ = character_pointer_
                ? rloft::StartupPresenter::menu_selection_from_y(row)
                : -1;
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

    void release_gdi_backbuffer() noexcept {
        if(gdi_bitmap_ && gdi_dc_ && gdi_old_bitmap_)
            SelectObject(gdi_dc_,gdi_old_bitmap_);
        if(gdi_bitmap_)DeleteObject(gdi_bitmap_);
        if(gdi_dc_)DeleteDC(gdi_dc_);
        gdi_dc_=nullptr;
        gdi_bitmap_=nullptr;
        gdi_old_bitmap_=nullptr;
        gdi_width_=gdi_height_=0;
    }
    void ensure_gdi_backbuffer(HDC dc,int width,int height) {
        if(gdi_dc_ && gdi_bitmap_ &&
           gdi_width_==width && gdi_height_==height)return;
        release_gdi_backbuffer();
        HDC compatible=CreateCompatibleDC(dc);
        if(!compatible)return;
        HBITMAP bitmap=CreateCompatibleBitmap(dc,width,height);
        if(!bitmap){DeleteDC(compatible);return;}
        HGDIOBJ previous=SelectObject(compatible,bitmap);
        if(!previous || previous==HGDI_ERROR) {
            DeleteObject(bitmap);
            DeleteDC(compatible);
            return;
        }
        gdi_dc_=compatible;
        gdi_bitmap_=bitmap;
        gdi_old_bitmap_=previous;
        gdi_width_=width;
        gdi_height_=height;
    }
    void paint() {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(hwnd_, &ps);
        RECT client{};
        GetClientRect(hwnd_, &client);
        const int width = std::max<LONG>(1, client.right - client.left);
        const int height = std::max<LONG>(1, client.bottom - client.top);
        if(gpu_scene_ && bgra_.size()==320U*400U) {
            try {
                gpu_scene_->paint(render_rect(),width,height,bgra_,
                    gpu_world_visible_,gpu_filter_mode_,gpu_pitch_,
                    gpu_menu_overlay_);
                EndPaint(hwnd_,&ps);
                return;
            }catch(const std::exception& error) {
                OutputDebugStringA(error.what());
                // An unsupported GPU must not corrupt the live game state.
                // Keep original 320x400 software/GDI playback available.
                gpu_scene_.reset();
            }
        }

        // Keep one size-matched GDI target across frames. The previous
        // software fallback allocated an HDC and bitmap for every paint,
        // adding needless 60 Hz allocation jitter and resize flicker.
        ensure_gdi_backbuffer(dc,width,height);
        HDC target=gdi_dc_?gdi_dc_:dc;
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
            SetStretchBltMode(target, smooth_scaling_?HALFTONE:COLORONCOLOR);
            if(smooth_scaling_)SetBrushOrgEx(target,0,0,nullptr);
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

        if(gdi_dc_)BitBlt(dc,0,0,width,height,gdi_dc_,0,0,SRCCOPY);
        EndPaint(hwnd_, &ps);
    }

    void toggle_fullscreen() {
        if(!hwnd_)return;
        if(!fullscreen_) {
            saved_placement_.length=sizeof(WINDOWPLACEMENT);
            if(!GetWindowPlacement(hwnd_,&saved_placement_))return;
            saved_style_=GetWindowLongPtrW(hwnd_,GWL_STYLE);
            MONITORINFO mi{sizeof(MONITORINFO)};
            if(!GetMonitorInfoW(MonitorFromWindow(hwnd_,MONITOR_DEFAULTTONEAREST),
                                &mi))return;
            SetWindowLongPtrW(hwnd_,GWL_STYLE,
                saved_style_ & ~static_cast<LONG_PTR>(WS_OVERLAPPEDWINDOW));
            const auto& r=mi.rcMonitor;
            SetWindowPos(hwnd_,HWND_TOP,r.left,r.top,
                r.right-r.left,r.bottom-r.top,
                SWP_FRAMECHANGED|SWP_NOOWNERZORDER);
            fullscreen_=true;
        } else {
            SetWindowLongPtrW(hwnd_,GWL_STYLE,saved_style_);
            SetWindowPlacement(hwnd_,&saved_placement_);
            SetWindowPos(hwnd_,nullptr,0,0,0,0,
                SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE|
                SWP_NOZORDER|SWP_NOOWNERZORDER);
            fullscreen_=false;
        }
    }

    LRESULT handle(UINT msg, WPARAM wparam, LPARAM lparam) {
        // Developer-only metadata, never original CODE.2 gameplay input.
        // Status rejects missing E0/E1 streams and unknown repeat timing.
        // Keep the existing Win32 key handler entirely unchanged below.
        // CODE.1 name entry uses the same Win32 host keyboard as
        // the earlier cinematic/title screens. Never capture original
        // character-name keystrokes (or reversible OEM scan bytes)
        // in the opt-in diagnostic session.
        if(session_trace_ && !character_mode_ &&
           (msg==WM_KEYDOWN || msg==WM_KEYUP ||
            msg==WM_SYSKEYDOWN || msg==WM_SYSKEYUP)) {
            const auto observation=
                rloft::Win32ScanObservationModel::decode(
                    static_cast<std::uint32_t>(msg),
                    static_cast<std::uint64_t>(lparam));
            // Trace parameters: a=OEM scan byte; b bits 0..7=status,
            // 8..23=repeat, 24=extended, 25=ALT context,
            // 26=previous down, 27=key up, 28=system key.
            const auto scan_flags=
                static_cast<std::uint32_t>(observation.status) |
                (static_cast<std::uint32_t>(observation.repeat_count)<<8U) |
                (static_cast<std::uint32_t>(observation.extended)<<24U) |
                (static_cast<std::uint32_t>(observation.alt_context)<<25U) |
                (static_cast<std::uint32_t>(observation.previous_down)<<26U) |
                (static_cast<std::uint32_t>(observation.key_up)<<27U) |
                (static_cast<std::uint32_t>(observation.system_key)<<28U);
            try {
                session_trace_->event("key.scan_observation",
                    static_cast<std::int64_t>(observation.oem_scan),
                    static_cast<std::int64_t>(scan_flags));
            }catch(...) {session_trace_=nullptr;}
        }
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
                if(session_trace_ &&
                   (static_cast<std::uint64_t>(lparam)&(1ULL<<30U))==0U) {
                    try {
                        if(wparam==VK_F12) {
                            session_trace_->request_marker();
                            return 0;
                        }
                        // F12 remains a diagnostic marker; all other
                        // CODE.1 key codes are private character input.
                        // Retain gameplay's actual key handling below.
                        if (!character_mode_) {
                            session_trace_->event("key.down",
                                static_cast<std::int64_t>(wparam));
                        }
                    }catch(...) { session_trace_=nullptr; }
                }
                if (wparam == VK_ESCAPE) {
                    escape_pressed_ = true;
                    return 0;
                }

                // One fullscreen presentation control across logos,
                // cinematics, title, CODE.1 and all CODE.2 screens.
                if (wparam==VK_F11 &&
                    (lparam&(1LL<<30U))==0) {
                    toggle_fullscreen();return 0;
                }
                if(menu_mode_) {
                    const auto result=rloft::TitleKeyboardModel::key_down(
                        static_cast<std::uint32_t>(wparam),
                        selection_,
                        (static_cast<std::uint64_t>(lparam)&
                         (1ULL<<30U))!=0U);
                    if(result.highlighted!=selection_) {
                        selection_=result.highlighted;
                        refresh_pointer_overlay();
                    }
                    if(result.activate)clicked_=*result.activate;
                    return 0;
                }
                if(exploration_input_ && !inventory_mode_ &&
                   !automap_mode_ && wparam==VK_F2 &&
                   (lparam&(1LL<<30U))==0) {
                    gpu_world_enabled_=!gpu_world_enabled_;
                    gpu_world_visible_=gpu_world_enabled_;
                    InvalidateRect(hwnd_,nullptr,FALSE);return 0;
                }
                if (exploration_input_ && !inventory_mode_ &&
                    !automap_mode_ && wparam==VK_F3 &&
                    (lparam&(1LL<<30U))==0) {
                    gpu_filter_mode_=(gpu_filter_mode_+1U)%3U;
                    smooth_scaling_=gpu_filter_mode_!=0U;
                    InvalidateRect(hwnd_,nullptr,FALSE);return 0;
                }
                if (exploration_input_ && !inventory_mode_ &&
                    !automap_mode_ && wparam=='M' &&
                    (lparam&(1LL<<30U))==0) {
                    toggle_mouse_look();return 0;
                }
                if (exploration_input_ && wparam==VK_TAB &&
                    (lparam&(1LL<<30U))==0) {
                    map_toggle_requested_=true;return 0;
                }
                if (automap_mode_ && exploration_input_) {
                    if (const auto action =
                            rloft::EnhancedAutomapKeyboard::action_for_key(
                                static_cast<std::uint32_t>(wparam),
                                (GetKeyState(VK_SHIFT) & 0x8000) != 0)) {
                        key_events_.push_back(*action);
                    }
                    return 0;
                }
                if (exploration_input_) {
                    if(inventory_mode_) {
                        // Inventory exclusively owns the keyboard. D is a
                        // world-drop command; arrows must not continue
                        // moving the party behind its modal panel.
                        if (wparam=='D' &&
                            (static_cast<std::uint64_t>(lparam)&
                             (1ULL<<30U))==0U) {
                            key_events_.push_back(0x44);
                        }
                        return 0;
                    }
                    if(wparam=='A')strafe_left_=true;
                    if(wparam=='D')strafe_right_=true;
                    const auto scan=legacy_scan_from_vk(wparam).value_or(
                        wparam=='W'?rloft::ExplorationLegacyInputController::kUp:
                        wparam=='S'?rloft::ExplorationLegacyInputController::kDown:
                        wparam=='Q'?rloft::ExplorationLegacyInputController::kLeft:
                        wparam=='E'?rloft::ExplorationLegacyInputController::kRight:0U);
                    if(scan)exploration_input_->press(scan);
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
                if(session_trace_ && wparam==VK_F12)return 0;
                if(session_trace_ && !character_mode_) {
                    try {
                        session_trace_->event("key.up",
                            static_cast<std::int64_t>(wparam));
                    }catch(...) {session_trace_=nullptr;}
                }
                if (automap_mode_ && exploration_input_) return 0;
                if (exploration_input_) {
                    if(inventory_mode_)return 0;
                    if(wparam=='A')strafe_left_=false;
                    if(wparam=='D')strafe_right_=false;
                    const auto scan=legacy_scan_from_vk(wparam).value_or(
                        wparam=='W'?rloft::ExplorationLegacyInputController::kUp:
                        wparam=='S'?rloft::ExplorationLegacyInputController::kDown:
                        wparam=='Q'?rloft::ExplorationLegacyInputController::kLeft:
                        wparam=='E'?rloft::ExplorationLegacyInputController::kRight:0U);
                    if(scan)exploration_input_->release(scan);
                    return 0;
                }
                break;

            case WM_KILLFOCUS:
                // Alt-Tab, another window or a native MessageBox can
                // interrupt any stage. Prevent queued input and
                // old hover/clicks from executing after focus returns.
                if (exploration_input_) exploration_input_->clear();
                stop_mouse_look();
                strafe_left_=strafe_right_=false;
                map_toggle_requested_=false;
                escape_pressed_ = false;
                key_events_.clear();
                clicked_.reset();
                character_click_.reset();
                exploration_click_.reset();
                exploration_release_.reset();
                exploration_secondary_click_.reset();
                selection_ = -1;
                character_pointer_.reset();
                refresh_pointer_overlay();
                return 0;

            case WM_SETFOCUS:
                if (menu_mode_ || character_mode_ || exploration_input_) {
                    update_pointer_from_system_cursor();
                    refresh_pointer_overlay();
                }
                return 0;

            case WM_MOUSEMOVE: {
                if(mouse_look_)return 0;
                const int row =
                    native_y_from_client(GET_Y_LPARAM(lparam));
                const int column =
                    native_x_from_client(GET_X_LPARAM(lparam));
                if (menu_mode_) {
                    selection_ = (row >= 0 && column >= 0)
                        ? rloft::StartupPresenter::menu_selection_from_y(row)
                        : -1;
                }

                if (menu_mode_ || character_mode_ || exploration_input_) {
                    const auto old_pointer = character_pointer_;
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
                    if (menu_mode_) selection_ = -1;
                    refresh_pointer_overlay();
                }
                return 0;

            case WM_LBUTTONDOWN:
                if(session_trace_ && exploration_input_) {
                    try {
                        session_trace_->event("mouse.left.down",
                            native_x_from_client(GET_X_LPARAM(lparam)),
                            native_y_from_client(GET_Y_LPARAM(lparam)));
                    }catch(...) {session_trace_=nullptr;}
                }
                if (menu_mode_) {
                    // The event's own coordinates determine which title
                    // action was clicked; a prior hover can be stale
                    // after an intro replay or window activation.
                    const int row =
                        native_y_from_client(GET_Y_LPARAM(lparam));
                    const int column =
                        native_x_from_client(GET_X_LPARAM(lparam));
                    selection_ = (row >= 0 && column >= 0)
                        ? rloft::StartupPresenter::menu_selection_from_y(row)
                        : -1;
                    if (selection_ >= 0) clicked_ = selection_;
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

            case WM_LBUTTONUP:
                if(session_trace_ && exploration_input_) {
                    try {
                        session_trace_->event("mouse.left.up",
                            native_x_from_client(GET_X_LPARAM(lparam)),
                            native_y_from_client(GET_Y_LPARAM(lparam)));
                    }catch(...) {session_trace_=nullptr;}
                }
                if(exploration_input_) {
                    const int row=native_y_from_client(
                        GET_Y_LPARAM(lparam));
                    const int column=native_x_from_client(
                        GET_X_LPARAM(lparam));
                    if(row>=0 && column>=0) {
                        character_pointer_=NativePoint{row,column};
                        exploration_release_=*character_pointer_;
                    }
                }
                return 0;

            case WM_RBUTTONDOWN:
                // Preserve original inventory/hand right-click action.
                // Ctrl+right-click is an optional mouse-look shortcut;
                // M alone also toggles capture without a mouse gesture.
                if(exploration_input_ && !inventory_mode_ &&
                   !automap_mode_ &&
                   (GetKeyState(VK_CONTROL)&0x8000)!=0) {
                    toggle_mouse_look();return 0;
                }
                if(session_trace_ && exploration_input_) {
                    try {
                        session_trace_->event("mouse.right.down",
                            native_x_from_client(GET_X_LPARAM(lparam)),
                            native_y_from_client(GET_Y_LPARAM(lparam)));
                    }catch(...) {session_trace_=nullptr;}
                }
                if (exploration_input_) {
                    const int row =
                        native_y_from_client(GET_Y_LPARAM(lparam));
                    const int column =
                        native_x_from_client(GET_X_LPARAM(lparam));
                    if (row >= 0 && column >= 0) {
                        character_pointer_ = NativePoint{row, column};
                        exploration_secondary_click_ =
                            *character_pointer_;
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
    // Lifetime of cached size-matched software backbuffer is the window,
    // never a per-frame CreateCompatibleBitmap/DeleteObject pair.
    HDC gdi_dc_{};
    HBITMAP gdi_bitmap_{};
    HGDIOBJ gdi_old_bitmap_{};
    int gdi_width_{},gdi_height_{};
    std::vector<std::uint8_t> base_indices_;
    rloft::Palette6 current_palette_{};
    std::vector<std::uint32_t> bgra_;
    std::optional<rloft::Type1Raster> game_cursor_;
    std::unique_ptr<rloft::Win32GpuScene> gpu_scene_;
    // Faithful DOS-derived world is the default until the GPU passes
    // live Level22 visual parity; F2 toggles the experimental OpenGL view.
    bool gpu_world_enabled_{false};
    bool gpu_world_visible_{};
    bool gpu_menu_overlay_{};
    // Nearest preserves original indexed VGA presentation; F3 opts
    // into smoothing/mipmapping without changing source data.
    unsigned gpu_filter_mode_{0U};
    double gpu_pitch_{};
    bool quit_{false};
    bool escape_pressed_{false};
    bool menu_mode_{false};
    bool character_mode_{false};
    bool inventory_mode_{false};
    bool automap_mode_{false};
    bool smooth_scaling_{false};
    bool fullscreen_{false};
    bool mouse_look_{false};
    bool strafe_left_{false},strafe_right_{false};
    bool map_toggle_requested_{false};
    double yaw_fraction_{};
    LONG_PTR saved_style_{};
    WINDOWPLACEMENT saved_placement_{sizeof(WINDOWPLACEMENT)};
    rloft::ExplorationLegacyInputController* exploration_input_{nullptr};
    rloft::DevSessionTrace* session_trace_{nullptr};
    int selection_{-1};
    std::optional<int> clicked_{};
    std::optional<NativePoint> character_click_{};
    std::optional<NativePoint> exploration_click_{};
    std::optional<NativePoint> exploration_release_{};
    std::optional<NativePoint> exploration_secondary_click_{};
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
    rloft::Win32MidiPlayer& midi_player);

std::filesystem::path native_save_path() {
    std::array<wchar_t,32768> path{};
    const auto n=GetEnvironmentVariableW(
        L"LOCALAPPDATA",path.data(),static_cast<DWORD>(path.size()));
    if(n==0U || n>=path.size())
        throw std::runtime_error("Windows LOCALAPPDATA required for game saves");
    return std::filesystem::path(path.data())/
        L"Hyperbog"/L"Ravenloft"/L"SavedGame-1.rls";
}
// Automap buttons 9 and 10 are actual CODE.2 03321/02A34 export
// commands. Keep DOS/CP437 bytes unchanged and write in the player's
// native writable directory, rather than to a potentially read-only CD.
std::filesystem::path write_original_automap(
    const rloft::LevelState& level,int source_level_id,
    bool printer_stream) {
    const auto source_relative=
        rloft::OriginalAutomapFileExport::source_filename(source_level_id);
    auto output=native_save_path().parent_path()/
        std::filesystem::path(source_relative);
    const auto bytes=printer_stream
        ? rloft::OriginalAutomapFileExport::printer_03321(level)
        : rloft::OriginalAutomapFileExport::merged_02a34(level);
    if(printer_stream)
        output.replace_extension(L".PRN");
    std::filesystem::create_directories(output.parent_path());
    auto staging=output;
    staging+=L".tmp";
    {
        std::ofstream file(staging,std::ios::binary|std::ios::trunc);
        if(!file)throw std::runtime_error("Cannot create original automap file");
        file.write(reinterpret_cast<const char*>(bytes.data()),
                   static_cast<std::streamsize>(bytes.size()));
        file.flush();
        if(!file)throw std::runtime_error("Cannot write original automap file");
    }
    std::error_code ec;
    std::filesystem::remove(output,ec);
    std::filesystem::rename(staging,output);
    return output;
}
std::uint64_t original_media_identity(const rloft::GameSource& source) {
    auto hash=std::uint64_t{14695981039346656037ULL};
    for(const auto* name:{"CODE.1","CODE.2","INIT","LEVELS.DAT",
                          "NPCS.DAT","OBJECTS.DAT"}) {
        const auto bytes=source.read(name);
        for(const auto ch:bytes) {
            hash^=ch;hash*=1099511628211ULL;
        }
        hash^=bytes.size();hash*=1099511628211ULL;
    }
    return hash;
}
rloft::EnhancedGameSnapshot read_native_save(
    const rloft::GameSource& media) {
    const auto path=native_save_path();
    std::ifstream in(path,std::ios::binary|std::ios::ate);
    if(!in)throw std::runtime_error(
        "No native Ravenloft saved game found. Start a New Game and Save first.");
    const auto length=in.tellg();
    if(length<24 || length>32LL*1024LL*1024LL)
        throw std::runtime_error("Native game save length invalid");
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(length));
    in.seekg(0);
    in.read(reinterpret_cast<char*>(bytes.data()),
            static_cast<std::streamsize>(bytes.size()));
    if(!in)throw std::runtime_error("Native save read was incomplete");
    auto state=rloft::EnhancedGameSaveCodec::decode(bytes);
    if(state.original_media_id!=original_media_identity(media))
        throw std::runtime_error(
            "Save was made with different original game files. "
            "Select the original media used when the save was made.");
    return state;
}
void write_native_save(const rloft::ExplorationRuntime& runtime,
                       const rloft::GameSource& media) {
    auto state=runtime.capture_native_checkpoint();
    state.original_media_id=original_media_identity(media);
    const auto bytes=rloft::EnhancedGameSaveCodec::encode(state);
    const auto path=native_save_path();
    std::filesystem::create_directories(path.parent_path());
    auto staging=path;staging+=L".tmp";
    {
        std::ofstream out(staging,std::ios::binary|std::ios::trunc);
        if(!out)throw std::runtime_error("Cannot create native save staging file");
        out.write(reinterpret_cast<const char*>(bytes.data()),
                  static_cast<std::streamsize>(bytes.size()));
        out.flush();
        if(!out)throw std::runtime_error("Native save write failed");
        out.close();
        if(!out)throw std::runtime_error("Native save close failed");
    }
    if(!MoveFileExW(staging.c_str(),path.c_str(),
        MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)) {
        std::error_code ec;
        std::filesystem::remove(staging,ec);
        throw std::runtime_error("Unable to commit native game save");
    }
}

int run_exploration(
    Host& host,
    rloft::RavenloftBootAssets& assets,
    rloft::Win32AudioPlayer& audio_player,
    rloft::Win32MidiPlayer& midi_player,
    const std::vector<std::uint8_t>& chars,
    bool exploration_preview = false,
    bool capture_render = false,
    std::size_t capture_limit = 16U,
    bool dev_inspect = false,
    bool dev_interact = false,
    bool dev_trace = false,
    const rloft::EnhancedGameSnapshot* saved_game = nullptr) {
    // The previous boot-session ends at the CODE.2 handoff. Begin the
    // separate, opt-in exploration diagnostic BEFORE initial world load;
    // otherwise a failing load_fresh never creates an exploration ZIP.
    std::unique_ptr<rloft::DevSessionTrace> trace;
    struct ExplorationTraceEarlyDetach final {
        Host& host;
        ~ExplorationTraceEarlyDetach() {host.set_session_trace(nullptr);}
    } trace_early_detach{host};
    if(dev_trace) {
        const auto root=std::filesystem::temp_directory_path()/
            "RLOFT-Demo-Diagnostics";
        const auto stamp=std::chrono::duration_cast<
            std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        const auto folder=root/
            (std::to_string(stamp)+"-"+std::to_string(GetCurrentProcessId()));
        trace=std::make_unique<rloft::DevSessionTrace>(
            folder,RLOFT_DEMO_SOURCE_SHA,
            rloft::to_string(assets.source().kind()));
        std::ofstream latest(root/"latest-session.txt",
                             std::ios::binary|std::ios::trunc);
        if(latest)latest<<folder.string()<<"\n";
        host.set_session_trace(trace.get());
    }
    const auto exploration_stage=[&](const char* stage) {
        if(!trace)return;
        try {trace->event(stage);}
        catch(...) {
            host.set_session_trace(nullptr);
            trace.reset();
        }
    };
    exploration_stage("exploration.load.begin");
    // CODE.2 expands/restores the mutable world immediately on level load.
    // Archive Level 22 uses the shipped RNG state/dice path; direct LEV022
    // state restores HP and actor state verbatim without consuming RNG.
    auto runtime=[&]() {
        try {
            return rloft::ExplorationRuntime::load_fresh(
                assets.source(), chars);
        } catch(...) {
            exploration_stage("exploration.load.failed");
            throw;
        }
    }();
    exploration_stage("exploration.load.ready");
    try {
        runtime.initialize_gameplay_state(assets.source());
        if(saved_game)
            runtime.restore_native_checkpoint(*saved_game,assets.source());
    } catch(...) {
        exploration_stage("exploration.world.failed");
        throw;
    }
    exploration_stage("exploration.world.ready");
    // Player-facing enhanced native RES3 rendering needs source world
    // plans, not the DOS self-modifying VGA instruction trace. Opt-in
    // --dev-trace can still generate the historical comparison evidence.
    runtime.set_enhanced_pc_scene_rendering(!dev_trace);
    rloft::ExplorationPresenter presenter(assets);
    rloft::TopEdgeMenuPresenter menu_presenter(assets);
    rloft::AutomapScreenPresenter automap_presenter(assets);
    const auto options_font=assets.res0_font(1);
    rloft::EnhancedOptionsPresenter options_presenter(options_font);
    rloft::PartyModalPresenter party_modal_presenter(assets);
    rloft::QuitConfirmPresenter quit_presenter(assets);
    exploration_stage("exploration.presenters.ready");

    // Enhanced PC audio: CODE.2 level header +0x29C is an authored RES5
    // XMIDI cue, not a DOS hardware timer or a made-up ambient track.
    // Keep the existing opening/character music unchanged; only activate
    // world music at the actual CODE.1 -> CODE.2 gameplay boundary.
    rloft::EnhancedWorldMusicState world_music_state;
    bool gameplay_music_enabled=true;
    bool gameplay_sound_enabled=true;
    // A new in-game session defaults to sampled SFX enabled,
    // even if an earlier exploration session disabled them.
    audio_player.set_enabled(true);
    const auto start_authored_world_music = [&]() {
        if (!gameplay_music_enabled) return;
        const auto level_id=runtime.session().level_id;
        const auto sequence_id=runtime.session().level.music_sequence;
        // Audio absence must not erase a playable scene. Deduplicate even
        // when the MIDI driver or the player's RES5 resource is unavailable.
        if(!world_music_state.should_start(level_id,sequence_id)) return;
        try {
            midi_player.play(
                rloft::parse_xmidi_sequence(
                    assets.music_sequence(
                        static_cast<int>(sequence_id)),1),
                0,100,0x9C4);
        } catch(const std::exception& error) {
            OutputDebugStringA("RLOFT native level-music unavailable: ");
            OutputDebugStringA(error.what());
            OutputDebugStringA("\n");
            if(trace) {
                try{trace->error("exploration.music",error.what());}
                catch(...) {}
            }
        }
    };
    start_authored_world_music();

    // Stage-only reporting on the very first actual gameplay viewport.
    // Rendering, original IRQ ticks and mutable world state are unchanged.
    bool first_scene_submitted = false;
    rloft::ExplorationLegacyInputController input;
    rloft::ExplorationUiController ui;
    std::optional<rloft::AutomapModalController> automap;
    std::optional<std::uint32_t> quit_random_seed;
    struct TraceHostDetach final {
        Host& host;
        rloft::IndexedFramebuffer& framebuffer;
        ~TraceHostDetach() {
            framebuffer.set_draw_audit(nullptr,nullptr);
            host.set_session_trace(nullptr);
        }
    } trace_detach{host,presenter.framebuffer()};
    if(trace) {
        presenter.framebuffer().set_draw_audit(
            [](void* context,const rloft::IndexedDrawAuditCall& draw) {
                try {
                    static_cast<rloft::DevSessionTrace*>(
                        context)->indexed_draw(draw);
                }catch(...) {
                    // Instrumentation must never interrupt original
                    // indexed framebuffer drawing.
                }
            },trace.get());
    }
    // Passive F12 diagnostic projections only; never invoke original
    // F790 renderer or change gameplay RNG on a native frame refresh.
    std::optional<rloft::OriginalTTextArchive> f790_trace_t_archive;
    if(trace)f790_trace_t_archive.emplace(assets.source().read("T"));
    std::optional<std::size_t> inventory_encumbrance_actor;
    std::optional<std::size_t> inventory_party_drag_source;
    // CODE.2 uses RES1 + OBJECTS item graphic for a held cursor,
    // returning to the original RES0 arrow after inventory closes.
    const auto normal_inventory_cursor=assets.res0_type1(
        static_cast<int>(assets.init_u16(0x00U))+0x2F);
    std::optional<std::int16_t> inventory_cursor_last_id;
    // Hit-testing is performed against the frame the player ACTUALLY saw,
    // never against a freshly reconstructed scene after the click arrives.
    std::optional<rloft::ExplorationRuntimeFrame> displayed_scene_frame;
    // Explicit native-PC two-click target selection. Original DOS
    // E5F0 also supports automatic DB94(-1), whose projected buffers
    // are not yet available. The hand is revalidated on actual hit.
    rloft::EnhancedCombatTargetArming pending_level22_weapon_hand;
    const auto combat_scene_stamp=[](
        const rloft::ExplorationRuntimeFrame& frame) {
        return rloft::EnhancedCombatSceneStamp{
            frame.world_scene_revision,frame.source_level_id,
            frame.source_coordinate_a,frame.source_coordinate_b,
            frame.source_facing};
    };
    // Palette is a CURRENT-LEVEL renderer parameter. A title Continue,
    // in-game Load, mist transition or ordinary portal may replace the
    // mutable world and its original DAC bank without replacing Host.
    auto palette_level_id=runtime.session().level_id;
    auto exploration_palette=
        assets.colors_palette(
            rloft::LevelDisplayModel::palette_index(
                static_cast<int>(palette_level_id)));
    const auto refresh_world_palette=[&]() {
        const auto level_id=runtime.session().level_id;
        if(level_id==palette_level_id)return;
        const auto next=assets.colors_palette(
            rloft::LevelDisplayModel::palette_index(
                static_cast<int>(level_id)));
        palette_level_id=level_id;
        exploration_palette=next;
        // The one Win32 WGL context stays alive. Only indexed material
        // textures need reupload with the new authentic source palette.
        host.begin_gpu_scene(assets,exploration_palette);
    };

    host.set_character_mode(false);
    host.set_menu_mode(false);
    host.set_exploration_input(&input);
    // The OpenGL renderer is a backend of the continuous CODE.2 game,
    // not a second world or a separate Level22 viewer. Original UI
    // remains in the same indexed framebuffer in all other modes.
    if(!dev_trace)host.begin_gpu_scene(assets,exploration_palette);
    SetWindowTextW(
        host.hwnd(),
        exploration_preview
            ? L"Ravenloft - Level 22 Walkaround Preview | "
              L"Arrow keys / Numpad to move | Esc to exit"
            : L"Ravenloft: Strahd's Possession - Exploration");
    std::wstring last_preview_title;
    // Opt-in source-neutral diagnostic, separate from the DOS
    // framebuffer and from writable game state.
    std::optional<rloft::DevLevelSnapshot> last_dev_snapshot;
    std::uint64_t dev_change_sequence=0U;
    // Both inventory render entry points share this identical
    // diagnostic overlay WITHOUT adding anything to original VGA.
    // Its lambda is invoked only in developer preview mode.
    const auto refresh_dev_inventory_caption=[&]() {
                if(exploration_preview && dev_inspect) {
                    const auto snapshot=rloft::DevLevelDiagnostics::capture(
                        runtime.session(),runtime.world(),runtime.inventory());
                    if(last_dev_snapshot &&
                       rloft::DevLevelDiagnostics::compare(
                           *last_dev_snapshot,snapshot).any())
                        ++dev_change_sequence;
                    last_dev_snapshot=snapshot;
                    const auto hex=[](std::uint64_t digest) {
                        const auto code=rloft::DevLevelSnapshot::digest8(digest);
                        return std::wstring(code.begin(),code.end());
                    };
                    const auto title=
                        std::wstring(L"Ravenloft - DEV Inventory | Actors ")+
                        std::to_wstring(snapshot.active_actors)+
                        L" | Open "+std::to_wstring(
                            snapshot.active_container_views)+
                        L" | Cursor "+std::to_wstring(
                            runtime.inventory().held.object_id)+
                        L" | Party "+hex(snapshot.party_hash)+
                        L" Inv "+hex(snapshot.inventory_hash)+
                        L" World "+hex(snapshot.world_hash)+
                        L" Story "+hex(snapshot.flags_hash)+
                        L" | State changes "+
                        std::to_wstring(dev_change_sequence);
                    SetWindowTextW(host.hwnd(),title.c_str());
                    // Restore the 3D preview title after modal exit,
                    // even when no state domain changed.
                    last_preview_title.clear();
                }
    };

    // Opt-in native-only forensic captures. No file I/O is performed
    // during ordinary game launches. One frame per changed cell/facing
    // with a fixed upper bound avoids retrace-paced disk flooding.
    std::vector<std::uint8_t> capture_code2;
    std::filesystem::path capture_directory;
    std::optional<std::tuple<std::uint32_t,int,int,std::uint32_t>>
        last_capture_position;
    std::size_t capture_count=0U;
    std::optional<std::string> capture_failure;
    capture_render = capture_render || dev_trace;
    if (capture_render) {
        capture_code2=assets.source().read("CODE.2");
        const auto session_id=std::chrono::duration_cast<
            std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        capture_directory=trace
            ? trace->directory()/"renderer"
            : std::filesystem::temp_directory_path()/
                "RLOFT-Demo-Captures"/std::to_string(session_id);
    }

    // Original CODE.2 201A7..201F7 anchors the simulation/input cycle
    // to a TWO-game-IRQ-tick deadline. The VGA page may be presented
    // at ~70 Hz, but running 1FC14/2E120 on EVERY retrace moved the
    // party roughly eight times too fast. BIOS 54,925-us tick duration
    // is a provisional source-backed fallback until real PIT data is
    // captured. Leave all original fixed-point movement math unchanged.
    rloft::ExplorationGameplayCadence gameplay_cadence;
    const auto exploration_epoch = Clock::now();
    auto next_frame = exploration_epoch;
    const auto screen_name=[&]() -> const char* {
        switch(ui.state().screen) {
            case rloft::ExplorationUiScreen::Viewport:return "viewport";
            case rloft::ExplorationUiScreen::Rest:return "rest";
            case rloft::ExplorationUiScreen::Pray:return "pray";
            case rloft::ExplorationUiScreen::Memorize:return "memorize";
            case rloft::ExplorationUiScreen::Automap:return "automap";
            case rloft::ExplorationUiScreen::Load:return "load";
            case rloft::ExplorationUiScreen::Save:return "save";
            case rloft::ExplorationUiScreen::QuitConfirm:return "quit_confirm";
            case rloft::ExplorationUiScreen::Options:return "options";
            case rloft::ExplorationUiScreen::Character:return "character";
            case rloft::ExplorationUiScreen::Inventory:return "inventory";
            case rloft::ExplorationUiScreen::PartyPanels:return "party_panels";
        }
        return "unknown";
    };
    const auto present_traced=[&](
        const rloft::IndexedFramebuffer& framebuffer,
        const rloft::Palette6& palette,
        const char* override_screen=nullptr) {
        if(trace) {
            try {
                const auto snapshot=rloft::DevLevelDiagnostics::capture(
                    runtime.session(),runtime.world(),runtime.inventory());
                trace->observe(
                    override_screen?override_screen:screen_name(),
                    framebuffer,palette,snapshot,
                    code1_tick_count(exploration_epoch));
            }catch(const std::exception& exc) {
                OutputDebugStringA(exc.what());
                host.set_session_trace(nullptr);
                presenter.framebuffer().set_draw_audit(nullptr,nullptr);
                trace.reset();
            }
        }
        host.present(framebuffer,palette);
    };

    while (host.pump()) {
        refresh_world_palette();
        host.set_gpu_world_visible(false);
        host.set_gpu_menu_overlay(false);
        if(host.consume_map_toggle()) {
            const auto current=ui.state().screen;
            if(current==rloft::ExplorationUiScreen::Automap) {
                ui.close_screen();
                automap.reset();
                host.set_automap_mode(false);
                input.clear();
            } else if(current==rloft::ExplorationUiScreen::Viewport) {
                rloft::ExplorationWorldModel::rebuild_automap_cache(
                    runtime.session().level,
                    rloft::ExplorationWorldModel::party_cell(runtime.session()));
                ui.open_automap();
                automap.emplace(runtime.session());
                host.set_automap_mode(true);
                input.clear();
            }
        }
        // Modal UI owns the pointer and keyboard exclusively; no captured
        // camera or held strafe may leak into an inventory/map/options panel.
        if(ui.state().screen!=rloft::ExplorationUiScreen::Viewport) {
            pending_level22_weapon_hand.cancel();
            host.stop_mouse_look();
        }
        // Re-select only if the authored level or its default cue changed,
        // or if a cinematic stopped the previous sequence.
        start_authored_world_music();
        if (host.consume_escape()) {
            if (ui.state().screen ==
                rloft::ExplorationUiScreen::Options) {
                ui.close_screen();
                input.clear();
                continue;
            }
            if (ui.state().screen ==
                rloft::ExplorationUiScreen::Automap) {
                // Host safety escape. The faithful mouse exit remains
                // automap panel action 11 below.
                ui.close_screen();
                automap.reset();
                host.set_automap_mode(false);
                input.clear();
                continue;
            }
            if (ui.state().screen ==
                rloft::ExplorationUiScreen::QuitConfirm) {
                // Host safety escape; CODE.2 07D44 itself is pointer-modal.
                ui.close_screen();
                quit_random_seed.reset();
                input.clear();
                continue;
            }
            if (ui.state().screen ==
                rloft::ExplorationUiScreen::PartyPanels) {
                // Host safety escape. The exact CODE.2 13118 modal-close
                // gesture is tracked separately and is not approximated here.
                ui.close_screen();
                input.clear();
                continue;
            }
            break;
        }

        if (ui.state().screen ==
            rloft::ExplorationUiScreen::Options) {
            // A native in-game options surface, not a second Windows
            // utility window. Options action applies immediately.
            if (const auto click=host.consume_exploration_click()) {
                const auto choice=rloft::EnhancedOptionsPresenter::action_at(
                    click->row,click->column);
                if (choice==
                    rloft::EnhancedOptionsSelection::ReturnToGame) {
                    ui.close_screen();
                    input.clear();
                    continue;
                }
                if (choice==
                    rloft::EnhancedOptionsSelection::ToggleMusic) {
                    gameplay_music_enabled=!gameplay_music_enabled;
                    if(!gameplay_music_enabled) {
                        midi_player.stop();
                    }
                    world_music_state.invalidate();
                    if(gameplay_music_enabled)start_authored_world_music();
                }
                if (choice==
                    rloft::EnhancedOptionsSelection::ToggleSound) {
                    gameplay_sound_enabled=!gameplay_sound_enabled;
                    audio_player.set_enabled(gameplay_sound_enabled);
                }
            }
            options_presenter.draw(
                presenter.framebuffer(),gameplay_music_enabled,
                gameplay_sound_enabled);
            present_traced(presenter.framebuffer(),exploration_palette);
            next_frame +=
                std::chrono::duration_cast<Clock::duration>(
                    std::chrono::duration<double>(
                        1.0 / kNativeGameplayRefreshHz));
            if (!host.wait_until(next_frame,false))break;
            const auto now=Clock::now();
            if(next_frame+std::chrono::milliseconds(100)<now)
                next_frame=now;
            continue;
        }

        if (ui.state().screen ==
            rloft::ExplorationUiScreen::Automap) {
            if (!automap) {
                automap.emplace(runtime.session());
            }

            // Repeatable arrow/Shift+arrow navigation uses the SAME
            // mapped map-controller actions as the clickable arrows.
            // The host captures these keys exclusively while the map
            // modal owns input: no latent party movement on return.
            if (const auto action=host.consume_key_event()) {
                (void)automap->apply_action(
                    *action,
                    runtime.session().level.width,
                    runtime.session().level.height);
            }

            if (const auto click =
                    host.consume_exploration_click()) {
                if (const auto action =
                        rloft::AutomapControlModel::action_at(
                            click->row,
                            click->column)) {
                    const auto result =
                        automap->apply_action(
                            *action,
                            runtime.session().level.width,
                            runtime.session().level.height);
                    if (result ==
                        rloft::AutomapModalResult::Close) {
                        ui.close_screen();
                        automap.reset();
                        host.set_automap_mode(false);
                        input.clear();
                        continue;
                    }
                    if(result==rloft::AutomapModalResult::DeferredAction) {
                        // CODE.2 02DB0 action9 is PRINT via 03321;
                        // action10 is SAVE via 02A34. Native map SAVE
                        // emits the original CP437/CRLF stream; PRINT
                        // preserves the original CR-overprint bytes in
                        // a .PRN file until a real printer path is added.
                        if(*action==9 || *action==10) {
                            try {
                                const auto output=write_original_automap(
                                    runtime.session().level,
                                    runtime.session().level_id,
                                    *action==9);
                                const auto message=(*action==9
                                    ? L"Original DOS printer stream saved to:\n"
                                      L"(Not sent to a Windows printer)\n"
                                    : L"Original automap saved to:\n")+
                                    output.wstring();
                                MessageBoxW(host.hwnd(),message.c_str(),
                                    L"Ravenloft - Automap",
                                    MB_OK|MB_ICONINFORMATION);
                            }catch(const std::exception& exc) {
                                const auto message=widen(exc.what());
                                MessageBoxW(host.hwnd(),message.c_str(),
                                    L"Ravenloft - Automap export",
                                    MB_OK|MB_ICONWARNING);
                            }
                        }
                    }
                }
            }

            automap_presenter.draw(
                presenter.framebuffer(),
                runtime.session(),
                runtime.renderer_tables(),
                automap->state(),
                automap->annotation_overlay_enabled());
            present_traced(presenter.framebuffer(),exploration_palette);

            next_frame +=
                std::chrono::duration_cast<Clock::duration>(
                    std::chrono::duration<double>(
                        1.0 / kNativeGameplayRefreshHz));
            if (!host.wait_until(next_frame, false)) {
                break;
            }
            const auto now = Clock::now();
            if (next_frame +
                    std::chrono::milliseconds(100) <
                now) {
                next_frame = now;
            }
            continue;
        }

        if (ui.state().screen ==
            rloft::ExplorationUiScreen::QuitConfirm) {
            if (!quit_random_seed) {
                // 314B0 leaves 30CA2 reseeded from the live IRQ-8 tick
                // (30700). The host tracks the same BIOS cadence from
                // exploration entry; whole-program pre-entry phase remains
                // intentionally outside this presentation model.
                quit_random_seed =
                    code1_tick_count(exploration_epoch);
            }

            if (const auto click =
                    host.consume_exploration_click()) {
                const auto choice =
                    rloft::QuitConfirmModel::choice_at(
                        click->row,
                        click->column);
                if (choice !=
                    rloft::QuitConfirmChoice::None) {
                    quit_presenter.draw(
                        presenter.framebuffer(),
                        *quit_random_seed,
                        choice);
                    present_traced(presenter.framebuffer(),exploration_palette);
                    if (!host.pause_frame(
                            std::chrono::duration<double>(
                                1.0 / kNativeGameplayRefreshHz))) {
                        break;
                    }

                    if (choice ==
                        rloft::QuitConfirmChoice::Confirm) {
                        host.set_exploration_input(nullptr);
                        return 0;
                    }

                    ui.close_screen();
                    quit_random_seed.reset();
                    input.clear();
                    continue;
                }
            }

            quit_presenter.draw(
                presenter.framebuffer(),
                *quit_random_seed);
            present_traced(presenter.framebuffer(),exploration_palette);

            next_frame +=
                std::chrono::duration_cast<Clock::duration>(
                    std::chrono::duration<double>(
                        1.0 / kNativeGameplayRefreshHz));
            if (!host.wait_until(next_frame, false)) {
                break;
            }
            const auto now = Clock::now();
            if (next_frame +
                    std::chrono::milliseconds(100) <
                now) {
                next_frame = now;
            }
            continue;
        }

        host.set_inventory_mode(
            ui.state().screen==rloft::ExplorationUiScreen::PartyPanels);
        if (ui.state().screen !=
            rloft::ExplorationUiScreen::PartyPanels &&
            inventory_cursor_last_id) {
            host.set_game_cursor(normal_inventory_cursor);
            inventory_cursor_last_id.reset();
        }
        if (ui.state().screen ==
            rloft::ExplorationUiScreen::PartyPanels) {
            // CODE.2 13118 -> 6BA64 maps scan 20h (the D key)
            // to 44h. Only this modal accepts world-drop commands.
            if(const auto key=host.consume_key_event()) {
                if(*key==0x44) {
                    // Original 2D5D0(y,x,0x190) now belongs to the
                    // runtime. The mode number must never be used
                    // as a Q16 radius for a UI-side approximation.
                    const auto dropped=runtime.drop_held_inventory_item();
                    if(dropped.status==
                        rloft::InventoryWorldDropStatus::Dropped) {
                        displayed_scene_frame.reset();
                        SetWindowTextW(host.hwnd(),
                            L"Ravenloft - Inventory | Item dropped");
                    }else if(dropped.status==
                        rloft::InventoryWorldDropStatus::
                            SourceSpecialActionPending) {
                        // Source CODE.2 21880 executes:
                        // fourth Seed append -> flag 44h ->
                        // 13EF8(8) / CINE08.ANM -> 15050(73h).
                        // Only this event-specific inventory action
                        // re-enters the already native ANM player.
                        // Original file is 182 frames and its source
                        // payload must load BEFORE starting the
                        // transactional world/party mutation.
                        try {
                            rloft::AnmCinematic cine08(
                                assets.source().read(
                                    rloft::Level16SeedRitualCheck::
                                        kNumberedEventFilename),
                                static_cast<int>(
                                    rloft::Level16SeedRitualCheck::
                                        kNumberedEventId),
                                assets);
                            const auto completed=
                                runtime.complete_pending_seed_ritual(
                                    [&]() {
                                        // The original 13EF8 event
                                        // takes full display ownership.
                                        // Prevent stale modal pointer
                                        // input and item cursor over
                                        // the full-screen animation.
                                        host.set_exploration_input(nullptr);
                                        host.set_inventory_mode(false);
                                        host.set_game_cursor(
                                            normal_inventory_cursor);
                                        const auto result=play_cinematic(
                                            host,cine08,assets,
                                            audio_player,midi_player);
                                        audio_player.stop_all();
                                        midi_player.stop();
                                        world_music_state.invalidate();
                                        host.set_exploration_input(&input);
                                        host.set_inventory_mode(true);
                                        if(result==CinematicResult::Quit)
                                            return rloft::
                                                Level16RitualPlayback::Quit;
                                        if(result==CinematicResult::Escaped)
                                            return rloft::
                                                Level16RitualPlayback::Escaped;
                                        return rloft::
                                            Level16RitualPlayback::Completed;
                                    });
                            // 13C88's complete DOS graphics teardown
                            // still needs standalone pixel/tick audit.
                            // Reset the host's presented-frame cache and
                            // retrace pacing after blocking event8.
                            displayed_scene_frame.reset();
                            inventory_cursor_last_id.reset();
                            next_frame=Clock::now();
                            if(completed.status==
                                rloft::Level16RitualCompletionStatus::
                                    Quit) {
                                host.set_exploration_input(nullptr);
                                return 0;
                            }
                            if(completed.status==
                                rloft::Level16RitualCompletionStatus::
                                    Completed) {
                                SetWindowTextW(host.hwnd(),
                                    L"Ravenloft - Level 16 | "
                                    L"Seed ritual completed");
                            }else if(completed.status==
                                rloft::Level16RitualCompletionStatus::
                                    Escaped) {
                                SetWindowTextW(host.hwnd(),
                                    L"Ravenloft - Level 16 | "
                                    L"Event 8 interrupted; Seed restored");
                            }else {
                                SetWindowTextW(host.hwnd(),
                                    L"Ravenloft - Level 16 | "
                                    L"Seed ritual placement unresolved");
                            }
                        }catch(const std::exception&) {
                            // Event file decode or graphics/audio failure:
                            // the core transaction restores world/cursor/
                            // story bit before this branch is reached.
                            // Never leave looping CINE08 audio playing
                            // over the resumed original inventory panel.
                            audio_player.stop_all();
                            midi_player.stop();
                            world_music_state.invalidate();
                            host.set_exploration_input(&input);
                            host.set_inventory_mode(true);
                            host.set_game_cursor(normal_inventory_cursor);
                            inventory_cursor_last_id.reset();
                            displayed_scene_frame.reset();
                            next_frame=Clock::now();
                            SetWindowTextW(host.hwnd(),
                                L"Ravenloft - Level 16 | "
                                L"Original event 8 unavailable");
                        }
                    }
                }
            }
            // CODE.2 13184..131BB compares unsigned 30700 tick
            // against 6DA8 once per modal loop, restoring the
            // item ribbon only after tick > deadline.
            const bool f790_expired=
                runtime.inventory().expire_scribe_feedback_13184(
                    code1_tick_count(exploration_epoch));
            if(trace && f790_expired) {
                try {
                    const auto event=
                        rloft::OriginalF790FeedbackSourceSnapshot::
                            after_expiry(true,runtime.inventory());
                    if(event) {
                        trace->event("f790.source.expiry",
                            event->source_e8_offset,event->item_id);
                        trace->event("f790.source.flags",
                            event->known?1:0,event->pending_6dac?1:0);
                    }
                }catch(const std::exception& exc) {
                    try{trace->error("f790.source.expiry",exc.what());}
                    catch(...) {}
                }
            }
            const auto held_cursor_id=runtime.inventory().held.object_id;
            if(!inventory_cursor_last_id ||
               *inventory_cursor_last_id!=held_cursor_id) {
                if(held_cursor_id>=0) {
                    const auto& object=runtime.object_catalog().at(
                        static_cast<std::size_t>(held_cursor_id));
                    try {
                        const auto sprite=assets.type1(
                            1,static_cast<int>(assets.init_u16(0x02U))+
                              object.ui_graphic_index);
                        host.set_game_cursor(sprite);
                    }catch(const std::exception&) {
                        host.set_game_cursor(normal_inventory_cursor);
                    }
                } else {
                    host.set_game_cursor(normal_inventory_cursor);
                }
                inventory_cursor_last_id=held_cursor_id;
            }
            if (host.consume_exploration_secondary_click()) {
                // CODE.2 12E70 cleanup on flags&2 calls 1F8FC, which sets
                // D680=0; 13118 then leaves the modal loop.
                ui.close_screen();
                inventory_encumbrance_actor.reset();
                inventory_party_drag_source.reset();
                input.clear();
                continue;
            }

            if (const auto click =
                    host.consume_exploration_click()) {
                if(inventory_encumbrance_actor) {
                    // Original 12988 weight modal consumes the next
                    // click before returning to ordinary PartyPanels.
                    inventory_encumbrance_actor.reset();
                }
                else if(click->row>=0 && click->row<0x10 &&
                        click->column>=0 && click->column<320) {
                    // CODE.2 12E70 -> 12470 starts actor-column
                    // drag in the first 16 source VGA rows. Finish
                    // only when Win32 reports the matching release.
                    const auto actor=static_cast<std::size_t>(
                        click->column/0x50);
                    if(runtime.session().party.active(actor)) {
                        inventory_party_drag_source=actor;
                    }
                }
                else if(const auto weight_actor=
                    rloft::PartyPanelControlModel::
                        modal_encumbrance_actor_at(
                            click->row,click->column)) {
                    if(runtime.session().party.active(*weight_actor)) {
                        if(trace) {
                            try {trace->event(
                                "party.encumbrance.open",
                                static_cast<std::int64_t>(*weight_actor));}
                            catch(...) {}
                        }
                        inventory_encumbrance_actor=*weight_actor;
                    }
                }
                else if (const auto actor =
                        rloft::PartyPanelControlModel::
                            modal_toggle_actor_at(
                                click->row,
                                click->column)) {
                    if (runtime.session().party.active(*actor)) {
                        ui.toggle_party_panel(*actor);
                    }
                }
                else if (const auto container_hit =
                             [&]() -> std::optional<
                                 rloft::PartyPanelControlModel::
                                    ContainerSlotHit> {
                    const auto& inv=runtime.inventory();
                    // Original CODE.2 12E70 resolves actor from the
                    // CLICKED column (signed division by 0x50)
                    // and reads that actor's own 6FA0 state. A
                    // previously clicked different actor must not
                    // hijack the container slot dispatch.
                    const auto actor=
                        rloft::PartyPanelControlModel::
                            modal_actor_at_column(click->column);
                    if(!actor || ui.party_panel_character_mode(*actor) ||
                       !runtime.session().party.active(*actor))
                        return std::nullopt;
                    const auto& context=
                        inv.party_container_ui_contexts[*actor];
                    if(!context.fully_open())
                        return std::nullopt;
                    const auto index=static_cast<std::size_t>(
                        context.raw[1]);
                    if(index>=inv.containers.size())
                        return std::nullopt;
                    const auto& grid=inv.containers[index];
                    if(grid.subtype!=context.raw[0])
                        return std::nullopt;
                    return rloft::PartyPanelControlModel::
                        modal_container_slot_at(
                            click->row,click->column,*actor,
                            grid.ui_columns,grid.ui_x_offset,
                            grid.items.size());
                }()) {
                    // Original CODE.2 12E70 -> 11C3C routes by
                    // ACTOR, not by whichever container the cursor
                    // touched last. Revalidate both real hand
                    // candidates before selecting that actor's
                    // phase3 source-backed container grid.
                    const auto result=runtime.inventory().
                        swap_actor_container_slot(
                            runtime.session().party,
                            runtime.object_catalog(),
                            container_hit->actor,
                            container_hit->slot);
                    if(result==rloft::InventoryContainerStatus::Inserted ||
                       result==rloft::InventoryContainerStatus::Taken) {
                        displayed_scene_frame.reset();
                    }
                }
                else if (const auto hit =
                             rloft::PartyPanelControlModel::
                                 modal_ordinary_slot_at(
                                     click->row, click->column)) {
                    // CODE.2 12E70/117A0: ordinary inventory identity
                    // and known-bit exchange. Character-mode columns,
                    // inactive actors, typed equipment and container
                    // insertion remain outside this bounded route.
                    if (!ui.party_panel_character_mode(hit->actor) &&
                        runtime.session().party.active(hit->actor)) {
                        // Source 1173B receives the scroll BEFORE
                        // the successful 1183E consumption. Preserve
                        // that old cursor and the original actor slot
                        // known bit before the native action mutates it.
                        const auto f790_hand_before=
                            runtime.inventory().held;
                        const auto f790_slot_before=
                            rloft::InventoryRuntimeState::
                                ordinary_slot_before_117a0(
                                    runtime.session().party,
                                    hit->actor,hit->slot);
                        const auto status =
                            runtime.inventory().swap_ordinary_slot(
                                runtime.session().party,
                                runtime.object_catalog(),
                                hit->actor,
                                hit->slot,
                                &runtime.session().persistent_event_flags,
                                code1_tick_count(exploration_epoch));
                        if(trace && f790_trace_t_archive) {
                            try {
                                const auto projected=
                                    rloft::OriginalF790OrdinarySlotEventPlan::
                                        after_117a0(
                                            status,f790_hand_before,
                                            f790_slot_before,
                                            runtime.inventory(),
                                            *f790_trace_t_archive);
                                for(std::size_t i=0;
                                    i<projected.count;++i) {
                                    const auto& event=projected.at(i);
                                    // Log E8 source, item ID and flags,
                                    // not pending message contents.
                                    trace->event("f790.source.ordinary",
                                        event.source_e8_offset,event.item_id);
                                    trace->event("f790.source.flags",
                                        event.known?1:0,
                                        event.pending_6dac?1:0);
                                }
                            }catch(const std::exception& exc) {
                                // Instrumentation must not break gameplay.
                                try{trace->error(
                                    "f790.source.ordinary",exc.what());}
                                catch(...) {}
                            }
                        }
                        if (status ==
                            rloft::InventoryOrdinarySwapStatus::Swapped ||
                            status ==
                            rloft::InventoryOrdinarySwapStatus::
                                ConsumedStoryFragment ||
                            status ==
                            rloft::InventoryOrdinarySwapStatus::
                                LearnedMageSpell ||
                            status ==
                            rloft::InventoryOrdinarySwapStatus::
                                MageScrollRejected ||
                            status ==
                            rloft::InventoryOrdinarySwapStatus::
                                InsertedInContainer) {
                            // Party equipment and cursor state are now
                            // different. Never act on a stale world hit
                            // snapshot after inventory mutations.
                            displayed_scene_frame.reset();
                            const auto held=runtime.inventory().held;
                            const auto text =
                                held.object_id == -1
                                    ? std::wstring(
                                        L"Ravenloft - Inventory | "
                                        L"Cursor empty")
                                    : std::wstring(
                                        L"Ravenloft - Inventory | "
                                        L"Held item ID ") +
                                        std::to_wstring(held.object_id) +
                                        (held.known != 0
                                            ? L" (identified)"
                                            : L" (unknown)");
                            SetWindowTextW(host.hwnd(),text.c_str());
                        } else if (status ==
                            rloft::InventoryOrdinarySwapStatus::Unsupported) {
                            SetWindowTextW(
                                host.hwnd(),
                                L"Ravenloft - Inventory | "
                                L"Special/container interaction pending");
                        }
                    }
                }
                else if (const auto equipment_hit =
                    party_modal_presenter.equipment_hit_at(
                        runtime.session().party,
                        runtime.inventory(),
                        click->row,click->column)) {
                    if(!ui.party_panel_character_mode(
                           equipment_hit->actor)) {
                        const auto result=runtime.inventory().
                            swap_equipment_slot(
                                runtime.session().party,
                                runtime.object_catalog(),
                                equipment_hit->actor,
                                equipment_hit->slot);
                        if(result==
                            rloft::InventoryEquipmentStatus::Swapped) {
                            // CODE.2 12180: original category-driven
                            // slot exchange preserves the known bit.
                            // An equipped category-1 hand object opens
                            // its own live nested container view.
                            displayed_scene_frame.reset();
                        } else if(result==
                            rloft::InventoryEquipmentStatus::ClassRestricted) {
                            SetWindowTextW(host.hwnd(),
                                L"Ravenloft - Inventory | "
                                L"Class cannot equip this armor");
                        } else if(result==
                            rloft::InventoryEquipmentStatus::WrongSlot) {
                            SetWindowTextW(host.hwnd(),
                                L"Ravenloft - Inventory | "
                                L"Wrong equipment slot");
                        }
                    }
                }
                // Remaining original spell-use, world-drop, nested
                // category-1 exceptions and scripted effects remain
                // distinct paths. Do not silently coerce them into
                // a generic item exchange.
            }
            if (const auto release=host.consume_exploration_release()) {
                if(inventory_party_drag_source) {
                    if(release->row>=0 && release->row<0x10 &&
                       release->column>=0 && release->column<320) {
                        const auto target=static_cast<std::size_t>(
                            release->column/0x50);
                        const auto source=*inventory_party_drag_source;
                        if(target!=source) {
                            auto panel_modes=
                                ui.state().party_panel_character_mode;
                            const auto done=runtime.exchange_party_order(
                                source,target,panel_modes);
                            if(done.changed) {
                                ui.exchange_party_panel_state(source,target);
                                displayed_scene_frame.reset();
                            }
                        }
                    }
                    inventory_party_drag_source.reset();
                }
            }

            const auto pointer=host.exploration_pointer();
            const auto modal_pointer=pointer
                ? std::optional<rloft::InventoryPointer>{
                      rloft::InventoryPointer{
                          pointer->row,pointer->column}}
                : std::nullopt;
            try {
                (void)party_modal_presenter.draw(
                    presenter.framebuffer(),
                    runtime.session().party,
                    ui.state().party_panel_character_mode,
                    ui.state().selected_party_actor,
                    &runtime.inventory(),modal_pointer,
                    inventory_encumbrance_actor);
            } catch(const std::exception& exc) {
                // The modal renderer can throw before present_traced()
                // observes a frame (as on #649's missing RES2 bank).
                // Persist source stage and reason in the F12 session
                // while preserving the original exception behavior.
                if(trace) {
                    try {trace->error("party_modal.draw",exc.what());}
                    catch(...) {}
                }
                throw;
            }
            present_traced(presenter.framebuffer(),exploration_palette);
            // Report the live authoritative inventory even while
            // its original modal is open. This is exclusively
            // text in the opt-in developer Windows title bar.
            refresh_dev_inventory_caption();

            next_frame +=
                std::chrono::duration_cast<Clock::duration>(
                    std::chrono::duration<double>(
                        1.0 / kNativeGameplayRefreshHz));
            if (!host.wait_until(next_frame, false)) {
                break;
            }
            const auto now = Clock::now();
            if (next_frame +
                    std::chrono::milliseconds(100) <
                now) {
                next_frame = now;
            }
            continue;
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

            if (pressed_menu >= 0) {
                pending_level22_weapon_hand.cancel();
                if(trace) {
                    try {trace->event("top_menu.select",pressed_menu);}
                    catch(...) {}
                }
                const auto action =
                    rloft::TopEdgeMenuModel::
                        action_for_selection(
                            pressed_menu);

                // The same native checkpoint powers Save/Load and the
                // title-screen Continue item. No DOS SAVE.DAT mutation.
                if(action==rloft::TopEdgeMenuAction::Save ||
                   action==rloft::TopEdgeMenuAction::Load) {
                    try {
                        if(action==rloft::TopEdgeMenuAction::Save) {
                            write_native_save(runtime,assets.source());
                            SetWindowTextW(host.hwnd(),
                                L"Ravenloft - Game Saved (native slot 1)");
                        } else {
                            auto saved=read_native_save(assets.source());
                            runtime.restore_native_checkpoint(
                                saved,assets.source());
                            displayed_scene_frame.reset();
                            input.clear();
                            world_music_state.invalidate();
                            start_authored_world_music();
                            SetWindowTextW(host.hwnd(),
                                L"Ravenloft - Game Loaded (native slot 1)");
                        }
                    }catch(const std::exception& e) {
                        const auto message=widen(e.what());
                        MessageBoxW(host.hwnd(),message.c_str(),
                            L"Ravenloft - Save / Load",
                            MB_OK|MB_ICONWARNING);
                    }
                }
                // Automap/Quit/Options remain original modal controllers.
                if (action ==
                        rloft::TopEdgeMenuAction::Automap ||
                    action ==
                        rloft::TopEdgeMenuAction::Quit ||
                    action ==
                        rloft::TopEdgeMenuAction::Options) {
                    ui.update_mouse(
                        click->column,
                        click->row);
                    if (ui.activate_top_edge_selection()) {
                        if (ui.state().screen ==
                            rloft::ExplorationUiScreen::Automap) {
                            // CODE.2 03634 runs 01558 before first map draw.
                            rloft::ExplorationWorldModel::
                                rebuild_automap_cache(
                                    runtime.session().level,
                                    rloft::ExplorationWorldModel::
                                        party_cell(
                                            runtime.session()));
                            automap.emplace(
                                runtime.session());
                            host.set_automap_mode(true);
                        } else if (
                            ui.state().screen ==
                            rloft::ExplorationUiScreen::QuitConfirm) {
                            quit_random_seed =
                                code1_tick_count(
                                    exploration_epoch);
                        }
                        input.clear();
                    }
                }
            }

            if (pressed_menu < 0) {
                bool opened_actor_panel = false;
                bool handled_hand_click = false;
                // Original D684 gameplay hand rectangles are distinct
                // from actor inventory panels. Source 214B8 first
                // validates active actor, then 24478 validates BOTH
                // equipped hands before 26018 categorizes the action.
                if(const auto hand=rloft::PartyPanelControlModel::
                       gameplay_hand_at(click->row,click->column)) {
                    handled_hand_click=true;
                    try {
                        const auto plan=
                            rloft::CombatLiveHandRequestModel::inspect(
                                runtime.session().party,
                                runtime.object_catalog(),
                                runtime.session().level,
                                hand->actor,hand->hand);
                        using HandStatus=rloft::CombatLiveHandStatus;
                        const wchar_t* status=L"Hand action pending";
                        switch(plan.status) {
                          case HandStatus::AttackCandidate:
                            if(rloft::DevLevelInteractionPolicy::allow(
                                   exploration_preview,dev_interact) &&
                               runtime.session().level_id==
                                   rloft::Level22Story::kLevelId) {
                                if(displayed_scene_frame) {
                                    pending_level22_weapon_hand.arm(
                                        hand->actor,hand->hand,
                                        combat_scene_stamp(
                                            *displayed_scene_frame));
                                } else {
                                    pending_level22_weapon_hand.cancel();
                                }
                                status=displayed_scene_frame
                                    ? L"Weapon selected | click the visible Assassin"
                                    : L"Source viewport not ready";
                            } else {
                                pending_level22_weapon_hand.cancel();
                                status=L"Original automatic combat "
                                       L"target cache pending";
                            }
                            break;
                          case HandStatus::ActorBlocked:
                            status=L"Character cannot attack";
                            break;
                          case HandStatus::ActionBusy:
                            status=L"Character hand is busy";
                            break;
                          case HandStatus::PairVetoed:
                            status=L"Original two-hand weapon rules "
                                   L"block this hand";
                            break;
                          case HandStatus::MissingOriginalEmptyHandData:
                            status=L"Original unarmed hand memory "
                                   L"is not yet reconstructed";
                            break;
                          case HandStatus::SpecialAction:
                            status=L"Original special-item "
                                   L"action pending";
                            break;
                          case HandStatus::CategoryEAction:
                            status=L"Original category-E "
                                   L"action pending";
                            break;
                          case HandStatus::OtherAction:
                            status=L"Original item action pending";
                            break;
                        }
                        if(plan.status!=HandStatus::AttackCandidate)
                            pending_level22_weapon_hand.cancel();
                        SetWindowTextW(host.hwnd(),status);
                        if(trace) {
                            try {trace->event(
                                "combat.hand.gate",
                                static_cast<std::int64_t>(hand->actor*2U+
                                    hand->hand),
                                static_cast<std::int64_t>(plan.status));}
                            catch(...) {}
                        }
                    }catch(const std::exception& error) {
                        pending_level22_weapon_hand.cancel();
                        const auto message=widen(error.what());
                        SetWindowTextW(host.hwnd(),message.c_str());
                        if(trace) {
                            try {trace->error("combat.hand.gate",
                                error.what());}
                            catch(...) {}
                        }
                    }
                }
                if (const auto actor =
                        rloft::PartyPanelControlModel::
                            gameplay_actor_at(
                                click->row,
                                click->column)) {
                    if (runtime.session().party.active(*actor)) {
                        ui.open_party_panels(*actor);
                        // 13118 resets 6DA8/6DAC at modal entry.
                        runtime.inventory().reset_scribe_feedback_13118();
                        input.clear();
                        opened_actor_panel = true;
                        pending_level22_weapon_hand.cancel();
                    }
                }

                // Enhanced-PC explicit target selection: clicking an
                // eligible hand arms the authentic E5F0 weapon slice;
                // clicking a source-verified class0 Assassin tests the
                // actual source NPC rather than a guessed billboard.
                bool attacked_assassin=false;
                if(!handled_hand_click && !opened_actor_panel &&
                   pending_level22_weapon_hand.pending()) {
                    // Every target click is one attempted action, even
                    // when no NPC is beneath it or the seen scene changed.
                    // It must NEVER fall through to loose-item pickup.
                    attacked_assassin=true;
                    const auto selected=pending_level22_weapon_hand.take(
                        displayed_scene_frame
                            ? std::optional<rloft::EnhancedCombatSceneStamp>(
                                combat_scene_stamp(*displayed_scene_frame))
                            : std::nullopt);
                    if(!selected.valid ||
                       runtime.session().level_id!=
                           rloft::Level22Story::kLevelId ||
                       !rloft::DevLevelInteractionPolicy::allow(
                           exploration_preview,dev_interact)) {
                        SetWindowTextW(host.hwnd(),
                            L"Ravenloft - Scene changed; reselect weapon");
                    } else try {
                        const auto outcome=
                            runtime.attack_level22_assassin_at(
                                *displayed_scene_frame,
                                click->row,click->column,
                                selected.actor,selected.hand);
                        if(!outcome.found_assassin) {
                            SetWindowTextW(host.hwnd(),
                                L"Ravenloft - No Assassin at that target");
                        }
                        if(outcome.found_assassin) {
                            attacked_assassin=true;
                            displayed_scene_frame.reset();
                            input.clear();
                            const auto caption=
                                outcome.injury && outcome.injury->died
                                    ? outcome.experience &&
                                        outcome.experience->
                                            level_change_services_pending
                                        ? L"Ravenloft - Assassin defeated | "
                                          L"XP earned; level-up pending"
                                        : L"Ravenloft - Assassin defeated | "
                                          L"XP earned and four relics dropped"
                                    : outcome.strike.
                                          target_suppressed_by_rear_party_rule
                                      ? L"Ravenloft - Rear-party attack "
                                        L"has no valid target"
                                      : outcome.strike.hit.hit
                                        ? L"Ravenloft - Assassin struck"
                                        : L"Ravenloft - Attack missed";
                            SetWindowTextW(host.hwnd(),caption);
                            if(outcome.injury && gameplay_sound_enabled) {
                                // Original E284 survivor and corpse sounds
                                // come from DIFFERENT INIT+14/+16 RES6
                                // banks, never an NPC type hardcoded sound.
                                // Native immediate SFX is the temporary
                                // presentation adapter until original 28110
                                // priority/queue sequencing is implemented.
                                try {
                                    const auto sound=
                                        rloft::OriginalNpcSoundModel::res6_id(
                                            assets.source().read("INIT"),
                                            *outcome.injury);
                                    if(sound)
                                        audio_player.play(
                                            rloft::decode_creative_voice(
                                                assets.sampled_audio(
                                                    6,*sound)));
                                }catch(const std::exception& audio_error) {
                                    // HP/RNG/world already committed: SFX
                                    // decoding must never roll them back
                                    // or masquerade as a failed attack.
                                    if(trace) {
                                        try{trace->error(
                                            "combat.level22.audio",
                                            audio_error.what());}
                                        catch(...) {}
                                    }
                                }
                            }
                            if(trace) {
                                try {trace->event(
                                    "combat.level22.strike",
                                    static_cast<std::int64_t>(
                                        outcome.strike.damage_after_c910),
                                    outcome.injury &&
                                        outcome.injury->died?1:0);}
                                catch(...) {}
                            }
                        }
                    }catch(const std::exception& error) {
                        attacked_assassin=true;
                        displayed_scene_frame.reset();
                        if(trace) {
                            try {trace->error(
                                "combat.level22.strike",error.what());}
                            catch(...) {}
                        }
                        const auto message=widen(error.what());
                        SetWindowTextW(host.hwnd(),message.c_str());
                    }
                }
                // An unclaimed viewport click keeps the original 223F4
                // family-C pickup route. Do not use click-to-damage
                // as a fallback for a rejected original weapon action.
                if (rloft::DevLevelInteractionPolicy::allow(
                        exploration_preview,dev_interact) &&
                    !opened_actor_panel && !handled_hand_click &&
                    !attacked_assassin &&
                    displayed_scene_frame.has_value() &&
                    runtime.session().level_id ==
                        rloft::Level22Story::kLevelId &&
                    // CODE.2's held-item replacement/swap UI is still
                    // unimplemented. Never silently destroy a held item.
                    runtime.inventory().held.object_id == -1) {
                    bool cinematic_input_detached=false;
                    try {
                        // Use ONE staged transaction for original 21704
                        // pickup and 21F70 mist. A cancelled/failed
                        // frame must NOT remove the fourth loose relic.
                        const auto progress=
                            runtime.pickup_and_advance_level22_at(
                                *displayed_scene_frame,
                                click->row,click->column,assets.source(),
                                [&](std::size_t frame,
                                    rloft::ExplorationRuntime& staged)
                                    ->bool {
                                    if(!cinematic_input_detached) {
                                        // Original event BF30 locks input.
                                        host.set_exploration_input(nullptr);
                                        host.set_gpu_world_visible(false);
                                        cinematic_input_detached=true;
                                    }
                                    const auto snapshot=staged.build_frame();
                                    const auto rendered=presenter.present(
                                        snapshot,staged.renderer_tables());
                                    // Explicit native enhancement until
                                    // original RES mist frame compositor
                                    // 2E510/1CD48 is fully reconstructed.
                                    const int fade=frame==21U?4:
                                        std::max(0,8-
                                            static_cast<int>(
                                                frame*8U/20U));
                                    const auto palette=
                                        rloft::StartupPresenter::fade_factor(
                                            rendered.palette,fade);
                                    host.present(
                                        presenter.framebuffer(),palette);
                                    return host.pause_frame(
                                        std::chrono::duration<double>(
                                            1.0/30.0));
                                },
                                code1_tick_count(exploration_epoch));
                        if(cinematic_input_detached)
                            host.set_exploration_input(&input);
                        if(progress.pickup) {
                            if(trace) {
                                try{trace->event(
                                    "world.pickup",
                                    progress.pickup->object_id,
                                    progress.entered_level23?1:0);}
                                catch(...) {}
                            }
                            displayed_scene_frame.reset();
                            input.clear();
                            if(progress.entered_level23) {
                                refresh_world_palette();
                                world_music_state.invalidate();
                                start_authored_world_music();
                                SetWindowTextW(host.hwnd(),
                                    L"Ravenloft - Level 23 | "
                                    L"Original quest transition completed");
                                if(trace) {
                                    try{trace->event(
                                        "level22.mist.enter23",23);}
                                    catch(...) {}
                                }
                            }
                        } else if(progress.transition_cancelled) {
                            // Neither fourth pickup nor new level nor
                            // consumption was committed. A subsequent
                            // pickup can retry the original event.
                            displayed_scene_frame.reset();
                            input.clear();
                        }
                    } catch(const std::logic_error&) {
                        if(cinematic_input_detached)
                            host.set_exploration_input(&input);
                        // Stale source hit selection or invalid quest
                        // preflight must not target a new live record.
                        displayed_scene_frame.reset();
                    } catch(const std::exception& error) {
                        if(cinematic_input_detached)
                            host.set_exploration_input(&input);
                        displayed_scene_frame.reset();
                        // The world transaction rolls back before its
                        // commit; post-commit music/palette errors cannot
                        // be described as a rolled-back transition.
                        if(runtime.session().level_id==23U) {
                            try{refresh_world_palette();}
                            catch(...) {}
                        }
                        const auto detail=widen(error.what());
                        MessageBoxW(host.hwnd(),detail.c_str(),
                            L"Ravenloft - Level22 transition",
                            MB_OK|MB_ICONWARNING);
                    }
                }
            }
        }

        // Native input adapter: for now a right-click on a hand tile
        // explicitly requests CODE.2 214B8 flag 08h (item exchange).
        // Original DOS button/held-time event mapping still needs a
        // captured input trace; do not pass primary flag 01h through
        // this route, as that is the unfinished use/attack command.
        if (ui.state().screen ==
                rloft::ExplorationUiScreen::Viewport) {
            if (const auto secondary =
                    host.consume_exploration_secondary_click()) {
                if (rloft::DevLevelInteractionPolicy::allow(
                        exploration_preview,dev_interact)) {
                    if (const auto hand =
                            rloft::PartyPanelControlModel::
                                gameplay_hand_at(
                                    secondary->row,
                                    secondary->column)) {
                        const auto status =
                            runtime.inventory().swap_hand_slot(
                                runtime.session().party,
                                runtime.object_catalog(),
                                hand->actor,
                                hand->hand);
                        if (status ==
                            rloft::InventoryHandSwapStatus::Swapped) {
                            // Live hand equipment and readiness changed.
                            // Rebuild scene selection on the next frame;
                            // never retarget a stale view after input.
                            displayed_scene_frame.reset();
                            input.clear();
                            const auto held = runtime.inventory().held;
                            const auto caption =
                                held.object_id == -1
                                    ? std::wstring(
                                        L"Ravenloft - Hand exchange | "
                                        L"Cursor empty")
                                    : std::wstring(
                                        L"Ravenloft - Hand exchange | "
                                        L"Held item ID ") +
                                        std::to_wstring(held.object_id) +
                                        (held.known != 0
                                            ? L" (identified)"
                                            : L" (unknown)");
                            SetWindowTextW(
                                host.hwnd(), caption.c_str());
                        } else if (status ==
                            rloft::InventoryHandSwapStatus::Unsupported) {
                            SetWindowTextW(
                                host.hwnd(),
                                L"Ravenloft - Hand interaction pending: "
                                L"special item or container");
                        }
                    } else if (const auto actor =
                                   rloft::PartyPanelControlModel::
                                       gameplay_actor_at(
                                           secondary->row,
                                           secondary->column)) {
                        // CODE.2 211D8 flag 08h actor-panel action.
                        // Stash the held ordinary object in the first
                        // empty one of the 12 common inventory slots.
                        const auto status =
                            runtime.inventory().quick_stash_held(
                                runtime.session().party,
                                runtime.object_catalog(),
                                *actor);
                        if (status ==
                            rloft::InventoryQuickStashStatus::Stashed) {
                            displayed_scene_frame.reset();
                            input.clear();
                            SetWindowTextW(
                                host.hwnd(),
                                L"Ravenloft - Item stored in first "
                                L"free inventory slot");
                        } else if (status ==
                            rloft::InventoryQuickStashStatus::Full) {
                            SetWindowTextW(
                                host.hwnd(),
                                L"Ravenloft - Inventory full; "
                                L"held item preserved");
                        } else if (status ==
                            rloft::InventoryQuickStashStatus::Unsupported) {
                            SetWindowTextW(
                                host.hwnd(),
                                L"Ravenloft - Special actor-panel "
                                L"item action pending");
                        }
                    }
                }
            }
        }

        if (ui.state().screen ==
            rloft::ExplorationUiScreen::PartyPanels) {
            // The opening actor-panel click is consumed above. Render the
            // modal immediately and do not advance 1FC14/2E120 locomotion.
            const auto pointer=host.exploration_pointer();
            const auto modal_pointer=pointer
                ? std::optional<rloft::InventoryPointer>{
                      rloft::InventoryPointer{
                          pointer->row,pointer->column}}
                : std::nullopt;
            try {
                (void)party_modal_presenter.draw(
                    presenter.framebuffer(),
                    runtime.session().party,
                    ui.state().party_panel_character_mode,
                    ui.state().selected_party_actor,
                    &runtime.inventory(),modal_pointer,
                    inventory_encumbrance_actor);
            } catch(const std::exception& exc) {
                // The modal renderer can throw before present_traced()
                // observes a frame (as on #649's missing RES2 bank).
                // Persist source stage and reason in the F12 session
                // while preserving the original exception behavior.
                if(trace) {
                    try {trace->error("party_modal.draw",exc.what());}
                    catch(...) {}
                }
                throw;
            }
            present_traced(presenter.framebuffer(),exploration_palette);
            // Report the live authoritative inventory even while
            // its original modal is open. This is exclusively
            // text in the opt-in developer Windows title bar.
            refresh_dev_inventory_caption();

            next_frame +=
                std::chrono::duration_cast<Clock::duration>(
                    std::chrono::duration<double>(
                        1.0 / kNativeGameplayRefreshHz));
            if (!host.wait_until(next_frame, false)) {
                break;
            }
            continue;
        }

        // CODE.2 080AC is modal while the pointer remains in the top
        // 64-pixel band (D680=3). Do not advance 1FC14/2E120 locomotion
        // while that dispatcher owns input.
        if (ui.state().screen ==
                rloft::ExplorationUiScreen::QuitConfirm) {
            quit_presenter.draw(
                presenter.framebuffer(),
                quit_random_seed.value_or(1U));
            present_traced(presenter.framebuffer(),exploration_palette);
            next_frame +=
                std::chrono::duration_cast<Clock::duration>(
                    std::chrono::duration<double>(
                        1.0 / kNativeGameplayRefreshHz));
            if (!host.wait_until(next_frame, false)) {
                break;
            }
            continue;
        }

        if (ui.state().screen ==
                rloft::ExplorationUiScreen::Automap) {
            // The click that entered map mode was consumed above; render the
            // modal immediately without advancing locomotion.
            automap_presenter.draw(
                presenter.framebuffer(),
                runtime.session(),
                runtime.renderer_tables(),
                automap->state(),
                automap->annotation_overlay_enabled());
            present_traced(presenter.framebuffer(),exploration_palette);
            next_frame +=
                std::chrono::duration_cast<Clock::duration>(
                    std::chrono::duration<double>(
                        1.0 / kNativeGameplayRefreshHz));
            if (!host.wait_until(next_frame, false)) {
                break;
            }
            continue;
        }

        // Mouse yaw samples at native presentation cadence, independent
        // of the original slower collision/movement integration clock.
        const int yaw=host.consume_mouse_yaw();
        if(yaw)runtime.session().facing=
            rloft::ExplorationMotionModel::normalize_heading(
                static_cast<int>(runtime.session().facing)-yaw);
        if (hovered_menu < 0 &&
            gameplay_cadence.should_integrate(Clock::now(),host.sprint_held())) {
            auto motion_inputs=input.tick(false);
            const int strafe=host.strafe_axis();
            if(strafe)motion_inputs.orthogonal_input=strafe*64;
            (void)runtime.tick_motion(motion_inputs);
        }

        if (!first_scene_submitted) {
            exploration_stage("exploration.frame.build.begin");
        }
        auto frame = [&]() {
            try {
                return runtime.build_frame();
            } catch (const std::exception& exc) {
                if (!first_scene_submitted) {
                    exploration_stage("exploration.frame.build.failed");
                }
                // The ordinary frame_0001..0016 cap is not a fatal-log
                // cap. Preserve the ACTUAL failing camera and native
                // error even if the last captured successful pose was
                // many cells or several minutes earlier.
                if(trace) {
                    try {trace->error("build_frame",exc.what());}
                    catch(...) {}
                }
                if (capture_render) {
                    try {
                        const auto& session=runtime.session();
                        rloft::ExplorationFrameCapture::write_failure(
                            capture_directory,
                            {static_cast<std::uint32_t>(session.level_id),
                             session.coordinate_a,session.coordinate_b,
                             session.facing,session.level.width,
                             session.level.height,capture_count,
                             RLOFT_DEMO_SOURCE_SHA,exc.what()});
                    } catch(const std::exception& write_error) {
                        OutputDebugStringA(write_error.what());
                    }
                }
                throw; // Never hide original unknown sweep behavior.
            }
        }();
        if (!first_scene_submitted) {
            exploration_stage("exploration.frame.build.ready");
        }
        const auto presented = [&]() {
            try {
                return presenter.present(frame,runtime.renderer_tables());
            }catch(const std::exception& exc) {
                if (!first_scene_submitted) {
                    exploration_stage("exploration.frame.raster.failed");
                }
                if(trace) {
                    try {trace->error("present_viewport",exc.what());}
                    catch(...) {}
                }
                throw;
            }
        }();
        if (!first_scene_submitted) {
            exploration_stage("exploration.frame.raster.ready");
        }

        if (capture_render && !capture_failure.has_value() &&
            capture_count < capture_limit) {
            const auto& session=runtime.session();
            const auto cell_x=static_cast<std::int32_t>(
                session.coordinate_a)>>16;
            const auto cell_y=static_cast<std::int32_t>(
                session.coordinate_b)>>16;
            const auto key=std::make_tuple(
                static_cast<std::uint32_t>(session.level_id),
                static_cast<int>(cell_x),static_cast<int>(cell_y),
                static_cast<std::uint32_t>(session.facing));
            if (!last_capture_position.has_value() ||
                *last_capture_position != key) {
                try {
                    rloft::ExplorationFrameCapture::write(
                        capture_directory,
                        {capture_count+1U,
                         static_cast<std::uint32_t>(session.level_id),
                         cell_x,cell_y,
                         static_cast<std::uint32_t>(session.facing)},
                        frame,presented,presenter.framebuffer(),
                        capture_code2,RLOFT_DEMO_SOURCE_SHA);
                    ++capture_count;
                    last_capture_position=key;
                } catch(const std::exception& error) {
                    capture_failure=error.what();
                    OutputDebugStringA(capture_failure->c_str());
                }
            }
        }

        if (hovered_menu >= 0) {
            menu_presenter.draw(
                presenter.framebuffer(),
                hovered_menu,
                pressed_menu);
        }

        // GPU scene coordinates and living sprites are obtained ONLY
        // from this same source frame, mutable session and world.
        if(!exploration_preview && !dev_trace) {
            host.bind_gpu_frame(runtime.session(),runtime.world(),
                                frame.world_scene_records,
                                presented.environment_backdrop_indices,
                                presented.palette);
            host.set_gpu_menu_overlay(hovered_menu>=0);
        }
        present_traced(presenter.framebuffer(),presented.palette,
            hovered_menu>=0 ? "top_edge_menu" : "viewport");
        if (!first_scene_submitted) {
            // Only means that the native framebuffer reached the host.
            // It is NOT player-visible verification or DOS pixel parity.
            exploration_stage("exploration.frame.present.submitted");
            first_scene_submitted = true;
        }
        if (exploration_preview) {
            // Explicitly native-only diagnostic HUD in the window
            // caption; the original 320x400 indexed framebuffer stays
            // untouched. Show location changes without flooding Win32.
            const auto& session = runtime.session();
            const auto column = static_cast<std::int32_t>(
                session.coordinate_a) >> 16;
            const auto row = static_cast<std::int32_t>(
                session.coordinate_b) >> 16;
            auto caption =
                std::wstring(L"Ravenloft - Exploration Preview | Build ") +
                executable_source_revision() +
                L" | Level " + std::to_wstring(session.level_id) +
                L" | Cell " + std::to_wstring(column) +
                L"," + std::to_wstring(row) +
                L" | Facing " + std::to_wstring(session.facing) +
                L" | SINTAB walls " +
                std::to_wstring(
                    presented.surface_coverage.sintab_main_runs_rasterized) +
                L"/" + std::to_wstring(
                    presented.surface_coverage.sintab_main_runs_rasterized +
                    presented.surface_coverage.sintab_main_runs_pending) +
                L" | Wall errors " +
                std::to_wstring(
                    presented.surface_coverage.sintab_main_runs_failed) +
                L" | RES3 calls " +
                std::to_wstring(
                    presented.surface_coverage.type7_calls_rasterized) +
                L"/" + std::to_wstring(
                    presented.surface_coverage.type7_calls_rasterized +
                    presented.surface_coverage.type7_scene_calls_pending +
                    presented.surface_coverage.type7_calls_zero_pixels +
                    presented.surface_coverage.type7_plan_calls_failed) +
                L" | RES3 zero pixels " +
                std::to_wstring(
                    presented.surface_coverage.type7_calls_zero_pixels) +
                L" | RES3 plan errors " +
                std::to_wstring(
                    presented.surface_coverage.type7_plan_calls_failed) +
                L" | Ground " +
                (presented.ground_textured_rasterized
                 ? L"RES4 textured" : L"flat fallback") +
                L" | RES3 trace errors " +
                std::to_wstring(
                    frame.scene_vga_scene_failures.size() +
                    (frame.scene_vga_trace_failure.has_value()?1U:0U)) +
                L" | Arrow keys / Numpad | Esc to exit";
            if (capture_render) {
                caption+=L" | Captures "+
                    std::to_wstring(capture_count)+L"/"+
                    std::to_wstring(capture_limit);
                if (capture_failure.has_value()) {
                    caption+=L" | Capture error";
                }
            }
            if (dev_inspect) {
                // Additional developer-only metadata lives ONLY in
                // the Win32 title bar. It cannot change original
                // 320x400 VGA pixels, actor bytes, item IDs or story.
                const auto snapshot=rloft::DevLevelDiagnostics::capture(
                    runtime.session(),runtime.world(),runtime.inventory());
                if(last_dev_snapshot &&
                   rloft::DevLevelDiagnostics::compare(
                       *last_dev_snapshot,snapshot).any())
                    ++dev_change_sequence;
                last_dev_snapshot=snapshot;
                const auto short_hex=[](std::uint64_t digest) {
                    const auto code=rloft::DevLevelSnapshot::digest8(digest);
                    return std::wstring(code.begin(),code.end());
                };
                caption+=L" | DEV Actors "+
                    std::to_wstring(snapshot.active_actors)+
                    L" NPC "+std::to_wstring(snapshot.npc_count)+
                    L" Fixed "+std::to_wstring(snapshot.placed_object_count)+
                    L" Loose "+std::to_wstring(snapshot.loose_item_count)+
                    L" Open "+std::to_wstring(
                        snapshot.active_container_views)+
                    L" Flags "+std::to_wstring(
                        snapshot.set_story_flags)+
                    L" | Party "+short_hex(snapshot.party_hash)+
                    L" Inv "+short_hex(snapshot.inventory_hash)+
                    L" World "+short_hex(snapshot.world_hash)+
                    L" Story "+short_hex(snapshot.flags_hash)+
                    L" | State changes "+
                    std::to_wstring(dev_change_sequence);
            }
            if (caption != last_preview_title) {
                SetWindowTextW(host.hwnd(), caption.c_str());
                last_preview_title = caption;
            }
        }
        displayed_scene_frame = std::move(frame);

        next_frame += std::chrono::duration_cast<Clock::duration>(
            std::chrono::duration<double>(
                1.0 / kNativeGameplayRefreshHz));
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

    host.stop_mouse_look();
    host.end_gpu_scene();
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
            if (!source.has_full_game_media()) {
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

struct DemoCommandLine final {
    std::optional<std::filesystem::path> source;
    bool explore_level22{};
    bool dev_inspect{};
    bool dev_interact{};
    bool dev_trace{};
    // Explicitly select the original data installation for user field tests.
    // This does NOT execute original R.EXE/CODE.1/CODE.2 machine code.
    bool choose_install{};
    // Tooling-only: temporary Alpha copies must never overwrite the
    // user's preferred original installation in HKCU. No gameplay effect.
    bool no_remember_install{};
    // Opt-in original-front-end presentation, never gameplay or CHARS I/O.
    bool front_end_only{};
    bool resume_existing_party{}; // Opt-in original CHARS compatibility
    bool capture_render{};
    std::size_t capture_limit{16U};
    bool custom_capture_limit{};
};

DemoCommandLine command_line_options() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        throw std::runtime_error(
            "Unable to parse Ravenloft command-line arguments");
    }
    DemoCommandLine result{};
    try {
        for (int index = 1; index < argc; ++index) {
            const std::wstring_view arg(argv[index]);
            if (arg == L"--explore-level22" || arg == L"--explore") {
                result.explore_level22 = true;
            } else if (arg == L"--dev-inspect") {
                result.dev_inspect = true;
            } else if (arg == L"--dev-interact") {
                result.dev_interact = true;
            } else if (arg == L"--dev-trace") {
                result.dev_trace = true;
            } else if (arg == L"--choose-install") {
                result.choose_install = true;
            } else if (arg == L"--no-remember-install") {
                result.no_remember_install = true;
            } else if (arg == L"--front-end-only") {
                result.front_end_only = true;
            } else if (arg == L"--resume-existing-party") {
                result.resume_existing_party = true;
            } else if (arg == L"--capture-render") {
                result.capture_render = true;
            } else if (arg.substr(0U,17U) == L"--capture-frames=") {
                const auto count = arg.substr(17U);
                result.capture_limit=
                    rloft::ExplorationFrameCapture::parse_frame_limit(count);
                result.custom_capture_limit=true;
            } else if (!arg.empty() && arg.front() == L'-') {
                throw std::invalid_argument(
                    "Unknown command-line option. Use --explore-level22 "
                    "[--dev-inspect] [--dev-interact] [--dev-trace] "
                    "[--choose-install] [--front-end-only] "
                    "[--no-remember-install] "
                    "[--capture-render] [--capture-frames=64] "
                    "and optionally one Ravenloft installation path.");
            } else if (result.source.has_value()) {
                throw std::invalid_argument(
                    "Specify at most one Ravenloft installation path");
            } else {
                result.source = std::filesystem::path(argv[index]);
            }
        }
    } catch (...) {
        LocalFree(argv);
        throw;
    }
    LocalFree(argv);
    if(!rloft::DevLevelInteractionPolicy::valid(
           result.explore_level22,result.dev_interact)) {
        throw std::invalid_argument(
            "--dev-interact requires explicit --explore-level22");
    }
    if(result.dev_inspect && !result.explore_level22) {
        throw std::invalid_argument(
            "--dev-inspect is developer-only and requires --explore-level22");
    }
    if(result.capture_render && !result.explore_level22) {
        throw std::invalid_argument(
            "--capture-render requires --explore-level22");
    }
    if(result.custom_capture_limit &&
       !result.capture_render && !result.dev_trace) {
        throw std::invalid_argument(
            "--capture-frames requires --capture-render or --dev-trace");
    }
    if(result.choose_install && result.source) {
        throw std::invalid_argument(
            "--choose-install cannot be combined with an installation path");
    }
    if(result.front_end_only && result.explore_level22) {
        throw std::invalid_argument(
            "--front-end-only cannot use the retired Level 22 preview");
    }
    if(result.dev_trace && !result.custom_capture_limit)
        result.capture_limit=64U;
    return result;
}

int run(HINSTANCE instance) {
    const auto options = command_line_options();
    rloft::GameSource source;

    // A field report must identify its original installed-data source.
    // Never silently fall back to the previously remembered installation
    // when the dedicated normal-path diagnostic requests this chooser.
    // Gameplay is STILL the incomplete native reimplementation.
    if (options.choose_install) {
        auto chosen = choose_valid_install(nullptr);
        if (!chosen) return 2;
        source = std::move(*chosen);
    } else if (const auto explicit_path = options.source) {
        source = rloft::GameSource::open(*explicit_path);
        if(!source.has_full_game_media())
            throw std::runtime_error(
                "This is a partial Ravenloft installation. The full "
                "CINE/RES/LEVELS/CODE media is required to play "
                "the entire original intro and Level 22.");
        // The isolated Alpha launcher passes an ephemeral %TEMP%
        // installation as an explicit argument. A trace session
        // MUST NOT replace the user's persistent owned-game folder
        // preference with that disposable test copy.
        // Non-diagnostic explicit normal launches still remember the
        // selected installation exactly as before.
        if (!options.dev_trace && !options.no_remember_install) {
            save_preferred_install(source.location().install_root);
        }
    } else {
        bool resolved = false;

        if (const auto saved = preferred_install()) {
            try {
                source = rloft::GameSource::open(*saved);
                resolved = source.has_full_game_media();
            } catch (...) {
                // A moved/uninstalled copy is not fatal; continue discovery.
            }
        }

        if (!resolved) {
            if (auto found = rloft::GameSource::auto_locate(true)) {
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

    // Direct EXE launches use a private writable overlay for CODE.1 -> CODE.2
    // CHARS transfer and never mutate owned DOS files. The packaged
    // --no-remember-install launcher already owns a disposable media copy.
    if(!options.no_remember_install) {
        std::array<wchar_t,32768> local{};
        const auto count=GetEnvironmentVariableW(
            L"LOCALAPPDATA",local.data(),static_cast<DWORD>(local.size()));
        if(count==0U || count>=local.size())
            throw std::runtime_error(
                "Private Ravenloft runtime folder unavailable: refusing "
                "to overwrite original DOS CHARS. Restore Windows "
                "LOCALAPPDATA before running the game.");
        const auto root=std::filesystem::path(local.data())/
            L"Hyperbog"/L"Ravenloft"/L"Runtime";
        std::filesystem::create_directories(root);
        source.set_runtime_overlay(root);
    }

    // CODE.2 204B3 checks the writable runtime CHARS transfer before the
    // cold-start presentation. A valid existing party enters the gameplay
    // import/level-load path directly; CHARS-absent startup continues through
    // the cinematic/menu/character workflow below.
    // The original CODE.2 CHARS-present startup remains untouched in normal
    // mode. Only the explicit, read-only front-end presentation hides CHARS
    // from the startup choice, so the existing intros and title can be seen.
    // Direct Ravenloft.exe and the packaged launcher now BOTH follow
    // the complete introduction -> title -> character creation route.
    // A stale original DOS CHARS file never silently skips those scenes.
    // An explicitly opted-in compatibility launch may still import it.
    auto startup_chars = options.resume_existing_party &&
                         !options.front_end_only
        ? source.read_runtime("CHARS")
        : std::optional<std::vector<std::uint8_t>>{};

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
    // All original intro, movie, title and CODE.1 creation frames now
    // present through the SAME persistent modern OpenGL backbuffer as
    // gameplay, with their indexed pixels/palettes preserved verbatim.
    host.begin_gpu_scene(assets,presenter.palette12());

    // OPT-IN host-only trace of the entire startup. Previously
    // --dev-trace started only AFTER the first gameplay level loaded:
    // any failed CINE00/title/CODE.1 stage had no diagnostic record.
    // Keep a separate early trace so game source/behavior is unchanged.
    std::unique_ptr<rloft::DevSessionTrace> startup_trace;
    if(options.dev_trace) {
        const auto root=std::filesystem::temp_directory_path()/
            "RLOFT-Demo-Diagnostics";
        const auto stamp=std::chrono::duration_cast<
            std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        const auto folder=root/
            (std::to_string(stamp)+"-boot-"+std::to_string(GetCurrentProcessId()));
        startup_trace=std::make_unique<rloft::DevSessionTrace>(
            folder,RLOFT_DEMO_SOURCE_SHA,
            rloft::to_string(assets.source().kind()));
        std::ofstream latest(root/"latest-boot-session.txt",
                             std::ios::binary|std::ios::trunc);
        if(latest)latest<<folder.string()<<"\n";
        host.set_session_trace(startup_trace.get());
    }
    struct BootTraceDetach final {
        Host& host;
        ~BootTraceDetach() {host.set_session_trace(nullptr);}
    } boot_trace_detach{host};
    const auto startup_event=[&](const char* stage,
                                 std::int64_t value=0) {
        if(!startup_trace)return;
        try {startup_trace->event(stage,value);}
        catch(...) {
            host.set_session_trace(nullptr);
            startup_trace.reset(); // Failed optional tracing cannot block play.
        }
    };
    const auto startup_handoff=[&]() {
        if(!startup_trace)return;
        startup_event("boot.exploration.handoff");
        host.set_session_trace(nullptr);
        if(startup_trace) {
            try {startup_trace->finish();}
            catch(...) {startup_trace.reset();}
        }
    };
    startup_event("boot.host.ready",startup_chars?1:0);

    rloft::Palette6 black{};
    host.present(presenter.framebuffer(), black);

    // Voluntary native walkaround: bypass original character creation
    // WITHOUT creating or overwriting the user's writable CHARS file.
    // Existing valid party bytes are reused, otherwise two clearly
    // synthetic actors exist only for the lifetime of this process.
    // Keep the original startup/menu route unchanged by default.
    if (options.explore_level22) {
        // --explore-level22 is an explicitly opted-in diagnostic
        // shortcut only. A newly created DOS party must pass through
        // CODE.1 character creation -> exact CHARS -> CODE.2 startup,
        // never splice unrelated characters out of SAVE.DAT.
        const auto preview = rloft::ExplorationPreview::party(
            startup_chars);
        startup_handoff();
        return run_exploration(
            host, assets, audio_player, midi_player, preview.chars, true, options.capture_render,
            options.capture_limit, options.dev_inspect,
            options.dev_interact, options.dev_trace);
    }

    if (startup_chars) {
        if (flow.snapshot().stage !=
                rloft::BootStage::Exploration ||
            flow.snapshot().module !=
                rloft::LegacyModule::Code2) {
            throw std::logic_error(
                "CODE.2 CHARS-present startup did not enter exploration");
        }
        startup_handoff();
        return run_exploration(
            host,
            assets, audio_player, midi_player,
            *startup_chars,false,false,16U,false,false,options.dev_trace);
    }

    bool skipped = false;

    // CODE.2 19888 begins with numbered cinematic event 0, then event 12.
    // 13EF8 maps those numbers directly to CINE00.ANM and CINE12.ANM.
    startup_event("boot.cine00.begin");
    {
        rloft::AnmCinematic cine00(
            assets.source().read("CINE00.ANM"), 0, assets);
        const auto result =
            play_cinematic(
                host, cine00, assets, audio_player, midi_player);
        audio_player.stop_all();
        midi_player.stop();
        startup_event("boot.cine00.result",
                      static_cast<std::int64_t>(result));
        if (result == CinematicResult::Quit) return 0;
        if (result == CinematicResult::Escaped) skipped = true;
    }

    if (!skipped) {
        startup_event("boot.cine12.begin");
        rloft::AnmCinematic cine12(
            assets.source().read("CINE12.ANM"), 12, assets);
        const auto result =
            play_cinematic(
                host, cine12, assets, audio_player, midi_player);
        audio_player.stop_all();
        midi_player.stop();
        startup_event("boot.cine12.result",
                      static_cast<std::int64_t>(result));
        if (result == CinematicResult::Quit) return 0;
        if (result == CinematicResult::Escaped) skipped = true;
    }

    const auto palette14 = presenter.palette14();
    const auto palette9 = presenter.palette9();
    const auto palette12 = presenter.palette12();

    // CODE.2 19888: nonzero 13EF8 status bypasses ALL logo steps.
    // Previously Esc in CINE00/CINE12 still waited for logo 1.
    if (!skipped) {
        startup_event("boot.splash.first");
        const auto deadline = Clock::now() + pit_ticks(120);
        presenter.first_logo();
        if (!fade_in(host, presenter, palette14)) return 0;
        if (!host.wait_until(deadline, true)) {
            if (host.quit_requested()) return 0;
            skipped = true;
        }
    }

    if (!skipped) {
        if (!fade_out(host, presenter, palette14)) return 0;
        startup_event("boot.splash.second");
        const auto deadline = Clock::now() + pit_ticks(106);
        presenter.second_logo();
        if (!fade_in(host, presenter, palette14)) return 0;
        if (!host.wait_until(deadline, true)) {
            if (host.quit_requested()) return 0;
            skipped = true;
        }
    }

    if (!skipped) {
        if (!fade_out(host, presenter, palette14)) return 0;
        startup_event("boot.splash.third");
        const auto deadline = Clock::now() + pit_ticks(121);
        presenter.third_logo();
        if (!fade_in(host, presenter, palette9)) return 0;
        if (!host.wait_until(deadline, true)) {
            if (host.quit_requested()) return 0;
            skipped = true;
        }
    }

    if (!skipped) {
        if (!fade_out(host, presenter, palette9)) return 0;
        startup_event("boot.title_background");
        const auto deadline = Clock::now() + pit_ticks(55);
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
    startup_event("boot.title.ready");
    int drawn_selection = -1;

    while (host.pump()) {
        const int selection = host.selection();
        if (selection != drawn_selection) {
            drawn_selection = selection;
            presenter.menu_frame(selection);
            host.present(presenter.framebuffer(), palette12);
        }

        if (const auto click = host.consume_click()) {
            startup_event("boot.title.action",*click);
            flow.select_startup_menu(*click);
            // CODE.2 19464 switch/current=0 -> 194B5 calls
            // 13EF8(event=0), restores the title/menu surface, resets
            // the hovered choice and returns to the original menu loop.
            // Replay the real CINE00.ANM; no fabricated slides or skip
            // straight into Level 22. Other original modal/page effects
            // (20294/325B0/322EC) still need DOS display parity.
            if (*click == 0) {
                startup_event("boot.title.cine_replay");
                host.set_menu_mode(false);
                rloft::AnmCinematic replay(
                    assets.source().read("CINE00.ANM"), 0, assets);
                const auto result = play_cinematic(
                    host, replay, assets, audio_player, midi_player);
                audio_player.stop_all();
                midi_player.stop();
                if (result == CinematicResult::Quit) return 0;

                // Restore the CODE.2 menu palette and cursor on both
                // ordinary and Esc-aborted replay paths.
                host.set_game_cursor(menu_cursor);
                presenter.menu_frame(-1);
                host.present(presenter.framebuffer(), palette12);
                drawn_selection = -1;
                host.set_menu_mode(true);
                continue;
            }

            // Title-screen Continue now loads the full independent
            // native session saved from the gameplay Save action.
            // Never substitute an unrelated original SAVE.DAT.
            if (*click == 1) {
                try {
                    auto saved=read_native_save(assets.source());
                    const auto chars=std::vector<std::uint8_t>(
                        saved.session.party.bytes.begin(),
                        saved.session.party.bytes.end());
                    host.set_menu_mode(false);
                    host.set_game_cursor(raw_cursor);
                    startup_handoff();
                    return run_exploration(
                        host,assets,audio_player,midi_player,chars,
                        false,false,16U,false,false,options.dev_trace,&saved);
                }catch(const std::exception& e) {
                    const auto message=widen(e.what());
                    MessageBoxW(host.hwnd(),message.c_str(),
                        L"Ravenloft - Continue",MB_OK|MB_ICONWARNING);
                    host.set_menu_mode(true);
                    host.set_game_cursor(menu_cursor);
                    presenter.menu_frame(-1);
                    host.present(presenter.framebuffer(),palette12);
                }
                continue;
            }

            if (*click == rloft::BootFlow::kCharacterWorkflowMenuIndex) {
                startup_event("boot.code1.creation.begin");
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
                startup_event("boot.code1.controller.ready");
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

                                startup_event("boot.code1.chars.build");
                                const auto image =
                                    rloft::build_character_transfer(
                                        transfer,
                                        transfer_tables);
                                // Showcase never deletes/writes an existing
                                // CHARS file or enters the Level 22 prototype.
                                if (options.front_end_only) {
                                    host.set_character_mode(false);
                                    MessageBoxW(host.hwnd(),
                                        L"Original intro/title and character\n"
                                        L"creation reached the transfer stage.\n\n"
                                        L"The complete game runtime is still\n"
                                        L"under reconstruction. No CHARS or\n"
                                        L"SAVE files were modified.",
                                        L"Ravenloft front-end checkpoint",
                                        MB_OK | MB_ICONINFORMATION);
                                    return 0;
                                }
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

                                startup_event("boot.code1.chars.disk_written");
                                // Original CODE.2 204B3 checks CHARS on disk;
                                // 204EF -> 16CD0 reopens and reads 0x920 bytes
                                // before 16ADC imports the two created actors.
                                // Do not bypass that module boundary by using
                                // CODE.1's still-live in-memory image.
                                const auto code2_chars =
                                    assets.source().read_runtime("CHARS");
                                if (!code2_chars ||
                                    code2_chars->size() != image.size()) {
                                    throw std::runtime_error(
                                        "CODE.2 failed to reopen complete "
                                        "0x920-byte CHARS transfer");
                                }
                                if (!std::equal(
                                        code2_chars->begin(),
                                        code2_chars->end(),
                                        image.begin())) {
                                    // Additional host integrity check: original
                                    // DOS does not compare to the old image.
                                    throw std::runtime_error(
                                        "CODE.2 CHARS disk bytes differ from "
                                        "completed CODE.1 transfer");
                                }
                                startup_event("boot.code2.chars.readback_ok");
                                // The fresh original level-22 entry remains
                                // at 4.5/4.5, facing 270, time 14:00.
                                flow.finish_code2_startup(true, false);
                                if (flow.snapshot().stage !=
                                        rloft::BootStage::Exploration) {
                                    throw std::logic_error(
                                        "CODE.2 CHARS startup did not enter "
                                        "exploration");
                                }

                                host.set_character_mode(false);
                                startup_handoff();
                                return run_exploration(
                                    host,
                                    assets, audio_player, midi_player,
                                    *code2_chars,
                                    false,false,16U,false,false,
                                    options.dev_trace);
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
        const auto text =
            widen(std::string(exc.what()) +
                  "\n\nExecutable source revision: " +
                  std::string(RLOFT_DEMO_SOURCE_SHA).substr(0, 12) +
                  "\nA previous build does not include later fixes.");
        MessageBoxW(
            nullptr,
            text.c_str(),
            L"Ravenloft: Strahd\'s Possession",
            MB_OK | MB_ICONERROR);
        return 1;
    }
}
#endif
