#pragma once

#include "rloft/original_f790_ordinary_slot_event_plan.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace rloft {

// CODE.2 117A0 inventory transaction -> original source F790 calls.
// The plan owns source-time arguments. An authentic successful spell
// scribble has TWO calls (1173B then 1184A). The regular host repaint
// is not a source event and must never dispatch through this class.
//
// Source-only integration; Win32 has not been connected to this bridge.
class OriginalF790OrdinarySlotDispatch final {
public:
    template<class ObserveTick30700>
    [[nodiscard]] static std::size_t dispatch(
        const OriginalF790OrdinarySlotEventPlan& plan,
        OriginalF790ModalSequence& modal,
        IndexedFramebuffer& framebuffer,
        const std::vector<std::uint8_t>& raw_objects,
        const ObjectCatalog& objects,
        const InventoryRuntimeState& inventory,
        const OriginalF790StaticResources& static_code2,
        const OriginalTTextArchive& original_t,
        const Type1Raster& res2_3,
        const Type2Font& font1,
        OriginalRandom& shared_game_rng,
        ObserveTick30700&& observe_tick_30700) {

        // Validate the WHOLE transaction before any VGA restore, IRQ
        // observation or shared 30CA2 RNG mutation. It must not
        // partially draw one half of an invalid double-call scribe.
        if(plan.count > plan.events.size())
            throw std::invalid_argument("117A0 F790 plan exceeds two calls");
        if(plan.count == 0U)
            return 0U;
        if(!modal.captured())
            throw std::logic_error("117A0 F790 before 112EC page capture");

        const auto first=plan.at(0).source_e8_offset;
        if(plan.count==2U) {
            if(first!=0x1173BU ||
               plan.at(1).source_e8_offset!=0x1184AU ||
               !plan.at(0).pending_6dac ||
               !plan.at(1).pending_6dac ||
               plan.at(0).owned_text_759c!=plan.at(1).owned_text_759c)
                throw std::invalid_argument("invalid 1173B/1184A scribe pair");
        } else if(first!=0x1178EU && first!=0x11882U &&
                  first!=0x118FFU && first!=0x119FCU) {
            throw std::invalid_argument("unproved 117A0 F790 status edge");
        }

        std::array<std::string,2> prepared_text{};
        for(std::size_t i=0; i<plan.count; ++i) {
            const auto& event=plan.at(i);
            const auto site=event.source_e8_offset;
            const auto origin=OriginalF790CallerCatalog::arguments_at(site);
            if(!origin)
                throw std::invalid_argument("unknown CODE.2 F790 callsite");
            if((site==0x1173BU || site==0x1178EU) &&
               (*origin!=OriginalF790ArgumentOrigin::GlobalHeld8600_8602 ||
                !event.pending_6dac || event.item_id<0))
                throw std::invalid_argument("missing original scribe hand");
            if(site==0x1184AU || site==0x11882U || site==0x118FFU) {
                if(*origin!=OriginalF790ArgumentOrigin::ExplicitEmpty ||
                   event.item_id!=-1 || event.known)
                    throw std::invalid_argument("F790 needs explicit empty");
            }
            if(site==0x119FCU &&
               *origin!=OriginalF790ArgumentOrigin::StackLocal)
                throw std::invalid_argument("119FC needs old local item");

            const auto args=event.arguments_now();
            prepared_text[i]=
                OriginalF790RuntimeEvent::resolve_text_for_source_call(
                    args,raw_objects,objects,inventory,static_code2,original_t);

            if(args.pending_6dac || args.item_id!=-1) {
                if(res2_3.count<6)
                    throw std::invalid_argument("F66C needs six RES2:3 frames");
                const auto width=OriginalFeedbackRibbonText::measure(
                    font1.widths,prepared_text[i]);
                if(width!=IndexedFontPresenter::measure(font1,prepared_text[i]))
                    throw std::invalid_argument("F66C font width mismatch");
            }
        }

        for(std::size_t i=0; i<plan.count; ++i) {
            const auto& event=plan.at(i);
            modal.source_event_f790(
                framebuffer,event.pending_6dac,event.item_id,prepared_text[i],
                res2_3,font1,shared_game_rng,observe_tick_30700);
        }
        return plan.count;
    }
};

} // namespace rloft
