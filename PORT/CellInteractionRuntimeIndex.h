#pragma once

#include "games/ravenloft/data/LevelDatabase.h"
#include "games/ravenloft/game/CellInteractionSystem.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace ravenloft::game {

// Reconstructs the CB3C/CD3C per-X runtime interaction lists built by the
// level loader around CODE.2 0x184AF..0x18650 and queried by 0x1E8C8/0x1E930.
class CellInteractionRuntimeIndex {
public:
    explicit CellInteractionRuntimeIndex(const data::LevelRecord& level);

    std::uint16_t width() const { return width_; }
    std::uint16_t height() const { return height_; }

    // CODE.2 0x1E8C8: static list first, then dynamic list.
    CellInteractionRecord* find(int cellX, int cellY);
    const CellInteractionRecord* find(int cellX, int cellY) const;

    // CODE.2 0x1E930: dynamic-list frame code for a linear cell index.
    std::uint16_t dynamicInteractionCode(std::int32_t linearCellIndex) const;

    const std::vector<CellInteractionRecord>& staticColumn(std::size_t x) const {
        return staticColumns_.at(x);
    }
    const std::vector<CellInteractionRecord>& dynamicColumn(std::size_t x) const {
        return dynamicColumns_.at(x);
    }

private:
    std::uint16_t width_ = 0;
    std::uint16_t height_ = 0;
    std::vector<std::vector<CellInteractionRecord>> staticColumns_;
    std::vector<std::vector<CellInteractionRecord>> dynamicColumns_;
};

} // namespace ravenloft::game
