#include "rloft/combat_hand_predicates.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <vector>

int main() {
    using rloft::CombatHandPredicates;
    rloft::PartyImage party{};
    party.bytes.fill(0xFFU);
    // Three original *signed* class-code bytes at 15h, 16h, 17h.
    party.bytes[0x15U] = 0U;
    party.bytes[0x16U] = 2U;
    party.bytes[0x17U] = 5U;
    assert(CombatHandPredicates::has_class(party,0U,0));
    assert(CombatHandPredicates::has_class(party,0U,2));
    assert(CombatHandPredicates::has_class(party,0U,5));
    assert(!CombatHandPredicates::has_class(party,0U,1));
    assert(CombatHandPredicates::has_class(party,1U,-1));
    assert(!CombatHandPredicates::has_class(party,1U,255));

    constexpr std::array<int,6> exceptions{
        0x2B,0x59,0x68,0x9F,0x102,0x103
    };
    for(const auto id:exceptions) {
        assert(CombatHandPredicates::is_pair_exception(id));
    }
    for(const auto id:{-1,0,0x2A,0x30,0x58,0x67,0x69,0x101,0x104}) {
        assert(!CombatHandPredicates::is_pair_exception(id));
    }

    std::vector<std::uint8_t> data(4U+0x105U*0x41U,0U);
    data[0]=0x05U;
    data[1]=0x01U;
    data[4U+1U*0x41U+0x2FU]=9U;
    data[4U+2U*0x41U+0x2FU]=10U;
    data[4U+3U*0x41U+0x2FU]=11U;
    data[4U+4U*0x41U+0x2FU]=8U;
    const auto catalog=rloft::ObjectCatalog::parse(data);
    for(const auto id:{1,2,3,0x30,0x7E,0x86}) {
        assert(CombatHandPredicates::is_attack_hand_item(id,catalog));
    }
    for(const auto id:{0,4,0x2B,0x7D,0x87}) {
        assert(!CombatHandPredicates::is_attack_hand_item(id,catalog));
    }

    bool threw=false;
    try {(void)CombatHandPredicates::is_attack_hand_item(-1,catalog);}
    catch(const std::out_of_range&) {threw=true;}
    assert(threw);
    threw=false;
    try {(void)CombatHandPredicates::is_attack_hand_item(0x105,catalog);}
    catch(const std::out_of_range&) {threw=true;}
    assert(threw);
    threw=false;
    try {(void)CombatHandPredicates::has_class(party,4U,0);}
    catch(const std::out_of_range&) {threw=true;}
    assert(threw);

    // All original party bytes remain unchanged through predicates.
    const auto old=party.bytes;
    (void)CombatHandPredicates::has_class(party,0U,0);
    assert(party.bytes==old);
}
