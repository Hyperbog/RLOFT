#include "rloft/combat_single_hand_eligibility.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
void set_class(rloft::PartyImage& party,std::size_t actor,
               std::size_t lane,std::int8_t code) {
    party.bytes[actor*rloft::PartyImage::kActorSize+
                0x15U+lane]=static_cast<std::uint8_t>(code);
}
void set_level(rloft::PartyImage& party,std::size_t actor,
               std::size_t lane,std::int16_t level) {
    const auto at=actor*rloft::PartyImage::kActorSize+
                  0x20U+2U*lane;
    party.bytes[at]=static_cast<std::uint8_t>(level);
    party.bytes[at+1U]=static_cast<std::uint8_t>(
        static_cast<std::uint16_t>(level)>>8U);
}
void set_status(rloft::PartyImage& party,std::size_t actor,
                std::uint16_t status) {
    const auto at=actor*rloft::PartyImage::kActorSize+0x10U;
    party.bytes[at]=static_cast<std::uint8_t>(status);
    party.bytes[at+1U]=static_cast<std::uint8_t>(status>>8U);
}
rloft::ObjectCatalog objects() {
    constexpr std::size_t count=0x130U;
    std::vector<std::uint8_t> bytes(4U+count*0x41U,0U);
    bytes[0]=0x30U;
    bytes[1]=0x01U;
    const auto set_category=[&](std::size_t id,std::uint8_t kind) {
        bytes[4U+id*0x41U+0x2FU]=kind;
    };
    set_category(1U,9U);
    set_category(2U,10U);
    set_category(0x30U,2U);
    set_category(0x31U,11U);
    set_category(0x32U,9U);
    set_category(0x2EU,2U);
    set_category(0x59U,11U); // 23EE4 paired exception and attack item
    return rloft::ObjectCatalog::parse(bytes);
}

} // namespace

int main() {
    using rloft::CombatSingleHandContext;
    using rloft::CombatSingleHandEligibilityModel;
    const auto catalog=objects();
    rloft::PartyImage party{};
    party.bytes.fill(0xFFU);
    for(std::size_t actor=0U;actor<4U;++actor) {
        set_status(party,actor,0U);
        for(std::size_t lane=0U;lane<3U;++lane) {
            set_class(party,actor,lane,-1);
            set_level(party,actor,lane,0);
        }
    }
    const auto check=[&](std::size_t actor,int item,
                        std::uint8_t mode=0U) {
        return CombatSingleHandEligibilityModel::evaluate(
            party,catalog,actor,item,CombatSingleHandContext{mode});
    };
    // Default non-special item and weapon-category classification.
    auto r=check(0U,0x20);
    assert(r.eligible && !r.classified);
    r=check(0U,1);
    assert(r.eligible && r.classified);

    // CODE.2 240EC..241E3: class4 restrictions use 34060;
    // class5 combinations OR 34060 with 33FC4 only in one branch.
    set_class(party,0U,0U,4);
    assert(!check(0U,1).eligible);
    assert(check(0U,0x59).eligible); // 34060, actually classified
    assert(check(0U,0x31).eligible); // nonclassified, different path
    assert(!check(0U,0x30).eligible);
    set_class(party,0U,1U,5);
    assert(check(0U,0x30).eligible);
    assert(!check(0U,1).eligible);
    set_class(party,0U,2U,0);
    assert(check(0U,1).eligible); // class0 bypasses class4/5 gate
    set_class(party,0U,0U,-1);
    set_class(party,0U,1U,5);
    set_class(party,0U,2U,-1);
    assert(!check(0U,1).eligible);
    assert(check(0U,0x2E).eligible); // 33FC4 effect-special
    set_class(party,0U,2U,3);
    assert(check(0U,1).eligible); // class3 cancels class5 gate
    assert(!check(0U,2).eligible); // class3 AND category10 veto
    set_class(party,0U,2U,-1);

    // CODE.2 2428A..243D7 spell/class/level ranges.
    assert(!check(0U,0xA7).eligible);
    assert(!check(0U,0xCA).eligible);
    assert(!check(0U,0x12D).eligible);
    set_level(party,0U,1U,9);
    assert(!check(0U,0xA7).eligible);
    set_level(party,0U,1U,10);
    assert(check(0U,0xA7).eligible);
    assert(check(0U,0xC9).eligible);
    assert(check(0U,0xCA).eligible);
    assert(check(0U,0xE4).eligible);
    assert(check(0U,0x12D).eligible);
    set_level(party,0U,1U,0);
    set_class(party,0U,1U,4);
    assert(check(0U,0xA7).eligible);
    assert(!check(0U,0xCA).eligible);
    set_class(party,0U,1U,3);
    assert(!check(0U,0xA7).eligible);
    assert(check(0U,0xCA).eligible);
    assert(check(0U,0x12D).eligible);

    // CODE.2 2420D..24281: actor 2 and 3, mode FFh, first two
    // actor states not both 8000h. An ordinary weapon is vetoed
    // unless its OBJECTS record +35h low2 == 2, while a 23F50
    // hardcoded special bypasses the item-record flag gate.
    set_class(party,2U,0U,-1);
    assert(check(2U,1,0U).eligible);
    assert(!check(2U,1,0xFFU).eligible);
    assert(check(2U,0x2E,0xFFU).eligible);
    // Matching both first-actor status words bypasses that gate.
    set_status(party,0U,0x8000U);
    set_status(party,1U,0x8000U);
    assert(check(2U,1,0xFFU).eligible);
    set_status(party,0U,0U);
    set_status(party,1U,0U);

    // The +35h qualifier is sourced directly from OBJECTS.DAT.
    // Demonstrate it independently through a new tiny catalog.
    std::vector<std::uint8_t> raw(4U+3U*0x41U,0U);
    raw[0]=3U;
    raw[4U+1U*0x41U+0x2FU]=9U;
    raw[4U+1U*0x41U+0x35U]=0xFEU; // low2 == 2
    const auto altered=rloft::ObjectCatalog::parse(raw);
    assert(altered.at(1U).raw_byte_35_low2==2U);
    assert(CombatSingleHandEligibilityModel::evaluate(
        party,altered,2U,1,CombatSingleHandContext{0xFFU}
    ).eligible);

    // No synthetic semantics for original out-of-range object[-1]
    // raw memory read until its original pointer context is known.
    bool rejected=false;
    try {(void)check(0U,-1);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try {(void)check(4U,1);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try {(void)check(0U,0x130);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);

    // Eligibility calculation cannot mutate persisted character state.
    const auto old_party=party.bytes;
    (void)check(0U,1);
    assert(party.bytes==old_party);
}
