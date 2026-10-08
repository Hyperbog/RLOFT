#include "rloft/combat_hand_pair_early_gate.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace {

void status(rloft::PartyImage& party, std::size_t actor,
            std::uint16_t word) {
    const auto at=actor*rloft::PartyImage::kActorSize+0x10U;
    party.bytes.at(at)=static_cast<std::uint8_t>(word);
    party.bytes.at(at+1U)=static_cast<std::uint8_t>(word>>8U);
}

} // namespace

int main() {
    using rloft::CombatHandPairEarlyGateModel;
    rloft::PartyImage party{};
    party.bytes.fill(0U);

    const auto apply=[&](std::size_t actor,
                         std::int32_t first,
                         std::int32_t second,
                         std::uint8_t mode,
                         bool eligible0=true,
                         bool eligible1=true) {
        return CombatHandPairEarlyGateModel::apply(
            party,actor,first,second,mode,eligible0,eligible1);
    };

    // 244CE both -1: bypass arbitration, retain original 240A8
    // candidate values unless special actor/party gate rejects them.
    auto r=apply(0U,-1,-1,0xFFU,false,true);
    assert(r.skip_object_arbitration);
    assert(!r.first_eligible && r.second_eligible);
    r=apply(2U,-1,-1,0x00U);
    assert(r.first_eligible && r.second_eligible);
    r=apply(2U,-1,-1,0xFFU);
    assert(!r.first_eligible && !r.second_eligible);

    // Exact 0x4000 threshold: earlier 240A8 independently checks
    // status ==0x8000, but 24478 requires both statuses >=0x4000.
    status(party,0U,0x4000U);
    status(party,1U,0x4000U);
    r=apply(2U,-1,-1,0xFFU);
    assert(r.first_eligible && r.second_eligible);
    status(party,0U,0x3FFFU);
    r=apply(2U,-1,-1,0xFFU);
    assert(!r.first_eligible && !r.second_eligible);
    status(party,0U,0x8000U);
    r=apply(2U,-1,-1,0xFFU);
    assert(r.first_eligible && r.second_eligible);
    status(party,1U,0x3FFFU);
    r=apply(2U,-1,-1,0xFFU);
    assert(!r.first_eligible && !r.second_eligible);
    status(party,1U,0x4000U);

    // 24522/23F34 paired pseudo items 7E..86: force both TRUE
    // regardless of prior 240A8 results and current actor 7240h.
    status(party,2U,0x7240U);
    r=apply(2U,0x7E,0x86,0U,false,false);
    assert(r.skip_object_arbitration);
    assert(r.first_eligible && r.second_eligible);
    r=apply(2U,0x7E,0x86,0xFFU,false,false);
    assert(r.first_eligible && r.second_eligible);
    status(party,0U,0U);
    r=apply(2U,0x7E,0x86,0xFFU);
    assert(!r.first_eligible && !r.second_eligible);
    assert(r.skip_object_arbitration);
    status(party,0U,0x4000U);
    status(party,2U,0U);

    // Single pseudo item can override the general status 7240h
    // veto. But with the special FFh/actor gate it becomes false.
    status(party,2U,0x0040U);
    r=apply(2U,0x7E,0x40,0U,false,true);
    assert(!r.skip_object_arbitration);
    assert(r.first_eligible && !r.second_eligible);
    r=apply(2U,0x40,0x7F,0U,true,false);
    assert(!r.first_eligible && r.second_eligible);
    status(party,0U,0U);
    r=apply(2U,0x7F,0x40,0xFFU,false,true);
    assert(!r.first_eligible && !r.second_eligible);
    status(party,0U,0x4000U);

    // One -1 is NOT the both-empty early return. Preserve the
    // other candidate unless the 7240h actor status mask vetoes.
    status(party,2U,0U);
    r=apply(2U,-1,0x40,0xFFU,false,true);
    assert(!r.skip_object_arbitration);
    assert(!r.first_eligible && r.second_eligible);
    status(party,0U,0U);
    r=apply(2U,-1,0x40,0xFFU,true,true);
    assert(!r.first_eligible && r.second_eligible);
    r=apply(2U,0x40,-1,0xFFU,true,true);
    assert(r.first_eligible && !r.second_eligible);
    status(party,0U,0x4000U);

    // Boundaries are inclusive: 7D/87 are ordinary, not pseudo.
    // The previous FFh/actor2 test left actor0 status at 0x4000.
    // For THIS actor0 ordinary-hand case that status is masked by
    // original 2459C (0x7240), vetoing both candidate hands.
    // Clear it explicitly so the test isolates item boundaries.
    status(party,0U,0U);
    r=apply(0U,0x7D,0x87,0U,false,true);
    assert(!r.skip_object_arbitration);
    assert(!r.first_eligible && r.second_eligible);
    r=apply(0U,0x7E,0x87,0U,false,true);
    assert(r.first_eligible && r.second_eligible);

    const auto saved=party.bytes;
    (void)apply(0U,0x7E,0x86,0xFFU);
    assert(party.bytes==saved);

    bool rejected=false;
    try {(void)apply(4U,0,0,0U);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try {(void)apply(0U,-2,1,0U);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
}
