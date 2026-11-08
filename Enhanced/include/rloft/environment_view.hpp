#pragma once

#include <array>
#include <cstdint>
#include <optional>

#include "rloft/exploration_state.hpp"
#include "rloft/exploration_world.hpp"

namespace rloft {

struct LevelResourceBanks final {
    std::array<std::uint16_t, 64> primary_a{};
    std::array<std::uint16_t, 64> alternate_a{};
    std::array<std::uint16_t, 64> primary_b{};
    std::array<std::uint16_t, 64> alternate_b{};
    std::array<std::uint16_t, 64> automap_style{};
};

struct CellVisualInputs final {
    CellPosition cell{};
    std::uint16_t raw_word{};
    std::uint8_t cell_code{};

    std::uint16_t primary_a{};
    std::uint16_t alternate_a{};
    std::uint16_t primary_b{};
    std::uint16_t alternate_b{};
    std::uint16_t automap_style{};

    bool special_geometry_center{};
    bool special_geometry_frame{};
    bool extra_render_prepass{};
    bool hidden_passage_topology{};
    bool record_state_variant_enabled{};
    std::uint8_t shade_decrement{};
    std::optional<std::uint8_t> record_state_variant;
};

class EnvironmentViewModel final {
public:
    [[nodiscard]] static LevelResourceBanks resource_banks(
        const LevelState& level);

    // Exposes the cell-local data consumed by the first-person renderer.
    // It deliberately does not perform the direction-dependent 0x0100
    // neighbor substitution; the ray/traversal layer owns which orthogonal
    // neighbor is selected.
    [[nodiscard]] static CellVisualInputs cell_visual_inputs(
        const LevelState& level,
        CellPosition cell);

private:
    [[nodiscard]] static const CellControlRecord* find_group1_record(
        const LevelState& level,
        CellPosition cell) noexcept;
};

}  // namespace rloft
