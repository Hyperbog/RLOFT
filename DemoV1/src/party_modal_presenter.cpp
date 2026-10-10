#include "rloft/party_modal_presenter.hpp"

#include "rloft/party_hud.hpp"
#include "rloft/original_party_modal_blit.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <stdexcept>

namespace rloft {

PartyModalPresenter::PartyModalPresenter(
    const RavenloftBootAssets& assets)
    : assets_(assets),
      objects_(ObjectCatalog::load(assets.source())),
      original_objects_bytes_(assets.source().read("OBJECTS.DAT")),
      original_code2_descriptions_(assets.source().read("CODE.2")),
      original_t_descriptions_(assets.source().read("T")),
      portrait_maps_(
          ExplorationHudPresenter::portrait_maps_from_code2(
              assets.source().read("CODE.2"))),
      res0_base_(assets.init_u16(0x00)),
      res1_object_base_(assets.init_u16(0x02)),
      res1_portrait_base_(assets.init_u16(0x04)),
      font1_(assets.res0_font(1)),
      experience_table_(
          CharacterExperienceTable::from_code2(
              assets.source().read("CODE.2"))),
      encumbrance_(
          InventoryEncumbranceModel::from_code2(
              assets.source().read("CODE.2"))),
      equipment_hits_(OriginalEquipmentHitTables::from_code2(
          assets.source().read("CODE.2"))) {}

void PartyModalPresenter::draw_res2_local(
    IndexedFramebuffer& framebuffer,
    int resource,
    int frame,
    int row,
    int column) const {
    // CODE.2 2975C -> 2F29C(item,slot); slot 2 is RES2,
    // and item is an absolute RES2 entry, not RES0+INIT[0].
    const auto raster=assets_.type1(2,resource);
    OriginalPartyModalBlit::frame_2975c(
        framebuffer,raster,frame,row,column);
}

void PartyModalPresenter::draw_res2_complete(
    IndexedFramebuffer& framebuffer,
    int resource,
    int row,
    int column) const {
    // CODE.2 2E510 -> 2F29C(resource,slot=2) -> 2E4A4
    // iterates the complete RES2 graphic descriptor.
    const auto raster=assets_.type1(2,resource);
    OriginalPartyModalBlit::complete_2e510(
        framebuffer,raster,row,column);
}

void PartyModalPresenter::draw_actor_base(
    IndexedFramebuffer& framebuffer,
    const PartyActorPresentation& actor) const {
    const int column =
        static_cast<int>(actor.actor_index) * 0x50;

    if (!actor.active) {
        // 11268 inactive path: 3E510(0,dst,2,2).
        draw_res2_complete(
            framebuffer,
            2,
            0,
            column);
        return;
    }

    // 0FF3C common actor-column base.
    draw_res2_complete(
        framebuffer,
        0,
        0,
        column);

    // 0FF3C -> 40508(2,dst,50h,name,1): centered actor name.
    IndexedFontPresenter::draw_centered(
        framebuffer,
        font1_,
        2,
        column,
        0x50,
        actor.name);

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
        draw_res2_local(
            framebuffer,
            0x13,
            0,
            0x2A,
            column + 0x44);
    }

