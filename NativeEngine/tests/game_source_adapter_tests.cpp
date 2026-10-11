#include "rloft_native/game_source_adapter.hpp"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {
void write_file(const fs::path& file, const std::vector<std::uint8_t>& bytes) {
    std::ofstream out(file, std::ios::binary);
    assert(out.is_open());
    out.write(reinterpret_cast<const char*>(bytes.data()),
              static_cast<std::streamsize>(bytes.size()));
    assert(out.good());
}

void check_fixture() {
    const auto suffix = std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    const auto root = fs::temp_directory_path() / ("rloft-native-source-fixture-" + suffix);
    fs::create_directories(root);
    try {
        // Synthetic media proves routing only; not a valid playable game.
        write_file(root / "res0", {1});
        write_file(root / "INIT", {2, 3});
        write_file(root / "T", {4});
        write_file(root / "RES4", {5, 6, 7});
        const auto source = rloft::GameSource::open(root);
        assert(source.kind() == rloft::GameSourceKind::LooseDirectory);
        const auto vfs = rloft_native::bind_original_media(source);
        assert(vfs.read("init").provider == "original");
        assert(vfs.read("init").data == (std::vector<std::uint8_t>{2, 3}));
        assert(vfs.read("res4").data == (std::vector<std::uint8_t>{5, 6, 7}));
        // This smoke test only targets real installed-media adapter semantics;
        // the synthetic files are never committed or confused with game data.
        assert(fs::file_size(root / "RES4") == 3);
    } catch (...) {
        fs::remove_all(root);
        throw;
    }
    fs::remove_all(root);
}
} // namespace

int main() { check_fixture(); }
