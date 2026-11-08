#include "rloft/assassin_relic_transition.hpp"

#include <cassert>

int main() {
    using rloft::AssassinRelicTransition;

    static_assert(AssassinRelicTransition::kAssassinNpcType == 9);
    static_assert(AssassinRelicTransition::kTransitionFrameCount == 22);
    static_assert(AssassinRelicTransition::kLevelChangeFrame == 20);
    static_assert(AssassinRelicTransition::kDestinationLevel == 23);
    static_assert(
        AssassinRelicTransition::kDestinationCoordinateA ==
        0x00098000U);
    static_assert(
        AssassinRelicTransition::kDestinationCoordinateB ==
        0x003A8000U);
    static_assert(
        AssassinRelicTransition::kDestinationFacing == 180);

    assert(AssassinRelicTransition::kDrops[0].screen_seed == 0x6E);
    assert(AssassinRelicTransition::kDrops[0].object_id == 0x88);
    assert(AssassinRelicTransition::kDrops[1].screen_seed == 0x78);
    assert(AssassinRelicTransition::kDrops[1].object_id == 0x87);
    assert(AssassinRelicTransition::kDrops[2].screen_seed == 0x82);
    assert(AssassinRelicTransition::kDrops[2].object_id == 0xE9);
    assert(AssassinRelicTransition::kDrops[3].screen_seed == 0x8C);
    assert(AssassinRelicTransition::kDrops[3].object_id == 0x74);

    // Any relic can be the apparent trigger when it is collected last.
    for (const auto picked : AssassinRelicTransition::kRelics) {
        assert(AssassinRelicTransition::completes_on_pickup(
            picked,
            [picked](std::uint16_t item) {
                return item != picked;
            }));
    }

    assert(!AssassinRelicTransition::completes_on_pickup(
        0x87,
        [](std::uint16_t item) {
            return item != 0x87 && item != 0x88;
        }));
    assert(!AssassinRelicTransition::completes_on_pickup(
        0x42,
        [](std::uint16_t) {
            return true;
        }));

    assert(
        AssassinRelicTransition::consumed_after_transition(0x87));
    assert(
        AssassinRelicTransition::consumed_after_transition(0x88));
    assert(
        AssassinRelicTransition::consumed_after_transition(0x74));
    assert(
        !AssassinRelicTransition::consumed_after_transition(0xE9));
}
