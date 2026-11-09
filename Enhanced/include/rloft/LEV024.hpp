#pragma once

#include "rloft/LEV011.hpp"
#include "rloft/LEV023.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// Indices refer to the pre-edit annotation list. The original CODE.2 label
// bytes are preserved verbatim (CP437, carriage returns, punctuation, etc.).
// null -> value inserts before index, value -> null erases, value -> value
// replaces; index == note count is valid only for an append insertion.
struct Lev024NotePatch final {
    std::size_t index{};
    std::optional<Lev011MapNote> expected;
    std::optional<Lev011MapNote> replacement;
};

struct Lev024Edits final {
    std::vector<Lev024NotePatch> notes;
    std::vector<Lev023ResourcePatch> resources;
    std::vector<Lev019ControlPatch> controls;
    std::vector<Lev012NpcPatch> npcs;
    std::vector<Lev018PlacedPatch> placed;
    std::vector<Lev021LoosePatch> loose;
};

// Level 24 has two original saved automap annotations and four changed
// resource-table bytes. This layer composes existing native LEV011/LEV023
// codecs; it does not define an alternative binary grammar or world model.
class Lev024File final {
public:
    explicit Lev024File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] std::vector<Lev011MapNote> notes() const;
    [[nodiscard]] std::uint16_t resource_word(
        Lev023ResourceBank bank, std::uint8_t cell_code) const;
    [[nodiscard]] Lev012NpcRecord npc(std::size_t index) const;
    [[nodiscard]] Lev018PlacedRecord placed_object(std::size_t index) const;
    [[nodiscard]] Lev021LooseRecord loose_item(std::size_t index) const;
    [[nodiscard]] std::vector<CellControlRecord> controls(
        Lev019ControlGroup group, int column) const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;

    // A single all-or-nothing transaction. Pre-edit expected records and
    // annotations must match byte-exactly; all unrelated bytes survive.
    void apply(const Lev024Edits& edits);
    void set_music_sequence(std::uint8_t resource_id);

private:
    Lev023File direct_;
};

} // namespace rloft
