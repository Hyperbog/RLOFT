#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <stdexcept>
#include <filesystem>
#include "rloft_native/windows_profile.hpp"

namespace rloft_native {
std::filesystem::path windows_native_profile_directory() {
    PWSTR known_folder = nullptr;
    const HRESULT hr = SHGetKnownFolderPath(
        FOLDERID_LocalAppData, KF_FLAG_CREATE, nullptr, &known_folder);
    if (FAILED(hr) || !known_folder) {
        if (known_folder) CoTaskMemFree(known_folder);
        throw std::runtime_error("Unable to locate Windows local app-data folder");
    }
    const auto root = std::filesystem::path(known_folder) /
        L"Hyperbog" / L"RLOFT" / L"NativeEngine" / L"Default";
    CoTaskMemFree(known_folder);
    return root;
}
} // namespace rloft_native
#endif
