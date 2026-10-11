#include "rloft_native/vfs_contract.hpp"
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
void write(const fs::path& path, std::initializer_list<unsigned char> bytes) {
    fs::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("fixture creation failed");
    for (auto b : bytes) out.put(static_cast<char>(b));
    if (!out) throw std::runtime_error("fixture write failed");
}
template <typename F> void reject(F fn) {
    bool rejected = false;
    try { fn(); } catch (const std::exception&) { rejected = true; }
    assert(rejected);
}
void test_folder_mounts() {
    const fs::path root = fs::temp_directory_path() /
       ("native-mount-proof-" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count()));
    const auto one = root / "one", two = root / "two", bad = root / "bad";
    fs::create_directories(one);
    fs::create_directories(two);
    fs::create_directories(bad);
    try {
        write(one / "sub" / "ITEM.RES", {4, 5});
        write(one / "res4", {7, 8, 9});
        write(two / "RES4", {10, 11});
        write(two / "EMPTY", {});
        write(bad / "DUP", {1});
        write(bad / "dup", {2});
        unsigned source_reads = 0;
        const std::vector<std::uint8_t> original{1, 2, 3};
        ReadOnlyVfs vfs(
            [](std::string_view p) { return p == "RES4" || p == "INIT"; },
            [&](std::string_view p) {
                ++source_reads;
                return p == "RES4" ? original : std::vector<std::uint8_t>{20};
            });
        vfs.mount_directory("one", one);
        vfs.mount({"mem", {{"RES4", {99}}}});
        vfs.mount_directory("two", two);
        assert(source_reads == 0);
        assert((vfs.providers("res4") ==
               std::vector<std::string>{"two", "mem", "one", "original"}));
        assert(vfs.read("ReS4").provider == "two");
        assert((vfs.read("ReS4").data == std::vector<std::uint8_t>{10, 11}));
        assert(source_reads == 0);
        assert(vfs.read("sub\\item.res").provider == "one");
        assert((vfs.read("sub/item.res").data == std::vector<std::uint8_t>{4, 5}));
        assert(vfs.read("EMPTY").data.empty());
        assert(vfs.read("INIT").provider == "original" && source_reads == 1);
        // On Windows/NTFS the two names refer to the SAME on-disk file:
        // the second write overwrites the first. A physical directory
        // cannot express a case-only collision there. Always exercise the
        // virtual canonicalization guard with distinct map keys instead.
        reject([&] { vfs.mount({"dup-canonical",
                                 {{"DUP", {1}}, {"dup", {2}}}}); });
        if (!fs::equivalent(bad / "DUP", bad / "dup")) {
            // On case-sensitive filesystems there really ARE two files.
            reject([&] { vfs.mount_directory("dup", bad); });
        }
        reject([&] { vfs.mount_directory("two", one); });
        reject([&] { vfs.read("../res4"); });
        write(one / "SAVE.DAT", {22});
        reject([&] { vfs.mount_directory("unsafe", one); });
        fs::remove(one / "SAVE.DAT");
        // File-backed read checks changes after indexing; it does not return
        // obsolete data or silently fall back to original if a mount breaks.
        fs::remove(two / "RES4");
        reject([&] { vfs.read("RES4"); });
        // Symlinks cannot escape selected mod root even if created after scan.
        std::error_code ec;
        fs::create_symlink(root / "outside.dat", one / "sub" / "ALIAS", ec);
        if (!ec) reject([&] { vfs.mount_directory("link", one); });
        fs::remove(one / "sub" / "ALIAS", ec);
        // A file replaced by a link after mount is rejected at read.
        fs::remove(one / "sub" / "ITEM.RES");
        fs::create_symlink(root / "outside.dat", one / "sub" / "ITEM.RES", ec);
        if (!ec) reject([&] { vfs.read("SUB/ITEM.RES"); });
        assert((original == std::vector<std::uint8_t>{1, 2, 3}));
    } catch (...) { fs::remove_all(root); throw; }
    fs::remove_all(root);
}
}
int main() { test_folder_mounts(); }
