#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/exploration_world.hpp"

namespace rloft {

struct AutomapAnnotation final {
    std::int16_t raw_x{};
    std::int16_t raw_y{};
    std::vector<std::uint8_t> payload;

    [[nodiscard]] CellPosition map_cell() const noexcept;
};

struct AutomapAnnotationLabel final {
    char first{'A'};
    char second{' '};

    [[nodiscard]] std::array<char, 2> bytes() const noexcept {
        return {first, second};
    }
};

class AutomapAnnotationModel final {
public:
    [[nodiscard]] static std::vector<AutomapAnnotation> parse(
        const std::vector<std::uint8_t>& blob);

    [[nodiscard]] static AutomapAnnotationLabel label_for_index(
        std::size_t index);

    [[nodiscard]] static std::optional<AutomapAnnotationLabel> label_at(
        const std::vector<AutomapAnnotation>& annotations,
        CellPosition cell);
};

}  // namespace rloft
