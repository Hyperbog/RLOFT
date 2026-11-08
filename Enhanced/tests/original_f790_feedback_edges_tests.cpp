#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_feedback_edges.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>

int main() {
    using Edges=rloft::OriginalF790FeedbackEdges;
    using Status=rloft::InventoryOrdinarySwapStatus;
    using Kind=rloft::MageScribeFeedbackKind;

    rloft::InventoryRuntimeState inventory{};
    assert(!Edges::after_ordinary_slot_117a0(Status::NoChange,inventory));
    assert(!Edges::after_ordinary_slot_117a0(Status::Swapped,inventory));
    assert(!Edges::after_ordinary_slot_117a0(Status::ConsumedStoryFragment,inventory));
    assert(!Edges::after_expire_13184(false,inventory));
    bool rejected=false;
    try {
        (void)Edges::after_ordinary_slot_117a0(
            Status::LearnedMageSpell,inventory);
    } catch (const std::logic_error&) { rejected=true; }
    assert(rejected);

    rloft::MageScribeFeedback pending{};
    pending.kind=Kind::Scribed;
    pending.pending_6dac=1U;
    pending.deadline_6da8=0x1234U;
    inventory.mage_scribe_feedback=pending;
    const auto successful=Edges::after_ordinary_slot_117a0(
        Status::LearnedMageSpell,inventory);
    assert(successful && successful->source_e8_offset==0x1173bU);
    assert(successful->pending_6dac);
    // Original F790 draws pending feedback on an original source event.
    // A host frame or a mouse event that did not scribe is not a new call.
    for(int i=0;i<1000;++i)
        assert(!Edges::after_ordinary_slot_117a0(Status::NoChange,inventory));
    assert(!Edges::after_expire_13184(
        inventory.expire_scribe_feedback_13184(0x1234U),inventory));
    assert(inventory.mage_scribe_feedback.has_value());

    const auto expired=inventory.expire_scribe_feedback_13184(0x1235U);
    assert(expired && !inventory.mage_scribe_feedback);
    const auto expiry_edge=Edges::after_expire_13184(expired,inventory);
    assert(expiry_edge && expiry_edge->source_e8_offset==0x131bbU);
    assert(!expiry_edge->pending_6dac);
    assert(!Edges::after_expire_13184(
        inventory.expire_scribe_feedback_13184(0x1236U),inventory));

    // Both original rejection reasons take 1176A->1178E, but do
    // not mutate ordinary item IDs or seed RNG in this model.
    for(const auto kind:std::array<Kind,2>{{Kind::CapacityReached,Kind::AlreadyKnown}}) {
        pending.kind=kind;
        pending.pending_6dac=1U;
        pending.deadline_6da8=0xFFFFFFFEU;
        inventory.mage_scribe_feedback=pending;
        const auto edge=Edges::after_ordinary_slot_117a0(
            Status::MageScrollRejected,inventory);
        assert(edge && edge->source_e8_offset==0x1178eU);
        assert(edge->pending_6dac);
        assert(!Edges::after_expire_13184(
            inventory.expire_scribe_feedback_13184(0xFFFFFFFEU),inventory));
        assert(inventory.expire_scribe_feedback_13184(0xFFFFFFFFU));
        assert(Edges::after_expire_13184(true,inventory)->source_e8_offset==
            0x131bbU);
    }

    // The original source uses UNSIGNED strict '>', not a wrap-safe
    // timer correction. A timer wrapped below the deadline stays live.
    pending.kind=Kind::Scribed;
    pending.deadline_6da8=0xFFFFFFFFU;
    inventory.mage_scribe_feedback=pending;
    assert(!inventory.expire_scribe_feedback_13184(0U));
    assert(!Edges::after_expire_13184(false,inventory));

    pending.deadline_6da8.reset();
    inventory.mage_scribe_feedback=pending;
    rejected=false;
    try {
        (void)Edges::after_ordinary_slot_117a0(
            Status::LearnedMageSpell,inventory);
    } catch(const std::logic_error&) {rejected=true;}
    assert(rejected);

    pending.kind=Kind::AlreadyKnown;
    pending.deadline_6da8=100U;
    inventory.mage_scribe_feedback=pending;
    rejected=false;
    try {
        (void)Edges::after_ordinary_slot_117a0(
            Status::LearnedMageSpell,inventory);
    } catch(const std::logic_error&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try { (void)Edges::after_expire_13184(true,inventory); }
    catch(const std::logic_error&) {rejected=true;}
    assert(rejected);
}
