#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>
#include <filesystem>
#include <string_view>
#include "rloft/win32_game_entry.hpp"
#include "rloft_native/windows_profile.hpp"

// Distinct original-game product entrypoint. The inherited source-traced
// cinematic/menu/character/gameplay host runs through run_windows_game.
// Only this executable selects RLOFT_NATIVE_ENGINE profile+no-dev policy.
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return 2;
    const bool self_test = argc == 2 &&
        std::wstring_view(argv[1]) == L"--self-test-product";
    LocalFree(argv);

    if (self_test) {
        // Headless Windows acceptance of the independent user-profile
        // locator. No original media, writes or GUI are required.
        try {
            const auto profile = rloft_native::windows_native_profile_directory();
            return profile.is_absolute() &&
                   profile.filename() == L"Default" &&
                   profile.parent_path().filename() == L"NativeEngine"
                   ? 0 : 3;
        } catch (...) {
            return 2;
        }
    }
    return rloft::run_windows_game(instance);
}
#endif
