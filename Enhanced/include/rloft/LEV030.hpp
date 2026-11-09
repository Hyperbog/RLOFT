#pragma once

#include "rloft/LEV027.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Original CODE.2 positions are 16.16 fixed-point, including fractional
// within-cell offsets. Indices identify an instance, not its object type.
struct Lev030NpcPositionPatch final {
    std::size_t index{};
    Lev012NpcRecord expected{};
    std::uint32_t replacement_x_16_16{};
    std::uint32_t replacement_y_16_16{};
};

struct Lev030LoosePositionPatch final {
    std::size_t index{};
    Lev021LooseRecord expected{};
    std::uint32_t replacement_x_16_16{};
    std::uint32_t replacement_y_16_16{};
};

struct Lev030Edits final {
    Lev027Edits state; // Existing resource, control, renderer, map, script edits.
    std::vector<Lev030NpcPositionPatch> npc_positions;
    std::vector<Lev030LoosePositionPatch> loose_positions;
};

// Level 30 has 17 identical type-164 item IDs but independent fractional
// positions and one live NPC. Preserve every opaque runtime byte and let the
// established Lev012/Lev021 codecs validate and serialize the full records.
class Lev030File final {
public:
    explicit Lev030File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] Lev012NpcRecord npc(std::size_t index) const;
    [[nodiscard]] Lev021LooseRecord loose_item(std::size_t index) const;
    [[nodiscard]] std::vector<CellControlRecord> controls(
        Lev019ControlGroup group, int column) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t slot) const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] std::uint8_t automap_cell(int column, int row) const;
    [[nodiscard]] std::int16_t serialized_render_floor() const;

    // Source-checked batch movement derives the cached cell from the full
    // fixed-point coordinates. Duplicates, stale images and off-map positions
    // fail atomically, including any other requested Lev027 edits.
    void apply(const Lev030Edits& edits);

private:
    Lev027File direct_;
};

} // namespace rloft
