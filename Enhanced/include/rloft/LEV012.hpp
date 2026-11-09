#pragma once

#include "rloft/LEV011.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// A complete original CODE.2 direct-save NPC image, exactly 0x16 bytes.
// These are raw runtime bytes. The canonical gameplay interpretation still
// belongs to LevelWorldStateCodec and the native ExplorationRuntime.
struct Lev012NpcRecord final {
    static constexpr std::size_t kBytes = 22U;
    std::array<std::uint8_t, kBytes> raw{};

    [[nodiscard]] std::uint32_t y_position_16_16() const noexcept;
    [[nodiscard]] std::uint32_t x_position_16_16() const noexcept;
    [[nodiscard]] std::uint32_t runtime_flags() const noexcept;
    [[nodiscard]] std::uint16_t cached_cell_index() const noexcept;
    [[nodiscard]] std::uint16_t npc_type_id() const noexcept;
    [[nodiscard]] std::uint16_t action_state_countdown() const noexcept;
    [[nodiscard]] std::int16_t current_hp() const noexcept;
    [[nodiscard]] std::uint16_t packed_runtime_state() const noexcept;

    // The containing file validates x/y grid bounds and cached cell on commit.
    void set_position_16_16(std::uint32_t x, std::uint32_t y,
                            std::uint16_t cached_cell) noexcept;
    void set_runtime_flags(std::uint32_t flags) noexcept;
    void set_action_state_countdown(std::uint16_t countdown) noexcept;
    void set_current_hp(std::int16_t hp) noexcept;
};

struct Lev012NpcPatch final {
    std::size_t index{};
    Lev012NpcRecord expected{};
    Lev012NpcRecord replacement{};
};

// Editing boundary for original LEV012.DAT, structurally usable for any
// LEVnnn.DAT. No new world loader/renderer: Lev011File -> Lev010File ->
// Lev005File -> Lev001File owns the existing direct-save grammar.
class Lev012File final {
public:
    explicit Lev012File(std::vector<std::uint8_t> original);

    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] Lev012NpcRecord npc(std::size_t index) const;

    // All changes apply in one validated operation; duplicate indexes,
    // stale originals and inconsistent changed positions are rejected
    // before any caller-visible state changes. Unrelated records and bytes
    // (including original dead-NPC flags) remain unchanged.
    void apply_npc_patches(const std::vector<Lev012NpcPatch>& patches);

    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    [[nodiscard]] Lev005EventSlot event_slot(std::size_t index) const;
    [[nodiscard]] std::vector<Lev011MapNote> notes() const;
    void apply_cell_patches(const std::vector<Lev010CellPatch>& patches);
    void replace_event_slot(std::size_t index, const Lev005EventSlot& slot);
    void replace_notes(const std::vector<Lev011MapNote>& notes);
    void set_music_sequence(std::uint8_t res5_id);
    void replace_world_family(LevWorldFamily family,
                              const std::vector<std::uint8_t>& records22);

private:
    [[nodiscard]] std::size_t npc_offset(std::size_t index) const;
    Lev011File direct_;
};

} // namespace rloft
