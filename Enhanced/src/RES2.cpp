#include "rloft/RES2.hpp"
#include "rloft/resource_assets.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {

std::uint16_t read16(const std::vector<std::uint8_t>& b,
                     std::size_t i) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(b[i]) |
        (static_cast<unsigned>(b[i + 1U]) << 8U));
}

} // namespace

Res2ImageInfo inspect_res2_image(const ResArchive& archive, int index) {
    const auto raw = archive.entry(index);
    if (raw.size() < 13U || raw[0] != 'E' || raw[1] != 'H' ||
        raw[2] != 1U || read16(raw, 3U) != 8U ||
        read16(raw, 5U) == 0U || read16(raw, 5U) > 0x7FFFU ||
        read16(raw, 7U) == 0U || read16(raw, 7U) > 0x7FFFU ||
        raw[9U] == 0U || (raw[11U] != 0U && raw[11U] != 3U)) {
        throw std::runtime_error("RES2 entry is not a supported EH type-1 image");
    }
    const auto count = read16(raw, 5U);
    const auto height = read16(raw, 7U);
    const std::size_t expected = static_cast<std::size_t>(count) *
        static_cast<std::size_t>(height) *
        static_cast<std::size_t>(raw[9U]) * 8U;
    if (raw[11U] == 0U && raw.size() - 13U != expected) {
        throw std::runtime_error("RES2 raw image payload size mismatch");
    }
    return {count, height, raw[9U], raw[10U], raw[11U], raw[12U]};
}

Type1Raster decode_res2_image(const ResArchive& archive, int index) {
    (void)inspect_res2_image(archive, index);
    return archive.decode_type1(index);
}

void replace_res2_pixels(ResArchive& archive, int index,
                         const std::vector<std::uint8_t>& pixels) {
    (void)inspect_res2_image(archive, index);
    const auto decoded = archive.decode_type1(index);
    if (pixels.size() != decoded.pixels.size()) {
        throw std::invalid_argument("RES2 replacement image size differs");
    }
    auto record = archive.entry(index);
    record[11U] = 0U; // method 0: original, uncompressed type-1 payload
    record.resize(13U);
    record.insert(record.end(), pixels.begin(), pixels.end());
    archive.replace_entry(index, record);
}

} // namespace rloft
