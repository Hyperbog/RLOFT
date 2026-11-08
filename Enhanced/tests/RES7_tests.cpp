#include "rloft/RES7.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {

void require(bool valid, const char* reason) {
    if (!valid) throw std::runtime_error(reason);
}

template <class F> void rejects(F action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "invalid RES7 data was accepted");
}

void put32(std::vector<std::uint8_t>& data, std::size_t pos,
           std::uint32_t value) {
    for (unsigned i = 0; i != 4U; ++i) {
        data[pos + i] = static_cast<std::uint8_t>(value >> (8U * i));
    }
}

void synthetic() {
    // One valid 2-frame source (2x1x8), with the CODE.1 transpose flag.
    std::vector<std::uint8_t> bytes(8U + 13U + 16U, 0);
    put32(bytes, 0, 1);
    put32(bytes, 4, 8);
    auto* p = bytes.data() + 8;
    p[0] = 'E'; p[1] = 'H'; p[2] = 1; p[3] = 8;
    p[5] = 2; p[7] = 1; p[9] = 1;
    p[10] = 0xFE; p[11] = 0; p[12] = 1;
    for (unsigned i = 0; i < 16U; ++i) {
        p[13U + i] = static_cast<std::uint8_t>(i);
    }

    rloft::ResArchive archive(bytes);
    require(archive.encode() == bytes, "original entry was not preserved");
    auto image = rloft::decode_res7_image(archive, 0);
    require(image.count == 1 && image.width == 8 && image.height == 2 &&
            image.transparent_index == 0xFEU && image.pixels.size() == 16U,
            "CODE.1 one-row transform/geometry changed");
    image.pixels[2U] = 0x9AU;
    rloft::replace_res7_pixels(archive, 0, image.pixels);
    const auto edited = archive.encode();
    require(edited.size() == bytes.size() && edited[8U + 11U] == 0U,
            "raw replacement method or geometry changed");
    require(rloft::decode_res7_image(rloft::ResArchive(edited), 0).pixels ==
            image.pixels, "edited image cannot be decoded again");
    rejects([&] { rloft::replace_res7_pixels(archive, 0, {1U}); });
    rejects([&] { (void)rloft::decode_res7_image(archive, -1); });
    rejects([&] { (void)rloft::decode_res7_image(archive, 1); });
    auto invalid = bytes;
    invalid[8U + 11U] = 2;
    rejects([&] { (void)rloft::decode_res7_image(rloft::ResArchive(invalid), 0); });
    invalid = bytes;
    invalid[8U] = 0;
    rejects([&] { (void)rloft::decode_res7_image(rloft::ResArchive(invalid), 0); });
    invalid = bytes;
    invalid.pop_back();
    rejects([&] { (void)rloft::decode_res7_image(rloft::ResArchive(invalid), 0); });
}

void original(const std::vector<std::uint8_t>& bytes) {
    require(bytes.size() == 211133U, "unexpected original RES7 size");
    rloft::ResArchive archive(bytes);
    require(archive.entry_count() == 62, "RES7 must have 62 records");
    require(archive.encode() == bytes, "unaltered RES7 round trip differs");
    std::size_t total_pixels = 0;
    std::size_t transformed = 0;
    std::size_t untransformed = 0;
    for (int i = 0; i < archive.entry_count(); ++i) {
        const auto record = archive.entry(i);
        require(record.size() >= 13U && record[0] == 'E' &&
                record[1] == 'H' && record[2] == 1U &&
                record[11U] == 3U, "unexpected original record/compression");
        const auto picture = rloft::decode_res7_image(archive, i);
        const auto count = static_cast<std::size_t>(record[5U] |
                   (static_cast<unsigned>(record[6U]) << 8U));
        const auto height = static_cast<std::size_t>(record[7U] |
                    (static_cast<unsigned>(record[8U]) << 8U));
        const auto width = static_cast<std::size_t>(record[9U]) * 8U;
        require(picture.pixels.size() == count * height * width &&
                picture.width == static_cast<int>(width) &&
                picture.transparent_index == record[10U],
                "decoded image shape or indexed pixels differ");
        if (record[12U] != 0 && height == 1U) {
            require(picture.count == 1 && picture.height == static_cast<int>(count),
                    "transformed image rows did not match source descriptor");
            ++transformed;
        } else {
            require(picture.count == static_cast<int>(count) &&
                    picture.height == static_cast<int>(height),
                    "ordinary raster descriptor differs");
            ++untransformed;
        }
        total_pixels += picture.pixels.size();
    }
    require(transformed == 58U && untransformed == 4U,
            "original transform distribution differs");
    require(total_pixels == 709904U, "decoded pixel total differs");

    // CODE.1 character-creation presenter uses RES7:0x15 (21).
    auto pixels = rloft::decode_res7_image(archive, 0x15).pixels;
    pixels[0] ^= 0xFFU;
    rloft::replace_res7_pixels(archive, 0x15, pixels);
    const auto edited = archive.encode();
    require(edited != bytes, "edited archive unchanged");
    const rloft::ResArchive loaded(edited);
    require(rloft::decode_res7_image(loaded, 0x15).pixels == pixels,
            "modified character-creation artwork could not reload");
    const rloft::ResArchive initial(bytes);
    for (int i = 0; i < 62; ++i) {
        if (i == 0x15) continue;
        require(loaded.entry(i) == initial.entry(i),
                "updating one image changed another compressed resource");
    }
}
} // namespace

int main(int argc, char** argv) {
    synthetic();
    if (argc > 2) throw std::runtime_error("usage: RES7_tests [RES7]");
    if (argc == 2) {
        std::ifstream stream(argv[1], std::ios::binary);
        if (!stream) throw std::runtime_error("unable to open RES7");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(stream)),
            std::istreambuf_iterator<char>());
        original(bytes);
    }
}
