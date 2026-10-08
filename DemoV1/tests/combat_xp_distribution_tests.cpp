#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_xp_distribution.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace {
void write_word(rloft::PartyImage& p,unsigned actor,unsigned off,
                std::uint16_t value) {
    const auto at=static_cast<std::size_t>(actor)*rloft::PartyImage::kActorSize+off;
    p.bytes[at]=static_cast<std::uint8_t>(value);
    p.bytes[at+1U]=static_cast<std::uint8_t>(value>>8U);
}
void write_dword(rloft::PartyImage& p,unsigned actor,unsigned off,
                 std::uint32_t value) {
    const auto at=static_cast<std::size_t>(actor)*rloft::PartyImage::kActorSize+off;
    for(unsigned i=0U;i<4U;++i)
        p.bytes[at+i]=static_cast<std::uint8_t>(value>>(8U*i));
}
} // namespace

int main() {
    using rloft::CombatXpDistributionModel;
    rloft::PartyImage party{};
    // All four are initially blocked original unused-party slots.
    for(unsigned i=0U;i<4U;++i) write_word(party,i,0x10U,0x8000U);
    assert(!CombatXpDistributionModel::inspect(party,200U));

    // Three qualifying actors: normal, two-class and three-class.
    for(unsigned i=0U;i<3U;++i) write_word(party,i,0x10U,0U);
    for(unsigned i=0U;i<3U;++i) {
        const auto base=static_cast<std::size_t>(i)*party.kActorSize;
        party.bytes[base+0x15U]=1U;
        party.bytes[base+0x16U]=0xFFU;
        party.bytes[base+0x17U]=0xFFU;
    }
    party.bytes[1U*party.kActorSize+0x16U]=2U;
    party.bytes[2U*party.kActorSize+0x16U]=2U;
    party.bytes[2U*party.kActorSize+0x17U]=3U;
    write_dword(party,0U,0x26U,100U);
    write_dword(party,1U,0x26U,200U);
    write_dword(party,1U,0x2AU,201U);
    write_dword(party,2U,0x26U,300U);
    write_dword(party,2U,0x2AU,301U);
    write_dword(party,2U,0x2EU,302U);

    auto plan=CombatXpDistributionModel::inspect(party,5U);
    assert(plan && plan->definition_award==5U);
    assert(plan->eligible_party_members==3U);
    // DDA4: (5+1)/3=2 per actor, then (2+1)/2 or /3.
    assert(plan->rounded_party_share==2U);
    assert(plan->actors[0].class_track_count==1U);
    assert(plan->actors[0].per_track_award==2U);
    assert(plan->actors[0].projected_track_progress[0]==102U);
    assert(plan->actors[1].class_track_count==2U);
    assert(plan->actors[1].per_track_award==1U);
    assert(plan->actors[1].projected_track_progress[0]==201U);
    assert(plan->actors[1].projected_track_progress[1]==202U);
    assert(plan->actors[2].class_track_count==3U);
    assert(plan->actors[2].per_track_award==1U);
    assert(plan->actors[2].projected_track_progress[2]==303U);
    assert(!plan->actors[3].eligible);
    // A plan must never write back to the source party.
    assert(party.bytes[0U*party.kActorSize+0x26U]==100U);

    // DDA4's two minimum-one clauses apply even to a zero-word award.
    plan=CombatXpDistributionModel::inspect(party,0U);
    assert(plan && plan->rounded_party_share==1U);
    assert(plan->actors[2].per_track_award==1U);

    // Eligibility is exact F240h, not hp, occupancy or high-bit alone.
    write_word(party,1U,0x10U,0x0240U);
    write_word(party,2U,0x10U,0x4000U);
    plan=CombatXpDistributionModel::inspect(party,1U);
    assert(plan && plan->eligible_party_members==1U);
    assert(plan->actors[0].per_track_award==1U);
    assert(!plan->actors[1].eligible && !plan->actors[2].eligible);

    // DWORD ADD wraps in the original; no host signed overflow UB.
    write_dword(party,0U,0x26U,0xFFFFFFFFU);
    plan=CombatXpDistributionModel::inspect(party,1U);
    assert(plan && plan->actors[0].projected_track_progress[0]==0U);

    // Status bit 0100h is not in the exact F240h eligibility mask.
    write_word(party,0U,0x10U,0x0100U);
    plan=CombatXpDistributionModel::inspect(party,1U);
    assert(plan && plan->eligible_party_members==1U);

    // Original counts class slot 3 even if slot 2 is empty.
    // Its loop still visits contiguous track positions 0..count-1.
    write_word(party,2U,0x10U,0U);
    party.bytes[2U*party.kActorSize+0x16U]=0xFFU;
    party.bytes[2U*party.kActorSize+0x17U]=3U;
    plan=CombatXpDistributionModel::inspect(party,3U);
    assert(plan && plan->actors[2].class_track_count==2U);
    assert(plan->actors[2].projected_track_progress[2]==0U);
}
