#pragma once

#include "rloft/LEV012.hpp"
#include "rloft/LEV018.hpp"
#include "rloft/LEV019.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

struct Lev020CachePatch final {
    int column{};
    int row{};
    std::uint8_t expected{};
    std::uint8_t replacement{};
};
struct Lev020EventPatch final {
    std::size_t slot{};
    Lev005EventSlot expected{};
    Lev005EventSlot replacement{};
};
struct Lev020Edits final {
    std::vector<Lev012NpcPatch> npcs;
    std::vector<Lev018PlacedPatch> placed;
    std::vector<Lev019ControlPatch> controls;
    std::vector<Lev010CellPatch> cells;
    std::vector<Lev020CachePatch> cache;
    std::vector<Lev020EventPatch> events;
};

// LEV020.DAT uses the shared original direct-save format. This surface adds
// transactional, multi-domain edits without a second decoder or world state.
// Opaque bits, inactive event bytes and writer-unowned byte zero survive.
class Lev020File final {
public:
    explicit Lev020File(std::vector<std::uint8_t> source);
    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] Lev012NpcRecord npc(std::size_t index) const;
    [[nodiscard]] Lev018PlacedRecord placed_object(std::size_t index) const;
    [[nodiscard]] std::vector<CellControlRecord> controls(
        Lev019ControlGroup group, int column) const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] std::uint8_t automap_cell(int column, int row) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t slot) const;
    void apply(const Lev020Edits& edits);
private:
    Lev001File direct_;
};

} // namespace rloft
