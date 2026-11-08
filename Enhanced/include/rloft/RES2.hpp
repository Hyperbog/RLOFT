#pragma once

#include <cstdint>
#include <vector>

namespace rloft {

class ResArchive;
struct Type1Raster;

// Original EH type-1 descriptor, before CODE.1's optional row transform.
struct Res2ImageInfo {
    std::uint16_t source_count{};
    std::uint16_t source_height{};
    std::uint8_t width_units{};
    std::uint8_t transparent_index{};
    std::uint8_t compression{};
    std::uint8_t transform{};
};

// RES2 owns inventory/encumbrance graphics, loaded on demand by the existing
// RavenloftBootAssets. The shared ResArchive remains the only RES decoder.
[[nodiscard]] Res2ImageInfo inspect_res2_image(const ResArchive& archive,
                                               int index);
[[nodiscard]] Type1Raster decode_res2_image(const ResArchive& archive,
                                            int index);

// Replace exact decoded pixels without touching other resources. The source
// EH descriptor is retained, but the replacement uses supported raw method 0
// rather than inventing an LZSS stream. ResArchive::encode preserves all
// other original compressed entry bytes, offset order and count.
void replace_res2_pixels(ResArchive& archive, int index,
                         const std::vector<std::uint8_t>& pixels);

} // namespace rloft
