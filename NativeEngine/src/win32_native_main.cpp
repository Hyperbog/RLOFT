#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "rloft/win32_game_entry.hpp"

// Independent product entrypoint. The full source-traced cinematic, menu,
// CODE.1 and CODE.2 runtime is linked from its existing shared game sources.
// RLOFT_NATIVE_ENGINE is set for the shared host TU by NativeEngine CMake:
// separate registry identity, independent writable profile, no demo warps.
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    return rloft::run_windows_game(instance);
}
#endif
