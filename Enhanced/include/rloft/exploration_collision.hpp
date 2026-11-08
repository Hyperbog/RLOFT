#pragma once

#include <cstdint>
#include <optional>

#include "rloft/exploration_state.hpp"
#include "rloft/exploration_world.hpp"

namespace rloft {

enum class DynamicBarrierPhase : std::uint8_t {
    Missing,
    Open,
    Transitional,
    Closed,
};

struct CellPassability final {
    bool special_geometry{};
    bool blocked{};
    DynamicBarrierPhase dynamic_phase{DynamicBarrierPhase::Missing};
    std::optional<std::uint8_t> dynamic_position;
};

class ExplorationCollisionModel final {
public:
    // Mirrors the CODE.2 traversal distinction:
    //   0x0080 -> dynamic group-2 lookup
    //   otherwise low-six-bit code zero/nonzero controls passability.
    [[nodiscard]] static CellPassability passability(
        const LevelState& level,
        CellPosition cell);

private:
    [[nodiscard]] static const CellControlRecord* find_dynamic_record(
        const LevelState& level,
        CellPosition cell) noexcept;
};

}  // namespace rloft
