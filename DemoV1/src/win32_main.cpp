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
#include <exception>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include "rloft/anm_cinematic.hpp"
#include "rloft/boot_flow.hpp"
#include "rloft/character_creation_presenter.hpp"
#include "rloft/game_source.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"
#include "rloft/voc_decoder.hpp"
#include "rloft/win32_audio.hpp"
#include "rloft/win32_midi.hpp"
#include "rloft/xmidi.hpp"

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
        wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
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

Clock::duration code1_ticks(int ticks) {
    // CODE.1 hooks INT 8 before Miles AIL. AIL later DDA-chains that prior
    // vector at the BIOS 54,925-us cadence even when a sound driver requests
    // a faster service interrupt. 87D8(2,0) therefore waits two BIOS ticks.
    return std::chrono::duration_cast<Clock::duration>(
        std::chrono::microseconds(54925LL * ticks));
}

bool code1_palette_reveal(
    Host& host,
    rloft::CharacterCreationPresenter& presenter,
    const rloft::Palette6& source) {
    // BFE0 performs factors 1..8. 6618 -> A010 uploads the 768-byte DAC bank
    // in four 0xC0-byte bursts, and A010 waits for vertical retrace before
    // each burst. Preserve the partial-bank progression instead of collapsing
    // each factor to one host frame.
    constexpr double kVgaRefreshHz = 70.0863;
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
    rloft::Win32AudioPlayer audio_player;
    rloft::Win32MidiPlayer midi_player;
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
                const auto character_palette = character_presenter.palette();
                rloft::Palette6 black{};

                SetWindowTextW(
                    host.hwnd(),
                    L"Ravenloft: Strahd's Possession — Character Creation");

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

                    if (frame == 0 &&
                        !code1_palette_reveal(
                            host, character_presenter, character_palette)) {
                        return 0;
                    }
                }

                character_presenter.first_screen();
                host.present(
                    character_presenter.framebuffer(),
                    character_palette);

                // Demo-v1 milestone currently holds on CODE.1 state 0.
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
