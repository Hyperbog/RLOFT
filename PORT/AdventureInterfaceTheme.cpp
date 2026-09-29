#include "games/ravenloft/game/AdventureInterfaceTheme.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
// Full three-byte records are preserved here even though only byte 0 is
// currently consumed by the interface-theme selector.  Keeping the recovered
// records intact prevents us from silently losing executable evidence needed
// when the remaining two fields are implemented later.
constexpr std::array<std::array<std::uint8_t,3>,32> kLevelThemeRecords{{
    {{4,10,3}}, {{4,10,3}}, {{4,10,3}}, {{2,10,3}},
    {{2,10,3}}, {{3,10,3}}, {{3,10,3}}, {{2,10,3}},
    {{4,0,0}},  {{2,10,3}}, {{3,10,3}}, {{3,0,0}},
    {{2,10,3}}, {{2,10,3}}, {{2,10,3}}, {{4,10,3}},
    {{4,0,0}},  {{4,0,0}},  {{3,0,0}},  {{4,10,3}},
    {{3,10,3}}, {{3,10,3}}, {{4,0,0}},  {{3,0,0}},
    {{3,0,0}},  {{4,0,0}},  {{4,0,0}},  {{3,10,3}},
    {{2,10,3}}, {{0,10,3}}, {{3,10,3}}, {{4,0,0}}
}};
}

std::uint8_t AdventureInterfaceTheme::themeForLevel(std::uint8_t level) {
    if (level >= kLevelThemeRecords.size()) throw std::out_of_range("Ravenloft level interface theme");
    return kLevelThemeRecords[level][0];
}

AdventureInterfaceResources AdventureInterfaceTheme::resourcesForLevel(
    std::uint8_t level,
    const ravenloft::data::InitDatabase& init) {
    const auto theme=themeForLevel(level);
    const auto base=init.interfaceResourceBase();
    const auto offset=static_cast<std::uint16_t>(3u*theme);
    return {
        theme,
        static_cast<std::uint16_t>(base+1u+offset),
        static_cast<std::uint16_t>(base+2u+offset),
        static_cast<std::uint16_t>(base+3u+offset)
    };
}

}
