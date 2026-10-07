#include "rloft/surface_texture.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

void put16(std::vector<std::uint8_t>& b,
           std::size_t o,
           std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1U] = static_cast<std::uint8_t>(v >> 8U);
}

void put32(std::vector<std::uint8_t>& b,
           std::size_t o,
           std::uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        b[o + static_cast<std::size_t>(i)] =
            static_cast<std::uint8_t>(v >> (i * 8));
    }
}

}  // namespace

int main() {
    constexpr std::size_t decoded_size = 4096U;
    std::vector<std::uint8_t> decoded(decoded_size);
    for (std::size_t i = 0; i < decoded.size(); ++i) {
        decoded[i] = static_cast<std::uint8_t>(i & 0xFFU);
    }

    // Literal-only LZSS: one FF control byte followed by eight literals.
    std::vector<std::uint8_t> compressed;
    compressed.reserve(decoded_size + decoded_size / 8U);
    for (std::size_t i = 0; i < decoded.size(); i += 8U) {
        compressed.push_back(0xFF);
        for (std::size_t j = 0; j < 8U; ++j) {
            compressed.push_back(decoded[i + j]);
        }
    }

    const std::size_t entry_size = 7U + compressed.size();
    std::vector<std::uint8_t> archive(8U + entry_size, 0);
    put32(archive, 0, 1);
    put32(archive, 4, 8);

    std::size_t p = 8;
    archive[p++] = 'E';
    archive[p++] = 'H';
    archive[p++] = 6;
    put16(archive, p, 0);
    p += 2U;
    put16(archive, p, static_cast<std::uint16_t>(decoded_size));
    p += 2U;
    for (const auto value : compressed) {
        archive[p++] = value;
    }

    rloft::SurfaceTextureArchive textures(std::move(archive));
    const auto texture = textures.texture(0);

    assert(texture.width == 32);
    assert(texture.texels_column_major.size() == decoded_size);
    assert(texture.texel(0, 0) == decoded[0]);
    assert(texture.texel(0, 127) == decoded[127]);
    assert(texture.texel(1, 0) == decoded[128]);
    assert(texture.texel(31, 127) == decoded.back());

    bool rejected = false;
    try {
        (void)texture.texel(32, 0);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);
}
