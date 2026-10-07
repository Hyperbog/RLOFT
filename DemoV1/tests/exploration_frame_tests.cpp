#include "rloft/exploration_frame.hpp"

#include <cassert>
#include <stdexcept>
#include <vector>

int main() {
    rloft::ExplorationSessionState session{};
    session.level_id = 22;
    session.coordinate_a = 0x00048000U;
    session.coordinate_b = 0x00048000U;
    session.facing = 270;
    session.time_of_day_16_16_hours = 0x000E0000U;
    session.level_from_direct_save = false;
    session.level.width = 30;
    session.level.height = 15;
    session.level.cell_words.assign(30U * 15U, 0);
    session.level.environment_profile = 0;
    session.level.music_sequence = 7;
    session.level.renderer_fill_base_a = 0x2C;
    session.level.renderer_fill_base_b = 0x31;
    session.level.render_threshold_floor = 1;

    rloft::EnvironmentProfile profile{};
    profile.res8_base = 8;
    profile.stored_record_count = 1;
    profile.placements.push_back(rloft::EnvironmentPlacement{});
    std::vector<rloft::EnvironmentProfile> profiles{profile};

    const auto frame =
        rloft::ExplorationFrameBuilder::build(session, profiles);
    assert(frame.level_id == 22);
    assert(frame.party_cell.column == 4);
    assert(frame.party_cell.row == 4);
    assert(frame.facing_degrees == 270);
    assert(frame.time_of_day_16_16_hours == 0x000E0000U);
    assert(frame.environment_profile_id == 0);
    assert(frame.level_music_sequence == 7);
    assert(frame.renderer_fill_base_a == 0x2C);
    assert(frame.renderer_fill_base_b == 0x31);
    assert(frame.render_threshold_floor == 1);
    assert(frame.environment_profile.has_value());
    assert(frame.environment_profile->res8_base == 8);

    session.level.environment_profile = 0xFF;
    const auto fallback =
        rloft::ExplorationFrameBuilder::build(session, profiles);
    assert(!fallback.environment_profile.has_value());

    session.level.environment_profile = 1;
    bool rejected = false;
    try {
        (void)rloft::ExplorationFrameBuilder::build(session, profiles);
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);
}
