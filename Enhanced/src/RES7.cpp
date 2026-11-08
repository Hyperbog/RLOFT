#include "rloft/RES7.hpp"
#include "rloft/resource_assets.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {

std::uint16_t read16(const std::vector<std::uint8_t>& entry,
                     std::size_t at) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(entry[at]) |
        (static_cast<unsigned>(entry[at + 1U]) << 8U));
}

void validate_image(const std::vector<std::uint8_t>& entry) {
    if (entry.size() < 13U || entry[0] != 'E' || entry[1] != 'H' ||
        entry[2] != 1U || read16(entry, 3U) != 8U ||
        read16(entry, 5U) == 0U || read16(entry, 5U) > 0x7FFFU ||
        read16(entry, 7U) == 0U || read16(entry, 7U) > 0x7FFFU ||
        entry[9U] == 0U || (entry[11U] != 0U && entry[11U] != 3U)) {
        throw std::runtime_error("RES7 entry is not a supported EH type-1 raster");
    }
    const auto decoded_bytes = static_cast<std::size_t>(read16(entry, 5U)) *
        static_cast<std::size_t>(read16(entry, 7U)) *
        static_cast<std::size_t>(entry[9U]) * 8U;
    if (entry[11U] == 0U && entry.size() - 13U != decoded_bytes) {
        throw std::runtime_error("RES7 raw image does not match its descriptor");
    }
}

} // namespace

Type1Raster decode_res7_image(const ResArchive& archive, int index) {
    validate_image(archive.entry(index));
    return archive.decode_type1(index);
}

void replace_res7_pixels(ResArchive& archive, int index,
                         const std::vector<std::uint8_t>& pixels) {
    const auto decoded = decode_res7_image(archive, index);
    if (pixels.size() != decoded.pixels.size()) {
        throw std::invalid_argument("RES7 replacement image size differs");
    }
    auto entry = archive.entry(index);
    // Preserve the original twelve remaining EH/descriptor bytes, changing
    // only compression from LZSS (3) to the executable-supported raw (0).
    entry[11U] = 0U;
    entry.resize(13U);
    entry.insert(entry.end(), pixels.begin(), pixels.end());
    archive.replace_entry(index, entry);
}

} // namespace rloft
