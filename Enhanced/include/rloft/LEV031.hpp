#pragma once

#include "rloft/LEV027.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// The retail Level 31 save has four three-entry resource-bank cycles at
// cell codes 60, 61, and 62. The editing rule is generic for any distinct
// three-code cycle, retaining all unrelated original header fields.
struct Lev031ResourceCycle final {
    Lev023ResourceBank bank{Lev023ResourceBank::PrimaryA};
    std::array<std::uint8_t, 3> cell_codes{};
    std::array<std::uint16_t, 3> expected{};
    bool rotate_right{true};
};

struct Lev031Edits final {
    Lev027Edits state; // Checked NPC, object, control-transfer, map, script edits.
    std::vector<Lev031ResourceCycle> resource_cycles;
};

// Uses the established LEV001..LEV030 direct-save codec and resource table
// editor. Neither the binary format nor live game state is decoded twice.
// A failed resource cycle or nested state edit cannot change the source.
class Lev031File final {
public:
    explicit Lev031File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] std::uint16_t resource_word(
        Lev023ResourceBank bank, std::uint8_t cell_code) const;
    [[nodiscard]] std::array<std::uint16_t, 3> resource_triplet(
        Lev023ResourceBank bank, const std::array<std::uint8_t, 3>& codes) const;
    [[nodiscard]] std::vector<CellControlRecord> controls(
        Lev019ControlGroup group, int column) const;
    [[nodiscard]] Lev012NpcRecord npc(std::size_t index) const;
    [[nodiscard]] Lev018PlacedRecord placed_object(std::size_t index) const;
    [[nodiscard]] Lev021LooseRecord loose_item(std::size_t index) const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] std::uint8_t automap_cell(int column, int row) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t slot) const;

    void apply(const Lev031Edits& edits);

private:
    Lev027File direct_;
};

} // namespace rloft
