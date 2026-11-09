#pragma once

#include "rloft/exploration_state.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Native editor/reader for original, mutable LEVnnn.DAT level saves.
// Named after the authenticated LEV001.DAT input; the format and class
// intentionally work for ALL numbered LEV saves, including LEV022.DAT.
// Compact LEVELS.DAT records are owned by LEVELS.cpp, not this module.
enum class LevWorldFamily : std::uint8_t { Npcs = 0, Placed = 1, Loose = 2 };

struct LevDirectSummary final {
    int width{};
    int height{};
    std::uint8_t music_sequence{};
    std::uint8_t writer_unowned_byte{};
    std::uint8_t active_animation_count{};
    std::array<std::uint32_t, 7> boundaries{};
    std::size_t group1_controls{};
    std::size_t group2_controls{};
    std::array<std::size_t, 3> world_counts{};
    std::size_t active_event_slots{};
    std::size_t active_event_commands{};
    std::size_t annotation_bytes{};
};

class Lev001File final {
public:
    explicit Lev001File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept { return summary_; }
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept { return bytes_; }
    [[nodiscard]] std::vector<std::uint8_t> encode() const { return bytes_; }
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;

    // All modifications are transactional: the existing source-based level
    // decoder validates the candidate before it replaces the retained bytes.
    void set_cell_word(int column, int row, std::uint16_t word);
    void set_music_sequence(std::uint8_t res5_index);
    void replace_world_family(LevWorldFamily family,
                              const std::vector<std::uint8_t>& records22);
    void replace_automap_annotations(const std::vector<std::uint8_t>& blob);

private:
    static LevDirectSummary inspect(const std::vector<std::uint8_t>& candidate);
    void accept(std::vector<std::uint8_t> candidate);
    [[nodiscard]] std::size_t grid_word_at(int column, int row) const;

    std::vector<std::uint8_t> bytes_;
    LevDirectSummary summary_{};
};

}  // namespace rloft
