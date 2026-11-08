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

struct CharacterStartingLoadout final {
    static constexpr std::size_t kSlotCount=21U;
    std::size_t family{};
    std::size_t race{};
    // Item slots 0..9 are backpack inventory, 10..13 unused
    // at creation, 14..17 the four original initial equipment
    // slots, 18..20 unused. Every -1 is an original empty slot.
    std::array<std::int16_t,kSlotCount> slots{};

    [[nodiscard]] bool has_shipped_items() const noexcept;
};

struct CharacterTransferTables {
    // function_5F4C addresses family*0xA8 + race*0x1C + 0988h.
    std::array<std::array<std::array<std::uint16_t, 14>, 6>, 15>
        family_race_words{};
    std::vector<std::uint8_t> object_conflict_flag;

    [[nodiscard]] static CharacterTransferTables from_assets(
        const RavenloftBootAssets& assets);
    // Decode every byte of the shipped 15-class-family x 6-race
    // 14-word CODE.1 0988h table from ORIGINAL executable bytes.
    // This is also usable by source-corpus diagnostics without
    // loading startup graphics, music or an unrelated saved game.
    [[nodiscard]] static CharacterTransferTables from_original_bytes(
        const std::vector<std::uint8_t>& code1,
        const std::vector<std::uint8_t>& objects);
    // Describe all 90 table rows, including explicitly unavailable
    // all-FFFF combinations. No made-up generic race/class gear.
    [[nodiscard]] CharacterStartingLoadout starting_loadout(
        std::size_t family,std::size_t race) const;
};

using CharacterTransferImage = std::array<std::uint8_t, 0x920>;

[[nodiscard]] CharacterTransferImage build_character_transfer(
    const std::array<CharacterTransferActor, 2>& actors,
    const CharacterTransferTables& tables);

}  // namespace rloft
