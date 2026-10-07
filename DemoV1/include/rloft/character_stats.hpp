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

class CharacterStatsModel final {
public:
    explicit CharacterStatsModel(const RavenloftBootAssets& assets);

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

    const RavenloftBootAssets& assets_;
    std::uint32_t rng_state_{};
    CharacterStats stats_{};
};

}  // namespace rloft
