#include "rloft/combat_attack_timing.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

void set16(rloft::PartyImage& party,std::size_t offset,
           std::uint16_t value) {
    party.bytes.at(offset)=static_cast<std::uint8_t>(value);
    party.bytes.at(offset+1U)=static_cast<std::uint8_t>(value>>8U);
}
void set32(rloft::PartyImage& party,std::size_t offset,
           std::uint32_t value) {
    for(unsigned i=0;i<4U;++i)
        party.bytes.at(offset+i)=
            static_cast<std::uint8_t>(value>>(i*8U));
}
rloft::ObjectCatalog catalog() {
    std::vector<std::uint8_t> bytes(4U+0x88U*0x41U,0U);
    bytes[0]=0x88U;
    const auto set=[&](unsigned id,std::uint16_t word) {
        const auto at=4U+id*0x41U+0x35U;
        bytes.at(at)=static_cast<std::uint8_t>(word);
        bytes.at(at+1U)=static_cast<std::uint8_t>(word>>8U);
    };
    set(1,0x0019U); // original SPEAR-like: qualifier1, interval54
    set(2,0x0008U); // original throwing knife-like: interval18
    set(3,0x001EU); // original BOW-like: qualifier2, interval63
    set(4,0xFFFFU); // only bits2..5 enter timing: interval135
    set(5,0U);      // no imposed positive minimum: interval0
    set(6,0xF003U); // qualifier3; bits2..5=0, interval0
    return rloft::ObjectCatalog::parse(bytes);
}

} // namespace

int main() {
    using rloft::CombatAttackTimingModel;
    rloft::PartyImage party{};
    party.bytes.fill(0U);
    const auto objects=catalog();
    set32(party,0x132U,0U);
    const auto plan=[&](unsigned hand,bool hit,
                        std::uint32_t now=100U,
                        std::int32_t damage=7) {
        return CombatAttackTimingModel::inspect(
            party,objects,0U,hand,now,hit,damage);
    };

    // E5F0:1E9E9 unarmed and pseudo spell hand are exactly 18,
    // not a modern fixed FPS. Both hands have their own slot.
    set16(party,0x40U+14U*2U,0xFFFFU);
    set16(party,0x40U+15U*2U,0x0086U);
    auto p=plan(0U,true);
    assert(p.animation_interval==18U);
    assert(p.hand_ready_deadline==109U);
    assert(p.stored_damage==7);
    p=plan(1U,true);
    assert(p.animation_interval==18U && p.hand_ready_deadline==109U);

    // E5F0:1E9F5 full word +35h -> bits 2..5 -> times nine.
    for(const auto item : {1U,2U,3U,4U,5U,6U}) {
        set16(party,0x40U+14U*2U,
              static_cast<std::uint16_t>(item));
        p=plan(0U,true);
        const std::uint32_t expected[] = {0U,54U,18U,63U,
                                           135U,0U,0U};
        assert(p.animation_interval==expected[item]);
        assert(p.hand_ready_deadline==
               100U+expected[item]/2U);
    }
    assert(objects.at(1U).raw_word_35==0x0019U);
    assert(objects.at(1U).raw_byte_35_low2==1U);
    assert(objects.at(3U).raw_word_35==0x001EU);
    assert(objects.at(3U).raw_byte_35_low2==2U);

    // E5F0:1EA15 flags may halve the original duration. With
    // duration63, integer division produces31, deadline+15.
    set16(party,0x40U+14U*2U,3U);
    set32(party,0x132U,0x00000010U);
    p=plan(0U,true);
    assert(p.animation_interval==31U);
    assert(p.hand_ready_deadline==115U);
    set32(party,0x132U,0x01000000U);
    p=plan(0U,true);
    assert(p.animation_interval==31U);
    set32(party,0x132U,0x00000001U);
    p=plan(0U,true);
    assert(p.animation_interval==63U);
    set32(party,0x132U,0U);

    // On miss/no-target, E5F0 bypasses BOTH deadline and damage
    // assignments, while retaining the action animation argument.
    p=plan(0U,false,100U,-20);
    assert(p.animation_interval==63U);
    assert(!p.hand_ready_deadline && !p.stored_damage);
    p=plan(0U,true,0xFFFFFFFDU,-12);
    assert(p.hand_ready_deadline==28U); // unsigned wrap +31
    assert(p.stored_damage==-12);

    // Planning never modifies original character bytes or A7E4.
    const auto original=party.bytes;
    (void)plan(0U,true);
    assert(party.bytes==original);

    bool rejected=false;
    try { (void)CombatAttackTimingModel::inspect(
        party,objects,4U,0U,0U,false,0); }
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try { (void)plan(2U,false); }
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    set16(party,0x40U+14U*2U,0xFFFEU);
    rejected=false;
    try { (void)plan(0U,false); }
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
}
