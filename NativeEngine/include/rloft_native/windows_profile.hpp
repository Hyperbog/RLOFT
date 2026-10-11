#pragma once
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <filesystem>
namespace rloft_native {
// User-owned per-product profile; MUST be outside installed game media.
[[nodiscard]] std::filesystem::path windows_native_profile_directory();
}
#endif
