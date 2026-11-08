#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/level22_quest_playback.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    using rloft::Level22QuestPlayback;
    std::vector<std::string> events;
    const auto done=Level22QuestPlayback::execute(
        [&](std::size_t frame) {
            events.push_back("frame "+std::to_string(frame));
            return true;
        },
        [&](std::uint32_t level) {
            assert(level==23U);
            events.push_back("load 23");
        },
        [&](std::uint16_t object_id) {
            events.push_back("consume "+std::to_string(object_id));
        });
    assert(done);
    assert(events.size()==26U); // 22 draws, one level entry, 3 removals
    assert(events.at(0)=="frame 0");
    assert(events.at(20)=="frame 20");
    assert(events.at(21)=="load 23");
    assert(events.at(22)=="frame 21");
    assert(events.at(23)=="consume 135"); // 0x87
    assert(events.at(24)=="consume 136"); // 0x88
    assert(events.at(25)=="consume 116"); // 0x74
    for(const auto& event:events)assert(event!="consume 233"); // E9

    // Every declined presentation preserves the three quest artifacts
    // in the caller's still-staged inventory and never commits a level.
    for(std::size_t interrupted=0U;interrupted<22U;++interrupted) {
        bool entered=false,consumed=false;
        const bool completed=Level22QuestPlayback::execute(
            [&](std::size_t frame){return frame!=interrupted;},
            [&](std::uint32_t){entered=true;},
            [&](std::uint16_t){consumed=true;});
        assert(!completed && !consumed);
        assert(entered==(interrupted>20U));
    }
    bool rejected=false;
    try {
        (void)Level22QuestPlayback::execute({},[](std::uint32_t){},
            [](std::uint16_t){});
    }catch(const std::invalid_argument&){rejected=true;}
    assert(rejected);
}
