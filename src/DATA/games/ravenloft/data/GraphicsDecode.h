#pragma once
#include <cstdint>
#include <span>
#include <vector>
namespace ravenloft::data {
struct IndexedImage {
    std::uint16_t width=0,height=0;
    // Type-7 sprites carry an explicit source bounding rectangle consumed by
    // CODE.2 0x21BF0. Textures leave these at the full image bounds.
    std::int16_t left=0,right=0,top=0,bottom=0;
    std::vector<std::uint8_t> pixels,alpha;
};
IndexedImage decodeTextureType6(std::span<const std::uint8_t> raw);
IndexedImage decodeSpriteType7(std::span<const std::uint8_t> raw);
IndexedImage decodeBitmapType1(std::span<const std::uint8_t> raw);
}
