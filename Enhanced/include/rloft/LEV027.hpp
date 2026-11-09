#pragma once

#include "rloft/LEV026.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// Moves one original 10-byte CODE.2 control between the two serialized
// control lists for the same column. Only the bytes that encode its runtime
// state may change: kind and map position must remain identical.
// Indices are evaluated after any accompanying Lev026Edits have been staged.
struct Lev027ControlTransfer final {
    Lev019ControlGroup source{Lev019ControlGroup::Primary};
    Lev019ControlGroup destination{Lev019ControlGroup::Secondary};
    int column{};
    std::size_t source_index{};
    std::size_t destination_index{};
    CellControlRecord expected{};
    CellControlRecord replacement{};
};

struct Lev027Edits final {
    Lev026Edits state; // Existing terrain, resource, NPC, script and cache edits.
    std::optional<Lev027ControlTransfer> transfer;
};

// In the uploaded Level 27, a kind-1 control at column 1, row 6 moved from
// the first serialized group to the second; byte 6 changed 0x08 -> 0x00.
// This interface edits that format feature without guessing its game meaning
// or implementing a second direct-save decoder.
class Lev027File final {
public:
    explicit Lev027File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] std::vector<CellControlRecord> controls(
        Lev019ControlGroup group, int column) const;
    [[nodiscard]] std::uint16_t resource_word(
        Lev023ResourceBank bank, std::uint8_t cell_code) const;
    [[nodiscard]] Lev012NpcRecord npc(std::size_t index) const;
    [[nodiscard]] Lev018PlacedRecord placed_object(std::size_t index) const;
    [[nodiscard]] Lev021LooseRecord loose_item(std::size_t index) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t slot) const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] std::uint8_t automap_cell(int column, int row) const;
    [[nodiscard]] std::int16_t serialized_render_floor() const;

    // Source-matched transfer and prior LEV026 edits commit together.
    // Duplicate, stale, out-of-bounds or cross-column transfers fail without
    // modifying live bytes or silently synthesizing automap information.
    void apply(const Lev027Edits& edits);

private:
    Lev026File direct_;
};

} // namespace rloft
