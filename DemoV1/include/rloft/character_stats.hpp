#pragma once

#include <array>
#include <cstdint>

#include "rloft/character_creation_flow.hpp"
#include "rloft/resource_assets.hpp"

namespace rloft {

struct CharacterStats {
    std::array<int, 6> abilities{};
    std::array<int, 3> levels{};
    int exceptional_strength{};
    int base_6810{};
    int secondary_6813{10};
    int hit_points{};
};

struct CharacterStatTables {
    std::array<std::uint16_t, 120> progression{};
    std::array<std::uint8_t, 18> race_roll{};
    std::array<std::uint8_t, 72> race_bounds{};
    std::array<std::int8_t, 36> race_bonus{};
    std::array<std::uint8_t, 90> class_minima{};
    std::array<std::int8_t, 6> hit_die{};
    std::array<std::int32_t, 38> hp_variant{};
    std::array<std::int8_t, 32> secondary_bonus{};
    bool step_mode{};

    [[nodiscard]] static CharacterStatTables from_assets(
        const RavenloftBootAssets& assets);
};

class CharacterStatsModel final {
public:
    explicit CharacterStatsModel(const RavenloftBootAssets& assets);
    explicit CharacterStatsModel(CharacterStatTables tables);

    void seed(std::uint32_t value) noexcept;
    void generate(const CharacterDraft& draft);

    [[nodiscard]] const CharacterStats& value() const noexcept { return stats_; }
    [[nodiscard]] int secondary_display_value() const;

    // Mirrors function_3C58's selected-stat edits followed by function_3404.
    // Valid selections are ability 0..5 and HP 10.
    bool adjust(const CharacterDraft& draft, int selection, bool increase);

private:
    [[nodiscard]] std::uint32_t next_rand() noexcept;
    [[nodiscard]] int roll(int count, int scale, int add_each = 0, int add_final = 0) noexcept;
    void normalize(const CharacterDraft& draft);
    [[nodiscard]] int class_count(const CharacterDraft& draft) const noexcept;
    [[nodiscard]] int hp_variant_extra(int cls, int variant) const;

    CharacterStatTables tables_{};
    std::uint32_t rng_state_{};
    CharacterStats stats_{};
};

}  // namespace rloft
