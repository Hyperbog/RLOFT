#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "rloft/game_source.hpp"

namespace rloft {

struct ObjectDefinition final {
    static constexpr std::size_t kSerializedSize = 0x41;

    // Preserve every original byte (including unknown/reserved fields).
    std::array<std::uint8_t, kSerializedSize> original_bytes{};
    std::string name;
    std::uint16_t scene_shape_base_local{};
    std::uint8_t scene_shape_stride{};
    std::uint8_t scene_influence_step{};
    std::uint8_t unresolved_2c{};
    std::uint16_t ui_graphic_index{};
    std::uint8_t item_category{};
    std::uint8_t weight_half_units{};
    std::int8_t container_state_index{}; // Original category-dependent state selector.
    std::uint16_t intrinsic_magic_marker{};
    std::int8_t armor_class_adjustment{};
    // Keep the full original LE attack-profile word, including reserved bits.
    std::uint16_t raw_word_35{};
    // Compatibility read view for existing hand-eligibility callers.
    std::uint8_t raw_byte_35_low2{};
    std::uint8_t enchantment{};
    std::uint8_t damage_dice_count{};
    std::uint8_t damage_dice_sides{};
    std::uint8_t damage_per_die_bonus{};
    std::uint8_t damage_lg_dice{};
    std::uint8_t damage_lg_sides{};
    std::uint8_t damage_lg_bonus{};
    std::uint8_t range_short{};
    std::uint8_t range_medium{};
    std::uint8_t range_long{};

    [[nodiscard]] std::uint8_t hand_pair_class() const noexcept {
        return static_cast<std::uint8_t>(raw_word_35 & 3U);
    }
    [[nodiscard]] std::uint8_t weapon_speed_factor() const noexcept {
        return static_cast<std::uint8_t>((raw_word_35 >> 2U) & 15U);
    }
};

class ObjectCatalog final {
public:
    [[nodiscard]] static ObjectCatalog parse(const std::vector<std::uint8_t>& bytes);
    [[nodiscard]] static ObjectCatalog load(const GameSource& source);
    [[nodiscard]] std::size_t size() const noexcept { return objects_.size(); }
    [[nodiscard]] const ObjectDefinition& at(std::size_t object_id) const;
    [[nodiscard]] ObjectDefinition& edit(std::size_t object_id);
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
private:
    std::vector<ObjectDefinition> objects_;
};

} // namespace rloft
