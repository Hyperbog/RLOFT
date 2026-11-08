#include "rloft/RES4.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kEh6Prefix = 7U;
constexpr std::size_t kColumnHeight = 128U;

std::uint16_t le16(const std::vector<std::uint8_t>& b,
                   std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(b[offset]) |
        (static_cast<unsigned>(b[offset + 1U]) << 8U));
}

std::vector<std::uint8_t> literal_lzss(
    const std::vector<std::uint8_t>& pixels) {
    // Original CODE.2 37700: LSB-first flags; bit 1 means one literal.
    // An FF control followed by eight literals is always a legal stream.
    if (pixels.size() % 8U != 0U)
        throw std::invalid_argument("RES4 decoded texture must be 8-byte aligned");
    std::vector<std::uint8_t> compressed;
    compressed.reserve(pixels.size() + pixels.size() / 8U);
    for (std::size_t pos = 0; pos < pixels.size(); pos += 8U) {
        compressed.push_back(0xFFU);
        compressed.insert(compressed.end(), pixels.begin() +
            static_cast<std::ptrdiff_t>(pos), pixels.begin() +
            static_cast<std::ptrdiff_t>(pos + 8U));
    }
    return compressed;
}
}  // namespace

Res4TextureInfo inspect_res4_texture(const ResArchive& archive, int index) {
    const auto raw = archive.entry(index);
    if (raw.size() <= kEh6Prefix || raw[0] != 'E' || raw[1] != 'H' ||
        raw[2] != 6U || le16(raw, 3U) != 0U) {
        throw std::runtime_error("RES4 entry is not an EH type-6 texture");
    }
    const auto decoded_bytes = le16(raw, 5U);
    if (decoded_bytes != 4096U && decoded_bytes != 8192U) {
        throw std::runtime_error("RES4 texture is not 32x128 or 64x128");
    }
    return {static_cast<int>(decoded_bytes / kColumnHeight),
            static_cast<int>(kColumnHeight), decoded_bytes,
            raw.size() - kEh6Prefix};
}

SurfaceTexture decode_res4_texture(const ResArchive& archive, int index) {
    const auto info = inspect_res4_texture(archive, index);
    // Reuse the single canonical CODE.2 LZSS implementation. It returns
    // [original u16 decoded_length][column-major decoded texels].
    const auto block = archive.decode_type6_block(index);
    if (block.size() != static_cast<std::size_t>(info.decoded_bytes) + 2U ||
        le16(block, 0U) != info.decoded_bytes) {
        throw std::runtime_error("RES4 type-6 decoded length mismatch");
    }
    SurfaceTexture texture{};
    texture.width = info.width;
    texture.texels_column_major.assign(block.begin() + 2, block.end());
    return texture;
}

void replace_res4_texture(ResArchive& archive, int index,
                          const SurfaceTexture& replacement) {
    const auto info = inspect_res4_texture(archive, index);
    if (replacement.width != info.width ||
        replacement.texels_column_major.size() != info.decoded_bytes) {
        throw std::invalid_argument("RES4 replacement changes source dimensions");
    }
    // Preserve EH6 header and 16-bit decoded length. Compress the changed
    // pixels with canonical LZSS *literal flags*, then rebuild RES offsets.
    auto entry = archive.entry(index);
    entry.resize(kEh6Prefix);
    const auto compressed = literal_lzss(replacement.texels_column_major);
    entry.insert(entry.end(), compressed.begin(), compressed.end());
    archive.replace_entry(index, entry);
}

std::uint8_t SurfaceTexture::texel(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= kHeight)
        throw std::out_of_range("surface texture coordinate");
    if (texels_column_major.size() !=
        static_cast<std::size_t>(width) * kColumnHeight) {
        throw std::runtime_error("surface texture payload does not match dimensions");
    }
    return texels_column_major[static_cast<std::size_t>(x) * kColumnHeight +
                               static_cast<std::size_t>(y)];
}

SurfaceTextureArchive::SurfaceTextureArchive(
    std::vector<std::uint8_t> res4_bytes)
    : res4_(std::move(res4_bytes)) {}

SurfaceTextureArchive::SurfaceTextureArchive(const GameSource& source)
    : SurfaceTextureArchive(source.read("RES4")) {}

int SurfaceTextureArchive::entry_count() const noexcept {
    return res4_.entry_count();
}

SurfaceTexture SurfaceTextureArchive::texture(int index) const {
    return decode_res4_texture(res4_, index);
}

void SurfaceTextureArchive::replace_texture(
    int index, const SurfaceTexture& replacement) {
    replace_res4_texture(res4_, index, replacement);
}

std::vector<std::uint8_t> SurfaceTextureArchive::encode() const {
    return res4_.encode();
}

}  // namespace rloft
