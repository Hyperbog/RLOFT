#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_ordinary_slot_event_plan.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void u32(std::vector<std::uint8_t>& data,std::size_t at,std::uint32_t value){
    for(unsigned k=0;k<4U;++k)
        data[at+k]=static_cast<std::uint8_t>(value>>(8U*k));
}
rloft::OriginalTTextArchive fixture(){
    std::vector<std::uint8_t> data(4U+0x43U*4U);
    u32(data,0U,0x43U);
    const auto empty=data.size();
    data.insert(data.end(),{0U,0U});
    for(std::size_t i=0;i<0x43U;++i)
        u32(data,4U+4U*i,static_cast<std::uint32_t>(empty));
    const std::string spell="FIREBALL";
    u32(data,4U+4U*0x42U,static_cast<std::uint32_t>(data.size()));
    data.push_back(static_cast<std::uint8_t>(spell.size()));
    data.push_back(0U);
    for(const auto character:spell)
        data.push_back(static_cast<std::uint8_t>(character)^0xDCU);
    return rloft::OriginalTTextArchive(std::move(data));
}
}
int main(){
    using Plan=rloft::OriginalF790OrdinarySlotEventPlan;
    using Status=rloft::InventoryOrdinarySwapStatus;
    using Kind=rloft::MageScribeFeedbackKind;
    rloft::InventoryRuntimeState inventory{};
    const auto text=fixture();
    const rloft::HeldItemState scroll{0xA7,1U};
    const rloft::HeldItemState prior_slot{0x35,0U};

    assert(Plan::after_117a0(Status::NoChange,scroll,prior_slot,
                             inventory,text).count==0U);
    assert(Plan::after_117a0(Status::Unsupported,scroll,prior_slot,
                             inventory,text).count==0U);

    rloft::MageScribeFeedback feedback{};
    feedback.kind=Kind::Scribed;
    feedback.pending_6dac=1U;
    feedback.deadline_6da8=1234U;
    feedback.t_spell_name_index=0x42U;
    feedback.source_text=" LEARNED!";
    inventory.mage_scribe_feedback=feedback;
    inventory.held={-1,0U}; // after original 1183E consumes it
    auto calls=Plan::after_117a0(Status::LearnedMageSpell,scroll,
                                  prior_slot,inventory,text);
    assert(calls.count==2U);
    assert(calls.at(0).source_e8_offset==0x1173bU);
    assert(calls.at(0).item_id==0xA7 && calls.at(0).known);
    assert(calls.at(0).pending_6dac);
    assert(calls.at(1).source_e8_offset==0x1184aU);
    assert(calls.at(1).item_id==-1 && !calls.at(1).known);
    assert(calls.at(1).pending_6dac);
    assert(calls.at(0).owned_text_759c=="FIREBALL LEARNED!");
    assert(calls.at(1).owned_text_759c==calls.at(0).owned_text_759c);
    bool rejected=false;
    try{(void)calls.at(2);}catch(const std::out_of_range&){rejected=true;}
    assert(rejected);

    // Two source events, not one. Both would draw the pending
    // ribbon and each would advance the actual shared 30CA2 RNG;
    // this planner itself MUST NEVER consume RNG.
    inventory.held=scroll;
    for(const auto kind:std::array<Kind,2>{{
            Kind::CapacityReached,Kind::AlreadyKnown}}){
        feedback.kind=kind;
        feedback.source_text=kind==Kind::CapacityReached
            ?"CANNOT UNDERSTAND":"ALREADY KNOWN";
        inventory.mage_scribe_feedback=feedback;
        calls=Plan::after_117a0(Status::MageScrollRejected,scroll,
                                 prior_slot,inventory,text);
        assert(calls.count==1U);
        assert(calls.at(0).source_e8_offset==0x1178eU);
        assert(calls.at(0).item_id==0xA7 && calls.at(0).known);
        assert(calls.at(0).owned_text_759c==feedback.source_text);
    }

    inventory.mage_scribe_feedback.reset();
    inventory.held={-1,0U};
    calls=Plan::after_117a0(Status::ConsumedStoryFragment,scroll,
                             prior_slot,inventory,text);
    assert(calls.count==1U && calls.at(0).source_e8_offset==0x11882U);
    assert(calls.at(0).item_id==-1 && !calls.at(0).pending_6dac);
    calls=Plan::after_117a0(Status::InsertedInContainer,scroll,
                             prior_slot,inventory,text);
    assert(calls.count==1U && calls.at(0).source_e8_offset==0x118ffU);
    // An empty destination slot may retain its old per-slot known
    // bit. Native 11C3C can return held {-1,1}; original 118FF
    // still explicitly passes (-1,0) to F790.
    inventory.held={-1,1U};
    calls=Plan::after_117a0(Status::InsertedInContainer,scroll,
                             prior_slot,inventory,text);
    assert(calls.count==1U && calls.at(0).source_e8_offset==0x118ffU);
    assert(calls.at(0).item_id==-1 && !calls.at(0).known);

    inventory.held=prior_slot;
    calls=Plan::after_117a0(Status::Swapped,scroll,
                             prior_slot,inventory,text);
    assert(calls.count==1U && calls.at(0).source_e8_offset==0x119fcU);
    assert(calls.at(0).item_id==0x35 && !calls.at(0).known);
    assert(!calls.at(0).pending_6dac);
    inventory.held={-1,0U};
    rejected=false;
    try{(void)Plan::after_117a0(Status::Swapped,scroll,prior_slot,
                                 inventory,text);}
    catch(const std::logic_error&){rejected=true;}
    assert(rejected);

    // Even an explicit-empty original F790 redraw consumes pending
    // artwork's RNG while 6DAC is still set.
    feedback.kind=Kind::AlreadyKnown;
    feedback.source_text="ALREADY KNOWN";
    inventory.mage_scribe_feedback=feedback;
    calls=Plan::after_117a0(Status::ConsumedStoryFragment,scroll,
                             prior_slot,inventory,text);
    assert(calls.count==1U && calls.at(0).pending_6dac);
    assert(calls.at(0).owned_text_759c=="ALREADY KNOWN");

    // Bad feedback state / failure to consume the held scroll is
    // rejected before any renderer/RNG side effect.
    inventory.mage_scribe_feedback=feedback;
    rejected=false;
    try{(void)Plan::after_117a0(Status::LearnedMageSpell,scroll,
                                 prior_slot,inventory,text);}
    catch(const std::logic_error&){rejected=true;}
    assert(rejected);
}
