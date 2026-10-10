#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_feedback_source_snapshot.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void word32(std::vector<std::uint8_t>& out,std::size_t at,std::uint32_t n) {
    for(std::size_t i=0;i<4U;++i)
        out[at+i]=static_cast<std::uint8_t>(n>>(i*8U));
}
rloft::OriginalTTextArchive original_spell_fixture() {
    // Synthetic original-T indexed format with authentic XOR-DC
    // encoding, including original success-only spell name index42.
    std::vector<std::uint8_t> data(4U+0x43U*4U,0U);
    word32(data,0U,0x43U);
    const auto empty=data.size();
    data.push_back(0U);data.push_back(0U);
    for(std::size_t i=0;i<0x43U;++i)
        word32(data,4U+4U*i,static_cast<std::uint32_t>(empty));
    const auto at=data.size();
    word32(data,4U+4U*0x42U,static_cast<std::uint32_t>(at));
    const std::string spell="FIREBALL";
    data.push_back(static_cast<std::uint8_t>(spell.size()));
    data.push_back(0U);
    for(char value:spell)
        data.push_back(static_cast<std::uint8_t>(value)^0xDCU);
    return rloft::OriginalTTextArchive(std::move(data));
}
}

int main() {
    using Snapshot=rloft::OriginalF790FeedbackSourceSnapshot;
    using Status=rloft::InventoryOrdinarySwapStatus;
    using Kind=rloft::MageScribeFeedbackKind;
    const auto t=original_spell_fixture();
    rloft::InventoryRuntimeState inv{};
    inv.held={74,1U};
    assert(!Snapshot::after_scribe(Status::NoChange,inv,t));
    assert(!Snapshot::after_expiry(false,inv));

    rloft::MageScribeFeedback pending{};
    pending.kind=Kind::Scribed;
    pending.t_spell_name_index=0x42U;
    pending.source_text=" LEARNED!";
    pending.pending_6dac=1U;
    pending.deadline_6da8=123U;
    inv.mage_scribe_feedback=pending;
    const auto success=Snapshot::after_scribe(
        Status::LearnedMageSpell,inv,t);
    assert(success && success->source_e8_offset==0x1173bU);
    assert(success->owned_text_759c=="FIREBALL LEARNED!");
    assert(success->pending_6dac && success->item_id==74);
    assert(success->known);
    auto args=success->arguments_now();
    assert(args.pending_text_759c=="FIREBALL LEARNED!");
    assert(args.item_id==74 && args.known && args.pending_6dac);

    // The source's success branch consumed the scroll BEFORE F790.
    inv.held={-1,0U};
    const auto consumed=Snapshot::after_scribe(
        Status::LearnedMageSpell,inv,t);
    assert(consumed && consumed->item_id==-1);
    assert(consumed->owned_text_759c=="FIREBALL LEARNED!");

    for(const auto kind:
        std::array<Kind,2>{{Kind::CapacityReached,Kind::AlreadyKnown}}) {
        pending.kind=kind;
        pending.source_text=(kind==Kind::CapacityReached)
            ? "CANNOT UNDERSTAND" : "ALREADY KNOWN";
        inv.mage_scribe_feedback=pending;
        const auto reject=Snapshot::after_scribe(
            Status::MageScrollRejected,inv,t);
        assert(reject && reject->source_e8_offset==0x1178eU);
        assert(reject->pending_6dac);
        assert(reject->owned_text_759c==pending.source_text);
        assert(reject->arguments_now().item_id==-1);
        assert(reject->arguments_now().pending_text_759c==
               pending.source_text);
    }

    assert(!Snapshot::after_expiry(false,inv));
    assert(!inv.expire_scribe_feedback_13184(123U));
    assert(!Snapshot::after_expiry(false,inv));
    assert(inv.expire_scribe_feedback_13184(124U));
    const auto expiry=Snapshot::after_expiry(true,inv);
    assert(expiry && expiry->source_e8_offset==0x131bbU);
    assert(expiry->item_id==-1 && !expiry->pending_6dac);
    assert(expiry->owned_text_759c.empty());
    assert(expiry->arguments_now().pending_text_759c.empty());
    assert(!Snapshot::after_expiry(false,inv));

    // Validate snapshot ownership of text across changes to the
    // actual mutable mage feedback and global held cursor.
    pending.kind=Kind::Scribed;
    pending.source_text=" LEARNED!";
    inv.mage_scribe_feedback=pending;
    inv.held={90,0U};
    auto owned=Snapshot::after_scribe(Status::LearnedMageSpell,inv,t);
    assert(owned && owned->item_id==90);
    inv.mage_scribe_feedback.reset();
    inv.held={-1,1U};
    assert(owned->owned_text_759c=="FIREBALL LEARNED!");
    assert(owned->arguments_now().item_id==90);
    assert(!owned->arguments_now().known);
    // Time, render cadence and one gameplay RNG are deliberately absent.
}
