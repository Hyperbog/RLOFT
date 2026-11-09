#pragma once

#include "rloft/LEV001.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// Complete 22-byte CODE.2 direct-save family-C item instance. This is not an
// OBJECTS.DAT definition. Unidentified state fields stay raw and reversible.
struct Lev021LooseRecord final {
    static constexpr std::size_t kBytes = 22U;
    std::array<std::uint8_t, kBytes> raw{};

    [[nodiscard]] std::uint32_t y_position_16_16() const noexcept;
    [[nodiscard]] std::uint32_t x_position_16_16() const noexcept;
    [[nodiscard]] std::uint32_t runtime_flags() const noexcept;
    [[nodiscard]] std::uint16_t cached_cell_index() const noexcept;
    [[nodiscard]] std::uint16_t object_type_id() const noexcept;
    [[nodiscard]] std::uint16_t runtime_word_10() const noexcept;
    [[nodiscard]] std::uint16_t runtime_word_12() const noexcept;
    [[nodiscard]] std::uint16_t packed_runtime_state() const noexcept;

    void set_position_16_16(std::uint32_t x, std::uint32_t y,
                            std::uint16_t cached_cell) noexcept;
    void set_object_type_id(std::uint16_t id) noexcept;
    void set_runtime_flags(std::uint32_t flags) noexcept;
    void set_runtime_word_10(std::uint16_t value) noexcept;
    void set_runtime_word_12(std::uint16_t value) noexcept;
    void set_packed_runtime_state(std::uint16_t value) noexcept;
};

// Indices refer to the original, pre-edit family-C list. Exactly one edit
// per source index is allowed. Null -> value inserts before that index;
// value -> null erases; value -> value replaces after a byte-exact guard.
// An insertion at index == original count appends the item.
struct Lev021LoosePatch final {
    std::size_t index{};
    std::optional<Lev021LooseRecord> expected;
    std::optional<Lev021LooseRecord> replacement;
};

// The source-backed Lev001File decoder/encoder owns the original LEV grammar.
// LEV021 adds checked grow/shrink/edit operations for *mutable loose items*,
// whose count and ordering differ from the compact LEVELS.DAT baseline.
class Lev021File final {
public:
    explicit Lev021File(std::vector<std::uint8_t> original);
    [[nodiscard]] const LevDirectSummary& summary() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] LevelState decode() const;
    [[nodiscard]] Lev021LooseRecord loose_item(std::size_t index) const;
    void apply_loose_patches(const std::vector<Lev021LoosePatch>& patches);

    [[nodiscard]] std::uint16_t cell_word(int column, int row) const;
    void set_cell_word(int column, int row, std::uint16_t value);
    void set_music_sequence(std::uint8_t resource_id);
    void replace_automap_annotations(const std::vector<std::uint8_t>& blob);

private:
    Lev001File direct_;
};

} // namespace rloft
