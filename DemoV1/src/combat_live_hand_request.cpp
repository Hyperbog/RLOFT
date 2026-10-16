#include "rloft/combat_live_hand_request.hpp"

#include <cstddef>
#include <cstdint>

namespace rloft {

CombatLiveHandPlan CombatLiveHandRequestModel::inspect(
    const PartyImage& party,
    const ObjectCatalog& objects,
    const LevelState& level,
    std::size_t actor,
    std::size_t hand) {
    CombatLiveHandPlan out{};
    out.dispatch=CombatHandDispatchModel::inspect(
        party,objects,actor,hand);
    switch(out.dispatch.gate) {
        case CombatHandDispatchGate::InactiveActor:
        case CombatHandDispatchGate::ActorConditionBlocked:
            out.status=CombatLiveHandStatus::ActorBlocked;
            return out;
        case CombatHandDispatchGate::ActionStateBusy:
            out.status=CombatLiveHandStatus::ActionBusy;
            return out;
        case CombatHandDispatchGate::AvailableForPairValidation:
            break;
    }

    // The original 24478 loads both hands, even when only ONE hand
    // was clicked. Any empty-hand pair reaches OBJECTS[-1], whose
    // bytes are adjacent allocator memory, NOT serialized source.
    // Do not silently fabricate the category/weight/qualifier.
    const auto* rec=party.bytes.data()+actor*PartyImage::kActorSize;
    const auto hand_item=[rec](std::size_t slot) {
        const auto at=0x40U+2U*(14U+slot);
        const auto raw=static_cast<std::uint16_t>(rec[at]) |
            (static_cast<std::uint16_t>(rec[at+1U])<<8U);
        // Explicit two's-complement decode, not an implementation-
        // defined unsigned-to-signed narrowing on some C++ targets.
        return raw>=0x8000U
            ? static_cast<int>(raw)-0x10000
            : static_cast<int>(raw);
    };
    if(hand_item(0U)==-1 || hand_item(1U)==-1) {
        out.status=CombatLiveHandStatus::MissingOriginalEmptyHandData;
        return out;
    }

    // Current LEVELS.DAT/LEVnnn header+290 is DGROUP:258A8;
    // this is the same BYTE read by 24478, the automap and
    // the original environment/profile renderer.
    const auto pair=CombatHandPairEligibilityModel::evaluate(
        party,objects,actor,hand,
        CombatSingleHandContext{level.header[0x290U]});
    out.pair_checked=true;
    out.pair_selected_eligible=pair.selected_eligible;
    if(!pair.selected_eligible) {
        out.status=CombatLiveHandStatus::PairVetoed;
        return out;
    }
    switch(out.dispatch.route) {
        case CombatHandDispatchRoute::PlayerAttack:
            out.status=CombatLiveHandStatus::AttackCandidate;
            break;
        case CombatHandDispatchRoute::SpecialItem:
            out.status=CombatLiveHandStatus::SpecialAction;
            break;
        case CombatHandDispatchRoute::CategoryE:
            out.status=CombatLiveHandStatus::CategoryEAction;
            break;
        case CombatHandDispatchRoute::OtherAction:
            out.status=CombatLiveHandStatus::OtherAction;
            break;
    }
    return out;
}

} // namespace rloft
