#pragma once
#include "games/ravenloft/game/AutomapGlyphSelector.h"
#include "games/ravenloft/game/OpeningSequence.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace ravenloft::game {

struct AutomapBounds {
    std::size_t minX=0,minY=0,maxX=0,maxY=0;
    bool valid=false;
};

// UI-neutral automap presentation state. It preserves the exact persisted map
// bytes and CODE.2 glyph selection while leaving pixel rendering to the UI layer.
class AutomapView {
public:
    AutomapView(const AutomapState& map,const PartyState& party);

    std::uint16_t width() const { return width_; }
    std::uint16_t height() const { return height_; }
    const std::vector<std::uint8_t>& glyphs() const { return glyphs_; }
    std::uint8_t glyph(std::size_t x,std::size_t y) const;
    std::size_t partyCellX() const { return partyCellX_; }
    std::size_t partyCellY() const { return partyCellY_; }
    const AutomapBounds& discoveredBounds() const { return bounds_; }

private:
    std::uint16_t width_=0,height_=0;
    std::vector<std::uint8_t> glyphs_;
    std::size_t partyCellX_=0,partyCellY_=0;
    AutomapBounds bounds_{};
};

} // namespace ravenloft::game
