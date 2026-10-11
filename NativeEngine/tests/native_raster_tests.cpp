#include "rloft_native/native_assets.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
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

    auto short_header = record(1,1,1,0,0,0,eight);
    short_header[3] = 7;
    rejects([&] { decode_native_type1_raster(short_header); });
    auto oversize_header = record(1,1,1,0,0,0,eight);
    oversize_header[3] = 200;
    rejects([&] { decode_native_type1_raster(oversize_header); });
}
}
int main() { run_tests(); }
