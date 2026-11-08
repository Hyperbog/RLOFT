#include "rloft/RES2.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool condition, const char* why) {
    if (!condition) throw std::runtime_error(why);
}
template <typename F> void rejects(F fn) {
    bool rejected = false;
    try { fn(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "RES2 invalid input was accepted");
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, unsigned x) {
    for (unsigned i = 0; i < 4U; ++i)
        b[at + i] = static_cast<std::uint8_t>(x >> (i * 8U));
}
void synthetic() {
    // Preserve zero-length entries from existing native RES2 routing tests.
    std::vector<std::uint8_t> b(12U, 0);
    put32(b, 0, 2); put32(b, 4, 12); put32(b, 8, 12);
    const std::vector<std::uint8_t> rec{
        'E','H',1,8,0,2,0,1,0,1,0xFE,0,1,
        1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
    };
    b.insert(b.end(), rec.begin(), rec.end());
    rloft::ResArchive res(b);
    require(res.entry_count() == 2 && res.encode() == b,
            "zero-length entry archive was not preserved");
    rejects([&] { (void)rloft::decode_res2_image(res, 0); });
    const auto pic = rloft::decode_res2_image(res, 1);
    require(pic.count == 1 && pic.height == 2 && pic.width == 8 &&
            pic.transparent_index == 0xFEU, "transformed image geometry");
    auto pixels = pic.pixels; pixels[5] = 0xCC;
    rloft::replace_res2_pixels(res, 1, pixels);
    require(res.entry(0).empty() &&
            rloft::decode_res2_image(res, 1).pixels == pixels,
            "edited image lost or zero entry changed");
    require(rloft::ResArchive(res.encode()).decode_type1(1).pixels == pixels,
            "edited archive cannot be reloaded");
    rejects([&] { rloft::replace_res2_pixels(res, 1, {0}); });
    rejects([&] { (void)rloft::decode_res2_image(res, 2); });
    rejects([&] { res.replace_entry(-1, rec); });
    auto bad = rec; bad[11] = 2;
    res.replace_entry(1, bad);
    rejects([&] { (void)rloft::decode_res2_image(res, 1); });
    rejects([] { (void)rloft::ResArchive({}); });
}
void original(const std::vector<std::uint8_t>& bytes) {
    require(bytes.size() == 63736U, "original RES2 byte size differs");
    rloft::ResArchive archive(bytes);
    require(archive.entry_count() == 38 && archive.encode() == bytes,
            "original RES2 archive roundtrip differs");
    std::size_t pixels_count = 0U;
    for (int index = 0; index < archive.entry_count(); ++index) {
        const auto info = rloft::inspect_res2_image(archive, index);
        require(info.compression == 3U, "expected original LZSS resource");
        auto image = rloft::decode_res2_image(archive, index);
        const auto expected = static_cast<std::size_t>(info.source_count) *
            info.source_height * info.width_units * 8U;
        require(image.pixels.size() == expected &&
                image.width == info.width_units * 8 &&
                image.transparent_index == info.transparent_index,
                "RES2 original image geometry or raster length differs");
        pixels_count += image.pixels.size();
    }
    require(pixels_count > 150000U, "unexpected decoded RES2 image volume");
    auto pixels = rloft::decode_res2_image(archive, 0x25).pixels;
    pixels[0] ^= 0xFFU;
    rloft::replace_res2_pixels(archive, 0x25, pixels);
    const auto result = archive.encode();
    require(result != bytes, "edited RES2 file did not change");
    const rloft::ResArchive reopened(result);
    require(reopened.entry_count() == 38 &&
            rloft::decode_res2_image(reopened, 0x25).pixels == pixels,
            "modded inventory panel did not reload");
    const rloft::ResArchive source(bytes);
    for (int i = 0; i < 38; ++i) {
        if (i != 0x25) require(source.entry(i) == reopened.entry(i),
                                "editing one image modified another entry");
    }
    rejects([&] { (void)rloft::decode_res2_image(archive, 38); });
}
} // namespace
int main(int argc, char** argv) {
    synthetic();
    if (argc > 2) throw std::runtime_error("usage: RES2_tests [RES2]");
    if (argc == 2) {
        std::ifstream stream(argv[1], std::ios::binary);
        if (!stream) throw std::runtime_error("unable to open RES2");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(stream)),
            std::istreambuf_iterator<char>());
        original(bytes);
    }
}
