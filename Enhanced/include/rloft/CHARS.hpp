#pragma once

#include "rloft/exploration_state.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft {

// Typed, read-only projection of the confirmed prefix of one original 0x248
// character record. The authoritative bytes stay in PartyImage, including
// opaque fields, spell matrices, timers and legacy sentinel values.
struct CharsAbilityPair final {
    std::uint8_t base{};
    std::uint8_t current{};
};

struct CharsCharacterCore final {
    std::string name;
    std::uint16_t status{};
    bool occupied{}; // !(status & 0x8000); death (0x4000) is independent.
    bool dead{};
    std::uint8_t portrait{};
    std::uint8_t sex{};
    std::uint8_t race{};
    std::array<std::int8_t, 3> class_codes{};
    std::uint16_t age{};
    std::uint8_t alignment{};
    std::int8_t base_armor_class{};
    std::int16_t current_hp{};
    std::int16_t max_hp{};
    std::array<std::int16_t, 3> class_levels{};
    std::array<std::int32_t, 3> class_progress{};
    std::array<CharsAbilityPair, 7> abilities{}; // sixth + exceptional STR
    std::array<std::int16_t, 21> items{};
};

// CHARS is a transient CODE.1 -> CODE.2 party handoff, NOT a SAVE.DAT.
// It is exactly four packed records (4 * 0x248 = 0x920 bytes), with no
// leading count, header or checksum. Never use compiler struct packing.
class CharsCodec final {
public:
    [[nodiscard]] static PartyImage decode(const std::vector<std::uint8_t>& file);
    [[nodiscard]] static std::vector<std::uint8_t> encode(const PartyImage& image);
    [[nodiscard]] static CharsCharacterCore core(const PartyImage& image,
                                                 std::size_t actor);

    // In-place, field-limited edits on the same PartyImage used by gameplay.
    // Opaque original bytes and all other party records remain unchanged.
    static void set_status(PartyImage& image, std::size_t actor,
                           std::uint16_t status);
    static void set_current_hp(PartyImage& image, std::size_t actor,
                               std::int16_t hp);
    static void set_item(PartyImage& image, std::size_t actor,
                         std::size_t slot, std::int16_t object_id);
};

} // namespace rloft
