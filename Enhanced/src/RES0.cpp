#include "rloft/RES0.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kType1Header = 8U;
constexpr std::size_t kType2Header = 136U;
constexpr std::size_t kFontWidths = 128U;
constexpr std::size_t kEHHeader = 5U;
constexpr std::size_t kMaxDecoded = 64U * 1024U * 1024U;

std::uint16_t u16(const std::vector<std::uint8_t>& b,
                  std::size_t at) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(b[at]) |
        (static_cast<unsigned>(b[at + 1U]) << 8U));
}

Res0ResourceInfo inspect(const std::vector<std::uint8_t>& raw) {
    if (raw.size() < kEHHeader || raw[0] != 'E' || raw[1] != 'H' ||
        (raw[2] != 1U && raw[2] != 2U)) {
        throw std::runtime_error("RES0 entry must be EH type-1 or type-2");
    }
    const auto type = raw[2];
    const auto header_size = type == 1U ? kType1Header : kType2Header;
    if (u16(raw, 3U) != header_size ||
        raw.size() < kEHHeader + header_size) {
        throw std::runtime_error("RES0 entry has unsupported EH header size");
    }
    const std::size_t desc = kEHHeader + (type == 2U ? kFontWidths : 0U);
    const auto count = u16(raw, desc);
    const auto height = u16(raw, desc + 2U);
    const auto width_units = raw[desc + 4U];
    const auto compression = raw[desc + 6U];
    if (count == 0U || count > 0x7FFFU || height == 0U ||
        height > 0x7FFFU || width_units == 0U ||
        (compression != 0U && compression != 3U) ||
        (type == 2U && (count != kFontWidths || width_units != 1U))) {
        throw std::runtime_error("RES0 raster/font descriptor is invalid");
    }
    const auto width = static_cast<std::size_t>(width_units) * 8U;
    const auto rows = static_cast<std::size_t>(count) * height;
    if (rows > kMaxDecoded / width) {
        throw std::runtime_error("RES0 decoded raster exceeds safe limit");
    }
    const auto expected = rows * width;
    const auto payload_size = raw.size() - (kEHHeader + header_size);
    if ((compression == 0U && payload_size != expected) ||
        (compression == 3U && payload_size == 0U)) {
        throw std::runtime_error("RES0 compressed/raw payload length invalid");
    }
    return Res0ResourceInfo{
        type == 1U ? Res0ResourceKind::Raster : Res0ResourceKind::Font,
        count, height, width_units, raw[desc + 5U], compression,
        raw[desc + 7U], expected};
}
}  // namespace

Res0ResourceInfo inspect_res0_entry(const ResArchive& archive, int index) {
    return inspect(archive.entry(index));
}

Res0ArchiveStats inspect_res0_archive(const ResArchive& archive) {
    Res0ArchiveStats stats{};
    stats.entries = static_cast<std::size_t>(archive.entry_count());
    for (int index = 0; index < archive.entry_count(); ++index) {
        const auto info = inspect_res0_entry(archive, index);
        if (info.kind == Res0ResourceKind::Font) ++stats.fonts;
        else ++stats.rasters;
        if (info.compression == 3U) ++stats.compressed;
        else ++stats.raw;
        if (info.decoded_bytes >
            std::numeric_limits<std::uint64_t>::max() - stats.decoded_bytes) {
            throw std::overflow_error("RES0 decoded pixel total overflow");
        }
        stats.decoded_bytes += info.decoded_bytes;
    }
    return stats;
}

Type1Raster decode_res0_image(const ResArchive& archive, int index) {
    if (inspect_res0_entry(archive, index).kind != Res0ResourceKind::Raster) {
        throw std::invalid_argument("RES0 record is not an image");
    }
    return archive.decode_type1(index);
}

Type2Font decode_res0_font(const ResArchive& archive, int index) {
    if (inspect_res0_entry(archive, index).kind != Res0ResourceKind::Font) {
        throw std::invalid_argument("RES0 record is not a font");
    }
    return archive.decode_type2(index);
}

void replace_res0_pixels(ResArchive& archive, int index,
                         const std::vector<std::uint8_t>& pixels) {
    const auto info = inspect_res0_entry(archive, index);
    if (info.kind != Res0ResourceKind::Raster ||
        pixels.size() != info.decoded_bytes) {
        throw std::invalid_argument("RES0 replacement image changes geometry");
    }
    auto raw = archive.entry(index);
    raw[11U] = 0U;  // EH type-1 compression byte.
    raw.resize(kEHHeader + kType1Header);
    raw.insert(raw.end(), pixels.begin(), pixels.end());
    archive.replace_entry(index, raw);
}

void replace_res0_font(ResArchive& archive, int index,
                       const Type2Font& font) {
    const auto info = inspect_res0_entry(archive, index);
    if (info.kind != Res0ResourceKind::Font ||
        font.raster.count != info.count ||
        font.raster.height != info.height ||
        font.raster.width != static_cast<int>(info.width_units) * 8 ||
        font.raster.transparent_index != info.transparent_index ||
        font.raster.pixels.size() != info.decoded_bytes) {
        throw std::invalid_argument("RES0 replacement font changes glyph geometry");
    }
    auto raw = archive.entry(index);
    std::copy(font.widths.begin(), font.widths.end(), raw.begin() + 5U);
    raw[5U + kFontWidths + 6U] = 0U;  // EH type-2 raster compression.
    raw.resize(kEHHeader + kType2Header);
    raw.insert(raw.end(), font.raster.pixels.begin(),
               font.raster.pixels.end());
    archive.replace_entry(index, raw);
}

}  // namespace rloft
