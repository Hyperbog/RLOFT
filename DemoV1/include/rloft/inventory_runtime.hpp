#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <optional>
#include <string>
#include <string_view>

#include "rloft/exploration_state.hpp"
#include "rloft/game_source.hpp"
#include "rloft/object_catalog.hpp"

namespace rloft {

struct HeldItemState final {
    std::int16_t object_id{-1};
    std::uint8_t known{};
};

struct ContainerRuntimeState final {
    std::uint8_t slot_count{};
    std::uint8_t ui_columns{};
    std::uint8_t capacity_pounds{};
    std::uint8_t ui_x_offset{};
    std::uint8_t subtype{};
    std::uint8_t per_slot_state_bits{};
    std::vector<std::int16_t> items;
};

enum class InventoryContainerStatus : std::uint8_t {
    NoChange,
    Opened,
    Closed,
    Inserted,
    Taken,
    Full,
    TooHeavy,
    Incompatible,
    Unsupported
};

struct OpenInventoryContainer final {
    std::size_t actor{};
    std::size_t hand{};
    std::int16_t object_id{-1};
    std::size_t descriptor_index{};
};

enum class InventoryQuickStashStatus : std::uint8_t {
    NothingHeld,
    Stashed,
    Full,
    Unsupported
};

enum class InventoryHandSwapStatus : std::uint8_t {
    NoChange,
    Swapped,
    Unsupported
};

enum class InventoryEquipmentStatus : std::uint8_t {
    NoChange,
    Swapped,
    WrongSlot,
    ClassRestricted,
    Unsupported
};

enum class InventoryOrdinarySwapStatus : std::uint8_t {
    NoChange,
    Swapped,
    InsertedInContainer,
    ConsumedStoryFragment,
    LearnedMageSpell,
    MageScrollRejected,
    Unsupported
};

// CODE.2 11630 copies six [highID, lowID] DWORD pairs from
// object-1 logical EEB0 and obtains actor Intelligence capacity
// from object-2 DGROUP 6153. The table mapping is not the
// object's spell dispatch enum.
struct MageScrollScribeSource final {
    std::array<std::uint32_t,12> id_ranges{};
    std::array<std::uint8_t,21> intelligence_capacities{};
    // Original CODE.2 object-2 DGROUP pointer literals 6D9C,
    // 6DA0, 6DA4. Do not replace these with translated guesses.
    std::string cannot_understand_text;
    std::string success_suffix;
    std::string already_known_text;

    [[nodiscard]] static MageScrollScribeSource from_code2(
        const std::vector<std::uint8_t>& code2);
};

// Source-order 116B4/11757 and 11717/1176A message branch.
// The game writes 6DAC=1 and 6DA8=30700()+0x36 on BOTH success
// and rejection. This is NOT the combat CF54 +0x5A timer.
enum class MageScribeFeedbackKind : std::uint8_t {
    Scribed, CapacityReached, AlreadyKnown
};
struct MageScribeFeedback final {
    MageScribeFeedbackKind kind{MageScribeFeedbackKind::Scribed};
    std::uint16_t t_spell_name_index{};
    std::string source_text;
    std::uint8_t pending_6dac{1U};
    std::optional<std::uint32_t> deadline_6da8;
    // Only success prefixes resource T[0x42 + bank*10 + slot].
    [[nodiscard]] std::string message(
        std::string_view actual_t_spell_name) const;
};

struct InventoryRuntimeState final {
    static constexpr std::size_t kSerializedContainerCount = 62;
    static constexpr std::size_t kRuntimeDescriptorCount = 63;

    HeldItemState held{};
    std::optional<MageScrollScribeSource> mage_scroll_source;
    std::optional<MageScribeFeedback> mage_scribe_feedback;
    // Original CODE.2 13184..131BB: only unsigned IRQ tick >
    // deadline_6da8 clears 6DAC and 6DA8 and calls F790 to
    // restore the ordinary cursor item description. Equality
    // must NOT expire the message; DWORD wrap is not corrected.
    [[nodiscard]] bool expire_scribe_feedback_13184(
        std::uint32_t observed_irq_tick_30700) noexcept;
    void reset_scribe_feedback_13118() noexcept {
        mage_scribe_feedback.reset();
    }
    std::array<ContainerRuntimeState, kRuntimeDescriptorCount> containers{};
    std::optional<OpenInventoryContainer> open_container;

    [[nodiscard]] static InventoryRuntimeState fresh(
        const GameSource& source);

