#include "rloft/game_source.hpp"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

namespace fs = std::filesystem;
namespace {
void write_bytes(const fs::path& file, const std::vector<std::uint8_t>& bytes) {
    fs::create_directories(file.parent_path());
    std::ofstream out(file, std::ios::binary | std::ios::trunc);
    if (!out) throw std::runtime_error("cannot create profile fixture");
    if (!bytes.empty()) out.write(
        reinterpret_cast<const char*>(bytes.data()),
        static_cast<std::streamsize>(bytes.size()));
    if (!out) throw std::runtime_error("cannot finish profile fixture");
}
std::vector<std::uint8_t> bytes_at(const fs::path& file) {
    std::ifstream in(file, std::ios::binary);
    if (!in) throw std::runtime_error("file absent");
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}
template<typename Fn> void reject(Fn fn) {
    bool bad = false;
    try { fn(); } catch (const std::exception&) { bad = true; }
    assert(bad);
}
void test_isolation() {
    const auto root = fs::temp_directory_path() /
        ("rloft-native-profile-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    const auto install = root / "OriginalGame";
    const auto profile = root / "NativeProfile";
    fs::create_directories(install);
    try {
        write_bytes(install / "INIT", {1,2});
        write_bytes(install / "RES0", {3});
        write_bytes(install / "T", {4});
        write_bytes(install / "CHARS", {0x11, 0x22});
        write_bytes(install / "SOUND.CFG", {0x44});
        auto game = rloft::GameSource::open(install);
        assert(game.read_runtime("CHARS") == std::vector<std::uint8_t>({0x11,0x22}));
        reject([&] {game.set_runtime_root(install);});
        reject([&] {game.set_runtime_root(install / "RLOFT" / "Native");});
        reject([&] {game.set_runtime_root(root);});
        game.set_runtime_root(profile);
        assert(fs::is_directory(profile));
        assert(!game.read_runtime("CHARS"));
        assert(!game.read_runtime("SOUND.CFG"));
        assert(game.runtime_output_path("CHARS") == profile / "CHARS");
        reject([&] {static_cast<void>(game.runtime_output_path("../SAVE.DAT"));});
        reject([&] {static_cast<void>(game.read_runtime("a\\b"));});
        write_bytes(game.runtime_output_path("CHARS"), {0x55,0x66,0x77});
        assert(game.read_runtime("CHARS") ==
               std::vector<std::uint8_t>({0x55,0x66,0x77}));
        assert(bytes_at(install / "CHARS") == std::vector<std::uint8_t>({0x11,0x22}));
        assert(bytes_at(install / "SOUND.CFG") == std::vector<std::uint8_t>({0x44}));
        assert(game.read("RES0") == std::vector<std::uint8_t>({3}));
        // The full original game host moves GameSource into
        // RavenloftBootAssets immediately after profile selection.
        // Profile isolation MUST survive that move without reverting to
        // the installed CHARS file.
        auto moved = std::move(game);
        assert(moved.runtime_output_path("CHARS") == profile / "CHARS");
        assert(moved.read_runtime("CHARS") ==
               std::vector<std::uint8_t>({0x55,0x66,0x77}));
        assert(bytes_at(install / "CHARS") ==
               std::vector<std::uint8_t>({0x11,0x22}));
    } catch (...) {
        fs::remove_all(root);
        throw;
    }
    fs::remove_all(root);
}
}
int main() { test_isolation(); }
