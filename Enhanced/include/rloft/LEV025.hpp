#pragma once

#include "rloft/LEV020.hpp"
#include "rloft/LEV024.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Original packed grid word, with its low six bits holding the cell code.
// Only the terrain code is replaced. Other persisted cell flags, including
// exploration, visibility and trigger state, survive bit-for-bit.
struct Lev025TerrainPatch final {
    int column{};
    int row{};
    std::uint16_t expected_word{};
    std::uint8_t replacement_code{}; // CODE.2 cell-code range 0..63.
};

struct Lev025Edits final {
    Lev024Edits world; // Resource words, controls, NPCs, placed/loose, notes.
    std::vector<Lev025TerrainPatch> terrain;
    std::vector<Lev020CachePatch> cache;
    std::vector<Lev020EventPatch> events;
};

// Source-preserving LEV025 editing boundary. Uses the same direct-level
// LevelState and original format codecs as LEV001..LEV024. In particular,
// its automap cache is independently serialized and is never regenerated
// when terrain changes. Runtime event-command bytes are not normalized.
class Lev025File final {
public:
    explicit Lev025File(std::vector<std::uint8_t> source);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] std::uint8_t automap_cell(int column, int row) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t slot) const;
    [[nodiscard]] std::uint16_t resource_word(
        Lev023ResourceBank bank, std::uint8_t cell_code) const;
    [[nodiscard]] Lev012NpcRecord npc(std::size_t index) const;
    [[nodiscard]] Lev018PlacedRecord placed_object(std::size_t index) const;
    [[nodiscard]] Lev021LooseRecord loose_item(std::size_t index) const;
    [[nodiscard]] std::vector<CellControlRecord> controls(
        Lev019ControlGroup group, int column) const;
    [[nodiscard]] std::vector<Lev011MapNote> notes() const;

    // Checks source words, resource values, record images and event slots.
    // Stages all changes before assigning to the live file, including
    // variable-length loose-item, control and annotation sections.
    void apply(const Lev025Edits& edits);
    void set_music_sequence(std::uint8_t resource_id);

private:
    Lev024File direct_;
};

} // namespace rloft
