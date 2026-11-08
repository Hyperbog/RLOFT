#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/game_source.hpp"
#include "rloft/resource_assets.hpp"

namespace rloft {

// Original RES4: EH type-6, zero-length type header, 16-bit decoded
// length, then 4 KiB LZSS. CODE.2 33647/338C8 reads 128-byte columns.
struct Res4TextureInfo final {
    int width{}; // Original corpus: 32 or 64
    int height{128};
    std::uint16_t decoded_bytes{};
    std::size_t compressed_bytes{};
};

struct SurfaceTexture final {
    static constexpr int kHeight = 128;
    int width{};
    std::vector<std::uint8_t> texels_column_major;

    [[nodiscard]] std::uint8_t texel(int x, int y) const;
};

[[nodiscard]] Res4TextureInfo inspect_res4_texture(
    const ResArchive& archive, int index);
[[nodiscard]] SurfaceTexture decode_res4_texture(
    const ResArchive& archive, int index);

// Changes only the requested source entry, preserving all other original
// compressed entries. Replacements are literal-only *valid LZSS streams*,
// not a made-up method-0 form (EH6 has no compression-selector byte).
void replace_res4_texture(ResArchive& archive, int index,
                          const SurfaceTexture& replacement);

// Stable native renderer-facing facade; no independent resource cache or
// competing decoder. Former surface_texture.hpp forwards to this header.
class SurfaceTextureArchive final {
public:
    explicit SurfaceTextureArchive(std::vector<std::uint8_t> res4_bytes);
    explicit SurfaceTextureArchive(const GameSource& source);

    [[nodiscard]] int entry_count() const noexcept;
    [[nodiscard]] SurfaceTexture texture(int index) const;
    void replace_texture(int index, const SurfaceTexture& replacement);
    [[nodiscard]] std::vector<std::uint8_t> encode() const;

private:
    ResArchive res4_;
};

}  // namespace rloft
