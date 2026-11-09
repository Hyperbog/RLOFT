#pragma once

#include "rloft/LEV010.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// CODE.2 automap annotation record: signed placement coordinates and a
// length-prefixed byte label. Coordinates are *not* map-cell indices;
// retail LEV011 has x=1033, negative positions and embedded CR bytes.
// Preserve raw CP437/text bytes, including "//" and embedded line breaks.
struct Lev011MapNote final {
    std::int16_t x{};
    std::int16_t y{};
    std::vector<std::uint8_t> label;
};

// A Level 11-specific, reversible map-label editor. The file format itself
// is shared by all LEVnnn.DAT files; parsing and runtime state belong to
// Lev001File/Lev005File/Lev010File and ExplorationStateCodec.
class Lev011File final {
public:
    explicit Lev011File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] std::vector<Lev011MapNote> notes() const;

    // All note edits are transactional and retain every unrelated source
    // byte, including saved object positions, renderer cache and scripts.
    void replace_notes(const std::vector<Lev011MapNote>& notes);
    void set_note(std::size_t index, const Lev011MapNote& note);
    void insert_note(std::size_t index, const Lev011MapNote& note);
    void erase_note(std::size_t index);

    // Existing direct-level editing APIs, delegated without a second codec.
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] std::uint8_t automap_cell(int column, int row) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t index) const;
    void apply_cell_patches(const std::vector<Lev010CellPatch>& patches);
    void replace_automap_cache(const std::vector<std::uint8_t>& cache);
    void replace_event_slot(std::size_t index, const Lev005EventSlot& slot);
    void set_music_sequence(std::uint8_t res5_id);
    void replace_world_family(LevWorldFamily family,
                              const std::vector<std::uint8_t>& records22);

private:
    Lev010File direct_;
};

} // namespace rloft
