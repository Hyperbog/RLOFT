#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include "rloft/level22_encounter.hpp"

namespace rloft {

// CODE.2 object1 16D..16E loads INIT sound bank words into 258C0/258BC.
// E553 surviving damage: 258C0 + NPCS +41 (creature base);
// E4D7 lethal damage: 258BC + NPCS +43 (death base).
// This is a SOURCE RESOURCE RESOLVER, not a reproduction of the
// DOS priority/channel queue 28110. Host playback remains separate.
class OriginalNpcSoundModel final {
public:
    [[nodiscard]] static std::optional<std::uint16_t> res6_id(
        const std::vector<std::uint8_t>& init,
        const NpcDamageResult& result);
};

} // namespace rloft
