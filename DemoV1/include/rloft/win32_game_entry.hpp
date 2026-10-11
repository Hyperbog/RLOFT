#pragma once
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
namespace rloft {
// Existing source-traced Windows game path, compiled into both the demo and
// the independent native-engine application. Product policy is selected at
// compile time; original startup controllers are not duplicated.
int run_windows_game(HINSTANCE instance);
}
#endif
