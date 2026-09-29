#pragma once

#include "games/ravenloft/data/InitDatabase.h"
#include <array>
#include <cstdint>

namespace ravenloft::game {

struct AdventureInterfaceResources {
    std::uint8_t theme = 0;
    std::uint16_t workspacePrimary = 0;
    std::uint16_t workspaceSecondary = 0;
    std::uint16_t widgets = 0;
};

// Recovered from CODE.2 data object DS:0x81AC.  The executable indexes this
// table as 3 * level and passes byte 0 to CODE.2 0x1F8E0.  These values are
// executable behavior, not a replacement for game-authored LEVELS.DAT data.
class AdventureInterfaceTheme {
public:
    static std::uint8_t themeForLevel(std::uint8_t level);
    static AdventureInterfaceResources resourcesForLevel(
        std::uint8_t level,
        const ravenloft::data::InitDatabase& init);
};

}
