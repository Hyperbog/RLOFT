#pragma once

#include "rloft/original_f790_feedback_edges.hpp"
#include "rloft/original_f790_runtime_event.hpp"
#include "rloft/original_t_text_archive.hpp"

#include <cstdint>
#include <stdexcept>
#include <optional>
#include <string>
#include <string_view>

namespace rloft {

// An OWNED snapshot of original F790 source arguments and 759C text.
// 1173B/1178E run after 6DAC is set; 131BB runs after it is cleared.
// No UI repaint, source IRQ polling, art draw, or RNG calls occur here.
struct OriginalF790FeedbackSourceSnapshot final {
    std::uint32_t source_e8_offset{};
    std::int32_t item_id{-1};
    bool known{};
    bool pending_6dac{};
    std::string owned_text_759c;

    // This view is valid only while this snapshot remains alive.
    // Construct a fresh view at dispatch, never store it across moves.
    [[nodiscard]] OriginalF790SourceArguments arguments_now() const noexcept {
        return {item_id,known,pending_6dac,owned_text_759c};
    }

    [[nodiscard]] static std::optional<OriginalF790FeedbackSourceSnapshot>
    after_scribe(
        InventoryOrdinarySwapStatus status,
        const InventoryRuntimeState& current,
        const OriginalTTextArchive& original_t,
        HeldItemState before_transaction_hand_8600) {
        // CODE.2 11630 calls F790 at 1173B while the scroll is
        // STILL HELD. The surrounding 117A0 then consumes that
        // scroll at 1183E, and invokes F790 again at 1184A
        // with explicit (-1,0). A post-transaction inventory
        // alone cannot reconstruct 1173B's real input pair.
        const auto edge=OriginalF790FeedbackEdges::after_ordinary_slot_117a0(
            status,current);
        if(!edge)return std::nullopt;
        const auto& feedback=*current.mage_scribe_feedback;
        if(status==InventoryOrdinarySwapStatus::LearnedMageSpell &&
           (current.held.object_id!=-1 || current.held.known!=0U))
            throw std::logic_error("source 1183E must consume held scroll");
        if(status==InventoryOrdinarySwapStatus::MageScrollRejected &&
           (current.held.object_id!=before_transaction_hand_8600.object_id ||
            current.held.known!=before_transaction_hand_8600.known))
            throw std::logic_error("source rejection must retain held scroll");
        const std::string spell_name=
            feedback.kind==MageScribeFeedbackKind::Scribed
                ? original_t.decoded(feedback.t_spell_name_index)
                : std::string{};
        return OriginalF790FeedbackSourceSnapshot{
            edge->source_e8_offset,before_transaction_hand_8600.object_id,
            before_transaction_hand_8600.known!=0U,true,
            feedback.message(spell_name)};
    }

    [[nodiscard]] static std::optional<OriginalF790FeedbackSourceSnapshot>
    after_expiry(
        bool did_expire,
        const InventoryRuntimeState& current) {
        const auto edge=OriginalF790FeedbackEdges::after_expire_13184(
            did_expire,current);
        if(!edge)return std::nullopt;
        const auto args=OriginalF790RuntimeEvent::arguments_for_direct_call(
            edge->source_e8_offset,current,-1,false,false,{});
        return OriginalF790FeedbackSourceSnapshot{
            edge->source_e8_offset,args.item_id,args.known,false,{}};
    }
};
} // namespace rloft
