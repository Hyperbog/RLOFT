#pragma once

#include "rloft/LEV001.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// A single original LEV005 event slot: count + five five-byte commands.
// Unused command bytes are opaque and must be preserved, not zero-filled.
struct Lev005EventSlot final {
    static constexpr std::size_t kBytes = 26U;
    static constexpr std::size_t kCommands = 5U;
    std::array<std::uint8_t, kBytes> raw{};

    [[nodiscard]] std::uint8_t active_count() const noexcept {
        return raw[0];
    }
    [[nodiscard]] std::array<std::uint8_t, 5> command(std::size_t ordinal) const;
};

// Level 5's direct-save editing boundary. The file does NOT contain its
// filename/level number; the caller chooses the level identity. All direct
// LEVnnn.DAT structural parsing and serializing stays in Lev001File.
// This facade adds source-checked event-slot edits needed for Level 5.
class Lev005File final {
public:
    explicit Lev005File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t index) const;

    // Transactional: refuse invalid active counts/opcodes before committing
    // a replacement. The original 26-byte slot is replaced in-place and
    // every other script/world/automap byte is retained verbatim.
    void replace_event_slot(std::size_t index, const Lev005EventSlot& slot);
    void set_cell_word(int column, int row, std::uint16_t word);
    void set_music_sequence(std::uint8_t res5_id);
    void replace_world_family(LevWorldFamily family,
                              const std::vector<std::uint8_t>& records22);
    void replace_automap_annotations(const std::vector<std::uint8_t>& blob);

private:
    [[nodiscard]] std::size_t event_offset(std::size_t index) const;
    Lev001File direct_;
};

}  // namespace rloft
