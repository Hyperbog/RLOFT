#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

class ResArchive;
struct Type1Raster;
struct Type2Font;

enum class Res0ResourceKind : std::uint8_t { Raster = 1, Font = 2 };

struct Res0ResourceInfo final {
    Res0ResourceKind kind{Res0ResourceKind::Raster};
    std::uint16_t count{};
    std::uint16_t height{};
    std::uint8_t width_units{};
    std::uint8_t transparent_index{};
    std::uint8_t compression{};
    std::uint8_t transform{};
    std::size_t decoded_bytes{};
};

struct Res0ArchiveStats final {
    std::size_t entries{};
    std::size_t rasters{};
    std::size_t fonts{};
    std::size_t compressed{};
    std::size_t raw{};
    std::uint64_t decoded_bytes{};
};

// RES0 is a mixed EH type-1 bitmap and EH type-2 font archive.
// All offsets and compressed data remain owned by the shared ResArchive.
[[nodiscard]] Res0ResourceInfo inspect_res0_entry(
    const ResArchive& archive, int index);
[[nodiscard]] Res0ArchiveStats inspect_res0_archive(
    const ResArchive& archive);

[[nodiscard]] Type1Raster decode_res0_image(const ResArchive& archive,
                                             int index);
[[nodiscard]] Type2Font decode_res0_font(const ResArchive& archive,
                                        int index);

// Replace only the named record; untouched records retain original bytes.
// Method 0 is the original executable-supported raw raster encoding.
void replace_res0_pixels(ResArchive& archive, int index,
                         const std::vector<std::uint8_t>& pixels);
void replace_res0_font(ResArchive& archive, int index,
                       const Type2Font& font);

}  // namespace rloft
