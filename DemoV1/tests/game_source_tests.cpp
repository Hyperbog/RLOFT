#include "rloft/game_source.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using rloft::GameSource;
using rloft::GameSourceKind;

namespace {

constexpr std::size_t kSector = 2048;

void expect_runtime_output(
    const GameSource& source,
    const fs::path& expected_parent,
    const char* context) {
    const auto output = source.runtime_output_path("CHARS");
    if (output.filename() != fs::path("CHARS")) {
        throw std::runtime_error(
            std::string(context) +
            ": runtime filename is " + output.filename().string());
    }

    std::error_code ec;
    const bool same =
        fs::equivalent(output.parent_path(), expected_parent, ec);
    if (ec || !same) {
        throw std::runtime_error(
            std::string(context) +
            ": runtime parent mismatch; actual=" +
            output.parent_path().string() +
            " expected=" + expected_parent.string() +
            " error=" + ec.message());
    }
}

void put_le32(std::uint8_t* p, std::uint32_t v) {
    p[0] = static_cast<std::uint8_t>(v);
    p[1] = static_cast<std::uint8_t>(v >> 8U);
    p[2] = static_cast<std::uint8_t>(v >> 16U);
    p[3] = static_cast<std::uint8_t>(v >> 24U);
    p[4] = p[3];
    p[5] = p[2];
    p[6] = p[1];
    p[7] = p[0];
}

std::size_t dir_record(std::uint8_t* out,
                       std::uint32_t extent,
                       std::uint32_t size,
                       std::uint8_t flags,
                       const std::vector<std::uint8_t>& name) {
    const std::size_t len =
        33 + name.size() + (name.size() % 2 == 0 ? 1 : 0);
    std::fill(out, out + len, 0);
    out[0] = static_cast<std::uint8_t>(len);
    put_le32(out + 2, extent);
    put_le32(out + 10, size);
    out[25] = flags;
    out[28] = 1;
    out[31] = 1;
    out[32] = static_cast<std::uint8_t>(name.size());
    std::copy(name.begin(), name.end(), out + 33);
    return len;
}

void write_iso(const fs::path& path, bool raw2352) {
    std::vector<std::array<std::uint8_t, kSector>> sectors(32);

    auto& pvd = sectors[16];
    pvd[0] = 1;
    std::copy_n("CD001", 5, pvd.begin() + 1);
    pvd[6] = 1;
    dir_record(pvd.data() + 156, 20, kSector, 2, {0});

    auto& term = sectors[17];
    term[0] = 255;
    std::copy_n("CD001", 5, term.begin() + 1);
    term[6] = 1;

    auto& root = sectors[20];
    std::size_t pos = 0;
    pos += dir_record(root.data() + pos, 20, kSector, 2, {0});
    pos += dir_record(root.data() + pos, 20, kSector, 2, {1});
    pos += dir_record(
        root.data() + pos, 21, 3, 0,
        {'R', 'E', 'S', '0', ';', '1'});
    pos += dir_record(
        root.data() + pos, 22, 4, 0,
        {'I', 'N', 'I', 'T', ';', '1'});
    pos += dir_record(
        root.data() + pos, 23, 5, 0,
        {'T', ';', '1'});

    sectors[21][0] = 1;
    sectors[21][1] = 2;
    sectors[21][2] = 3;
    sectors[22][0] = 4;
    sectors[22][1] = 5;
    sectors[22][2] = 6;
    sectors[22][3] = 7;
    std::copy_n("hello", 5, sectors[23].begin());

    std::ofstream out(path, std::ios::binary);
    for (const auto& sector : sectors) {
        if (!raw2352) {
            out.write(
                reinterpret_cast<const char*>(sector.data()),
                sector.size());
        } else {
            std::array<std::uint8_t, 2352> raw{};
            raw[15] = 1;
            std::copy(
                sector.begin(), sector.end(), raw.begin() + 16);
            out.write(
                reinterpret_cast<const char*>(raw.data()),
                raw.size());
        }
    }
}

void test_iso(bool raw) {
    const auto base = fs::temp_directory_path() /
        (raw ? "rloft_raw_iso_test" : "rloft_iso_test");
    fs::remove_all(base);
    fs::create_directories(base);
    write_iso(base / "game.gog", raw);

    {
        auto source = GameSource::open(base);
        assert(source.kind() == GameSourceKind::GogCdImage);
        assert(source.contains("res0"));
        expect_runtime_output(source, base, "ISO runtime path");
        assert(source.contains("INIT"));
        assert(source.read("RES0") ==
               std::vector<std::uint8_t>({1, 2, 3}));

        const auto text = source.read("T");
        assert(std::string(text.begin(), text.end()) == "hello");
    }
    fs::remove_all(base);
}

void test_loose_and_env_discovery() {
    const auto base =
        fs::temp_directory_path() / "rloft_loose_test";
    fs::remove_all(base);
    fs::create_directories(base);

    for (const auto& name : {"RES0", "INIT", "T"}) {
        std::ofstream(base / name, std::ios::binary) << name;
    }

    {
        auto source = GameSource::open(base);
        assert(source.kind() == GameSourceKind::LooseDirectory);
        assert(source.contains("res0"));
        {
            const auto output = source.runtime_output_path("CHARS");
            assert(output.filename() == fs::path("CHARS"));
            assert(fs::equivalent(output.parent_path(), base));
        }
    }

    {
        const auto nested =
            fs::temp_directory_path() / "rloft_gog_runtime_test";
        fs::remove_all(nested);
        fs::create_directories(nested / "RLOFT");
        write_iso(nested / "game.gog", false);
        {
            auto source = GameSource::open(nested);
            assert(source.kind() == GameSourceKind::GogCdImage);
            expect_runtime_output(
                source,
                nested / "RLOFT",
                "GOG nested runtime path");
        }
        // Windows cannot remove game.gog while Iso9660Image still owns the
        // open stream. Destroy the source before deleting the fixture.
        fs::remove_all(nested);
    }

#ifdef _WIN32
    _putenv_s("RLOFT_GAME_DIR", base.string().c_str());
#else
    setenv("RLOFT_GAME_DIR", base.string().c_str(), 1);
#endif

    {
        auto found = GameSource::auto_locate();
        assert(found.has_value());
        assert(found->kind() == GameSourceKind::LooseDirectory);
    }

#ifdef _WIN32
    _putenv_s("RLOFT_GAME_DIR", "");
#else
    unsetenv("RLOFT_GAME_DIR");
#endif

    fs::remove_all(base);
}

}  // namespace

int main() {
    test_iso(false);
    test_iso(true);
    test_loose_and_env_discovery();
    std::cout << "game_source_tests: PASS\n";
}
