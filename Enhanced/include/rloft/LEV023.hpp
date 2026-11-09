#pragma once

#include "rloft/LEV005.hpp"
#include "rloft/LEV012.hpp"
#include "rloft/LEV018.hpp"
#include "rloft/LEV019.hpp"
#include "rloft/LEV021.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Original CODE.2 level header contains five separate tables of 64 words.
// The first four select first-person resources; the fifth configures automap
// rendering. Preserve the original words rather than substituting asset IDs.
enum class Lev023ResourceBank : std::uint8_t {
    PrimaryA = 0,
    AlternateA = 1,
    PrimaryB = 2,
    AlternateB = 3,
    AutomapStyle = 4,
};

struct Lev023ResourcePatch final {
    Lev023ResourceBank bank{Lev023ResourceBank::PrimaryA};
    std::uint8_t cell_code{};
    std::uint16_t expected{};
    std::uint16_t replacement{};
};

struct Lev023Edits final {
    std::vector<Lev023ResourcePatch> resources;
    std::vector<Lev019ControlPatch> controls;
    std::vector<Lev012NpcPatch> npcs;
    std::vector<Lev018PlacedPatch> placed;
    std::vector<Lev021LoosePatch> loose;
};

// Native LEV023.DAT authoring surface. Shared codecs remain the only owners
// of the direct-save grammar, world records, and section-offset bookkeeping.
// A batch may change resource mapping, controls and world state together;
// nothing is committed if any expected-byte or source validation fails.
class Lev023File final {
public:
    explicit Lev023File(std::vector<std::uint8_t> source);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] std::uint16_t resource_word(
        Lev023ResourceBank bank, std::uint8_t cell_code) const;
    [[nodiscard]] Lev012NpcRecord npc(std::size_t index) const;
    [[nodiscard]] Lev018PlacedRecord placed_object(std::size_t index) const;
    [[nodiscard]] Lev021LooseRecord loose_item(std::size_t index) const;
    [[nodiscard]] std::vector<CellControlRecord> controls(
        Lev019ControlGroup group, int column) const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t index) const;

    void apply(const Lev023Edits& edits);
    void set_music_sequence(std::uint8_t sequence_id);

private:
    Lev001File direct_;
};

} // namespace rloft
