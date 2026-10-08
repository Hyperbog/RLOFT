#include "rloft/combat_hand_pair_eligibility.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

void set16(rloft::PartyImage& p,std::size_t actor,
           std::size_t offset,std::uint16_t value) {
    const auto base=actor*rloft::PartyImage::kActorSize+offset;
    p.bytes.at(base)=static_cast<std::uint8_t>(value);
    p.bytes.at(base+1U)=static_cast<std::uint8_t>(value>>8U);
}

void set32(rloft::PartyImage& p,std::size_t actor,
           std::size_t offset,std::uint32_t value) {
    const auto base=actor*rloft::PartyImage::kActorSize+offset;
    for(unsigned b=0;b<4U;++b)
        p.bytes.at(base+b)=static_cast<std::uint8_t>(value>>(8U*b));
}

rloft::ObjectCatalog catalog_with(
    int qual_a,int qual_b,int weight_a,int weight_b) {
    constexpr auto count=0x104U;
    std::vector<std::uint8_t> bytes(4U+count*0x41U,0U);
    bytes[0]=0x04U; bytes[1]=0x01U;
    const auto field=[&](unsigned id,unsigned offset,unsigned value) {
        bytes[4U+id*0x41U+offset]=
            static_cast<std::uint8_t>(value);
    };
    field(1,0x2FU,9U); field(2,0x2FU,9U);
    field(1,0x35U,static_cast<unsigned>(qual_a));
    field(2,0x35U,static_cast<unsigned>(qual_b));
    field(1,0x30U,static_cast<unsigned>(weight_a));
    field(2,0x30U,static_cast<unsigned>(weight_b));
    field(3,0x2FU,5U); // ordinary nonclassified
    field(4,0x2FU,9U); field(4,0x35U,2U);
    field(5,0x2FU,5U); field(5,0x35U,2U);
    // 23EE4 paired exceptions which are ALSO classified by
    // 23E58/23F50: 59 is effect special, 9F is classed by cat9.
    field(0x59,0x2FU,11U); field(0x59,0x30U,7U);
    field(0x9F,0x2FU,9U); field(0x9F,0x30U,1U);
    return rloft::ObjectCatalog::parse(bytes);
}

} // namespace

