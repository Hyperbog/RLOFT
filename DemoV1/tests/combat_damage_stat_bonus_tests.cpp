#include "rloft/combat_damage_stat_bonus.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <initializer_list>

namespace {

void set(rloft::PartyImage& party,std::size_t actor,
         std::size_t off,std::uint8_t value) {
    party.bytes.at(actor*rloft::PartyImage::kActorSize+off)=value;
}

} // namespace

int main() {
    using rloft::CombatDamageStatBonusModel;
    rloft::PartyImage party{};
    party.bytes.fill(0xFFU);
    const auto bonus=[&](std::size_t actor) {
        return CombatDamageStatBonusModel::inspect(party,actor);
    };

    // Only strength exactly 18 AND at least one signed class code
    // from {1,5,0,2} accesses the exceptional-percentage branch.
    set(party,0U,0x33U,18U);
    set(party,0U,0x15U,1U);
    for(const auto [percentage,expected] :
        {std::pair<int,int>{0,3},{75,3},{76,4},
         {90,4},{91,5},{99,5},{100,6},{255,6}}) {
        set(party,0U,0x3FU,static_cast<std::uint8_t>(percentage));
        assert(bonus(0U)==expected);
    }
    set(party,0U,0x15U,0xFFU);
    for(const auto code:{5U,0U,2U}) {
        set(party,0U,0x16U,static_cast<std::uint8_t>(code));
        assert(bonus(0U)==6);
    }

    // Raw signed -1 does NOT satisfy class0. Also class3/4
    // alone do not satisfy the exceptional branch.
    set(party,0U,0x16U,0xFFU);
    for(const auto code:{3U,4U}) {
        set(party,0U,0x15U,static_cast<std::uint8_t>(code));
        bool rejected=false;
        try {(void)bonus(0U);}
        catch(const std::invalid_argument&) {rejected=true;}
        assert(rejected);
        assert(CombatDamageStatBonusModel::inspect(
            party,0U,std::int8_t{-5})==-5);
    }

    // Strength other than 18 always uses original signed table,
    // even when a permitted class code would exist.
    set(party,0U,0x15U,1U);
    set(party,0U,0x33U,17U);
    assert(CombatDamageStatBonusModel::inspect(
        party,0U,std::int8_t{7})==7);
    set(party,0U,0x33U,19U);
    assert(CombatDamageStatBonusModel::inspect(
        party,0U,std::int8_t{-12})==-12);

    // Actor-specific data must not leak across party records.
    set(party,1U,0x33U,18U);
    set(party,1U,0x15U,5U);
    set(party,1U,0x3FU,76U);
    assert(bonus(1U)==4);
    assert(CombatDamageStatBonusModel::inspect(
        party,0U,std::int8_t{0})==0);

    bool rejected=false;
    const auto original=party.bytes;
    try {(void)bonus(4U);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    assert(party.bytes==original);
}
