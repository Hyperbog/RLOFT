#pragma once

#include "rloft/original_f790_runtime_event.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {

// CODE.2 13118 -> 13133 -> 112EC -> 1134C -> F790, reconstructed
// as one original source-owned modal entry rather than 70-Hz repaints.
// The caller's column renderer MUST draw only the source 11268 actor
// lanes (no provisional F790 text or encumbrance overlay).
//
// This is an isolated native integration boundary. The production Win32
// host will use it only after the capture/redraw paths are fully wired.
class OriginalF790ModalEntry final {
public:
    template<class Draw112EcActorColumns,class ObserveTick30700>
    static void enter_13118(
        OriginalF790ModalSequence& modal,
        IndexedFramebuffer& framebuffer,
        Draw112EcActorColumns&& draw_actor_columns_112ec,
        const InventoryRuntimeState& inventory,
        const std::vector<std::uint8_t>& raw_objects,
        const ObjectCatalog& objects,
        const OriginalF790StaticResources& static_code2,
        const OriginalTTextArchive& original_t,
        const Type1Raster& original_res2_3,
        const Type2Font& original_font1,
        OriginalRandom& shared_game_rng,
        ObserveTick30700&& observe_tick_30700) {
        // 112EC is called once by 13118. It saves the 320x16 strip at
        // row 0x62 after drawing all four source actor columns.
        if(modal.captured())
            throw std::logic_error("original 112EC page already captured");

        // CODE.2 13118 clears 6DAC/deadline before 112EC. The caller
        // must have performed the native equivalent before entry.
        if(inventory.mage_scribe_feedback &&
           inventory.mage_scribe_feedback->pending_6dac)
            throw std::logic_error("13118 must clear source 6DAC before 112EC");

        // Source E8 1134C passes global 8600/8602 (held object/known),
        // never a guessed selected or previous inventory-slot item.
        const auto args=OriginalF790RuntimeEvent::arguments_for_direct_call(
            0x1134CU,inventory,-1,false,false,{});
        const auto message=OriginalF790RuntimeEvent::resolve_text_for_source_call(
            args,raw_objects,objects,inventory,static_code2,original_t);

        // Source-game data errors fail BEFORE changing any framebuffer
        // or touching the shared game RNG. The host must never fall
        // back to a fabricated F66C picture or its own local PRNG.
        if(args.item_id!=-1) {
            if(original_res2_3.count<6)
                throw std::invalid_argument("F66C requires original RES2:3");
            const auto w=OriginalFeedbackRibbonText::measure(
                original_font1.widths,message);
            if(w!=IndexedFontPresenter::measure(original_font1,message))
                throw std::invalid_argument("F66C original font disagreement");
        }

        modal.enter_modal_13118();
        std::forward<Draw112EcActorColumns>(draw_actor_columns_112ec)(
            framebuffer);
        modal.capture_after_actor_columns_112ec(framebuffer);
        // F790 always restores the saved page first; pending is clear
        // here. Held item triggers the source RNG/tick bracket, empty
        // hand performs only the original background restoration.
        modal.source_event_f790(
            framebuffer,false,args.item_id,message,original_res2_3,
            original_font1,shared_game_rng,
            std::forward<ObserveTick30700>(observe_tick_30700));
    }
};

} // namespace rloft
