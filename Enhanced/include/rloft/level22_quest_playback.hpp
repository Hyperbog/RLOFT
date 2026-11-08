#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>

#include "rloft/assassin_relic_transition.hpp"

namespace rloft {

// Original 21F70..223DE quest frame/action ORDER, backed by a
// STAGED game state owned by the caller. The presenter callbacks
// supply an enhanced PC visualization, not original DOS 2E510 art.
// This executes no mutation when frame presentation declines.
class Level22QuestPlayback final {
public:
    using PresentFrame = std::function<bool(std::size_t)>;
    using EnterLevel = std::function<void(std::uint32_t)>;
    using ConsumeRelic = std::function<void(std::uint16_t)>;

    // Return false ONLY on declined presentation. Callbacks which throw
    // must be run against caller-owned staged/rollback-capable data.
    [[nodiscard]] static bool execute(
        const PresentFrame& present,
        const EnterLevel& enter,
        const ConsumeRelic& consume);
};

} // namespace rloft
