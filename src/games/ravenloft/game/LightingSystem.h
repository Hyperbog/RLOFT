#pragma once
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/TrigTable.h"
#include <array>
#include <cstdint>
#include <vector>

namespace ravenloft::game {

struct LightingState {
    std::uint16_t hour = 0;
    std::uint8_t skyShade = 0;                 // CODE.2 DS:F918
    std::int32_t baseDarkness = 0;             // DS:F920 after hourly table lookup
    std::int32_t ambientModifier = 0;           // DS:F91C
    std::int32_t partyLightOverride = 0;        // return from CODE.2 0x31E74
    std::array<std::uint8_t,128> floorShade{}; // DS:FE15..FE94 in draw order
    std::vector<std::uint16_t> distanceRemap;  // DS:258C4, 509 WORDs for shipped SINTAB
};

class LightingSystem {
public:
    explicit LightingSystem(const data::TrigTable& trig):trig_(trig){}

    // Reconstructs the live outdoor light state selected by CODE.2 0x31F94 and
    // the shade/remap tables built by 0x31A14. The save supplies the clock and
    // the two user/game modifiers exactly as the original loader restores them.
    LightingState fromQuickStart(const data::QuickStartSave& save) const;
    LightingState build(std::uint32_t clock,
                        std::int32_t savedBaseDarkness,
                        std::int32_t ambientModifier,
                        const std::array<data::CharacterRecord,4>& party) const;

    static std::int32_t partyLightOverride(const std::array<data::CharacterRecord,4>& party);
    static std::int32_t hourlyDarkness(std::uint16_t hour);

private:
    const data::TrigTable& trig_;
};

}