    // CODE.2 117A0 ordinary-slot/0x12B known-bit swaps.
    // Its 11857 Tome(0x35)+Parchment Fragment(0x61..0x64) branch
    // calls 1C124 to set one bit in the persistent 0x22200 vector,
    // then consumes ONLY the held fragment. Supply the actual
    // session's 32-byte event vector to enable this verified action.
    // Mage Spell Book(0xA6) + held scroll(0xA7..0xC9)
    // enters 11630: event flag 0xBA is set even on a failed
    // capacity/duplicate check; a successful scribe clears
    // precisely the matrix-A spell byte and consumes the scroll.
    // UI dialogue/IRQ scheduling is not claimed here.
    [[nodiscard]] InventoryOrdinarySwapStatus swap_ordinary_slot(
        PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor,
        std::size_t slot,
        std::array<std::uint8_t,32>* persistent_event_flags=nullptr,
        std::optional<std::uint32_t> observed_irq_tick_30700=std::nullopt);

    // CODE.2 211D8 flag 08h generic actor-panel fallback:
    // place a held ordinary object in the first empty slot 0..11,
    // clear the cursor, leave party item-known bits unchanged.
    // Category 0F/0D special paths are intentionally separate.
    [[nodiscard]] InventoryQuickStashStatus quick_stash_held(
        PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor);

    // CODE.2 214B8, pointer flag 08h: swap held item with actor
    // item[14+hand]. Only the ordinary, non-container / non-special
    // exchange is enabled. Clear only this hand's +19E/+1A2 readiness dword.
    // Separate pointer flag 01h invokes *use/attack*, NOT this method.
    [[nodiscard]] InventoryHandSwapStatus swap_hand_slot(
        PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor,
        std::size_t hand);

    // CODE.2 12180..1246E: original category-directed equipment
    // destination, hard-coded QUIVER list, single-class Mage guard,
    // and class-5 armor whitelist. Slot 14/15 delegates 214B8.
    [[nodiscard]] static std::size_t original_equipment_target(
        const ObjectDefinition& definition,std::int16_t object_id,
        std::size_t hand_side);
    [[nodiscard]] InventoryEquipmentStatus swap_equipment_slot(
        PartyImage& party,const ObjectCatalog& objects,
        std::size_t actor,std::size_t slot);

    // Original 21404: source category/slot-type gating and recursive
    // half-pound weight capacity. The container is open only while
    // its object still occupies the chosen party hand slot.
    [[nodiscard]] InventoryContainerStatus open_hand_container(
        const PartyImage& party, const ObjectCatalog& objects,
        std::size_t actor, std::size_t hand);
    void close_container() noexcept { open_container.reset(); }
    [[nodiscard]] InventoryContainerStatus insert_held_into_open_container(
        const PartyImage& party, const ObjectCatalog& objects);
    [[nodiscard]] InventoryContainerStatus take_open_container_slot(
        const PartyImage& party, const ObjectCatalog& objects,
        std::size_t slot);
    // Original 11C3C cell-click semantics: take, insert or exchange
    // through the selected source-descriptor slot and known bit.
    [[nodiscard]] InventoryContainerStatus swap_open_container_slot(
        const PartyImage& party, const ObjectCatalog& objects,
        std::size_t slot);
    [[nodiscard]] InventoryContainerStatus
    insert_held_into_container_object(
        const ObjectCatalog& objects, std::int16_t object_id);
    [[nodiscard]] InventoryContainerStatus put_held_in_container_slot(
        const ObjectCatalog& objects, std::int16_t object_id,
        std::size_t slot);
    [[nodiscard]] std::uint32_t container_weight_half_units(
        const ObjectCatalog& objects,std::size_t descriptor_index) const;
    // Original recursive OBJECTS +0x30 item burden through each of the
    // actor's 21 canonical CHARS slots, including nested containers.
    [[nodiscard]] std::uint32_t actor_weight_half_units(
        const PartyImage& party,const ObjectCatalog& objects,
        std::size_t actor) const;

    [[nodiscard]] bool has_object(
        const PartyImage& party,
        const ObjectCatalog& objects,
        std::int16_t target) const;

    // CODE.2 0F540 ordering: held item first, then each active actor and each
    // of 21 slots, with recursive container removal. Direct party-slot removal
    // intentionally leaves the stale item-known bit because 0F238 is called
    // only after the slot was set to -1 and therefore returns immediately.
    void remove_object(
        PartyImage& party,
        const ObjectCatalog& objects,
        std::int16_t target);
};

}  // namespace rloft
