#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "rloft/character_sheet.hpp"
#include "rloft/character_modal_stats.hpp"
#include "rloft/exploration_hud_presenter.hpp"
#include "rloft/indexed_font_presenter.hpp"
#include "rloft/inventory_screen.hpp"
#include "rloft/inventory_runtime.hpp"
#include "rloft/inventory_item_description.hpp"
#include "rloft/original_f790_runtime_description.hpp"
#include "rloft/inventory_encumbrance.hpp"
#include "rloft/object_catalog.hpp"
#include "rloft/party_presentation.hpp"
#include "rloft/party_panel_controls.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

struct PartyModalCoverage final {
    std::size_t active_columns{};
    std::size_t character_text_columns_pending{};
    std::size_t base_text_columns_pending{};
    std::size_t mirrored_equipment_draws_pending{};

    [[nodiscard]] bool complete() const noexcept {
        return character_text_columns_pending == 0 &&
               base_text_columns_pending == 0 &&
               mirrored_equipment_draws_pending == 0;
    }
};

struct InventoryPointer final {
    int row{};
    int column{};
};

class PartyModalPresenter final {
public:
    explicit PartyModalPresenter(
        const RavenloftBootAssets& assets);

    // Faithful read-only portion of CODE.2 112EC/11268:
    // four 80-pixel actor columns, common 0FF3C actor base, and either
    // 1118C/10BBC inventory/equipment graphics or 10158 character chrome.
    //
    // Text emitted by 0FF3C/10158 and the 3A258 mirrored-equipment transform
    // remain explicitly coverage-bounded rather than approximated.
    struct EquipmentHit final {
        std::size_t actor{};
        std::size_t slot{};
    };
    // Equipment click targeting: CODE.2 12180 category routing when
    // holding an object; for an empty cursor, exact race-selected
    // original 12C00 ten-descriptor table, not sprite-dimension guesses.
    [[nodiscard]] std::optional<EquipmentHit> equipment_hit_at(
        const PartyImage& party,
        const InventoryRuntimeState& inventory,
        int row,int column) const;

    [[nodiscard]] PartyModalCoverage draw(
        IndexedFramebuffer& framebuffer,
        const PartyImage& party,
        const std::array<bool, 4>& character_mode,
        std::size_t selected_actor,
        const InventoryRuntimeState* inventory=nullptr,
        std::optional<InventoryPointer> pointer=std::nullopt,
        std::optional<std::size_t> encumbrance_actor=std::nullopt) const;

private:
    // CODE.2 slot 2 maps directly to the original RES2 archive:
    // no INIT RES0 namespace offset is applied to these entry indices.
    void draw_res2_local(
        IndexedFramebuffer& framebuffer,
        int resource,
        int frame,
        int row,
        int column) const;

    // Original 2E510 draws every frame; 2975C draws a selected
    // frame. Both resolve actual original RES2 archive indices.
    void draw_res2_complete(
        IndexedFramebuffer& framebuffer,
        int resource,
        int row,
        int column) const;

    void draw_actor_base(
        IndexedFramebuffer& framebuffer,
        const PartyActorPresentation& actor) const;

    std::size_t draw_inventory(
        IndexedFramebuffer& framebuffer,
        const PartyActorPresentation& actor) const;

    void draw_character_centered_text(
        IndexedFramebuffer& framebuffer,
        const CharacterSheet& sheet) const;

    void draw_encumbrance_panel(
        IndexedFramebuffer& framebuffer,
        const PartyImage& party,
        const InventoryRuntimeState& inventory,
        std::size_t actor) const;
    void draw_open_container(
        IndexedFramebuffer& framebuffer,
        const InventoryRuntimeState& inventory,
        const std::array<bool,4>& character_mode,
        std::size_t actor) const;
    void draw_held_cursor_and_description(
        IndexedFramebuffer& framebuffer,
        const InventoryRuntimeState& inventory,
        std::optional<InventoryPointer> pointer) const;

    const RavenloftBootAssets& assets_;
    ObjectCatalog objects_;
    // Immutable source bytes for CODE.2 F790 text reconstruction;
    // inventory/container contents themselves remain in live InventoryRuntimeState.
    std::vector<std::uint8_t> original_objects_bytes_{};
    OriginalF790StaticResources original_code2_descriptions_;
    OriginalTTextArchive original_t_descriptions_;
    ExplorationHudPresenter::PortraitMaps portrait_maps_{};
    std::uint16_t res0_base_{};
    std::uint16_t res1_object_base_{};
    std::uint16_t res1_portrait_base_{};
    Type2Font font1_{};
    CharacterExperienceTable experience_table_{};
    InventoryEncumbranceModel encumbrance_{};
    OriginalEquipmentHitTables equipment_hits_{};
};

}  // namespace rloft
