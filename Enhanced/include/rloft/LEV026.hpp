#pragma once

#include "rloft/LEV025.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// Signed CODE.2 level-header word at offset 0x29A, immediately before the
// music resource byte. A saved zero represents live threshold one after the
// original loader normalizes it; never rewrite zero just by opening a file.
struct Lev026RenderFloorPatch final {
    std::int16_t expected_serialized{};
    std::int16_t replacement_serialized{};
};

struct Lev026Edits final {
    std::optional<Lev026RenderFloorPatch> render_floor;
    Lev025Edits level; // Control columns, NPCs, resources, map and scripts.
};

// LEV026 preserves a saved, normalized renderer threshold and three edited
// kind-3 control records. This layer adds the missing checked header edit;
// the canonical Lev001File/LevelState parser remains the only format owner.
class Lev026File final {
public:
    explicit Lev026File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] std::int16_t serialized_render_floor() const;
    [[nodiscard]] std::int16_t effective_render_floor() const;
    [[nodiscard]] std::vector<CellControlRecord> controls(
        Lev019ControlGroup group, int column) const;
    [[nodiscard]] Lev012NpcRecord npc(std::size_t index) const;
    [[nodiscard]] Lev018PlacedRecord placed_object(std::size_t index) const;
    [[nodiscard]] Lev021LooseRecord loose_item(std::size_t index) const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] std::uint8_t automap_cell(int column, int row) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t slot) const;
    [[nodiscard]] std::uint16_t resource_word(
        Lev023ResourceBank bank, std::uint8_t cell_code) const;
    [[nodiscard]] std::vector<Lev011MapNote> notes() const;

    // One all-or-nothing transaction for signed renderer threshold and any
    // existing LEV025 edits. A stale expected header word or failed world
    // edit never mutates the current file, and unrelated bytes stay intact.
    void apply(const Lev026Edits& edits);
    void set_music_sequence(std::uint8_t resource_id);

private:
    Lev025File direct_;
};

} // namespace rloft
