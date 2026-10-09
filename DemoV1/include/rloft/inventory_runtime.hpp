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

// Runtime CODE.2 DGROUP:6FA0, one raw four-byte record per
// character. This is SAVE state, unlike native open_container.
// Original 10FAC opens through phases 0,1,2,3. Original 110B4
// closes via 3,2,1,0, then writes FF FF 00 FF (NOT all FF).
// Original 21648 may establish phase 3 directly on hand transfer.
struct PartyContainerUiContext final {
    std::array<std::uint8_t,4> raw{{0xFFU,0xFFU,0xFFU,0xFFU}};
    [[nodiscard]] bool active() const noexcept {
        return raw[0]!=0xFFU;
    }
    [[nodiscard]] bool fully_open() const noexcept {
        return active() && raw[2]==3U;
    }
    [[nodiscard]] bool inactive() const noexcept {
        return raw[0]==0xFFU && raw[1]==0xFFU &&
            raw[3]==0xFFU &&
            (raw[2]==0xFFU || raw[2]==0U);
    }
    // The DOS closing loop 110B4 retains a zero phase byte
    // even though the other three bytes return to -1.
    void mark_closed_110b4() noexcept {
        raw={{0xFFU,0xFFU,0U,0xFFU}};
    }
    // Original 10FAC enters with phase zero and 11025 sets
    // each 0..3 frame before calling 10BBC/1A724. A future
    // DOS-renderer binding may drive these steps with actual
    // 0xC050 page flip/draw callbacks. This layer only projects
    // the exact serialized bytes, not those graphical effects.
    void begin_open_10fac(std::uint8_t subtype,
                          std::uint8_t descriptor,
                          std::int16_t object_id) noexcept {
        raw={{subtype,descriptor,0U,
              static_cast<std::uint8_t>(object_id)}};
    }
    [[nodiscard]] bool advance_open_phase_10fac() noexcept {
        if(!active() || raw[2]>=3U)return false;
        ++raw[2];
        return true;
    }
    // 110B4 draws phase 3,2,1,0. Only after phase 0 did
    // 1116F/11176/1117D clear bytes 0,1,3; phase stays 0.
    // Return true while one of those four original frame
    // transitions remains to be consumed.
    [[nodiscard]] bool advance_close_phase_110b4() noexcept {
        if(!active() || raw[2]>3U)return false;
        if(raw[2]>0U) --raw[2];
        else mark_closed_110b4();
        return true;
    }
    // Original 21693..216C4 uses the *low byte* of object_id:
    // IDs >=256 therefore cannot be recovered from 6FA3 alone.
    void mark_open_21693(std::uint8_t subtype,
                          std::uint8_t descriptor,
                          std::int16_t object_id) noexcept {
        raw={{subtype,descriptor,3U,
              static_cast<std::uint8_t>(object_id)}};
    }
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
    std::array<PartyContainerUiContext,PartyImage::kActorCount>
        party_container_ui_contexts{};
    // Four independent source contexts may be concurrently open.
    // This optional is the CURRENT interaction selection only, not
    // a source-backed global owner. Switching actors must not close
    // other characters' 6FA0 panels or mutate their saved records.
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
    void close_actor_container(std::size_t actor) noexcept {
        if(actor>=party_container_ui_contexts.size()) return;
        if(party_container_ui_contexts[actor].active())
            party_container_ui_contexts[actor].mark_closed_110b4();
        if(open_container && open_container->actor==actor)
            open_container.reset();
    }
    void close_container() noexcept {
        if(open_container)
            close_actor_container(open_container->actor);
    }

    // Original 12E70 selects an actor by column/80 and reads that
    // actor's 6FA0 record before calling 11C3C. Resolve a single
    // source-verified hand owner without changing another lane.
    [[nodiscard]] InventoryContainerStatus select_container_actor(
        const PartyImage& party, const ObjectCatalog& objects,
        std::size_t actor);
    [[nodiscard]] InventoryContainerStatus swap_actor_container_slot(
        const PartyImage& party, const ObjectCatalog& objects,
        std::size_t actor, std::size_t slot);

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
