#include "rloft/RES1.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {

void require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}

template <class Action>
void rejects(Action action) {
    bool failed = false;
    try { action(); } catch (const std::exception&) { failed = true; }
    require(failed, "invalid RES1 operation accepted");
}

void set32(std::vector<std::uint8_t>& bytes,
           std::size_t offset, std::uint32_t value) {
    for (unsigned shift = 0U; shift < 32U; shift += 8U) {
        bytes[offset + shift / 8U] = static_cast<std::uint8_t>(value >> shift);
    }
}

void synthetic() {
    // One raw marker, empty entry and one method-0 transposed image.
    std::vector<std::uint8_t> bytes(16U, 0U);
    set32(bytes, 0U, 3U);
    set32(bytes, 4U, 16U);
    set32(bytes, 8U, 20U);
    set32(bytes, 12U, 20U);
    bytes.insert(bytes.end(), {0x60U, 0U, 0U, 0U});
    const std::vector<std::uint8_t> image{
        'E', 'H', 1, 8, 0, 2, 0, 1, 0, 1, 0xFE, 0, 1,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    };
    bytes.insert(bytes.end(), image.begin(), image.end());
    rloft::ResArchive archive(bytes);
    require(archive.encode() == bytes, "synthetic archive round trip");
    require(rloft::inspect_res1_entry(archive, 0).kind ==
                rloft::Res1EntryKind::RawMarker, "marker classification");
    require(rloft::inspect_res1_entry(archive, 1).kind ==
                rloft::Res1EntryKind::Empty, "empty classification");
    // The third entry is the uncompressed transformed image.
    auto normal = archive;
    require(rloft::inspect_res1_entry(normal, 2).kind ==
            rloft::Res1EntryKind::Type1Raster, "raster classification");
    const auto decoded = rloft::decode_res1_image(normal, 2);
    require(decoded.count == 1 && decoded.width == 8 &&
            decoded.height == 2 && decoded.pixels.size() == 16U,
            "transposed geometry lost");
    auto replacement = decoded.pixels;
    replacement[2U] = 0xCCU;
    rloft::replace_res1_pixels(normal, 2, replacement);
    require(rloft::ResArchive(normal.encode()).decode_type1(2).pixels ==
            replacement, "modified raster cannot reload");
    require(normal.entry(0) == archive.entry(0) &&
            normal.entry(1).empty(), "non-image slots were damaged");
    rejects([&] { (void)rloft::decode_res1_image(normal, 0); });
    rejects([&] { (void)rloft::decode_res1_image(normal, 1); });
    rejects([&] { rloft::replace_res1_pixels(normal, 0, replacement); });
    rejects([&] { rloft::replace_res1_pixels(normal, 2, {1U}); });
    rejects([&] { (void)rloft::inspect_res1_entry(normal, 3); });
    auto invalid = normal.encode();
    invalid[20U] = 0x61U;
    rejects([&] { (void)rloft::inspect_res1_archive(rloft::ResArchive(invalid)); });
}

void original(const std::vector<std::uint8_t>& bytes) {
    const rloft::ResArchive archive(bytes);
    require(bytes.size() == 354310U && archive.entry_count() == 428,
            "shipped RES1 geometry differs");
    require(archive.encode() == bytes, "RES1 is not byte-exact on round trip");
    const auto stats = rloft::inspect_res1_archive(archive);
    require(stats.type1_rasters == 405U && stats.empty_entries == 21U &&
            stats.raw_markers == 2U && stats.decoded_pixels == 967432U,
            "RES1 entry inventory differs from original");
    for (int i = 0; i < 428; ++i) {
        const auto info = rloft::inspect_res1_entry(archive, i);
        if (info.kind == rloft::Res1EntryKind::Type1Raster) {
            require(info.compression == 3U, "original image is not LZSS");
            const auto image = rloft::decode_res1_image(archive, i);
            require(image.pixels.size() == info.decoded_pixels &&
                    image.width == static_cast<int>(info.width_units) * 8,
                    "RES1 image decode size mismatch");
        }
    }
    for (int i : {380, 426}) {
        require(rloft::inspect_res1_entry(archive, i).kind ==
                    rloft::Res1EntryKind::Empty, "portrait empty slot lost");
    }
    for (int i : {381, 427}) {
        const auto info = rloft::inspect_res1_entry(archive, i);
        require(info.kind == rloft::Res1EntryKind::RawMarker &&
                info.raw_marker == 0x60U, "portrait marker lost");
    }
    // Local item graphics use 168..335; portraits use 336..427.
    auto edited = archive;
    auto item_pixels = rloft::decode_res1_image(edited, 168).pixels;
    auto portrait_pixels = rloft::decode_res1_image(edited, 336).pixels;
    item_pixels[0] ^= 0xFFU;
    portrait_pixels[1] ^= 0xFFU;
    rloft::replace_res1_pixels(edited, 168, item_pixels);
    rloft::replace_res1_pixels(edited, 336, portrait_pixels);
    const auto output = edited.encode();
    const rloft::ResArchive reopened(output);
    require(reopened.entry_count() == 428 && output != bytes &&
            rloft::decode_res1_image(reopened, 168).pixels == item_pixels &&
            rloft::decode_res1_image(reopened, 336).pixels == portrait_pixels,
            "item/portrait edits failed to reload");
    for (int i = 0; i < 428; ++i) {
        if (i != 168 && i != 336) {
            require(reopened.entry(i) == archive.entry(i),
                    "non-target compressed/empty/marker record was altered");
        }
    }
}
}  // namespace

int main(int argc, char** argv) {
    synthetic();
    if (argc > 2) throw std::runtime_error("usage: RES1_tests [RES1]");
    if (argc == 2) {
        std::ifstream input(argv[1], std::ios::binary);
        if (!input) throw std::runtime_error("unable to open RES1 fixture");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>());
        original(bytes);
    }
}
