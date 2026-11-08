#include "rloft/surface_texture.hpp"

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {

std::uint16_t u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

std::vector<std::uint8_t> lzss_4k_exact(
    const std::uint8_t* src,
    std::size_t src_size,
    std::size_t target_size) {
    std::array<int, 4096> history{};
    history.fill(-1);
    for (std::size_t i = 0; i < 0xFEE; ++i) {
        history[i] = 0xFE;
    }

    std::size_t hp = 0xFEE;
    std::size_t ip = 0;
    std::uint32_t flags = 0;
    std::vector<std::uint8_t> out;
    out.reserve(target_size);

    while (out.size() < target_size) {
        flags >>= 1U;
        if ((flags & 0x100U) == 0) {
            if (ip >= src_size) {
                throw std::runtime_error("type-6 LZSS control EOF");
            }
            flags = static_cast<std::uint32_t>(src[ip++]) | 0xFF00U;
        }

        if ((flags & 1U) != 0) {
            if (ip >= src_size) {
                throw std::runtime_error("type-6 LZSS literal EOF");
            }
            const auto value = src[ip++];
            out.push_back(value);
            history[hp] = value;
            hp = (hp + 1U) & 0xFFFU;
        } else {
            if (ip + 1U >= src_size) {
                throw std::runtime_error("type-6 LZSS backref EOF");
            }
            const auto lo = src[ip++];
            const auto hi = src[ip++];
            const auto offset =
                static_cast<std::size_t>(lo) |
                ((static_cast<std::size_t>(hi) & 0xF0U) << 4U);
            const auto count =
                static_cast<std::size_t>(hi & 0x0FU) + 3U;
            for (std::size_t j = 0;
                 j < count && out.size() < target_size;
                 ++j) {
                const auto source = (offset + j) & 0xFFFU;
                const int stored = history[source];
                if (stored < 0) {
                    throw std::runtime_error(
                        "type-6 LZSS undefined dictionary read");
                }
                const auto value = static_cast<std::uint8_t>(stored);
                out.push_back(value);
                history[hp] = value;
                hp = (hp + 1U) & 0xFFFU;
            }
        }
    }

    if (ip != src_size) {
        throw std::runtime_error(
            "type-6 LZSS stream has trailing bytes");
    }
    return out;
}

SurfaceTexture decode_type6_entry(
    const std::vector<std::uint8_t>& raw) {
    if (raw.size() < 7U ||
        raw[0] != 'E' || raw[1] != 'H' || raw[2] != 6) {
        throw std::runtime_error(
            "RES4 entry is not an EH type-6 texture");
    }
    if (u16(raw.data() + 3) != 0) {
        throw std::runtime_error(
            "shipped type-6 resource has nonzero type header");
    }

    const auto decoded_size =
        static_cast<std::size_t>(u16(raw.data() + 5));
    if (decoded_size != 4096U && decoded_size != 8192U) {
        throw std::runtime_error(
            "type-6 decoded size is not 32x128 or 64x128");
    }

    auto pixels = lzss_4k_exact(
        raw.data() + 7U,
        raw.size() - 7U,
        decoded_size);

    SurfaceTexture out{};
    out.width = static_cast<int>(
        decoded_size / SurfaceTexture::kHeight);
    out.texels_column_major = std::move(pixels);
    return out;
}

}  // namespace

std::uint8_t SurfaceTexture::texel(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= kHeight) {
        throw std::out_of_range("surface texture coordinate");
    }
    const auto expected =
        static_cast<std::size_t>(width) *
        static_cast<std::size_t>(kHeight);
    if (texels_column_major.size() != expected) {
        throw std::runtime_error(
            "surface texture payload does not match dimensions");
    }
    return texels_column_major[
        static_cast<std::size_t>(x) * kHeight +
        static_cast<std::size_t>(y)];
}

SurfaceTextureArchive::SurfaceTextureArchive(
    std::vector<std::uint8_t> res4_bytes)
    : res4_(std::move(res4_bytes)) {}

SurfaceTextureArchive::SurfaceTextureArchive(
    const GameSource& source)
    : SurfaceTextureArchive(source.read("RES4")) {}

SurfaceTexture SurfaceTextureArchive::texture(int index) const {
    return decode_type6_entry(res4_.entry(index));
}

}  // namespace rloft
