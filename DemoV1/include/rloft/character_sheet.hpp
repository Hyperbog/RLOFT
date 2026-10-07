#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include "rloft/exploration_state.hpp"

namespace rloft {

struct CharacterClassTrack final {
    std::int8_t class_code{-1};
    std::int16_t level{};
    std::int32_t progress{};

    [[nodiscard]] bool present() const noexcept {
        return class_code >= 0;
    }
    [[nodiscard]] int text_resource_index() const noexcept {
        return present()
            ? 0x15 + static_cast<int>(class_code)
            : -1;
    }
};

struct CharacterAbility final {
    std::uint8_t base{};
    std::uint8_t current{};
    int label_text_resource{};
};

struct CharacterSheet final {
    std::size_t actor_index{};
    std::string name;
    std::uint16_t status_word{};
    bool active{};

    std::uint8_t portrait_index{};
    std::uint8_t sex_code{};
    std::uint8_t race_code{};
    std::uint16_t age{};
    std::uint8_t alignment_code{};

    std::int8_t base_armor_class{};
    std::int16_t current_hp{};
    std::int16_t max_hp{};

    std::array<CharacterClassTrack, 3> classes{};
    std::array<CharacterAbility, 6> abilities{};
    std::uint8_t exceptional_strength_base{};
    std::uint8_t exceptional_strength_current{};

    [[nodiscard]] int race_text_resource_index() const noexcept {
        return 0x0F + static_cast<int>(race_code);
    }
    [[nodiscard]] int sex_text_resource_index() const noexcept {
        return 0x1B + static_cast<int>(sex_code);
    }
    [[nodiscard]] int alignment_text_resource_index() const noexcept {
        return 0x1D + static_cast<int>(alignment_code);
    }
    [[nodiscard]] std::size_t class_track_count() const noexcept;
};

class CharacterSheetModel final {
public:
    [[nodiscard]] static CharacterSheet actor(
        const PartyImage& party,
        std::size_t actor_index);
};

}  // namespace rloft
