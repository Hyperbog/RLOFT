#pragma once
#include "games/ravenloft/game/AutomapState.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace ravenloft::game {

// Faithful automap cell-to-glyph selector recovered from CODE.2 0x09B0.
// Returned values are the original map-glyph indices consumed by the automap
// renderer. 0xFF remains the shipped "undiscovered / draw nothing" sentinel.
class AutomapGlyphSelector {
public:
    static std::uint8_t select(const AutomapState& map,std::size_t x,std::size_t y);
    static std::vector<std::uint8_t> buildGrid(const AutomapState& map);

private:
    static bool ordinaryNeighbor(std::uint8_t v) { return v < 0x20u; }
    static bool specialNeighbor(std::uint8_t v) { return v == 0x2Eu || v == 0x2Fu; }
};

} // namespace ravenloft::game
