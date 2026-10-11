#include "rloft_native/mod_mount_plan.hpp"
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace rloft_native;

namespace {
void write(const fs::path& file, const std::vector<unsigned char>& bytes) {
    fs::create_directories(file.parent_path());
    std::ofstream output(file, std::ios::binary);
    if (!output) throw std::runtime_error("fixture open failed");
    output.write(reinterpret_cast<const char*>(bytes.data()),
                 static_cast<std::streamsize>(bytes.size()));
    if (!output) throw std::runtime_error("fixture write failed");
}
template <typename F> void rejects(F f) {
    bool caught = false;
    try { f(); } catch (const std::exception&) { caught = true; }
    assert(caught);
}

void exercise() {
    const auto root = fs::temp_directory_path() /
        ("rloft-mod-order-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    const auto base = root / "base", dependent = root / "dependent";
    const auto valid = root / "valid", invalid = root / "invalid";
    fs::create_directories(base);
    fs::create_directories(dependent);
    fs::create_directories(valid);
    fs::create_directories(invalid);
    try {
        write(base / "RES4", {7});
        write(dependent / "res4", {8});
        write(valid / "RES4", {9});
        write(invalid / "SAVE.DAT", {10});
        const std::vector<std::uint8_t> original{3};
        ReadOnlyVfs vfs(
            [](std::string_view key) { return key == "RES4"; },
            [&](std::string_view key) {
                assert(key == "RES4");
                return original;
            });
        const std::vector<ModDirectoryRequest> mods{
            {{"Dependent", "1.0", 1, 1, {"Base"}}, dependent},
            {{"Base", "1.0", 1, 1, {}}, base},
        };
        const auto plan = apply_mod_directory_plan(vfs, mods);
        assert(plan.size() == 2);
        assert(plan[0].id == "base" && plan[1].id == "dependent");
        assert(vfs.read("RES4").provider == "dependent");
        assert((vfs.read("RES4").data == std::vector<std::uint8_t>{8}));
        assert((vfs.providers("RES4") ==
               std::vector<std::string>{"dependent", "base", "original"}));
        // Reject *after* first validated mod; old live plan remains intact.
        rejects([&] {
            apply_mod_directory_plan(vfs, {
                {{"new-valid", "1.0", 1, 1, {}}, valid},
                {{"new-invalid", "1.0", 1, 1, {}}, invalid},
            });
        });
        assert(vfs.read("RES4").provider == "dependent");
        assert(vfs.providers("RES4").size() == 3);
        rejects([&] {
            apply_mod_directory_plan(vfs, {
                {{"cycle-a", "1", 1, 1, {"cycle-b"}}, valid},
                {{"cycle-b", "1", 1, 1, {"cycle-a"}}, base},
            });
        });
        assert(vfs.read("RES4").provider == "dependent");
        assert((original == std::vector<std::uint8_t>{3}));
    } catch (...) { fs::remove_all(root); throw; }
    fs::remove_all(root);
}
}

int main() { exercise(); }
