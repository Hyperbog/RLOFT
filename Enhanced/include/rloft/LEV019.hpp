#pragma once

#include "rloft/LEV001.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Both original direct-save control groups contain width column lists.
// Their records are opaque CODE.2 10-byte control structures. No second
// parser, world simulation, or speculative interpretation is introduced.
enum class Lev019ControlGroup : std::uint8_t { Primary = 0, Secondary = 1 };

struct Lev019ControlPatch final {
    Lev019ControlGroup group{Lev019ControlGroup::Primary};
    int column{};
    std::vector<CellControlRecord> expected;
    std::vector<CellControlRecord> replacement;
};

class Lev019File final {
public:
    explicit Lev019File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] std::vector<CellControlRecord> controls(
        Lev019ControlGroup group, int column) const;

    // Atomically replaces selected columns in either/both groups. A patch
    // requires the exact prior list, rejects duplicate columns, keeps the
    // one-byte animation state, adjusts all five later section boundaries,
    // and validates the candidate with the existing direct-save decoder.
    void apply_control_patches(const std::vector<Lev019ControlPatch>& patches);

    // Existing editing functions remain owned by the canonical codec.
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    void set_cell_word(int column, int row, std::uint16_t value);
    void set_music_sequence(std::uint8_t resource_id);
    void replace_world_family(LevWorldFamily family,
                              const std::vector<std::uint8_t>& records22);
    void replace_automap_annotations(const std::vector<std::uint8_t>& notes);

private:
    Lev001File direct_;
};

}  // namespace rloft
