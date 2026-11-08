#pragma once

#include "rloft/npc_catalog.hpp"

#include <cstdint>
#include <vector>

namespace rloft {

// Original NPCS.DAT: little-endian u32 count followed by count fixed 0x45
// records. The typed result is the SAME NpcDefinition used by the game.
// Raw name tails, unresolved bytes and complete flag words are retained.
[[nodiscard]] std::vector<NpcDefinition> decode_npcs_dat(
    const std::vector<std::uint8_t>& bytes);

// Semantic serializer: all documented fields are written from the typed NPC;
// post-NUL bytes in the original 20-byte name remain untouched unless
// overlapped by an explicitly changed name. Does not pack a C++ struct.
[[nodiscard]] std::vector<std::uint8_t> encode_npcs_dat(
    const std::vector<NpcDefinition>& definitions);

} // namespace rloft
