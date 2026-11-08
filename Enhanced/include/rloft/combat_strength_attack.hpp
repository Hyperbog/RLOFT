#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "rloft/exploration_state.hpp"

namespace rloft {

// Original CODE.2 C7C4..C80A: the SIGNED Strength attack adjustment.
// Original object-2 DGROUP 6832 is a 26-byte table. Strength >25 would
// read unrelated adjacent DOS memory and is not inferred for a mod.
class CombatStrengthAttackModel final {
public:
    [[nodiscard]] static CombatStrengthAttackModel from_code2(
        const std::vector<std::uint8_t>& code2);
    [[nodiscard]] int inspect(const PartyImage& party,
                               std::size_t actor) const;
    [[nodiscard]] int score(std::uint8_t strength,
                            std::uint8_t exceptional) const;
private:
    std::array<std::int8_t,26U> table_{};
};

} // namespace rloft
