#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/exploration_session.hpp"

#include <algorithm>
#include <cstddef>
#include <cassert>
#include <stdexcept>
#include <utility>
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

    // Fresh CODE.1 -> CHARS -> CODE.2 transfer must preserve BOTH
    // created actors byte for byte: portraits, starting equipment,
    // class/stats and the unowned raw state. SAVE.DAT is NOT a source.
    auto created=chars;
    const auto actor1=rloft::PartyImage::kActorSize;
    std::fill(created.begin()+static_cast<std::ptrdiff_t>(actor1),
              created.begin()+static_cast<std::ptrdiff_t>(actor1+16U),0U);
    created[actor1]='B';
    created[actor1+1U]='E';
    created[actor1+0x10U]=0U;
    created[actor1+0x11U]=0U;
    created[0x12U]=7U;
    created[actor1+0x12U]=9U;
    created[0x40U]=0x2AU;
    created[0x41U]=0U;
    created[actor1+0x40U]=0x3BU;
    created[actor1+0x41U]=0U;
    rloft::LevelState fresh_level{};
    fresh_level.width=30;
    fresh_level.height=15;
    fresh_level.cell_words.assign(450U,0U);
    auto fresh=rloft::ExplorationSessionState::fresh_party(
        rloft::PartyImage::from_bytes(created),
        std::move(fresh_level));
    assert(fresh.party.name(0)=="ANN");
    assert(fresh.party.name(1)=="BE");
    assert(fresh.party.active(0));
    assert(fresh.party.active(1));
    assert(fresh.party.bytes[0x12U]==7U);
    assert(fresh.party.bytes[actor1+0x12U]==9U);
    assert(fresh.party.bytes[0x40U]==0x2AU);
    assert(fresh.party.bytes[actor1+0x40U]==0x3BU);
    assert(std::equal(created.begin(),created.end(),
                      fresh.party.bytes.begin()));
    assert(fresh.level_id==rloft::ExplorationSessionState::kFreshLevelId);
    assert(fresh.coordinate_a==rloft::ExplorationSessionState::kFreshCoordinateA);
    assert(fresh.coordinate_b==rloft::ExplorationSessionState::kFreshCoordinateB);
    assert(fresh.facing==rloft::ExplorationSessionState::kFreshFacing);

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
