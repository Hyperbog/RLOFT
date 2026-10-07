#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/game_source.hpp"

namespace rloft {

struct SceneSourceBanks final {
    std::array<std::uint16_t, 4> base{};

    [[nodiscard]] std::uint16_t at(
        std::size_t source_class) const;

    [[nodiscard]] static SceneSourceBanks parse(
        const std::vector<std::uint8_t>& init_bytes);
    [[nodiscard]] static SceneSourceBanks load(
        const GameSource& source);
};

}  // namespace rloft