int main() {
    using rloft::CombatHandPairEligibilityModel;
    using rloft::CombatSingleHandContext;
    rloft::PartyImage p{};
    p.bytes.fill(0xFFU);
    set16(p,0U,0x10U,0U);
    p.bytes[0x15U]=0xFFU;
    p.bytes[0x16U]=0xFFU;
    p.bytes[0x17U]=0xFFU;
    set32(p,0U,0x10BU,0U);
    set32(p,0U,0x10FU,0U);
    set16(p,0U,0x40U+14U*2U,1U);
    set16(p,0U,0x40U+15U*2U,2U);
    const auto evaluate=[&](const rloft::ObjectCatalog& objects,
                            std::size_t hand=0U) {
        return CombatHandPairEligibilityModel::evaluate(
            p,objects,0U,hand,CombatSingleHandContext{0U});
    };

    // Equal qualifiers 0, no grouped classes: CODE.2 weight
    // arbitration chooses lower-byte-weight item; equal favors first.
    auto objects=catalog_with(0,0,4,1);
    auto result=evaluate(objects);
    assert(!result.first_eligible && result.second_eligible);
    assert(!result.selected_eligible);
    assert(evaluate(objects,1U).selected_eligible);
    assert(result.first_classified && result.second_classified);
    assert(result.requires_action_dispatch);

    objects=catalog_with(0,0,1,4);
    result=evaluate(objects);
    assert(result.first_eligible && !result.second_eligible);
    objects=catalog_with(0,0,4,4);
    result=evaluate(objects);
    assert(result.first_eligible && !result.second_eligible);
    // If both qualifiers equal 2, 3474B cancels both hands.
    objects=catalog_with(2,2,4,1);
    result=evaluate(objects);
    assert(!result.first_eligible && !result.second_eligible);

    // Unequal qualifier with either value 2 vetoes both.
    objects=catalog_with(2,0,4,1);
    result=evaluate(objects);
    assert(!result.first_eligible && !result.second_eligible);
    objects=catalog_with(0,2,4,1);
    result=evaluate(objects);
    assert(!result.first_eligible && !result.second_eligible);

    // qual(first)>qual(second), no class0/2/5/1: lower
    // weight keeps its hand; the opposite hand is vetoed.
    objects=catalog_with(1,0,4,1);
    result=evaluate(objects);
    assert(!result.first_eligible && result.second_eligible);
    objects=catalog_with(1,0,1,4);
    result=evaluate(objects);
    assert(result.first_eligible && !result.second_eligible);

    // Original class group 0/2/5/1 makes the same unequal
    // comparison ASYMMETRIC: smaller second weight keeps BOTH.
    p.bytes[0x15U]=0U;
    objects=catalog_with(1,0,4,1);
    result=evaluate(objects);
    assert(result.first_eligible && result.second_eligible);
    objects=catalog_with(1,0,1,4);
    result=evaluate(objects);
    assert(result.first_eligible && !result.second_eligible);

    // qual(first)<qual(second), grouped: distinct weights
    // permit both; equality suppresses the first hand only.
    objects=catalog_with(0,1,4,1);
    result=evaluate(objects);
    assert(result.first_eligible && result.second_eligible);
    objects=catalog_with(0,1,4,4);
    result=evaluate(objects);
    assert(!result.first_eligible && result.second_eligible);
    // 23EE4 exception pair bypasses weight comparisons entirely.
    set16(p,0U,0x40U+14U*2U,0x59U);
    set16(p,0U,0x40U+15U*2U,0x9FU);
    objects=catalog_with(0,0,4,1);
    result=evaluate(objects);
    assert(result.first_eligible && result.second_eligible);

    // One classified, one ordinary: qualifier 2 suppresses
    // the other candidate, not necessarily the classified hand.
    p.bytes[0x15U]=0xFFU;
    set16(p,0U,0x40U+14U*2U,4U);
    set16(p,0U,0x40U+15U*2U,3U);
    result=evaluate(objects);
    assert(result.first_eligible && !result.second_eligible);
    set16(p,0U,0x40U+14U*2U,3U);
    set16(p,0U,0x40U+15U*2U,4U);
    result=evaluate(objects);
    assert(!result.first_eligible && result.second_eligible);
    // Both unclassified can remain available simultaneously.
    set16(p,0U,0x40U+15U*2U,5U);
    result=evaluate(objects);
    assert(result.first_eligible && result.second_eligible);
    assert(!result.first_classified && !result.second_classified);

    set16(p,0U,0x40U+14U*2U,1U);
    set16(p,0U,0x40U+15U*2U,2U);
    objects=catalog_with(0,0,1,4);
    set16(p,0U,0x10U,0x0040U);
    result=evaluate(objects);
    assert(!result.first_eligible && !result.second_eligible);
    set16(p,0U,0x10U,0U);

    // Selected-hand action busy gate is +10B/+10F, and independent
    // from item-swap hand readiness at +19E/+1A2.
    set32(p,0U,0x19EU,0xFFFFFFFFU);
    set32(p,0U,0x1A2U,0xFFFFFFFFU);
    result=evaluate(objects);
    assert(result.first_eligible && !result.second_eligible);
    set32(p,0U,0x10BU,1U);
    result=evaluate(objects);
    assert(!result.first_eligible && !result.second_eligible);
    set32(p,0U,0x10BU,0U);

    // Ordinary scoped model never pretends to handle raw
    // pre-OBJECTS[-1] or original pseudo-hand special branches.
    bool rejected=false;
    set16(p,0U,0x40U+14U*2U,0xFFFFU);
    try {(void)evaluate(objects);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    set16(p,0U,0x40U+14U*2U,0x007EU);
    rejected=false;
    try {(void)evaluate(objects);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    set16(p,0U,0x40U+14U*2U,1U);
    rejected=false;
    try {
        (void)CombatHandPairEligibilityModel::evaluate(
            p,objects,4U,0U,CombatSingleHandContext{0U});
    } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);

    const auto before=p.bytes;
    (void)evaluate(objects);
    assert(p.bytes==before);
}
