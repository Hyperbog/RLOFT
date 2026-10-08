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
        // Strength 18 without a qualifying class uses the
        // original shipped signed table at index 18: +2.
        assert(bonus(0U)==2);
        assert(CombatDamageStatBonusModel::inspect(
            party,0U,std::int8_t{2})==2);
        bool rejected=false;
        try {
            (void)CombatDamageStatBonusModel::inspect(
                party,0U,std::int8_t{-5});
        } catch(const std::invalid_argument&) {rejected=true;}
        assert(rejected);
    }

    // Strength other than 18 always uses original signed table,
    // even when a permitted class code would exist.
    set(party,0U,0x15U,1U);
    set(party,0U,0x33U,17U);
    assert(bonus(0U)==1);
    set(party,0U,0x33U,19U);
    assert(bonus(0U)==7);
    // Exhaust the 26 original DGROUP signed bytes, including
    // the unusual score-0 entry and score-25 high bound.
    constexpr std::int8_t original[26]={
        7,-4,-2,-1,-1,-1,0,0,0,0,0,0,0,
        0,0,0,1,1,2,7,8,9,10,11,12,14
    };
    set(party,0U,0x15U,0xFFU);
    for(unsigned strength=0U;strength<26U;++strength) {
        set(party,0U,0x33U,static_cast<std::uint8_t>(strength));
        assert(bonus(0U)==original[strength]);
    }
    // Original indexed reads for invalid scores proceed into
    // adjacent DGROUP. Preserve the unknown-memory dependency.
    for(const auto invalid:{26U,42U,255U}) {
        set(party,0U,0x33U,
            static_cast<std::uint8_t>(invalid));
        bool rejected=false;
        try {(void)bonus(0U);}
        catch(const std::invalid_argument&) {rejected=true;}
        assert(rejected);
        assert(CombatDamageStatBonusModel::inspect(
            party,0U,std::int8_t{-3})==-3);
    }

    // Actor-specific data must not leak across party records.
    set(party,1U,0x33U,18U);
    set(party,1U,0x15U,5U);
    set(party,1U,0x3FU,76U);
    assert(bonus(1U)==4);
    // Actor0 retains the deliberately invalid strength 255,
    // for which only the caller-observed adjacent byte is valid.
    assert(CombatDamageStatBonusModel::inspect(
        party,0U,std::int8_t{0})==0);

    bool rejected=false;
    const auto original=party.bytes;
    try {(void)bonus(4U);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    assert(party.bytes==original);
}
