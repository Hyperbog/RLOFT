#include "rloft/exploration_session.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>

int main() {
    assert(rloft::ExplorationSessionLoader::standalone_level_filename(0) ==
           "LEV000.DAT");
    assert(rloft::ExplorationSessionLoader::standalone_level_filename(22) ==
           "LEV022.DAT");
    assert(rloft::ExplorationSessionLoader::standalone_level_filename(999) ==
           "LEV999.DAT");

    bool rejected = false;
    try {
        (void)rloft::ExplorationSessionLoader::standalone_level_filename(-1);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);

    std::vector<std::uint8_t> chars(rloft::PartyImage::kSize, 0xFF);
    std::fill(chars.begin(), chars.begin() + 16, 0);
    chars[0] = 'A';
    chars[1] = 'N';
    chars[2] = 'N';
    chars[0x10] = 0;
    chars[0x11] = 0;

    rloft::LevelState level{};
    level.width = 30;
    level.height = 15;
    level.environment_profile = 1;
    level.music_sequence = 0;
    level.cell_words.resize(30U * 15U);

    auto session = rloft::ExplorationSessionState::fresh_party(
        rloft::PartyImage::from_bytes(chars),
        std::move(level));

    assert(session.level_id == 22);
    assert(session.coordinate_a == 0x00048000U);
    assert(session.coordinate_b == 0x00048000U);
    assert(session.facing == 270);
    assert(session.time_of_day_16_16_hours == 0x000E0000U);
    assert(session.party.name(0) == "ANN");
    assert(session.party.active(0));
    assert(!session.party.active(1));
    assert(session.level.width == 30);
    assert(session.level.height == 15);
    assert(session.level.environment_profile == 1);
    assert(session.level.music_sequence == 0);
    assert(!session.level_from_direct_save);
    assert(std::all_of(
        session.persistent_event_flags.begin(),
        session.persistent_event_flags.end(),
        [](std::uint8_t value) { return value == 0; }));

    rejected = false;
    try {
        rloft::LevelState too_small{};
        too_small.width = 4;
        too_small.height = 15;
        (void)rloft::ExplorationSessionState::fresh_party(
            rloft::PartyImage::from_bytes(chars),
            std::move(too_small));
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);
}
