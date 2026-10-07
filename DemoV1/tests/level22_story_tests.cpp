#include "rloft/level22_story.hpp"

#include <array>
#include <cassert>
#include <cstdint>

int main() {
    const auto& drops = rloft::Level22Story::assassin_death_drops();
    assert(drops[0].screen_seed == 0x6E);
    assert(drops[0].object_id == 0x88);
    assert(drops[1].screen_seed == 0x78);
    assert(drops[1].object_id == 0x87);
    assert(drops[2].screen_seed == 0x82);
    assert(drops[2].object_id == 0xE9);
    assert(drops[3].screen_seed == 0x8C);
    assert(drops[3].object_id == 0x74);

    const auto& quest = rloft::Level22Story::quest_objects();
    assert(quest[0] == 0x87);
    assert(quest[1] == 0x88);
    assert(quest[2] == 0x74);
    assert(quest[3] == 0xE9);

    for (const auto picked : quest) {
        auto has_all_others = [picked](std::uint16_t id) {
            return id != picked;
        };
        assert(rloft::Level22Story::pickup_completes_set(
            picked, has_all_others));

        bool skipped_one = false;
        auto missing_one_other = [&](std::uint16_t id) {
            if (id == picked) return false;
            if (!skipped_one) {
                skipped_one = true;
                return false;
            }
            return true;
        };
        assert(!rloft::Level22Story::pickup_completes_set(
            picked, missing_one_other));
    }

    assert(!rloft::Level22Story::pickup_completes_set(
        0x01,
        [](std::uint16_t) { return true; }));

    constexpr auto transition = rloft::Level22Story::mist_transition();
    static_assert(transition.kFrameCount == 22);
    static_assert(transition.kLoadFrame == 20);
    static_assert(transition.kFrameResourceOffset == 0x67);
    static_assert(transition.target_level == 23);
    static_assert(transition.coordinate_a == 0x00098000U);
    static_assert(transition.coordinate_b == 0x003A8000U);
    static_assert(transition.facing == 180U);
    static_assert(transition.removed_after_scene[0] == 0x87);
    static_assert(transition.removed_after_scene[1] == 0x88);
    static_assert(transition.removed_after_scene[2] == 0x74);
}
