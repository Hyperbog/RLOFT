#include "rloft/exploration_frame.hpp"

#include <cstddef>
#include <stdexcept>

namespace rloft {

ExplorationFrameState ExplorationFrameBuilder::build(
    const ExplorationSessionState& session,
    const std::vector<EnvironmentProfile>& environment_profiles) {
    ExplorationFrameState out{};
    out.level_id = session.level_id;
    out.party_cell = ExplorationWorldModel::party_cell(session);
    if (!ExplorationWorldModel::in_bounds(session.level, out.party_cell)) {
        throw std::runtime_error(
            "party world coordinates lie outside current level");
    }

    out.coordinate_a = session.coordinate_a;
    out.coordinate_b = session.coordinate_b;
    out.facing_degrees = session.facing;
    out.time_of_day_16_16_hours = session.time_of_day_16_16_hours;
    out.environment_profile_id = session.level.environment_profile;
    out.level_music_sequence = session.level.music_sequence;
    out.renderer_fill_base_a = session.level.renderer_fill_base_a;
    out.renderer_fill_base_b = session.level.renderer_fill_base_b;
    out.render_threshold_floor = session.level.render_threshold_floor;
    out.level_from_direct_save = session.level_from_direct_save;
    out.resource_banks =
        EnvironmentViewModel::resource_banks(session.level);

    if (out.environment_profile_id != 0xFFU) {
        const auto index =
            static_cast<std::size_t>(out.environment_profile_id);
        if (index >= environment_profiles.size()) {
            throw std::runtime_error(
                "level selects unavailable INIT environment profile");
        }
        out.environment_profile = environment_profiles[index];
    }

    return out;
}

}  // namespace rloft
