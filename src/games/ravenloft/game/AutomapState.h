#pragma once
#include "games/ravenloft/data/LevelDatabase.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace ravenloft::game {

struct AutomapNote {
    std::int16_t x=0,y=0;
    std::string text;
};

// Exact persisted map state from LEVELS/LEVxxx section 6. Rendering is deliberately
// separate: these bytes are game state, not an invented modern map representation.
class AutomapState {
public:
    explicit AutomapState(const data::AutomapInitialState& state);
    std::uint16_t width() const { return width_; }
    std::uint16_t height() const { return height_; }
    std::uint8_t cell(std::size_t x,std::size_t y) const;
    bool discovered(std::size_t x,std::size_t y) const { return cell(x,y)!=0xFFu; }
    std::size_t discoveredCount() const;
    const std::vector<AutomapNote>& notes() const { return notes_; }
private:
    std::uint16_t width_=0,height_=0;
    std::vector<std::uint8_t> cells_;
    std::vector<AutomapNote> notes_;
};

} // namespace ravenloft::game
