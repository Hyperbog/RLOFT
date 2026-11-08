#include "rloft/level22_quest_playback.hpp"
#include <stdexcept>

namespace rloft {

bool Level22QuestPlayback::execute(
    const PresentFrame& present,
    const EnterLevel& enter,
    const ConsumeRelic& consume) {
    if(!present || !enter || !consume)
        throw std::invalid_argument(
            "Level22 quest needs frame, level and inventory handlers");
    for(std::size_t frame=0U;
        frame<AssassinRelicTransition::kTransitionFrameCount;
        ++frame) {
        // Original 22359 runs its frame action before frame-20
        // 181B4 enters the next level. Frame21 is the NEW level.
        if(!present(frame))return false;
        if(frame==AssassinRelicTransition::kLevelChangeFrame)
            enter(AssassinRelicTransition::kDestinationLevel);
    }
    // Original 223B5/223C2/223CF happen AFTER the final frame.
    // The parchment (E9) deliberately remains with the party.
    for(auto relic:AssassinRelicTransition::kConsumedAfterTransition)
        consume(relic);
    return true;
}

} // namespace rloft
