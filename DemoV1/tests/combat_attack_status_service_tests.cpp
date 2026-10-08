#include "rloft/combat_attack_status_service.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace {

std::size_t at(std::size_t actor,std::size_t off) {
    return actor*rloft::PartyImage::kActorSize+off;
}

} // namespace

int main() {
    using rloft::CombatAttackStatusService;
    rloft::PartyImage party{};
    party.bytes.fill(0U);
    // The original CF5A is a byte array, not animation time.
    std::array<std::uint8_t,4U> redraw{{0U,4U,0U,7U}};

    // All four characters get their pending bit20h cleared.
    party.bytes[at(0,0x132U)]=0xA0U; // bit20 plus preserved bit80
    party.bytes[at(1,0x132U)]=0x21U; // bit20 plus preserved bit01
    party.bytes[at(3,0x132U)]=0x20U;

    // +133 bit02h cleanup happens ONLY to the selected attacker.
    party.bytes[at(2,0x133U)]=0xA2U;
    party.bytes[at(3,0x133U)]=0x02U;
    for(unsigned i=0U;i<4U;++i) {
        party.bytes[at(2,0x15AU+i)]=0xF0U;
        party.bytes[at(3,0x15AU+i)]=0xEEU;
    }

    CombatAttackStatusService::apply(party,redraw,2U);
    assert(party.bytes[at(0,0x132U)]==0x80U);
    assert(party.bytes[at(1,0x132U)]==0x01U);
    assert(party.bytes[at(2,0x132U)]==0U);
    assert(party.bytes[at(3,0x132U)]==0U);
    assert(party.bytes[at(2,0x133U)]==0xA0U);
    assert(party.bytes[at(3,0x133U)]==0x02U);
    for(unsigned i=0U;i<4U;++i) {
        assert(party.bytes[at(2,0x15AU+i)]==0U);
        assert(party.bytes[at(3,0x15AU+i)]==0xEEU);
    }
    assert(redraw[0]==1U && redraw[1]==1U);
    assert(redraw[2]==1U && redraw[3]==1U);

    // Existing dirty marks on unaffected actors are untouched.
    redraw={{8U,9U,10U,11U}};
    const auto snapshot=party.bytes;
    CombatAttackStatusService::apply(party,redraw,0U);
    assert(party.bytes==snapshot);
    assert((redraw==std::array<std::uint8_t,4U>{8U,9U,10U,11U}));

    // A subsequent attack by actor3 finally services its own
    // dormant +133 bit02h and +15A state, without touching others.
    CombatAttackStatusService::apply(party,redraw,3U);
    assert(party.bytes[at(3,0x133U)]==0U);
    for(unsigned i=0U;i<4U;++i)
        assert(party.bytes[at(3,0x15AU+i)]==0U);
    assert(redraw[3]==1U);
    assert(redraw[0]==8U && redraw[1]==9U && redraw[2]==10U);

    bool rejected=false;
    const auto final=party.bytes;
    const auto marks=redraw;
    try {CombatAttackStatusService::apply(party,redraw,4U);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    assert(party.bytes==final && redraw==marks);
}
