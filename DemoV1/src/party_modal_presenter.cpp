#include "rloft/party_modal_presenter.hpp"

#include "rloft/party_hud.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

PartyModalPresenter::PartyModalPresenter(
    const RavenloftBootAssets& assets)
    : assets_(assets),
      objects_(ObjectCatalog::load(assets.source())),
      portrait_maps_(
          ExplorationHudPresenter::portrait_maps_from_code2(
              assets.source().read("CODE.2"))),
      res0_base_(assets.init_u16(0x00)),
      res1_object_base_(assets.init_u16(0x02)),
      res1_portrait_base_(assets.init_u16(0x04)) {}

void PartyModalPresenter::draw_res0_local(
    IndexedFramebuffer& framebuffer,
    int local_resource,
    int frame,
    int row,
    int column) const {
    const auto raster =
        assets_.type1(
            0,
            static_cast<int>(res0_base_) +
                local_resource);
    framebuffer.draw_frame(
        raster,
        frame,
        row,
        column);
}

void PartyModalPresenter::draw_actor_base(
    IndexedFramebuffer& framebuffer,
    const PartyActorPresentation& actor) const {
    const int column =
        static_cast<int>(actor.actor_index) * 0x50;

    if (!actor.active) {
        // 11268 inactive path: 3E510(0,dst,2,2).
        draw_res0_local(
            framebuffer,
            2,
            0,
            0,
            column);
        return;
    }

    // 0FF3C common actor-column base.
    draw_res0_local(
        framebuffer,
        0,
        0,
        0,
        column);

    const auto hud =
        PartyHudModel::panel(
            actor,
            res0_base_,
            res1_portrait_base_,
            0);
    if (hud.portrait_state < 0 ||
        hud.portrait_state >=
            static_cast<int>(
                portrait_maps_.size())) {
        throw std::out_of_range(
            "party modal portrait state");
    }

    framebuffer.draw_frame_mapped(
        assets_.type1(
            1,
            static_cast<int>(
                res1_portrait_base_) +
                actor.portrait_local_index()),
        0,
        0x10,
        column + 3,
        portrait_maps_[
            static_cast<std::size_t>(
                hud.portrait_state)]);

    if (hud.health_bar.drawable()) {
        const auto amount =
            hud.health_bar.scaled_height;
        if (amount > 0x50) {
            framebuffer.fill_rect(
                0x10,
                column + 0x2F,
                0x50,
                4,
                0x1B);
        } else if (amount > 0) {
            framebuffer.fill_rect(
                0x60 - amount,
                column + 0x2F,
                amount,
                4,
                amount > 0x14
                    ? 0x43
                    : 0x57);
        }
    }

    if (actor.condition_effect_flags != 0U) {
        // 0FF3C: local 13h, frame 0 at row 2Ah / column dst+44h.
        draw_res0_local(
            framebuffer,
            0x13,
            0,
            0x2A,
            column + 0x44);
    }

    if ((actor.status_word & 0x1F00U) != 0U) {
        // 0FF3C: local 12h, frame 0 at row 2Ah / column dst+38h.
        draw_res0_local(
            framebuffer,
            0x12,
            0,
            0x2A,
            column + 0x38);
    }
}

std::size_t PartyModalPresenter::draw_inventory(
    IndexedFramebuffer& framebuffer,
    const PartyActorPresentation& actor) const {
    const auto model =
        InventoryScreenModel::build(
            actor,
            objects_,
            res1_object_base_);
    const int column =
        static_cast<int>(actor.actor_index) * 0x50;

    // 1118C fixed inventory chrome.
    draw_res0_local(
        framebuffer,
        1,
        0,
        0x45,
        column + 0x38);
    draw_res0_local(
        framebuffer,
        4,
        0,
        0x128,
        column);

    // 10BBC body/equipment base. The selected local RES0 graphic is
    // sex + side*2 + 5.
    draw_res0_local(
        framebuffer,
        static_cast<int>(actor.sex_code) +
            model.equipment_side * 2 +
            5,
        0,
        0x72,
        column);

    std::size_t mirrored_pending = 0;
    for (const auto& draw : model.draws) {
        if (draw.temporarily_flip_sprite) {
            // 10BBC brackets this copy with external helper 3A258. Its
            // transformation axis is not yet certified in the native port.
            ++mirrored_pending;
            continue;
        }

        const auto raster =
            assets_.type1(
                1,
                draw.absolute_res1_id);

        int draw_column = draw.y;
        if (draw.bottom_align_bias.has_value()) {
            // Despite the legacy model's historical field name this is the
            // second screen coordinate: right-align inside the actor column.
            draw_column =
                column +
                *draw.bottom_align_bias -
                raster.width;
        }

        if (draw.clear_32x16_before_draw) {
            framebuffer.fill_rect(
                draw.x,
                draw_column,
                0x20,
                0x10,
                0);
        }

        framebuffer.draw_frame(
            raster,
            0,
            draw.x,
            draw_column);
    }

    return mirrored_pending;
}

PartyModalCoverage PartyModalPresenter::draw(
    IndexedFramebuffer& framebuffer,
    const PartyImage& party,
    const std::array<bool, 4>& character_mode,
    std::size_t selected_actor) const {
    if (selected_actor >= PartyImage::kActorCount) {
        throw std::out_of_range(
            "party modal selected actor");
    }

    framebuffer.clear(0);

    PartyModalCoverage coverage{};
    for (std::size_t actor_index = 0;
         actor_index < PartyImage::kActorCount;
         ++actor_index) {
        const auto actor =
            PartyPresentationModel::actor(
                party,
                actor_index);
        draw_actor_base(
            framebuffer,
            actor);

        if (!actor.active) {
            continue;
        }
        ++coverage.active_columns;

        // 0FF3C always emits actor name, AC/rank and other text. The native
        // modal text renderer is deliberately tracked separately.
        ++coverage.base_text_columns_pending;

        const int column =
            static_cast<int>(actor_index) *
            0x50;
        if (character_mode[actor_index]) {
            // 10158 begins with local RES0 24h at row 45h, column dst+38h,
            // followed by the class/level/race/sex/stat/HP/XP text families.
            draw_res0_local(
                framebuffer,
                0x24,
                0,
                0x45,
                column + 0x38);
            ++coverage.character_text_columns_pending;
        } else {
            coverage.mirrored_equipment_draws_pending +=
                draw_inventory(
                    framebuffer,
                    actor);
        }
    }

    // 11268: selected party actor receives a 400x80 outline in color 1Ah.
    ExplorationHudPresenter::draw_outline(
        framebuffer,
        0,
        static_cast<int>(selected_actor) *
            0x50,
        0x190,
        0x50,
        0x1A);

    return coverage;
}

}  // namespace rloft
