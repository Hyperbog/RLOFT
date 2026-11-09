#pragma once

#include "rloft/LEV012.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Exact 22-byte CODE.2 direct-save placed-object image. These are mutable
// original runtime bytes, not a substitute for LiveObjectRecord or a new
// world-state model. Every unknown bit is preserved on read/edit/write.
struct Lev018PlacedRecord final {
    static constexpr std::size_t kBytes = 22U;
    std::array<std::uint8_t, kBytes> raw{};

    [[nodiscard]] std::uint32_t y_position_16_16() const noexcept;
    [[nodiscard]] std::uint32_t x_position_16_16() const noexcept;
    [[nodiscard]] std::uint32_t runtime_flags() const noexcept;
    [[nodiscard]] std::uint16_t cached_cell_index() const noexcept;
    [[nodiscard]] std::uint16_t object_type_id() const noexcept;
    [[nodiscard]] std::uint16_t state_word() const noexcept;
    [[nodiscard]] std::uint16_t auxiliary_word() const noexcept;
    [[nodiscard]] std::uint16_t packed_runtime_state() const noexcept;

    void set_position_16_16(std::uint32_t x, std::uint32_t y,
                            std::uint16_t cached_cell) noexcept;
    void set_runtime_flags(std::uint32_t value) noexcept;
    void set_state_word(std::uint16_t value) noexcept;
    void set_auxiliary_word(std::uint16_t value) noexcept;
    void set_packed_runtime_state(std::uint16_t value) noexcept;
};

struct Lev018PlacedPatch final {
    std::size_t index{};
    Lev018PlacedRecord expected{};
    Lev018PlacedRecord replacement{};
};

// Level 18 placed-object authoring boundary, usable for any compatible
// LEVnnn.DAT. Structural decoding and all gameplay remain the existing
// Lev001 -> Lev005 -> Lev010 -> Lev011 -> Lev012 -> ExplorationStateCodec.
class Lev018File final {
public:
    explicit Lev018File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] Lev018PlacedRecord placed_object(std::size_t index) const;
    // Batched, transactional, expected-byte-checked edits. The canonical
    // source parser validates the reconstructed file before any commit.
    // Repositioned objects must stay on the map and have a matching cached
    // cell; untouched positions/opaque flags are never normalized.
    void apply_placed_patches(const std::vector<Lev018PlacedPatch>& patches);

    [[nodiscard]] Lev012NpcRecord npc(std::size_t index) const;
    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t index) const;
    void apply_npc_patches(const std::vector<Lev012NpcPatch>& patches);
    void apply_cell_patches(const std::vector<Lev010CellPatch>& patches);
    void replace_event_slot(std::size_t index, const Lev005EventSlot& slot);
    void set_music_sequence(std::uint8_t res5_id);
    void replace_world_family(LevWorldFamily family,
                              const std::vector<std::uint8_t>& records22);

private:
    [[nodiscard]] std::size_t placed_offset(std::size_t index) const;
    Lev012File direct_;
};

} // namespace rloft
