#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/environment_view.hpp"
#include "rloft/exploration_session.hpp"
#include "rloft/exploration_world.hpp"
#include "rloft/resource_assets.hpp"

namespace rloft {

struct ExplorationFrameState final {
    std::uint32_t level_id{};
    CellPosition party_cell{};
    std::uint32_t coordinate_a{};
    std::uint32_t coordinate_b{};
    std::uint32_t facing_degrees{};
    std::uint32_t time_of_day_16_16_hours{};

    std::uint8_t environment_profile_id{0xFF};
    std::uint8_t level_music_sequence{};
    std::uint8_t renderer_fill_base_a{};
    std::uint8_t renderer_fill_base_b{};
    std::int16_t render_threshold_floor{1};

    bool level_from_direct_save{};
    LevelResourceBanks resource_banks{};
    std::optional<EnvironmentProfile> environment_profile;
};

class ExplorationFrameBuilder final {
public:
    [[nodiscard]] static ExplorationFrameState build(
        const ExplorationSessionState& session,
        const std::vector<EnvironmentProfile>& environment_profiles);
};

}  // namespace rloft
