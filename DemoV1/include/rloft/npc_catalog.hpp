#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "rloft/game_source.hpp"

namespace rloft {

struct NpcDefinition final {
    static constexpr std::size_t kSerializedSize = 0x45;

    std::string name;
    std::uint16_t local_shape_base{};
    std::uint16_t collision_half_extent{};
    std::uint8_t scene_frame_stride{};
    std::uint8_t animation_cycle_divisor{};
    std::uint8_t animation_span{};
    std::uint8_t scene_influence_step{};
    std::uint8_t damage_reaction_animation_flag{};
    std::uint8_t hit_dice_count{};
    std::uint8_t melee_damage_dice{};
    std::uint8_t melee_damage_sides{};
    std::uint8_t melee_damage_bonus{};
    std::int8_t armor_class{};
    std::uint8_t thac0{};
    std::uint16_t experience_award{};
    std::uint16_t action_delay{};
    std::uint16_t render_scale_lookup_bias{};
    std::array<std::uint16_t, 4> directional_offset{};
    std::uint32_t behavior_flags{};
    std::uint32_t initial_runtime_flags{};
    std::uint8_t behavior_flags_39{};
    std::uint8_t behavior_flags_3a{};
    std::uint16_t ambient_sound_id{};
    std::uint16_t attack_sound_id{};
    std::uint16_t damage_sound_id{};
    std::uint16_t death_sound_id{};
};

class NpcCatalog final {
public:
    [[nodiscard]] static NpcCatalog parse(
        const std::vector<std::uint8_t>& bytes);
    [[nodiscard]] static NpcCatalog load(
        const GameSource& source);

    [[nodiscard]] std::size_t size() const noexcept { return npcs_.size(); }
    [[nodiscard]] const NpcDefinition& at(std::size_t npc_id) const;

private:
    std::vector<NpcDefinition> npcs_;
};

}  // namespace rloft