    if ((actor.status_word & 0x1F00U) != 0U) {
        // 0FF3C: local 12h, frame 0 at row 2Ah / column dst+38h.
        draw_res2_local(
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
    draw_res2_local(
        framebuffer,
        1,
        0,
        0x45,
        column + 0x38);
    draw_res2_complete(
        framebuffer,
        4,
        0x128,
        column);

    // 10BBC body/equipment base. The selected original RES2 graphic is
    // sex + side*2 + 5.
    draw_res2_complete(
        framebuffer,
        static_cast<int>(actor.sex_code) +
            model.equipment_side * 2 +
            5,
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

void PartyModalPresenter::draw_character_centered_text(
    IndexedFramebuffer& framebuffer,
    const CharacterSheet& sheet) const {
    const int column =
        static_cast<int>(sheet.actor_index) * 0x50;

    // Every line here is emitted through app22_draw_40508 in CODE.2 10158:
    // row, actor-column base, 50h span, and font selector 1.
    IndexedFontPresenter::draw_centered(
        framebuffer, font1_,
        0x126, column, 0x50,
        assets_.text(0x3A));
    IndexedFontPresenter::draw_centered(
        framebuffer, font1_,
        0x15B, column, 0x50,
        assets_.text(0x3B));

    std::string classes;
    std::string levels =
        assets_.text(0x3C) + "  ";
    bool first = true;
    for (const auto& track : sheet.classes) {
        if (!track.present()) continue;
        if (!first) {
            classes += "/";
            levels += "/";
        }
        classes += assets_.text(
            track.text_resource_index());
        levels += std::to_string(track.level);
        first = false;
    }

    IndexedFontPresenter::draw_centered(
        framebuffer, font1_,
        0x7B, column, 0x50,
        classes);
    IndexedFontPresenter::draw_centered(
        framebuffer, font1_,
        0x88, column, 0x50,
        levels);
    IndexedFontPresenter::draw_centered(
        framebuffer, font1_,
        0x95, column, 0x50,
        assets_.text(
            sheet.alignment_text_resource_index()));

    const auto race_sex =
        assets_.text(
            sheet.race_text_resource_index()) +
        " " +
        assets_.text(
            sheet.sex_text_resource_index());
    IndexedFontPresenter::draw_centered(
        framebuffer, font1_,
        0xA2, column, 0x50,
        race_sex);

    // 10158 ability table: direct-position labels at +0Ch, current values
    // right-aligned to +2Ch, base values right-aligned to +43h.
    for (std::size_t i = 0;
         i < sheet.abilities.size();
         ++i) {
        const int row =
            0xB8 + static_cast<int>(i) * 0x0C;
        const auto& ability =
            sheet.abilities[i];

        IndexedFontPresenter::draw(
            framebuffer,
            font1_,
            row,
            column + 0x0C,
            assets_.text(
                ability.label_text_resource));

        char current[16]{};
        if (i == 0U &&
            ability.current == 0x12U &&
            sheet.exceptional_strength_current != 0U) {
            std::snprintf(
                current,
                sizeof(current),
                "%02d/%02d",
                static_cast<int>(ability.current),
                static_cast<int>(
                    sheet.exceptional_strength_current %
                    100U));
        } else {
            std::snprintf(
                current,
                sizeof(current),
                "%02d",
                static_cast<int>(ability.current));
        }
        IndexedFontPresenter::draw_right(
            framebuffer,
            font1_,
            row,
            column + 0x2C,
            current);

        char base[16]{};
        if (i == 0U &&
            sheet.exceptional_strength_base != 0U) {
            std::snprintf(
                base,
                sizeof(base),
                "%02d/%02d",
                static_cast<int>(ability.base),
                static_cast<int>(
                    sheet.exceptional_strength_base %
                    100U));
        } else {
            std::snprintf(
                base,
                sizeof(base),
                "%02d",
                static_cast<int>(ability.base));
        }
        IndexedFontPresenter::draw_right(
            framebuffer,
            font1_,
            row,
            column + 0x43,
            base);
    }

    // CODE.2 10158 derived armor class status at the exact
    // character-column section above the hit-point line. Use the
    // existing canonical character record's AC byte, not a hardcoded
    // armor value or a second inventory stat cache.
    IndexedFontPresenter::draw_centered(
        framebuffer,font1_,0x105,column,0x50,
        "A.C. "+std::to_string(
            static_cast<int>(sheet.base_armor_class)));

    // 10158 HP line: label + space + current/max, centered around
    // actor-column +28h, which is equivalent to a centered 50h-span draw.
    IndexedFontPresenter::draw_centered(
        framebuffer,
        font1_,
        0x112,
        column,
        0x50,
        CharacterModalStatsModel::hit_points_line(
            sheet,
            assets_.text(0x39)));

    // 10158 class/XP rows. Class names are right-aligned at +22h for both
    // current-XP and XP-needed sections. The numeric values use external
    // formatter 3E70Ch and remain separately coverage-bounded.
    const auto experience_rows =
        CharacterModalStatsModel::experience_rows(
            sheet,
            experience_table_);
    for (const auto& xp : experience_rows) {
        const auto class_name =
            assets_.text(
                xp.class_text_resource_index());
        IndexedFontPresenter::draw_right(
            framebuffer,
            font1_,
            xp.current_row,
            column + 0x22,
            class_name);
        IndexedFontPresenter::draw_right(
            framebuffer,
            font1_,
            xp.needed_row,
            column + 0x22,
            class_name);
        // Original 3E70C integer formatter prints the per-class
        // current and next-level XP counts, WITHOUT mixing totals
        // between multiclass tracks. Align at actor-local +0x49.
        IndexedFontPresenter::draw_right(
            framebuffer,font1_,
            xp.current_row,column+0x49,
            std::to_string(xp.experience));
        IndexedFontPresenter::draw_right(
            framebuffer,font1_,
            xp.needed_row,column+0x49,
            std::to_string(xp.needed_for_next));
    }
}

std::optional<PartyModalPresenter::EquipmentHit>
PartyModalPresenter::equipment_hit_at(
    const PartyImage& party,
    const InventoryRuntimeState& inventory,
    int row,int column) const {
    if(row<0x6E || row>0x123 ||
       column<0 || column>=320)
        return std::nullopt;
    const auto actor=static_cast<std::size_t>(column/0x50);
    if(actor>=PartyImage::kActorCount || !party.active(actor))
        return std::nullopt;
    const int local=column-static_cast<int>(actor)*0x50;
    if(local<2 || local>0x4D)return std::nullopt;
    if(inventory.held.object_id>=0) {
        const auto& object=objects_.at(
            static_cast<std::size_t>(inventory.held.object_id));
        const auto side=local<0x28?0U:1U;
        const auto slot=InventoryRuntimeState::original_equipment_target(
            object,inventory.held.object_id,side);
        return EquipmentHit{actor,slot};
    }
    // Original CODE.2 12180 4D252..4D2A9 reads ten 24-byte
    // equipment hit descriptors selected by the 0F368 race predicate.
    // A clicked existing item is taken only within its original
    // inclusive descriptor and if its slot is actually occupied.
    const auto info=PartyPresentationModel::actor(party,actor);
    const bool short_race=info.race_code==2U ||
        info.race_code==3U || info.race_code==4U;
    const auto slot=equipment_hits_.occupied_hit(
        short_race,row,local,1U,info.items);
    if(slot)return EquipmentHit{actor,*slot};
    return std::nullopt;
}

void PartyModalPresenter::draw_encumbrance_panel(
    IndexedFramebuffer& framebuffer,
    const PartyImage& party,
    const InventoryRuntimeState& inventory,
    std::size_t actor) const {
    const auto values=encumbrance_.actor(
        party,inventory,objects_,actor);
    const int lane=static_cast<int>(actor)*0x50;
    // CODE.2 12988 physical 4D8D7..4D8E3 loads the ORIGINAL
    // encumbrance dialog art: bank RES2, absolute entry 0x25,
    // row 0x73, actor lane origin. The original is a bitmap,
    // not an 0x45 flat red rectangle. The six real T labels and
    // source-derived values are overlaid on this authentic art.
    // CODE.2 129BF -> 2E510(0x73,lane,0x25,RES2):
    // the entire original descriptor, not its first frame.
    OriginalPartyModalBlit::complete_2e510(
        framebuffer,assets_.type1(2,0x25),0x73,lane);
    IndexedFontPresenter::draw(
        framebuffer,font1_,0x88,lane+7,assets_.text(0x266));
    IndexedFontPresenter::draw_right(
        framebuffer,font1_,0x88,lane+0x45,
        std::to_string(values.maximum_pounds));
    IndexedFontPresenter::draw(
        framebuffer,font1_,0x96,lane+7,assets_.text(0x267));
    IndexedFontPresenter::draw_right(
        framebuffer,font1_,0x96,lane+0x45,
        std::to_string(values.carried_pounds));
    IndexedFontPresenter::draw_centered(
        framebuffer,font1_,0xA4,lane+6,0x3C,
        assets_.text(0x268+values.burden_rank));
}

void PartyModalPresenter::draw_open_container(
    IndexedFramebuffer& framebuffer,
    const InventoryRuntimeState& inventory,
    const std::array<bool,4>& character_mode,
    std::size_t actor) const {
    // Original CODE.2 112EC calls 11268 for ONE actor lane at a
    // time. 1118C -> 10BBC -> 10A70 draws that actor's container
    // before 11268 draws its selection outline, then the next
    // actor lane begins. Do not overlay every container afterward.
    if(actor>=PartyImage::kActorCount || character_mode[actor])return;
    const auto& ui=inventory.party_container_ui_contexts[actor];
    // CODE.2 10A70: phases 1 and 2 draw the ORIGINAL
    // RES2 frame (0x14 + subtype*2 + phase - 1) at row
    // C4h. Phase 0 draws nothing. This models the source
    // animation artwork, not the still-pending 10FAC/110B4
    // VGA retrace schedule and buffer-flip ordering.
    if(ui.active() && (ui.raw[2]==1U || ui.raw[2]==2U)) {
        if(ui.raw[0]<7U) {
            const auto graphic=InventoryScreenModel::
                original_container_transition_res2_graphic(
                    ui.raw[0],ui.raw[2]);
            const int lane=static_cast<int>(actor)*0x50;
            // CODE.2 10ABE -> 2E510(0xC4,lane,graphic,RES2)
            OriginalPartyModalBlit::complete_2e510(
                framebuffer,assets_.type1(2,graphic),0xC4,lane);
        }
        return;
    }
    std::size_t state_index=inventory.containers.size();
    if(ui.fully_open())
        state_index=ui.raw[1];
    else if(ui.inactive() && inventory.open_container &&
            inventory.open_container->actor==actor)
        // Synthetic legacy tests may provide an interactive
        // selection without a 6FA0 state. Never fall back to
        // phase3 art for a real partial phase0/1/2 context.
        state_index=inventory.open_container->descriptor_index;
    if(state_index>=inventory.containers.size())return;
    const auto& desc=inventory.containers[state_index];
    if(desc.ui_columns==0U || desc.slot_count==0U ||
       desc.items.size()!=desc.slot_count)return;

    // 10A70/10ACB source: RES2[9 + subtype] at VGA row CEh,
    // character column 80*actor. Retain original surround
    // art and only phase-3 content. Partial 0/1/2 animation
    // frames are reserved for original 10FAC/110B4 renderer.
    const int lane=static_cast<int>(actor)*0x50;
    const auto panel_art=assets_.type1(
        2,InventoryScreenModel::original_container_res2_graphic(
            desc.subtype));
    // CODE.2 10ADF -> 2E510(0xCE,lane,9+subtype,RES2)
    OriginalPartyModalBlit::complete_2e510(
        framebuffer,panel_art,0xCE,lane);
    for(std::size_t index=0U;index<desc.items.size();++index) {
        const int row=0xDB+
            static_cast<int>(index/desc.ui_columns)*0x22;
        const int column=lane+desc.ui_x_offset+
            static_cast<int>(index%desc.ui_columns)*0x12;
        framebuffer.fill_rect(row,column,32,16,0);
        const auto id=desc.items[index];
        if(id<0)continue;
        const auto& object=objects_.at(
            static_cast<std::size_t>(id));
        const auto raster=assets_.type1(
            1,static_cast<int>(res1_object_base_)+
              object.ui_graphic_index);
        framebuffer.draw_frame(raster,0,row,column);
    }
}

void PartyModalPresenter::draw_held_cursor_and_description(
    IndexedFramebuffer& framebuffer,
    const InventoryRuntimeState& inventory,
    std::optional<InventoryPointer> pointer) const {
    // Original 116F5/11757 builds a 759C message then invokes
    // F790 while 6DAC=1. F790 checks this byte FIRST; it paints
    // the feedback rather than the normal held-item description.
    // This remains visible after successful scribing empties the
    // cursor, until 13184's unsigned IRQ deadline expires.
    std::string description;
    bool scribe_message=false;
    if(inventory.mage_scribe_feedback &&
       inventory.mage_scribe_feedback->pending_6dac!=0U) {
        const auto& feedback=*inventory.mage_scribe_feedback;
        const auto spell_name=
            feedback.kind==MageScribeFeedbackKind::Scribed
                ? assets_.text(feedback.t_spell_name_index)
                : std::string{};
        description=feedback.message(spell_name);
        scribe_message=true;
    } else {
        if(inventory.held.object_id<0)return;
        // Display-only fidelity improvement: native ordinary-held
        // text now uses the original CODE.2 F790 category/field
        // formatting, authentic CODE.2 DS punctuation and decoded T
        // labels, plus MUTABLE container weights from inventory.
        // This does NOT invoke F790, 30CA2, or source IRQ 30700
        // during the host's 70-Hz presentation cadence.
        description=OriginalF790RuntimeDescription::compose(
            original_objects_bytes_,objects_,inventory,
            inventory.held.object_id,inventory.held.known!=0U,
            original_code2_descriptions_,original_t_descriptions_);
    }
    // CODE.2 F66C -> 30508 places the actual message baseline
    // at VGA row 0x65. The exact original decorative ribbon
    // (F66C/2975C) still requires pixel-level DOS acceptance;
    // reuse the existing native inventory strip provisionally,
    // rather than inventing a second UI style.
    const int span=IndexedFontPresenter::measure(font1_,description);
    const int width=std::min(318,span+10);
    const int column=std::max(1,(320-width)/2);
    framebuffer.fill_rect(0x62,column,12,width,0x45);
    IndexedFontPresenter::draw_centered(
        framebuffer,font1_,scribe_message?0x65:0x64,
        column,width,description);

    // The actual pointer sprite is composited AFTER presentation
    // by Win32 Host::set_game_cursor, using the original
    // RES1 image for the currently held item. Do not also bake
    // the same object into the 320x400 background (which leaves
    // a duplicated ghost when the hardware pointer moves).
    (void)pointer;
}

PartyModalCoverage PartyModalPresenter::draw(
    IndexedFramebuffer& framebuffer,
    const PartyImage& party,
    const std::array<bool, 4>& character_mode,
    std::size_t selected_actor,
    const InventoryRuntimeState* inventory,
    std::optional<InventoryPointer> pointer,
    std::optional<std::size_t> encumbrance_actor) const {
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
            // CODE.2 11268 draws the selection outline even for
            // inactive actor columns, after the inactive backdrop.
            if(actor_index==selected_actor)
                ExplorationHudPresenter::draw_outline(
                    framebuffer,0,
                    static_cast<int>(actor_index)*0x50,
                    0x190,0x50,0x1A);
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
            // 10158: original RES2:0x24, frame0, row45h, x=dst+38h.
            draw_res2_local(
                framebuffer,
                0x24,
                0,
                0x45,
                column + 0x38);

            draw_character_centered_text(
                framebuffer,
                CharacterSheetModel::actor(
                    party,
                    actor_index));

            // Computed AC, HP styling side effects, and 3E70Ch XP numeric
            // formatting remain separately bounded. HP text, class labels,
            // and XP threshold math are now native and exact.
            ++coverage.character_text_columns_pending;
        } else {
            coverage.mirrored_equipment_draws_pending +=
                draw_inventory(
                    framebuffer,
                    actor);
            // 1118C -> 10BBC -> 10A70: current actor's container
            // before the original 11268 selection outline.
            if(inventory!=nullptr)
                draw_open_container(
                    framebuffer,*inventory,character_mode,actor_index);
        }
        // CODE.2 11268 draws the outline within the per-actor
        // function, before 112EC advances to the next lane.
        if(actor_index==selected_actor)
            ExplorationHudPresenter::draw_outline(
                framebuffer,0,
                static_cast<int>(actor_index)*0x50,
                0x190,0x50,0x1A);
    }

    if(inventory!=nullptr) {
        draw_held_cursor_and_description(
            framebuffer,*inventory,pointer);
        if(encumbrance_actor &&
           *encumbrance_actor<PartyImage::kActorCount &&
           party.active(*encumbrance_actor)) {
            draw_encumbrance_panel(
                framebuffer,party,*inventory,*encumbrance_actor);
        }
    }
    return coverage;
}

}  // namespace rloft
