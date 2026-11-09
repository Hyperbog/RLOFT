#pragma once

#include "rloft/LEV005.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// A checked, positional change to a cell word of a direct LEVnnn.DAT map.
// Expected values protect edits from accidentally changing stale source data.
struct Lev010CellPatch final {
    int column{};
    int row{};
    std::uint16_t expected{};
    std::uint16_t replacement{};
};

// Named editing boundary for the original LEV010.DAT. The file does not
// encode its level number. The original source-backed direct-save grammar,
// world-state decoding and event editor remain owned by LEV001/LEV005.
// This class adds one-pass bulk map edits for the 128x128 retail Level 10.
class Lev010File final {
public:
    explicit Lev010File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] std::uint8_t automap_cell(int column, int row) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t index) const;

    // All patches commit together, after expected-word and full-record
    // checks. Duplicate coordinates, out-of-map cells and stale data fail
    // without changing a single byte. Empty edits are no-ops.
    void apply_cell_patches(const std::vector<Lev010CellPatch>& patches);
    // The automap is a separately serialized width*height byte cache; it
    // must not be silently regenerated or conflated with cell-word flags.
    void replace_automap_cache(const std::vector<std::uint8_t>& cache);
    void replace_event_slot(std::size_t index, const Lev005EventSlot& slot);
    void set_music_sequence(std::uint8_t res5_id);
    void replace_world_family(LevWorldFamily family,
                              const std::vector<std::uint8_t>& records22);
    void replace_automap_annotations(const std::vector<std::uint8_t>& blob);

private:
    [[nodiscard]] std::size_t cell_index(int column, int row) const;
    Lev005File direct_;
};

} // namespace rloft
