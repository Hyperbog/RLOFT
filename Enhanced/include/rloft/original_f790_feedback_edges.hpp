#pragma once

#include "rloft/inventory_runtime.hpp"
#include "rloft/original_f790_caller_catalog.hpp"

#include <cstdint>
#include <optional>
#include <stdexcept>

namespace rloft {

// Pure transition-to-E8-site model, SOURCE ONLY (not host wired).
// CODE.2 11717 -> 1173B success, 1176A -> 1178E rejection,
// and 13184 -> 131BB only after 6DAC is cleared on 30700>6DA8.
// Does not draw, construct item text, sample IRQ, or advance RNG.
struct OriginalF790FeedbackEdge final {
    std::uint32_t source_e8_offset{};
    bool pending_6dac{};
};

class OriginalF790FeedbackEdges final {
public:
    [[nodiscard]] static std::optional<OriginalF790FeedbackEdge>
    after_ordinary_slot_117a0(
        InventoryOrdinarySwapStatus status,
        const InventoryRuntimeState& current) {
        if(status!=InventoryOrdinarySwapStatus::LearnedMageSpell &&
           status!=InventoryOrdinarySwapStatus::MageScrollRejected)
            return std::nullopt;
        if(!current.mage_scribe_feedback ||
           current.mage_scribe_feedback->pending_6dac!=1U ||
           !current.mage_scribe_feedback->deadline_6da8)
            throw std::logic_error(
                "source F790 scribing requires pending 6DAC and 30700+0x36 deadline");
        const bool succeeded =
            status==InventoryOrdinarySwapStatus::LearnedMageSpell;
        const bool matching_kind=succeeded
            ? current.mage_scribe_feedback->kind==
                MageScribeFeedbackKind::Scribed
            : current.mage_scribe_feedback->kind==
                MageScribeFeedbackKind::CapacityReached ||
              current.mage_scribe_feedback->kind==
                MageScribeFeedbackKind::AlreadyKnown;
        if(!matching_kind)
            throw std::logic_error("source F790 scribe status/kind disagree");
        const auto e8=succeeded ? 0x1173bU : 0x1178eU;
        // Every feedback F790 call uses the global cursor arguments
        // even after scribing consumes the scroll.
        if(OriginalF790CallerCatalog::arguments_at(e8)!=
           OriginalF790ArgumentOrigin::GlobalHeld8600_8602)
            throw std::logic_error("original F790 scribe call provenance changed");
        return OriginalF790FeedbackEdge{e8,true};
    }

    [[nodiscard]] static std::optional<OriginalF790FeedbackEdge>
    after_expire_13184(
        bool expired,
        const InventoryRuntimeState& current) {
        if(!expired)return std::nullopt;
        // Original 131AA+131B4 clears deadline and pending BEFORE
        // its 131BB F790 call. This is not a repaint-timer event.
        if(current.mage_scribe_feedback &&
           current.mage_scribe_feedback->pending_6dac!=0U)
            throw std::logic_error("F790 expiry edge before clearing 6DAC");
        if(OriginalF790CallerCatalog::arguments_at(0x131bbU)!=
           OriginalF790ArgumentOrigin::GlobalHeld8600_8602)
            throw std::logic_error("original expiry provenance changed");
        return OriginalF790FeedbackEdge{0x131bbU,false};
    }
};
} // namespace rloft
