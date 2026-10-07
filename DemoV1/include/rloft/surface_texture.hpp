#pragma once

#include <cstdint>
#include <vector>

#include "rloft/game_source.hpp"
#include "rloft/resource_assets.hpp"

namespace rloft {

struct SurfaceTexture final {
    static constexpr int kHeight = 128;

    int width{};
    std::vector<std::uint8_t> texels_column_major;

    [[nodiscard]] std::uint8_t texel(int x, int y) const;
};

class SurfaceTextureArchive final {
public:
    explicit SurfaceTextureArchive(std::vector<std::uint8_t> res4_bytes);
    explicit SurfaceTextureArchive(const GameSource& source);

    [[nodiscard]] SurfaceTexture texture(int index) const;

private:
    ResArchive res4_;
};

}  // namespace rloft
