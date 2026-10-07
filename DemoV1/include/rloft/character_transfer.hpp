#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/character_creation_flow.hpp"
#include "rloft/character_stats.hpp"
#include "rloft/resource_assets.hpp"

namespace rloft {

struct CharacterTransferActor {
    CharacterDraft draft;
    CharacterStats stats;
};

struct CharacterTransferTables {
    // function_5F4C addresses family*0xA8 + race*0x1C + 0988h.
    std::array<std::array<std::array<std::uint16_t, 14>, 6>, 15>
        family_race_words{};
    std::vector<std::uint8_t> object_conflict_flag;

    [[nodiscard]] static CharacterTransferTables from_assets(
        const RavenloftBootAssets& assets);
};

using CharacterTransferImage = std::array<std::uint8_t, 0x920>;

[[nodiscard]] CharacterTransferImage build_character_transfer(
    const std::array<CharacterTransferActor, 2>& actors,
    const CharacterTransferTables& tables);

}  // namespace rloft
