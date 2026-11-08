#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_live_hand_request.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
void put16(rloft::PartyImage& party,unsigned actor,unsigned offset,
           unsigned value) {
    const auto at=actor*rloft::PartyImage::kActorSize+offset;
    party.bytes.at(at)=static_cast<std::uint8_t>(value);
    party.bytes.at(at+1U)=static_cast<std::uint8_t>(value>>8U);
}
void put32(rloft::PartyImage& party,unsigned actor,unsigned offset,
           unsigned value) {
    const auto at=actor*rloft::PartyImage::kActorSize+offset;
    for(unsigned i=0;i<4U;++i)
        party.bytes.at(at+i)=static_cast<std::uint8_t>(value>>(i*8U));
}
rloft::ObjectCatalog objects() {
    std::vector<std::uint8_t> bytes(4U+0x35U*0x41U,0U);
    bytes[0]=0x35U;
    // Both are real positive IDs, weapon cat9, original qualifier 0.
    bytes[4U+1U*0x41U+0x2FU]=9U;
    bytes[4U+2U*0x41U+0x2FU]=9U;
    // Higher weight loses arbitration at 24478.
    bytes[4U+1U*0x41U+0x30U]=1U;
    bytes[4U+2U*0x41U+0x30U]=4U;
    bytes[4U+3U*0x41U+0x2FU]=14U; // category E
    bytes[4U+3U*0x41U+0x30U]=1U;
    bytes[4U+4U*0x41U+0x2FU]=0x0DU; // special category
    bytes[4U+4U*0x41U+0x30U]=1U;
    return rloft::ObjectCatalog::parse(bytes);
}
}

int main() {
    using rloft::CombatLiveHandRequestModel;
    using S=rloft::CombatLiveHandStatus;
    auto obj=objects();
    rloft::PartyImage p{};
    p.bytes.fill(0xFFU);
    rloft::LevelState l{};
    l.header[0x290U]=0U; // original DGROUP:258A8
    put16(p,0U,0x10U,0U);
    put32(p,0U,0x10BU,0U);
    put32(p,0U,0x10FU,0U);
    put16(p,0U,0x40U+14U*2U,1U);
    put16(p,0U,0x40U+15U*2U,2U);
    auto plan=CombatLiveHandRequestModel::inspect(p,obj,l,0U,0U);
    assert(plan.status==S::AttackCandidate);
    assert(plan.pair_checked && plan.pair_selected_eligible);
    assert(plan.may_enter_player_attack());
    // Other hand is a real weapon but loses original arbitration.
    plan=CombatLiveHandRequestModel::inspect(p,obj,l,0U,1U);
    assert(plan.status==S::PairVetoed);
    assert(!plan.may_enter_player_attack());
    // Both empty, or mixed positive/empty, must NOT conjure the
    // original heap-adjacent OBJECTS[-1] bytes.
    put16(p,0U,0x40U+15U*2U,0xFFFFU);
    plan=CombatLiveHandRequestModel::inspect(p,obj,l,0U,0U);
    assert(plan.status==S::MissingOriginalEmptyHandData);
    assert(!plan.pair_checked && !plan.may_enter_player_attack());
    put16(p,0U,0x40U+14U*2U,0xFFFFU);
    plan=CombatLiveHandRequestModel::inspect(p,obj,l,0U,0U);
    assert(plan.status==S::MissingOriginalEmptyHandData);
    // Original actor conditions/busy state veto before OBJECTS[-1].
    put16(p,0U,0x10U,0x8000U);
    plan=CombatLiveHandRequestModel::inspect(p,obj,l,0U,0U);
    assert(plan.status==S::ActorBlocked);
    put16(p,0U,0x10U,0U);
    put32(p,0U,0x10BU,0x11223344U);
    plan=CombatLiveHandRequestModel::inspect(p,obj,l,0U,0U);
    assert(plan.status==S::ActionBusy);
    put32(p,0U,0x10BU,0U);
    put16(p,0U,0x40U+14U*2U,1U);
    put16(p,0U,0x40U+15U*2U,2U);
    put32(p,0U,0x19EU,0xFFFFFFFFU);
    plan=CombatLiveHandRequestModel::inspect(p,obj,l,0U,0U);
    assert(plan.may_enter_player_attack()); // +19E != +10B

    // Routed nonattack items must never fall into player damage.
    put16(p,0U,0x40U+14U*2U,3U);
    plan=CombatLiveHandRequestModel::inspect(p,obj,l,0U,0U);
    assert(plan.status==S::CategoryEAction);
    assert(!plan.may_enter_player_attack());
    put16(p,0U,0x40U+14U*2U,4U);
    plan=CombatLiveHandRequestModel::inspect(p,obj,l,0U,0U);
    assert(plan.status==S::SpecialAction);
    assert(!plan.may_enter_player_attack());

    bool rejected=false;
    try{(void)CombatLiveHandRequestModel::inspect(p,obj,l,4U,0U);}
    catch(const std::out_of_range&){rejected=true;}
    assert(rejected);
    rejected=false;
    try{(void)CombatLiveHandRequestModel::inspect(p,obj,l,0U,2U);}
    catch(const std::out_of_range&){rejected=true;}
    assert(rejected);
}
