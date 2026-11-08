#pragma once

#include "rloft/original_f790_feedback_source_snapshot.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace rloft {

// Source-only E8 F790 event order for one CODE.2 117A0 ordinary-slot
// transaction. The live native caller must capture both BEFORE values.
// This does not draw, read IRQ ticks, seed/draw from RNG, or imply
// a host refresh is a game event.
struct OriginalF790OrdinarySlotEventPlan final {
    using Snapshot=OriginalF790FeedbackSourceSnapshot;
    std::array<Snapshot,2> events{};
    std::size_t count{};

    [[nodiscard]] const Snapshot& at(std::size_t index) const {
        if(index>=count)throw std::out_of_range("original F790 event index");
        return events[index];
    }

    [[nodiscard]] static OriginalF790OrdinarySlotEventPlan after_117a0(
        InventoryOrdinarySwapStatus status,
        HeldItemState hand_before_117a0,
        HeldItemState actor_slot_before_117a0,
        const InventoryRuntimeState& after,
        const OriginalTTextArchive& original_t) {
        OriginalF790OrdinarySlotEventPlan out{};

        // Every original feedback override remains active for *any*
        // F790 call while 6DAC is nonzero. Avoid reading the default
        // object text on an explicit (-1,0) pending call.
        const auto pending_text=[&]() -> std::string {
            if(!after.mage_scribe_feedback ||
               after.mage_scribe_feedback->pending_6dac==0U)return {};
            const auto& f=*after.mage_scribe_feedback;
            return f.message(f.kind==MageScribeFeedbackKind::Scribed
                ? original_t.decoded(f.t_spell_name_index) : std::string{});
        };
        const auto pending=[&]() {
            return after.mage_scribe_feedback &&
                   after.mage_scribe_feedback->pending_6dac!=0U;
        };
        const auto put=[&](std::size_t at,std::uint32_t e8,
                           std::int32_t id,bool known,
                           bool is_pending,const std::string& text) {
            if(at>=out.events.size() ||
               !OriginalF790CallerCatalog::arguments_at(e8))
                throw std::logic_error("unexpected original F790 call count/site");
            out.events[at]=Snapshot{e8,id,known,is_pending,text};
            out.count=at+1U;
        };
        switch(status) {
        case InventoryOrdinarySwapStatus::LearnedMageSpell: {
            // 11630 first invokes 1173B with the STILL-HELD scroll,
            // then 117A0's 1183E consumes it and 1184A explicitly
            // redraws the *same pending text* with empty (-1,0).
            const auto first=Snapshot::after_scribe(
                status,after,original_t,hand_before_117a0);
            if(!first)throw std::logic_error("missing successful 1173B");
            put(0U,0x1173bU,first->item_id,first->known,
                first->pending_6dac,first->owned_text_759c);
            put(1U,0x1184aU,-1,false,true,first->owned_text_759c);
            break;
        }
        case InventoryOrdinarySwapStatus::MageScrollRejected: {
            // 1178E is called from inside 11630; original 11834
            // branches away before the later explicit-empty call.
            const auto first=Snapshot::after_scribe(
                status,after,original_t,hand_before_117a0);
            if(!first)throw std::logic_error("missing rejected 1178E");
            put(0U,0x1178eU,first->item_id,first->known,
                first->pending_6dac,first->owned_text_759c);
            break;
        }
        case InventoryOrdinarySwapStatus::ConsumedStoryFragment:
        case InventoryOrdinarySwapStatus::InsertedInContainer: {
            // 11882: Tome 0x35 consumes held parchment.
            // 118FF: 11404 container insertion consumed held item.
            // Both original callsites PUSH 0; PUSH -1 after the
            // transaction; any existing pending 759C still wins.
            // When native insertion swaps with an *empty* container
            // cell, its legacy known bit can remain set. Original
            // F790's 118FF still pushes explicit (-1,0), so ignore
            // post-insertion known-bit residue whenever ID==-1.
            if(after.held.object_id!=-1 ||
               (status==InventoryOrdinarySwapStatus::ConsumedStoryFragment &&
                after.held.known!=0U))
                throw std::logic_error("original explicit-empty F790 after nonempty hand");
            put(0U,status==InventoryOrdinarySwapStatus::ConsumedStoryFragment
                  ? 0x11882U : 0x118ffU,-1,false,pending(),pending_text());
            break;
        }
        case InventoryOrdinarySwapStatus::Swapped: {
            // 119FC passes stack-local EBP (original actor-slot ID)
            // and local extracted known bit, never a newly sampled
            // screen cursor. Native post-swap held must agree.
            if(after.held.object_id!=actor_slot_before_117a0.object_id ||
               after.held.known!=actor_slot_before_117a0.known)
                throw std::logic_error("117A0 swap source-local tuple drift");
            put(0U,0x119fcU,actor_slot_before_117a0.object_id,
                actor_slot_before_117a0.known!=0U,pending(),pending_text());
            break;
        }
        case InventoryOrdinarySwapStatus::NoChange:
        case InventoryOrdinarySwapStatus::Unsupported:
            // No original F790 in either no-op/rejected exchange.
            break;
        }
        return out;
    }
};

} // namespace rloft
