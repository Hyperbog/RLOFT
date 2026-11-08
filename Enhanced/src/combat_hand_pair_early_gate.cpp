#include "rloft/combat_hand_pair_early_gate.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

bool pseudo_item(std::int32_t id) noexcept {
    // CODE.2 23F34, 24522, 24641, 24693 use unsigned
    // comparisons; signed -1 is NOT a pseudo item.
    return id>=0x7E && id<=0x86;
}

bool privileged_party_condition(
    const PartyImage& party,
    std::size_t actor,
    std::uint8_t mode) {
    // Original 244D9..24506 (repeated at 24551,245BE,24603,
    // 2465B and 246B1), distinct from 240A8's exact ==8000h
    // tests. A 16-bit status is zero-extended and compared to 4000h.
    // Mode FFh with actor 2/3 requires BOTH status words >=4000h.
    if (mode!=0xFFU || actor<2U) return true;
    return party.status_word(0U)>=0x4000U &&
           party.status_word(1U)>=0x4000U;
}

} // namespace

CombatHandPairEarlyGateResult CombatHandPairEarlyGateModel::apply(
    const PartyImage& party,
    std::size_t actor,
    std::int32_t first_item,
    std::int32_t second_item,
    std::uint8_t party_mode_258a8,
    bool first_initial_eligible,
    bool second_initial_eligible) {
    if (actor>=PartyImage::kActorCount) {
        throw std::out_of_range("CODE.2 24478 party actor");
    }
    if (first_item<-1 || second_item<-1) {
        throw std::out_of_range("CODE.2 24478 signed item ID");
    }

    CombatHandPairEarlyGateResult result{
        first_initial_eligible,second_initial_eligible,false};
    const bool allowed=privileged_party_condition(
        party,actor,party_mode_258a8);

    // 244CE..2451D: both hands empty. Jump *straight* to
    // busy-state checks; ordinary OBJECTS qualifier/weight reads
    // never execute. This gate does not set both to true: keep
    // original 240A8 input candidates unless disallowed.
    if (first_item==-1 && second_item==-1) {
        if (!allowed) {
            result.first_eligible=false;
            result.second_eligible=false;
        }
        result.skip_object_arbitration=true;
        return result;
    }

    // 24522..24597: if hand0 is pseudo and hand1 is pseudo,
    // set BOTH candidate registers to one, then apply the
    // FFh/actor>=2/party >=4000h override. This precedes
    // the general 7240h actor-status mask entirely.
    if (pseudo_item(first_item) && pseudo_item(second_item)) {
        result.first_eligible=allowed;
        result.second_eligible=allowed;
        result.skip_object_arbitration=true;
        return result;
    }

    // 2459C..245B9: condition mask on current actor.
    if ((party.status_word(actor)&0x7240U)!=0U) {
        result.first_eligible=false;
        result.second_eligible=false;
    }

    // 245B9..24641: each empty hand is explicitly vetoed when
    // FFh/actor>=2 and either first-party status is below 4000h.
    if (first_item==-1 && !allowed) result.first_eligible=false;
    if (second_item==-1 && !allowed) result.second_eligible=false;

    // 24641..246E9: individual pseudo-hand candidate is forced to
    // the privileged gate result, *even after* the generic 7240h
    // status filter. These paths do not bypass later arbitration.
    if (pseudo_item(first_item)) result.first_eligible=allowed;
    if (pseudo_item(second_item)) result.second_eligible=allowed;

    return result;
}

} // namespace rloft
