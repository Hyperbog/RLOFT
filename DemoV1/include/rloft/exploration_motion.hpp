#pragma once

#include <cstdint>
#include <optional>

#include "rloft/renderer_tables.hpp"

namespace rloft {

struct ExplorationMotionVector final {
    std::uint32_t delta_x{};
    std::uint32_t delta_y{};
};

struct ExplorationMotionInputs final {
    // Raw CODE.2 runtime values corresponding to C6EC/C6F0/C6E8.
    std::int32_t angular_input{};
    std::int32_t orthogonal_input{};
    std::int32_t velocity{};
    bool grid_mode{};
};

struct ExplorationMotionState final {
    std::uint32_t heading{};
    std::int32_t angular_remainder{};
    std::int32_t velocity{};
};

struct ExplorationMotionStep final {
    ExplorationMotionState state{};
    std::optional<ExplorationMotionVector> translation;
    std::uint32_t translation_heading{};
    std::uint32_t translation_distance{};
};

class ExplorationMotionModel final {
public:
    [[nodiscard]] static std::uint32_t normalize_heading(
        std::int32_t heading) noexcept;

    // CODE.2 2DF2C. In free-angle mode this uses the original SINTAB
    // signed-magnitude Q16 factors. In grid mode it uses the exact eight-way
    // one-cell lookup table.
    [[nodiscard]] static ExplorationMotionVector direction_vector(
        std::uint32_t distance,
        std::uint32_t heading,
        bool grid_mode,
        const RendererTables& tables);

    // CODE.2 2E120 with the externally-produced input globals made explicit.
    // Collision/application side effects are intentionally separate.
    [[nodiscard]] static ExplorationMotionStep integrate(
        ExplorationMotionState state,
        ExplorationMotionInputs inputs,
        const RendererTables& tables);
};

}  // namespace rloft
