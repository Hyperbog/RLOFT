#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Original CODE.2 SAVE.DAT file. This is distinct from the native
// EnhancedGameSaveCodec and from mutable LEVnnn.DAT world records.
// Unknown character/effect bytes remain untouched and round-trippable.
struct SaveDatLayout final {
    std::uint32_t transient_count{};
    std::size_t transient_start{};
    std::size_t spell_selection_at{};
    std::size_t gameplay_ticks_at{};
    std::size_t container_at{};
    std::size_t shading_at{};
    std::size_t charges_at{};
    std::size_t roster_at{};
    std::size_t container_ui_at{};
};

struct SaveDatGlobals final {
    std::array<std::uint8_t, 50> description{};
    std::array<std::uint8_t, 32> event_flags{};
    std::uint32_t level{};
    std::uint32_t coordinate_a{};
    std::uint32_t coordinate_b{};
    std::uint32_t facing{};
    std::array<std::uint8_t, 10> shared_effect_state{};
    std::uint32_t interrupt_ticks{};
    std::uint32_t time_of_day_16_16{};
    std::int16_t held_object_id{-1};
    std::uint8_t held_item_known{};
};

struct SaveDatContainer final {
    std::vector<std::int16_t> object_ids;
    std::uint8_t state_bits{};
};

class SaveDat final {
public:
    static constexpr std::size_t kPrefixBytes = 0x77U;
    static constexpr std::size_t kCharacterBytes = 0x248U;
    static constexpr std::size_t kPartyActors = 4U;
    static constexpr std::size_t kRosterActors = 10U;
    static constexpr std::size_t kEffectBytes = 0x16U;
    static constexpr std::uint32_t kMaxEffects = 150U;
    static constexpr std::size_t kEffectCountAt = 0x997U;
    static constexpr std::size_t kZeroEffectFileBytes = 0x2448U;
    static constexpr std::size_t kContainerBytes = 0x3AAU;
    // CODE.2 serialized descriptor bank: indices 0..61. Index 62 is
    // intentionally NOT serialized; preserve the legacy quiver mismatch.
    static constexpr std::array<std::uint8_t, 62> kContainerSlots{{
        4,8,8,8,6,6,8,6,8,8,4,8,8,6,8,4,
        8,4,8,8,8,8,8,8,6,8,8,4,6,8,8,8,
        8,8,8,8,8,8,4,8,8,8,4,8,8,6,6,8,
        8,8,4,8,8,8,6,8,4,6,8,8,6,8
    }};
    using CharacterImage = std::array<std::uint8_t, kCharacterBytes>;
    using TransientImage = std::array<std::uint8_t, kEffectBytes>;
    using ContainerUiImage = std::array<std::uint8_t, 4>;

    explicit SaveDat(std::vector<std::uint8_t> bytes);
    [[nodiscard]] const SaveDatLayout& layout() const noexcept { return layout_; }
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept { return bytes_; }
    [[nodiscard]] std::vector<std::uint8_t> encode() const { return bytes_; }

    [[nodiscard]] SaveDatGlobals globals() const;
    void set_globals(const SaveDatGlobals& globals);
    [[nodiscard]] CharacterImage party(std::size_t actor) const;
    void set_party(std::size_t actor, const CharacterImage& actor_image);
    [[nodiscard]] CharacterImage roster(std::size_t actor) const;
    void set_roster(std::size_t actor, const CharacterImage& actor_image);
    [[nodiscard]] TransientImage transient(std::size_t index) const;
    void replace_transients(const std::vector<TransientImage>& effects);
    [[nodiscard]] std::array<std::uint32_t, 2> selected_spell_characters() const;
    void set_selected_spell_characters(const std::array<std::uint32_t, 2>& ids);
    [[nodiscard]] std::uint32_t gameplay_ticks() const;
    void set_gameplay_ticks(std::uint32_t ticks);
    [[nodiscard]] SaveDatContainer container(std::size_t index) const;
    void set_container(std::size_t index, const SaveDatContainer& value);
    [[nodiscard]] std::uint32_t environment_shading_level() const;
    [[nodiscard]] std::uint32_t level_light_adjustment() const;
    void set_environment_controls(std::uint32_t shading, std::uint32_t adjustment);
    [[nodiscard]] std::array<std::uint8_t, 15> charged_items() const;
    void set_charged_items(const std::array<std::uint8_t, 15>& counts);
    [[nodiscard]] ContainerUiImage container_ui(std::size_t actor) const;
    void set_container_ui(std::size_t actor, const ContainerUiImage& value);

private:
    [[nodiscard]] static SaveDatLayout verify(const std::vector<std::uint8_t>& bytes);
    [[nodiscard]] std::size_t container_offset(std::size_t index) const;
    std::vector<std::uint8_t> bytes_;
    SaveDatLayout layout_{};
};

} // namespace rloft
