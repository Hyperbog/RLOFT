#pragma once

#include "rloft/LEV020.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Level 22 stores visible/explored cell flags and the automap cache as
// separate mutable fields. Original CODE.2 cache reconstruction has a known
// provenance discrepancy for another LEV022 snapshot; never synthesize cache
// bytes when editing a saved checkpoint. Supply both values explicitly.
struct Lev022ExplorationPatch final {
    int column{};
    int row{};
    std::uint16_t expected_word{};
    std::uint16_t replacement_word{};
    std::uint8_t expected_cache{};
    std::uint8_t replacement_cache{};
};

struct Lev022CheckpointEdits final {
    std::vector<Lev022ExplorationPatch> exploration;
    std::vector<Lev012NpcPatch> npcs;
};

struct Lev022SnapshotSummary final {
    std::size_t cells{};
    std::size_t explored{};             // cell flag 0x0400
    std::size_t currently_visible{};    // cell flag 0x0040
    std::size_t cache_unmapped{};       // serialized cache byte 0xFF
    std::size_t cache_floor_class{};    // serialized cache byte 0x2E
    std::size_t npc_instances{};
    std::size_t placed_instances{};
};

// Adds an atomic, source-checked exploration+NPC checkpoint editing boundary
// over the existing canonical LEVnnn loader. No hardcoded map, actor state,
// original game assets, or duplicate LevelState implementation is introduced.
class Lev022File final {
public:
    explicit Lev022File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] Lev022SnapshotSummary snapshot() const;
    [[nodiscard]] Lev012NpcRecord npc(std::size_t index) const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] std::uint8_t automap_cell(int column, int row) const;

    // All modifications are checked against expected values, validated with
    // the existing codec, and committed together. Only visibility/exploration
    // bits 0x0440 may change in the packed grid words; renderer/topology
    // bits, untouched cache bytes, and opaque original state stay identical.
    void apply_checkpoint(const Lev022CheckpointEdits& edits);

private:
    Lev020File direct_;
};

} // namespace rloft
