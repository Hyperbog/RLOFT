#include "rloft/RES8.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool ok, const char* reason) {
    if (!ok) throw std::runtime_error(reason);
}
template <typename Action> void rejects(Action action) {
    bool caught = false;
    try { action(); } catch (const std::exception&) { caught = true; }
    require(caught, "invalid RES8 data accepted");
}
void write32(std::vector<std::uint8_t>& b,
             std::size_t pos, unsigned v) {
    for (unsigned i = 0; i < 4U; ++i)
        b[pos + i] = static_cast<std::uint8_t>(v >> (8U * i));
}
void synthetic() {
    // 1 original type-1 image, 2 stacked rows x 8 pixels, uncompressed.
    std::vector<std::uint8_t> bytes(8U + 13U + 16U, 0U);
    write32(bytes, 0, 1U); write32(bytes, 4, 8U);
    auto* r = bytes.data() + 8;
    r[0] = 'E'; r[1] = 'H'; r[2] = 1; r[3] = 8;
    r[5] = 2; r[7] = 1; r[9] = 1; r[10] = 0xFE; r[11] = 0; r[12] = 1;
    for (unsigned i = 0; i < 16U; ++i) r[13 + i] = static_cast<std::uint8_t>(i);
    rloft::Res8Archive bank(bytes);
    require(bank.entry_count() == 1 && bank.encode() == bytes,
            "lossless synthetic archive round trip");
    const auto picture = bank.decode_type1(0);
    require(picture.count == 1 && picture.width == 8 && picture.height == 2 &&
            picture.pixels.size() == 16 && picture.transparent_index == 0xFE,
            "original type-1 transpose semantics");
    auto pixels = picture.pixels;
    pixels[5] = 201;
    bank.replace_pixels(0, pixels);
    require(bank.decode_type1(0).pixels == pixels, "raster edit didn't apply");
    const auto edited = bank.encode();
    require(edited[8 + 11] == 0 && edited.size() == bytes.size(),
            "method-0 replacement geometry");
    require(rloft::Res8Archive(edited).decode_type1(0).pixels == pixels,
            "edited data failed to reload");
    rejects([&] { (void)bank.decode_type1(-1); });
    rejects([&] { (void)bank.entry(1); });
    rejects([&] { bank.replace_pixels(0, {0}); });
    auto bad = bytes; bad[8] = 0;
    rejects([&] { (void)rloft::Res8Archive(bad); });
    bad = bytes; bad[8 + 11] = 2;
    rejects([&] { (void)rloft::Res8Archive(bad); });
    bad = bytes; bad.pop_back();
    rejects([&] { (void)rloft::Res8Archive(bad); });
    bad = bytes; bad[4] = 4;
    rejects([&] { (void)rloft::Res8Archive(bad); });
    rejects([] { (void)rloft::Res8Archive({}); });
}
void original(const std::vector<std::uint8_t>& source) {
    require(source.size() == 58772U, "unexpected RES8 fixture length");
    rloft::Res8Archive bank(source);
    require(bank.entry_count() == 73, "RES8 entry count");
    require(bank.encode() == source, "original RES8 byte-exact round trip");
    for (int index = 0; index < bank.entry_count(); ++index) {
        const auto record = bank.entry(index);
        require(record.size() >= 13 && record[11] == 3 && record[12] == 1,
                "original RES8 compression or transform flags");
        const auto image = bank.decode_type1(index);
        require(image.count == 1 && image.height == record[5] &&
                image.width == record[9] * 8 && image.transparent_index == record[10] &&
                image.pixels.size() == static_cast<std::size_t>(image.height) *
                    static_cast<std::size_t>(image.width),
                "RES8 original type-1 image decode");
    }
    auto pixels = bank.decode_type1(50).pixels;
    pixels[0] ^= 0xFF;
    bank.replace_pixels(50, pixels);
    const auto modified = bank.encode();
    require(modified != source, "edited archive unchanged");
    const rloft::Res8Archive loaded(modified);
    require(loaded.entry_count() == 73 && loaded.decode_type1(50).pixels == pixels,
            "modified image failed reload");
    for (int index = 0; index < 73; ++index) {
        if (index == 50) continue;
        require(loaded.entry(index) == rloft::Res8Archive(source).entry(index),
                "neighboring resource bytes modified");
    }
}
} // namespace

int main(int argc, char** argv) {
    synthetic();
    if (argc > 2) throw std::runtime_error("usage: RES8_tests [RES8]");
    if (argc == 2) {
        std::ifstream file(argv[1], std::ios::binary);
        if (!file) throw std::runtime_error("unable to open original RES8");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        original(bytes);
    }
}
