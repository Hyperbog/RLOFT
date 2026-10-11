// Independent, asset-free command-line media preflight. This is NOT the
// playable rloft_native runtime and must not be packaged as that engine.
#include "rloft/game_source.hpp"
#include "rloft_native/install_probe.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#ifndef RLOFT_NATIVE_SOURCE_SHA
#define RLOFT_NATIVE_SOURCE_SHA "local-unversioned"
#endif

namespace {

void usage(std::ostream& out) {
    out << "Ravenloft native installation inspector (READ ONLY)\n"
        << "Usage: rloft_install_probe --install <original-game-path>\n"
        << "       rloft_install_probe --auto\n"
        << "       rloft_install_probe --help\n"
        << "No game assets are bundled, copied, extracted, or modified.\n"
        << "This does NOT start the original game.\n";
}

int inspect(rloft::GameSource source) {
    const auto result = rloft_native::inspect_original_install(
        [&source](std::string_view path) { return source.contains(path); },
        [&source](std::string_view path) { return source.read(path); });
    std::cout << "SOURCE_REVISION\t" << RLOFT_NATIVE_SOURCE_SHA << "\n";
    std::cout << "SOURCE_TYPE\t" << rloft::to_string(source.kind()) << "\n";
    std::cout << "SOURCE_VERSION\tUNVERIFIED\n";
    for (const auto& file : result.files) {
        std::cout << "ASSET\t" << file.filename << "\t"
                  << (file.present ? "PRESENT" : "MISSING") << "\t"
                  << (file.required_for_probe ? "SIGNATURE" : "INVENTORY") << "\t";
        if (file.size_known) std::cout << file.byte_count;
        else std::cout << "-";
        std::cout << "\n";
    }
    if (!result.minimum_media_present) {
        std::cerr << "MEDIA_INCOMPLETE: missing or empty RES0/INIT/T signature\n";
        return 3;
    }
    std::cout << "MINIMUM_MEDIA_SIGNATURE_PASS\n";
    std::cout << "PLAYABLE_GAME_NOT_YET_IMPLEMENTED\n";
    return 0;
}

int run_from_install(std::optional<std::filesystem::path> install,
                     bool auto_locate) {
    if (auto_locate) {
        auto found = rloft::GameSource::auto_locate();
        if (!found) {
            std::cerr << "MEDIA_NOT_FOUND: no supported Ravenloft install discovered\n";
            return 2;
        }
        return inspect(std::move(*found));
    }
    if (!install) throw std::invalid_argument("no original install selected");
    return inspect(rloft::GameSource::open(*install));
}

#ifdef _WIN32
// Windows file paths must be obtained through the UTF-16 OS command line.
// Narrow argv is locale-dependent and corrupts some GOG/Steam install paths.
int run(int argc, wchar_t** argv) {
    if (argc == 2 && std::wstring_view(argv[1]) == L"--help") {
        usage(std::cout);
        return 0;
    }
    if (argc == 2 && std::wstring_view(argv[1]) == L"--auto")
        return run_from_install(std::nullopt, true);
    if (argc == 3 && std::wstring_view(argv[1]) == L"--install")
        return run_from_install(std::filesystem::path(argv[2]), false);
    usage(std::cerr);
    return 64;
}
#else
// Retained only as a source-compatible entrypoint; Windows is the target.
int run(int argc, char** argv) {
    if (argc == 2 && std::string_view(argv[1]) == "--help") {
        usage(std::cout);
        return 0;
    }
    if (argc == 2 && std::string_view(argv[1]) == "--auto")
        return run_from_install(std::nullopt, true);
    if (argc == 3 && std::string_view(argv[1]) == "--install")
        return run_from_install(std::filesystem::path(argv[2]), false);
    usage(std::cerr);
    return 64;
}
#endif

} // namespace

#ifdef _WIN32
int wmain(int argc, wchar_t** argv) {
#else
int main(int argc, char** argv) {
#endif
    try { return run(argc, argv); }
    catch (const std::exception& error) {
        std::cerr << "MEDIA_PROBE_ERROR: " << error.what() << "\n";
        return 2;
    }
}
