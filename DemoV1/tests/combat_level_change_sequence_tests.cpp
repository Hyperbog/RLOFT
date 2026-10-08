#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_level_change_sequence.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace {
void put16(rloft::PartyImage& p,std::size_t actor,
           std::size_t at,std::uint16_t word) {
    const auto k=actor*rloft::PartyImage::kActorSize+at;
    p.bytes[k]=static_cast<std::uint8_t>(word);
    p.bytes[k+1U]=static_cast<std::uint8_t>(word>>8U);
}
}

int main() {
    using rloft::CombatLevelChangeSequenceModel;
    using S=rloft::CombatLevelChangeService;
    rloft::PartyImage party{};
    const auto inspect=[&](std::size_t actor,std::size_t lane,
                           std::int16_t old_level,std::int16_t new_level) {
        return CombatLevelChangeSequenceModel::inspect(
            party,actor,lane,old_level,new_level);
    };
    party.bytes[0x15U]=3U;
    party.bytes[0x16U]=0xFFU;
    party.bytes[0x17U]=0xFFU;
    put16(party,0U,0x20U,2U);
    auto trace=inspect(0U,0U,1,2);
    assert(trace && trace->count==5U);
    assert(trace->selected_50dc_track==0);
    assert((trace->events==std::array<S,5>{{
        S::Select50DC,S::Class3DE4,S::Format3210F,
        S::Queue1C21C,S::HpIncreaseDC7C}}));
    assert(trace->actor==0U && trace->changed_track==0U);
    assert(trace->old_level==1 && trace->new_level==2);

    // If 50DC returns negative, presentation, timer and HP
    // services DO NOT disappear.
    party.bytes[0x15U]=0U;
    trace=inspect(0U,0U,1,2);
    assert(trace && trace->selected_50dc_track==-1);
    assert(trace->count==4U);
    assert(trace->events[0]==S::Select50DC);
    assert(trace->events[1]==S::Format3210F);
    assert(trace->events[2]==S::Queue1C21C);
    assert(trace->events[3]==S::HpIncreaseDC7C);

    // DDA4 class1 level9 special case: 50DC selects first
    // class lane, then DF3B calls 3EA4, NOT 3DE4.
    party.bytes[0x15U]=1U;
    put16(party,0U,0x20U,9U);
    trace=inspect(0U,0U,8,9);
    assert(trace && trace->count==5U);
    assert(trace->events[1]==S::Class3EA4);
    // Class3 on second lane also routes to 3EA4 if FIRST
    // class byte equals 1. The gate selection itself is 1.
    party.bytes[0x16U]=3U;
    put16(party,0U,0x22U,4U);
    trace=inspect(0U,1U,3,4);
    assert(trace && trace->selected_50dc_track==1);
    assert(trace->events[1]==S::Class3EA4);

    // Status exclusion suppresses only conditional class-service
    // calls. Original 3210F/1C21C/DC7C remain required.
    put16(party,0U,0x10U,0x4000U);
    trace=inspect(0U,1U,3,4);
    assert(trace && trace->count==4U && trace->selected_50dc_track==-1);
    assert(trace->events[3]==S::HpIncreaseDC7C);

    // The source snapshot must represent a REAL changed level
    // already stored, not arbitrary projected or stale state.
    assert(!inspect(0U,1U,4,4));
    assert(!inspect(0U,1U,3,9));
    assert(!inspect(0U,2U,3,4));
    assert(!inspect(4U,0U,1,2));
    assert(party.bytes[0x22U]==4U); // planner is read-only
}
