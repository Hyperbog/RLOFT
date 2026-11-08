#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// LEVELS.DAT is a 32-bit count/offset directory of compact level records.
// A record contains original fixed-header, grid, control lists, compact
// NPC/object families, event bytecode and automap metadata. Mutable LEVnnn
// saves use a *different* 22-byte-family layout and are not handled here.
struct LevelsRecordSummary final {
    int width{};
    int height{};
    std::uint8_t environment_profile{};
    std::uint8_t music_sequence{};
    std::size_t cell_count{};
    std::size_t control_records{};
    std::size_t family_a_npcs{};
    std::size_t family_b_objects{};
    std::size_t family_c_loose_items{};
    std::size_t active_event_slots{};
    std::size_t active_event_commands{};
    std::size_t annotation_bytes{};
};

// Single source of truth for the *archive container* and its serialization.
// Gameplay interpretation remains in ExplorationStateCodec, which reads a
// selected original record through this class. Unknown fields are raw bytes.
class LevelsArchive final {
public:
    explicit LevelsArchive(const std::vector<std::uint8_t>& bytes);
    [[nodiscard]] std::size_t level_count() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> record(std::size_t id) const;
    [[nodiscard]] const LevelsRecordSummary& summary(std::size_t id) const;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;

    // Transactional: reject invalid compact records and offsets without
    // changing the archive, then rebuild directory offsets on serialization.
    void replace_record(std::size_t id, const std::vector<std::uint8_t>& bytes);
    void set_cell_word(std::size_t id, int column, int row,
                       std::uint16_t cell_word);
    void set_music_sequence(std::size_t id, std::uint8_t res5_id);

private:
    [[nodiscard]] static LevelsRecordSummary validate_record(
        const std::vector<std::uint8_t>& bytes);
    std::vector<std::vector<std::uint8_t>> records_;
    std::vector<LevelsRecordSummary> summaries_;
};

}  // namespace rloft
