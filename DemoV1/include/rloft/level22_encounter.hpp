#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

#include "rloft/exploration_motion.hpp"
#include "rloft/inventory_runtime.hpp"
#include "rloft/level22_story.hpp"
#include "rloft/level_world_state.hpp"
#include "rloft/npc_catalog.hpp"
#include "rloft/object_catalog.hpp"
#include "rloft/renderer_tables.hpp"

namespace rloft {

enum class NpcSoundKind : std::uint8_t {
    None,
    DamageReaction,
    Death,
};

struct NpcDamageResult final {
    bool damaged{};
    bool died{};
    NpcSoundKind sound_kind{NpcSoundKind::None};
    std::uint16_t sound_id{};
    std::uint16_t experience_award{};
    std::size_t spawned_loose_items{};
};

struct LooseItemPickupResult final {
    std::int16_t object_id{-1};
    std::uint8_t known{};
    bool level22_mist_sequence{};
};

class Level22EncounterModel final {
public:
    using PlacementProbe =
        std::function<bool(
            std::uint32_t y_position_16_16,
            std::uint32_t x_position_16_16,
            std::uint32_t radius)>;

    // CODE.2 0E284 specialization for the Level 22 Assassin. The caller
    // supplies the broader 2D5D0 collision probe used by 2195C.
    [[nodiscard]] static NpcDamageResult damage_assassin(
        LevelWorldState& world,
        std::size_t npc_index,
        std::uint16_t amount,
        std::uint16_t reaction_mode,
        std::uint32_t party_facing,
        const NpcCatalog& npcs,
        const RendererTables& tables,
        const PlacementProbe& can_place);

    // CODE.2 21704 + the Level-22 set-completion branch in 223F4.
    [[nodiscard]] static LooseItemPickupResult pickup_loose_item(
        LevelWorldState& world,
        std::size_t item_index,
        InventoryRuntimeState& inventory,
        PartyImage& party,
        const ObjectCatalog& objects);
};

}  // namespace rloft
