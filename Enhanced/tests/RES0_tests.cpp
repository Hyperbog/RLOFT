#include "rloft/RES0.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool valid, const char* why) {
    if (!valid) throw std::runtime_error(why);
}
template <class F> void rejects(F action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "invalid RES0 action was accepted");
}
void put32(std::vector<std::uint8_t>& b, std::size_t p, std::uint32_t v) {
    for (unsigned i = 0; i < 4U; ++i)
        b[p + i] = static_cast<std::uint8_t>(v >> (8U * i));
}
std::vector<std::uint8_t> synthetic() {
    // Method-0 128-glyph font, followed by an EH type-1 two-row image.
    std::vector<std::uint8_t> font(5U + 136U + 128U * 8U, 0U);
    font[0] = 'E'; font[1] = 'H'; font[2] = 2U;
    font[3] = 136U;
    font[5U + 128U] = 128U; // count
    font[5U + 130U] = 1U;   // height
    font[5U + 132U] = 1U;   // width units
    font[5U + 133U] = 0xFEU;
    font[5U + 134U] = 0U;   // raw method
    for (unsigned i = 0; i < 128U; ++i) font[5U + i] = 6U;
    for (std::size_t i = 141U; i < font.size(); ++i)
        font[i] = static_cast<std::uint8_t>(i);
    const std::vector<std::uint8_t> raster{
        'E', 'H', 1U, 8U, 0U, 2U, 0U, 1U, 0U,
        1U, 0xFEU, 0U, 1U,
        0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U,
        8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U};
    std::vector<std::uint8_t> archive(12U, 0U);
    put32(archive, 0U, 2U);
    put32(archive, 4U, 12U);
    put32(archive, 8U, static_cast<std::uint32_t>(12U + font.size()));
    archive.insert(archive.end(), font.begin(), font.end());
    archive.insert(archive.end(), raster.begin(), raster.end());
    return archive;
}
void synthetic_checks() {
    const auto bytes = synthetic();
    rloft::ResArchive archive(bytes);
    require(archive.encode() == bytes, "synthetic RES archive changed");
    const auto stats = rloft::inspect_res0_archive(archive);
    require(stats.entries == 2U && stats.fonts == 1U &&
            stats.rasters == 1U && stats.raw == 2U &&
            stats.decoded_bytes == 1040U, "synthetic RES0 inventory");
    auto font = rloft::decode_res0_font(archive, 0);
    require(font.widths[0] == 6U && font.raster.pixels.size() == 1024U,
            "synthetic font decoded incorrectly");
    auto raster = rloft::decode_res0_image(archive, 1);
    require(raster.count == 1 && raster.height == 2 &&
            raster.width == 8 && raster.pixels.size() == 16U,
            "synthetic transformed image decoded incorrectly");
    font.widths[65U] = 7U;
    font.raster.pixels[0U] ^= 0x7FU;
    rloft::replace_res0_font(archive, 0, font);
    require(rloft::decode_res0_font(archive, 0).widths == font.widths,
            "edited font widths did not reload");
    require(rloft::decode_res0_font(archive, 0).raster.pixels == font.raster.pixels,
            "edited font pixels did not reload");
    raster.pixels[5U] ^= 0xFFU;
    rloft::replace_res0_pixels(archive, 1, raster.pixels);
    const rloft::ResArchive reloaded(archive.encode());
    require(rloft::decode_res0_image(reloaded, 1).pixels == raster.pixels,
            "edited synthetic image lost");
    rejects([&] { (void)rloft::decode_res0_image(archive, 0); });
    rejects([&] { (void)rloft::decode_res0_font(archive, 1); });
    rejects([&] { rloft::replace_res0_pixels(archive, 0, raster.pixels); });
    rejects([&] { rloft::replace_res0_font(archive, 1, font); });
    rejects([&] { rloft::replace_res0_pixels(archive, 1, {0U}); });
    rejects([&] { (void)rloft::inspect_res0_entry(archive, -1); });
    rejects([&] { (void)rloft::inspect_res0_entry(archive, 2); });
    auto invalid = bytes;
    invalid[12U] = 0x00U;
    rejects([&] { (void)rloft::inspect_res0_archive(rloft::ResArchive(invalid)); });
    invalid = bytes;
    invalid[12U + 5U + 134U] = 2U; // font compression
    rejects([&] { (void)rloft::inspect_res0_archive(rloft::ResArchive(invalid)); });
    invalid = bytes;
    invalid.pop_back();
    rejects([&] { (void)rloft::inspect_res0_archive(rloft::ResArchive(invalid)); });
}
void original_checks(const std::vector<std::uint8_t>& bytes) {
    require(bytes.size() == 2186991U, "not original RES0 file size");
    const rloft::ResArchive original(bytes);
    require(original.entry_count() == 141 && original.encode() == bytes,
            "RES0 original archive round trip differs");
    const auto stats = rloft::inspect_res0_archive(original);
    require(stats.entries == 141U && stats.fonts == 15U &&
            stats.rasters == 126U && stats.compressed == 118U &&
            stats.raw == 23U && stats.decoded_bytes == 4036760U,
            "original RES0 resource inventory differs");
    for (int id = 0; id < 141; ++id) {
        const auto info = rloft::inspect_res0_entry(original, id);
        if (id < 15) {
            require(info.kind == rloft::Res0ResourceKind::Font &&
                    info.count == 128U && info.compression == 3U,
                    "original font descriptor invalid");
            const auto font = rloft::decode_res0_font(original, id);
            require(font.raster.pixels.size() == info.decoded_bytes,
                    "original font raster size mismatch");
        } else {
            require(info.kind == rloft::Res0ResourceKind::Raster,
                    "original RES0 image descriptor invalid");
            const auto image = rloft::decode_res0_image(original, id);
            require(image.pixels.size() == info.decoded_bytes,
                    "original RES0 image pixel count mismatch");
        }
    }
    rloft::ResArchive edited(bytes);
    auto first = rloft::decode_res0_image(edited, 15).pixels;
    auto transposed = rloft::decode_res0_image(edited, 40).pixels;
    auto raw = rloft::decode_res0_image(edited, 126).pixels;
    auto font = rloft::decode_res0_font(edited, 0);
    first[0] ^= 0xFFU;
    transposed[5] ^= 0x7FU;
    raw[0] ^= 0x55U;
    font.widths[65] ^= 1U;
    font.raster.pixels[100] ^= 0xFFU;
    rloft::replace_res0_pixels(edited, 15, first);
    rloft::replace_res0_pixels(edited, 40, transposed);
    rloft::replace_res0_pixels(edited, 126, raw);
    rloft::replace_res0_font(edited, 0, font);
    const auto encoded = edited.encode();
    require(encoded != bytes, "RES0 edits did not alter archive");
    const rloft::ResArchive reloaded(encoded);
    require(reloaded.entry_count() == 141 &&
            rloft::decode_res0_image(reloaded, 15).pixels == first &&
            rloft::decode_res0_image(reloaded, 40).pixels == transposed &&
            rloft::decode_res0_image(reloaded, 126).pixels == raw &&
            rloft::decode_res0_font(reloaded, 0).widths == font.widths &&
            rloft::decode_res0_font(reloaded, 0).raster.pixels == font.raster.pixels,
            "modified RES0 images/fonts cannot reload");
    for (int i = 0; i < 141; ++i) {
        if (i != 0 && i != 15 && i != 40 && i != 126)
            require(reloaded.entry(i) == original.entry(i),
                    "RES0 edit modified an unrelated compressed record");
    }
}
}  // namespace

int main(int argc, char** argv) {
    synthetic_checks();
    if (argc > 2) throw std::runtime_error("usage: RES0_tests [RES0]");
    if (argc == 2) {
        std::ifstream file(argv[1], std::ios::binary);
        if (!file) throw std::runtime_error("unable to open original RES0");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        original_checks(bytes);
    }
}
