#pragma once

#include <cstddef>
#include <cstdint>

#include "rloft/exploration_state.hpp"

namespace rloft {

// CODE.2 logical 22E40: read an actor's matching class level.
// The first matching signed class byte at actor +15/+16/+17 wins;
// no match returns literal 1. Does not mutate party or advance RNG.
class CombatClassLevelModel final {
public:
    [[nodiscard]] static std::int32_t for_class(
        const PartyImage& party,
        std::size_t actor,
        std::int32_t class_code);
};

} // namespace rloft
