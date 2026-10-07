#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "rloft/exploration_state.hpp"
#include "rloft/npc_catalog.hpp"

namespace rloft {

struct LiveNpcRecord final {
    std::uint32_t y_position_16_16{};
    std::uint32_t x_position_16_16{};
    std::uint32_t runtime_flags{};
    std::uint16_t cached_cell_index{};
    std::uint16_t npc_type_id{};
    std::uint16_t action_state_countdown{};
    std::int16_t current_hp{};
    std::uint16_t packed_runtime_state{};

    [[nodiscard]] std::uint8_t facing() const noexcept {
        return static_cast<std::uint8_t>(
            (packed_runtime_state >> 3U) & 7U);
    }
};

struct LiveObjectRecord final {
    std::uint32_t y_position_16_16{};
    std::uint32_t x_position_16_16{};
    std::uint32_t runtime_flags{};
    std::uint16_t cached_cell_index{};
    std::uint16_t object_type_id{};
    std::uint16_t state_word{};
    std::uint16_t auxiliary_word{};
    std::uint16_t packed_runtime_state{};
};

struct LevelWorldState final {
    std::vector<LiveNpcRecord> npcs;
    std::vector<LiveObjectRecord> placed_objects;
    std::vector<LiveObjectRecord> loose_items;
};

class LevelWorldStateCodec final {
public:
    using DiceRoller =
        std::function<std::uint16_t(
            std::uint8_t count,
            std::uint8_t sides)>;

    // Expands archive 11-byte records exactly as CODE.2 181B4 does, or
    // restores direct-save 22-byte records verbatim. Archive NPC HP
    // initialization uses the supplied dice callback.
    [[nodiscard]] static LevelWorldState build(
        const LevelState& level,
        const NpcCatalog& npcs,
        const DiceRoller& roll);
};

}  // namespace rloft
