#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

class ResArchive;
struct Type1Raster;

// Archive 1 uses the shared RES offset directory and EH type-1 decoder.
// 21 empty slots and the two raw 0x60 markers are physical records, not
// images; they must remain at their original indices on export.
enum class Res1EntryKind : std::uint8_t {
    Empty,
    Type1Raster,
    RawMarker,
};

struct Res1EntryInfo final {
    Res1EntryKind kind{Res1EntryKind::Empty};
    std::uint16_t source_count{};
    std::uint16_t source_height{};
    std::uint8_t width_units{};
    std::uint8_t transparent_index{};
    std::uint8_t compression{};
    std::uint8_t transpose_flag{};
    std::uint32_t raw_marker{};
    std::size_t decoded_pixels{};
};

struct Res1ArchiveStats final {
    std::size_t entries{};
    std::size_t type1_rasters{};
    std::size_t empty_entries{};
    std::size_t raw_markers{};
    std::uint64_t decoded_pixels{};
};

// Exact raw-entry classification. Unknown non-EH records are rejected instead
// of silently interpreting them as image data or dropping them from RES1.
[[nodiscard]] Res1EntryInfo inspect_res1_entry(const ResArchive& archive,
                                              int index);
[[nodiscard]] Res1ArchiveStats inspect_res1_archive(const ResArchive& archive);

// Only image entries can be decoded/replaced. Keep their original geometry,
// transparent index and CODE.1 single-row transform semantics.
[[nodiscard]] Type1Raster decode_res1_image(const ResArchive& archive,
                                           int index);
void replace_res1_pixels(ResArchive& archive, int index,
                         const std::vector<std::uint8_t>& pixels);

// Reuse ResArchive::encode() for byte-exact round trips and offset rebuilding.
// No original game data or executable rasterizer code is embedded here.

}  // namespace rloft
