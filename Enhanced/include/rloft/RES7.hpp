#pragma once

#include <cstdint>
#include <vector>

namespace rloft {

class ResArchive;
struct Type1Raster;

// RES7 contains the original CODE.1 character-creation graphics. It uses the
// existing EH type-1 decoder, including its one-row transpose behavior.
// These helpers do not create another archive, raster cache or renderer.
[[nodiscard]] Type1Raster decode_res7_image(const ResArchive& archive,
                                            int index);

// Replace one original image's decoded 8-bit pixels while keeping its EH
// descriptor (count, geometry, transparency and transform) unchanged.
// The replacement uses CODE.1-supported raw method 0. The caller can write
// ResArchive::encode(); all untouched compressed entries remain byte-exact.
void replace_res7_pixels(ResArchive& archive, int index,
                         const std::vector<std::uint8_t>& pixels);

} // namespace rloft
