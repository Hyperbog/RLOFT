#include "rloft/combat_hand_dispatch.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
void set16(rloft::PartyImage& p, std::size_t actor,
           std::size_t offset, std::uint16_t value) {
    const auto at=actor*rloft::PartyImage::kActorSize+offset;
    p.bytes.at(at)=static_cast<std::uint8_t>(value);
    p.bytes.at(at+1U)=static_cast<std::uint8_t>(value>>8U);
}

void set32(rloft::PartyImage& p, std::size_t actor,
           std::size_t offset, std::uint32_t value) {
    const auto at=actor*rloft::PartyImage::kActorSize+offset;
    for(unsigned i=0U;i<4U;++i)
        p.bytes.at(at+i)=static_cast<std::uint8_t>(value>>(i*8U));
}

rloft::ObjectCatalog fixture() {
    std::vector<std::uint8_t> data(4U+0x80U*0x41U,0U);
    data[0]=0x80U;
    const auto set_category=[&](unsigned id,std::uint8_t kind){
        data[4U+id*0x41U+0x2FU]=kind;
    };
    set_category(1U,9U);
    set_category(2U,10U);
    set_category(3U,11U);
    set_category(4U,0x0EU);
    set_category(5U,8U);
    set_category(6U,0x0DU);
    set_category(7U,0x0FU);
    set_category(0x2EU,9U); // original hardcoded-special ID wins
    set_category(0x5AU,11U);
    return rloft::ObjectCatalog::parse(data);
}
}

int main() {
    using rloft::CombatHandDispatchGate;
    using rloft::CombatHandDispatchModel;
    using rloft::CombatHandDispatchRoute;
    const auto catalog=fixture();
    rloft::PartyImage party{};
    party.bytes.fill(0xFFU);
    set16(party,0U,0x10U,0U);
    set32(party,0U,0x10BU,0U);
    set32(party,0U,0x10FU,0U);
    set16(party,0U,0x40U+14U*2U,0xFFFFU);
    set16(party,0U,0x40U+15U*2U,1U);

    // 26018 legitimately selects an unarmed category 10, but 24478
    // hand-pair validation remains required before any game action.
    auto p=CombatHandDispatchModel::inspect(party,catalog,0U,0U);
    assert(p.gate==CombatHandDispatchGate::AvailableForPairValidation);
    assert(p.route==CombatHandDispatchRoute::PlayerAttack);
    assert(p.object_id==-1);
    assert(p.category==0x0AU);
    assert(!p.special_item && p.requires_24478_approval);

    p=CombatHandDispatchModel::inspect(party,catalog,0U,1U);
    assert(p.gate==CombatHandDispatchGate::AvailableForPairValidation);
    assert(p.route==CombatHandDispatchRoute::PlayerAttack);
    assert(p.object_id==1 && p.category==9U);

    // 214B8 changes only +19E/+1A2 hand readiness on swaps.
    // That must not be mistaken for the +10B/+10F action-state gate.
    set32(party,0U,0x19EU,0xFFFFFFFFU);
    set32(party,0U,0x1A2U,0xFFFFFFFFU);
    p=CombatHandDispatchModel::inspect(party,catalog,0U,0U);
    assert(!p.definitively_blocked());
    set32(party,0U,0x10BU,0xAABBCCDDU);
    p=CombatHandDispatchModel::inspect(party,catalog,0U,0U);
    assert(p.gate==CombatHandDispatchGate::ActionStateBusy);
    assert(p.original_action_state==0xAABBCCDDU);
    assert(p.definitively_blocked());
    p=CombatHandDispatchModel::inspect(party,catalog,0U,1U);
    assert(!p.definitively_blocked()); // other hand unaffected
    set32(party,0U,0x10BU,0U);

    set16(party,0U,0x10U,0x8000U);
    p=CombatHandDispatchModel::inspect(party,catalog,0U,0U);
    assert(p.gate==CombatHandDispatchGate::InactiveActor);
    set16(party,0U,0x10U,0x0040U);
    p=CombatHandDispatchModel::inspect(party,catalog,0U,0U);
    assert(p.gate==CombatHandDispatchGate::ActorConditionBlocked);
    set16(party,0U,0x10U,0U);

    for(unsigned id=1U;id<=7U;++id) {
        set16(party,0U,0x40U+14U*2U,
              static_cast<std::uint16_t>(id));
        p=CombatHandDispatchModel::inspect(party,catalog,0U,0U);
        const auto special=(id==6U || id==7U);
        assert(p.special_item==special);
        assert(p.route==(special
            ?CombatHandDispatchRoute::SpecialItem
            :id==4U?CombatHandDispatchRoute::CategoryE
            :id==5U?CombatHandDispatchRoute::OtherAction
            :CombatHandDispatchRoute::PlayerAttack));
    }
    for(auto id : {0x2EU,0x5AU}) {
        set16(party,0U,0x40U+14U*2U,
              static_cast<std::uint16_t>(id));
        p=CombatHandDispatchModel::inspect(party,catalog,0U,0U);
        assert(p.special_item);
        assert(p.route==CombatHandDispatchRoute::SpecialItem);
    }

    // Probing never mutates the authoritative serialized party.
    const auto original=party.bytes;
    (void)CombatHandDispatchModel::inspect(party,catalog,0U,0U);
    assert(party.bytes==original);

    bool failed=false;
    try {(void)CombatHandDispatchModel::inspect(party,catalog,4U,0U);}
    catch(const std::out_of_range&) {failed=true;}
    assert(failed);
    failed=false;
    try {(void)CombatHandDispatchModel::inspect(party,catalog,0U,2U);}
    catch(const std::out_of_range&) {failed=true;}
    assert(failed);
    set16(party,0U,0x40U+14U*2U,0xFFFEU);
    failed=false;
    try {(void)CombatHandDispatchModel::inspect(party,catalog,0U,0U);}
    catch(const std::out_of_range&) {failed=true;}
    assert(failed);
}
