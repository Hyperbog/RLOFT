#include "rloft_native/native_assets.hpp"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

using rloft_native::NativeType1Raster;
using rloft_native::decode_native_type1_raster;

namespace {
std::vector<std::uint8_t> record(
    std::uint16_t count, std::uint16_t rows,
    std::uint8_t width_units, std::uint8_t key,
    std::uint8_t codec, std::uint8_t transform,
    std::vector<std::uint8_t> data) {
    std::vector<std::uint8_t> result = {
        'E', 'H', 1, 8, 0,
        std::uint8_t(count), std::uint8_t(count >> 8),
        std::uint8_t(rows), std::uint8_t(rows >> 8),
        width_units, key, codec, transform
    };
    result.insert(result.end(), data.begin(), data.end());
    return result;
}

template <typename Fn> void rejects(Fn callable) {
    bool threw = false;
    try { callable(); } catch (const std::exception&) { threw = true; }
    assert(threw);
}

void run_tests() {
    const std::vector<std::uint8_t> eight{0,1,2,3,4,5,6,255};
    const auto plain = decode_native_type1_raster(
        record(1,1,1,255,0,0,eight));
    assert(plain.frames == 1 && plain.width == 8 && plain.height == 1);
    assert(plain.transparent_index == 255);
    assert(plain.pixels == eight); // FF is stored, not silently discarded

    // EH type1 compressed: FF flags => eight original literal pixels.
    const auto compressed = decode_native_type1_raster(
        record(1,1,1,17,3,0,{0xFF,0,1,2,3,4,5,6,255}));
    assert(compressed.pixels == eight);
    assert(compressed.transparent_index == 17);

    // A literal followed by a reference to the just-written 0xFEE cell.
    // The three referenced pixels all reproduce the first 'A'.
    const auto repeated = decode_native_type1_raster(
        record(1,1,1,0,3,0,{0x01,'A',0xEE,0xF0,0x00,0xF0,0x00,0xF0}));
    // This stream is constructed to fill only 8 pixels; references use
    // the initialized dictionary and overwrite the active tail.
    assert(repeated.pixels.size() == 8);
    assert(repeated.pixels[0] == 'A');
    assert(repeated.pixels[1] == 'A');

    // 0x000 references the original FE-initialized dictionary prefix.
    const auto seeded = decode_native_type1_raster(
        record(1,1,1,0,3,0,{0x00,0x00,0x02,0x00,0x02,0x00,0x02}));
    assert(seeded.pixels.size() == 8);
    for (auto byte : seeded.pixels) assert(byte == 0xFE);

    std::vector<std::uint8_t> twoRows{
        1,2,3,4,5,6,7,8, 8,7,6,5,4,3,2,1
    };
    auto transformed = decode_native_type1_raster(
        record(2,1,1,255,0,1,twoRows));
    assert(transformed.frames == 1 &&
           transformed.width == 8 && transformed.height == 2);
    assert(transformed.pixels == twoRows);

    rejects([] { decode_native_type1_raster({}); });
    rejects([] { decode_native_type1_raster({'E','H',2}); });
    rejects([] { decode_native_type1_raster(record(0,1,1,0,0,0,{})); });
    rejects([] { decode_native_type1_raster(record(1,0,1,0,0,0,{})); });
    rejects([] { decode_native_type1_raster(record(1,1,0,0,0,0,{})); });
    rejects([] { decode_native_type1_raster(record(1,1,1,0,0,0,{0,1})); });
    rejects([] { decode_native_type1_raster(record(1,1,1,0,2,0,eight)); });
    rejects([] { decode_native_type1_raster(record(1,1,1,0,3,0,{0xFF,1})); });
    rejects([] { decode_native_type1_raster(record(1,1,1,0,3,0,{0x00,0xFF,0xF0})); });
    rejects([] { decode_native_type1_raster(record(32767,32767,255,0,0,0,{})); });

    // Full native reader -> INIT selector -> RES0[47] -> EH image bridge.
    // This fixture is standalone original-format data, not demo code.
    namespace fs = std::filesystem;
    const auto folder = fs::temp_directory_path() /
        ("rloft-eh1-native-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directory(folder);
    try {
        const std::size_t entries = 48;
        const std::size_t header_length = 4 + 4 * entries;
        std::vector<std::uint8_t> archive(header_length,0);
        const auto put32 = [&](std::size_t at, std::uint32_t value) {
            for(unsigned k=0;k<4;++k)
                archive[at+k] = std::uint8_t(value >> (8*k));
        };
        put32(0, static_cast<std::uint32_t>(entries));
        for(std::size_t n=0;n<entries;++n)
            put32(4 + n*4, static_cast<std::uint32_t>(header_length));
        const auto cursor = record(1,1,1,255,0,0,eight);
        archive.insert(archive.end(),cursor.begin(),cursor.end());
        const auto write = [&](const char* name,
                               const std::vector<std::uint8_t>& bytes) {
            std::ofstream file(folder/name,std::ios::binary);
            if(!file)throw std::runtime_error("cannot construct native raster fixture");
            file.write(reinterpret_cast<const char*>(bytes.data()),
                       static_cast<std::streamsize>(bytes.size()));
            if(!file)throw std::runtime_error("native raster fixture incomplete");
        };
        write("INIT",{0,0});
        write("T",{1});
        write("RES0",archive);
        const auto source = rloft_native::NativeMedia::open(folder);
        const auto original = rloft_native::load_native_default_cursor(source);
        assert(original.res0_id == 47);
        assert(original.image.pixels == eight);
        assert(original.image.frames == 1 && original.image.width == 8);
        assert(fs::file_size(folder/"RES0") == archive.size());
        write("INIT",{0xFF,0xFF});
        const auto wrong = rloft_native::NativeMedia::open(folder);
        rejects([&] {static_cast<void>(rloft_native::load_native_default_cursor(wrong));});
    } catch (...) {
        fs::remove_all(folder);
        throw;
    }
    fs::remove_all(folder);

    auto short_header = record(1,1,1,0,0,0,eight);
    short_header[3] = 7;
    rejects([&] { decode_native_type1_raster(short_header); });
    auto oversize_header = record(1,1,1,0,0,0,eight);
    oversize_header[3] = 200;
    rejects([&] { decode_native_type1_raster(oversize_header); });
}
}
int main() { run_tests(); }
