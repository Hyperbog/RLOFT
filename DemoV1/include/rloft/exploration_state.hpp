#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft {

struct PartyImage final {
    static constexpr std::size_t kActorCount = 4;
    static constexpr std::size_t kActorSize = 0x248;
    static constexpr std::size_t kSize = kActorCount * kActorSize;

    std::array<std::uint8_t, kSize> bytes{};

    [[nodiscard]] static PartyImage from_bytes(
        const std::vector<std::uint8_t>& source);
    [[nodiscard]] std::string name(std::size_t actor) const;
    [[nodiscard]] std::uint16_t status_word(std::size_t actor) const;
    [[nodiscard]] bool active(std::size_t actor) const;
};

struct CellControlRecord final {
    std::array<std::uint8_t, 10> raw{};
    [[nodiscard]] std::uint8_t kind() const noexcept { return raw[0]; }
    [[nodiscard]] std::uint8_t control() const noexcept { return raw[1]; }
    [[nodiscard]] std::uint8_t row() const noexcept { return raw[4]; }
    [[nodiscard]] std::uint8_t column() const noexcept { return raw[5]; }
};

enum class LevelStorage : std::uint8_t {
    ArchiveCompact,
    DirectSave,
};

struct LevelState final {
    LevelStorage storage{LevelStorage::ArchiveCompact};
    std::uint8_t writer_unowned_marker{};
    std::array<std::uint32_t, 7> boundaries{};
    std::array<std::uint8_t, 0x2A4> header{};

    int width{};
    int height{};
    std::uint8_t environment_profile{0xFF};
    std::int8_t renderer_vertical_offset{};
    std::uint8_t renderer_fill_base_a{};
    std::uint8_t renderer_fill_base_b{};
    std::int16_t render_threshold_floor{1};
    std::uint8_t music_sequence{};

    std::vector<std::uint16_t> cell_words;
    std::vector<CellControlRecord> group1;
    std::vector<CellControlRecord> group2;
    std::uint8_t active_animation_count{};

    // Retained byte-for-byte but intentionally inert for this milestone.
    // family A contains NPC/creature instances; family B/C contain placed and
    // loose item/world records that later builds will activate.
    std::vector<std::uint8_t> family_a_raw;
    std::vector<std::uint8_t> family_b_raw;
    std::vector<std::uint8_t> family_c_raw;

    std::array<std::uint8_t, 0x514> event_scripts{};
    std::vector<std::uint8_t> automap_cache;
    std::vector<std::uint8_t> automap_annotations;
};

class ExplorationStateCodec final {
public:
    [[nodiscard]] static LevelState parse_archive_level(
        const std::vector<std::uint8_t>& levels_dat,
        int level_id);
    [[nodiscard]] static LevelState parse_direct_level(
        const std::vector<std::uint8_t>& lev_dat);
};

}  // namespace rloft
