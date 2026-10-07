#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "rloft/exploration_hud_presenter.hpp"
#include "rloft/inventory_screen.hpp"
#include "rloft/object_catalog.hpp"
#include "rloft/party_presentation.hpp"
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
    [[nodiscard]] PartyModalCoverage draw(
        IndexedFramebuffer& framebuffer,
        const PartyImage& party,
        const std::array<bool, 4>& character_mode,
        std::size_t selected_actor) const;

private:
    void draw_res0_local(
        IndexedFramebuffer& framebuffer,
        int local_resource,
        int frame,
        int row,
        int column) const;

    void draw_actor_base(
        IndexedFramebuffer& framebuffer,
        const PartyActorPresentation& actor) const;

    std::size_t draw_inventory(
        IndexedFramebuffer& framebuffer,
        const PartyActorPresentation& actor) const;

    const RavenloftBootAssets& assets_;
    ObjectCatalog objects_;
    ExplorationHudPresenter::PortraitMaps portrait_maps_{};
    std::uint16_t res0_base_{};
    std::uint16_t res1_object_base_{};
    std::uint16_t res1_portrait_base_{};
};

}  // namespace rloft
