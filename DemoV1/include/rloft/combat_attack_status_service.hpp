#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "rloft/exploration_state.hpp"

namespace rloft {

// CODE.2 logical E578 (objdump virtual 1E578), invoked by E5F0
// BEFORE NPC target/hit/weapon evaluation. These are original
// party-record mutations, not elapsed-time advancement.
class CombatAttackStatusService final {
public:
    // CF5A is the original four-byte per-actor UI dirty/redraw array.
    // Preserve unrelated pending redraw marks; set affected actors
    // to the original literal value 1, not a computed frame count.
    static void apply(
        PartyImage& party,
        std::array<std::uint8_t, PartyImage::kActorCount>& redraw_cf5a,
        std::size_t attacking_actor);
};

} // namespace rloft
