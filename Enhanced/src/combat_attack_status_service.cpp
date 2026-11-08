#include "rloft/combat_attack_status_service.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

void CombatAttackStatusService::apply(
    PartyImage& party,
    std::array<std::uint8_t, PartyImage::kActorCount>& redraw_cf5a,
    std::size_t attacking_actor) {
    if (attacking_actor>=PartyImage::kActorCount) {
        throw std::out_of_range("CODE.2 E578 actor index");
    }

    // E578:E57C..E59C: service pending effect flag +132 bit 20h
    // for ALL four party characters, not just the active attacker.
    // Original CF5A byte for each affected panel becomes 1.
    for (std::size_t actor=0U;actor<PartyImage::kActorCount;++actor) {
        auto& flags=party.bytes[
            actor*PartyImage::kActorSize+0x132U];
        if ((flags&0x20U)!=0U) {
            redraw_cf5a[actor]=1U;
            flags=static_cast<std::uint8_t>(flags&~0x20U);
        }
    }

    // E578:E59E..E5CB: service only the selected actor's
    // +133 bit02h. Clear a separate original 4-byte state at
    // actor +15Ah (global 21A3A), clear that effect bit and
    // mark this actor's panel dirty.
    const auto base=attacking_actor*PartyImage::kActorSize;
    auto& selected_flags=party.bytes[base+0x133U];
    if ((selected_flags&0x02U)!=0U) {
        for (unsigned i=0U;i<4U;++i) {
            party.bytes[base+0x15AU+i]=0U;
        }
        selected_flags=static_cast<std::uint8_t>(
            selected_flags&~0x02U);
        redraw_cf5a[attacking_actor]=1U;
    }
}

} // namespace rloft
