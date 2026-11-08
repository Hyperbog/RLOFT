#include "rloft/RES1.hpp"
#include "rloft/resource_assets.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {

std::uint16_t little16(const std::vector<std::uint8_t>& raw,
                       std::size_t at) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(raw[at]) |
        (static_cast<unsigned>(raw[at + 1U]) << 8U));
}

// The two physical 4-byte RES1 records both contain little-endian 0x60.
// Their producer/editor role is not proven, so do not turn them into EH images.
constexpr std::uint32_t kRawMarker = 0x60U;
constexpr std::size_t kType1HeaderBytes = 13U;
constexpr std::size_t kMaxDecodedPixels = 64U * 1024U * 1024U;

}  // namespace

Res1EntryInfo inspect_res1_entry(const ResArchive& archive, int index) {
    const auto raw = archive.entry(index); // Includes index bounds check.
    Res1EntryInfo info{};
    if (raw.empty()) return info;

    if (raw.size() == 4U &&
        raw[0] == static_cast<std::uint8_t>(kRawMarker) &&
        raw[1] == 0U && raw[2] == 0U && raw[3] == 0U) {
        info.kind = Res1EntryKind::RawMarker;
        info.raw_marker = kRawMarker;
        return info;
    }

    if (raw.size() < kType1HeaderBytes ||
        raw[0] != 'E' || raw[1] != 'H' || raw[2] != 1U ||
        little16(raw, 3U) != 8U) {
        throw std::runtime_error("RES1 entry is neither an EH type-1 raster,"
                                 " an empty slot nor the original 0x60 marker");
    }

    const auto count = little16(raw, 5U);
    const auto height = little16(raw, 7U);
    const auto units = raw[9U];
    const auto compression = raw[11U];
    const auto transpose = raw[12U];
    if (count == 0U || count > 0x7FFFU ||
        height == 0U || height > 0x7FFFU ||
        units == 0U ||
        (compression != 0U && compression != 3U) ||
        transpose > 1U) {
        throw std::runtime_error("RES1 EH type-1 descriptor is invalid");
    }

    const auto pixel_count = static_cast<std::size_t>(count) *
        static_cast<std::size_t>(height) *
        static_cast<std::size_t>(units) * 8U;
    if (pixel_count > kMaxDecodedPixels ||
        (compression == 0U &&
         raw.size() - kType1HeaderBytes != pixel_count) ||
        (compression == 3U && raw.size() == kType1HeaderBytes)) {
        throw std::runtime_error("RES1 raster payload has invalid size");
    }

    info.kind = Res1EntryKind::Type1Raster;
    info.source_count = count;
    info.source_height = height;
    info.width_units = units;
    info.transparent_index = raw[10U];
    info.compression = compression;
    info.transpose_flag = transpose;
    info.decoded_pixels = pixel_count;
    return info;
}

Res1ArchiveStats inspect_res1_archive(const ResArchive& archive) {
    Res1ArchiveStats stats{};
    stats.entries = static_cast<std::size_t>(archive.entry_count());
    for (int i = 0; i < archive.entry_count(); ++i) {
        const auto info = inspect_res1_entry(archive, i);
        switch (info.kind) {
            case Res1EntryKind::Empty:
                ++stats.empty_entries;
                break;
            case Res1EntryKind::RawMarker:
                ++stats.raw_markers;
                break;
            case Res1EntryKind::Type1Raster:
                ++stats.type1_rasters;
                if (stats.decoded_pixels >
                    std::numeric_limits<std::uint64_t>::max() -
                        static_cast<std::uint64_t>(info.decoded_pixels)) {
                    throw std::overflow_error("RES1 decoded image pixel total overflow");
                }
                stats.decoded_pixels += info.decoded_pixels;
                break;
        }
    }
    return stats;
}

Type1Raster decode_res1_image(const ResArchive& archive, int index) {
    if (inspect_res1_entry(archive, index).kind !=
        Res1EntryKind::Type1Raster) {
        throw std::invalid_argument("RES1 slot contains no image");
    }
    return archive.decode_type1(index);
}

void replace_res1_pixels(ResArchive& archive, int index,
                         const std::vector<std::uint8_t>& pixels) {
    const auto info = inspect_res1_entry(archive, index);
    if (info.kind != Res1EntryKind::Type1Raster) {
        throw std::invalid_argument("RES1 cannot replace an empty or marker slot");
    }
    if (pixels.size() != info.decoded_pixels) {
        throw std::invalid_argument("RES1 replacement changes source geometry");
    }

    // CODE.1's transform=1 on a one-row source alters *only* the returned
    // chunky raster geometry; the existing decoder preserves pixel order.
    // Therefore its decoded pixel vector is also a valid raw EH payload.
    auto raw = archive.entry(index);
    raw[11U] = 0U; // Uncompressed EH type-1, supported by existing decoder.
    raw.resize(kType1HeaderBytes);
    raw.insert(raw.end(), pixels.begin(), pixels.end());
    archive.replace_entry(index, raw); // Transactional shared RES serializer.
}

}  // namespace rloft
