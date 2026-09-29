#pragma once
#include "games/ravenloft/data/SaveGame.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace ravenloft::game {

struct AdventureHudCharacter {
    bool active = false;
    std::string name;
    std::uint8_t portrait = 0xFF;
    std::uint16_t status = 0;
    std::uint16_t currentHitPoints = 0;
    std::uint16_t maximumHitPoints = 0;
    std::size_t carriedItemCount = 0;
};

// UI-neutral adventure-screen state.  It deliberately exposes only character
// record fields whose offsets are proven by CODE.1/CODE.2.  Equipment/hand
// slot labels are kept out until their original slot semantics are traced.
class AdventureHudModel {
public:
    explicit AdventureHudModel(const std::array<data::CharacterRecord,4>& party);
    const std::array<AdventureHudCharacter,4>& characters() const { return characters_; }
private:
    std::array<AdventureHudCharacter,4> characters_{};
};

} // namespace ravenloft::game
