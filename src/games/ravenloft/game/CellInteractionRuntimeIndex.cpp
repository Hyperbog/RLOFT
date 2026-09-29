#include "games/ravenloft/game/CellInteractionRuntimeIndex.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
CellInteractionRecord decode(const data::Interaction& in) {
    CellInteractionRecord r{};
    r.type       = in.raw[0];
    r.flags      = in.raw[1];
    r.opaque2    = in.raw[2];
    r.opaque3    = in.raw[3];
    r.cellY      = in.raw[4];
    r.cellX      = in.raw[5];
    r.frame      = in.raw[6];
    r.frameDelta = static_cast<std::int8_t>(in.raw[7]);
    r.opaque8    = in.raw[8];
    r.opaque9    = in.raw[9];
    return r;
}

template<class Columns>
auto findImpl(Columns& staticColumns, Columns& dynamicColumns,
              std::uint16_t width, std::uint16_t height,
              int cellX, int cellY) -> decltype(&staticColumns[0][0]) {
    if (cellX < 0 || cellY < 0 || cellX >= width || cellY >= height)
        return nullptr;

    // 0x1E8E7..0x1E90A. A zero type with flag 0x10 is a valid static
    // record; zero type without that flag is the allocated list terminator.
    // Vectors omit that synthetic terminator, so every stored record is valid.
    for (auto& rec : staticColumns[static_cast<std::size_t>(cellX)]) {
        if (rec.cellY == static_cast<std::uint8_t>(cellY)) return &rec;
    }

    // 0x1E90A..0x1E92C. Dynamic records are zero-terminated in memory;
    // vectors likewise omit the terminator.
    for (auto& rec : dynamicColumns[static_cast<std::size_t>(cellX)]) {
        if (rec.cellY == static_cast<std::uint8_t>(cellY)) return &rec;
    }
    return nullptr;
}
}

CellInteractionRuntimeIndex::CellInteractionRuntimeIndex(const data::LevelRecord& level)
    : width_(level.width), height_(level.height),
      staticColumns_(level.width), dynamicColumns_(level.width) {
    for (const auto& src : level.interactions) {
        auto rec = decode(src);
        if (rec.cellX >= width_ || rec.cellY >= height_)
            throw std::runtime_error("Interaction coordinate outside level dimensions");

        // Loader mode 1 at 0x18528: type 1 records are withheld from CB3C,
        // then regrouped into CD3C by byte +5 (X) at 0x18594..0x18650.
        if (rec.type == 1)
            dynamicColumns_[rec.cellX].push_back(rec);
        else
            staticColumns_.at(src.inputColumn).push_back(rec);
    }
}

CellInteractionRecord* CellInteractionRuntimeIndex::find(int cellX, int cellY) {
    return findImpl(staticColumns_, dynamicColumns_, width_, height_, cellX, cellY);
}
const CellInteractionRecord* CellInteractionRuntimeIndex::find(int cellX, int cellY) const {
    return findImpl(staticColumns_, dynamicColumns_, width_, height_, cellX, cellY);
}

std::uint16_t CellInteractionRuntimeIndex::dynamicInteractionCode(
    std::int32_t linearCellIndex) const {
    // 0x1E930 uses signed idiv by level width. Candidate callers provide
    // nonnegative in-range cell indices; preserve the out-of-range zero return.
    if (linearCellIndex < 0 || width_ == 0) return 0;
    const auto x = linearCellIndex % static_cast<std::int32_t>(width_);
    const auto y = linearCellIndex / static_cast<std::int32_t>(width_);
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return 0;

    for (const auto& rec : dynamicColumns_[static_cast<std::size_t>(x)]) {
        if (rec.cellY == static_cast<std::uint8_t>(y))
            return static_cast<std::uint16_t>(rec.frame) << 12;
    }
    return 0x8000u;
}

} // namespace ravenloft::game
