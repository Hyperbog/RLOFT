#include "rloft/combat_xp_distribution.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rloft {
namespace {

std::uint16_t read_u16(const PartyImage& party,std::size_t at) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(party.bytes[at]) |
        (static_cast<std::uint16_t>(party.bytes[at+1U])<<8U));
}

std::uint32_t read_u32(const PartyImage& party,std::size_t at) noexcept {
    return static_cast<std::uint32_t>(party.bytes[at]) |
        (static_cast<std::uint32_t>(party.bytes[at+1U])<<8U) |
        (static_cast<std::uint32_t>(party.bytes[at+2U])<<16U) |
        (static_cast<std::uint32_t>(party.bytes[at+3U])<<24U);
}

bool eligible(const PartyImage& party,std::size_t actor) noexcept {
    // DDA4: DDAF..DDCE and DE0F..DE21, not merely HP>0.
    return (read_u16(party,actor*PartyImage::kActorSize+0x10U)&
        0xF240U)==0U;
}

} // namespace

std::optional<CombatXpDistributionPlan>
CombatXpDistributionModel::inspect(
    const PartyImage& party,
    std::uint16_t npc_definition_award) noexcept {
    CombatXpDistributionPlan out{};
    out.definition_award=npc_definition_award;
    for (std::size_t i=0U;i<PartyImage::kActorCount;++i) {
        if (eligible(party,i)) {
            ++out.eligible_party_members;
        }
    }
    // DDD8 x86 IDIV has no zero-eligible guard: fail closed instead
    // of claiming the original silently skips an award.
    if (out.eligible_party_members==0U) {
        return std::nullopt;
    }
    const auto n=static_cast<std::uint32_t>(out.eligible_party_members);
    // DDD0..DDEF: signed, nonnegative NPC award (a zero-extended WORD)
    // plus half the count, divided by the count; min quotient one.
    auto share=(static_cast<std::uint32_t>(npc_definition_award)+(n>>1U))/n;
    if (share==0U) share=1U;
    out.rounded_party_share=share;
    for (std::size_t i=0U;i<PartyImage::kActorCount;++i) {
        if (!eligible(party,i)) continue;
        auto& actor=out.actors[i];
        actor.eligible=true;
        const auto base=i*PartyImage::kActorSize;
        // DE27..DE5B: first class track is unconditional. Additional
        // tracks count if signed class byte is anything except FFh.
        auto count=1U;
        if (party.bytes[base+0x16U]!=0xFFU) ++count;
        if (party.bytes[base+0x17U]!=0xFFU) ++count;
        actor.class_track_count=static_cast<std::uint8_t>(count);
        auto each=(share+(count>>1U))/count;
        if (each==0U) each=1U;
        actor.per_track_award=each;
        for (std::size_t track=0U;track<count;++track) {
            // DEBC..DEC2: a DWORD ADD on each occupied track, before
            // calculating its class level and possible side effects.
            actor.projected_track_progress[track]=
                read_u32(party,base+0x26U+track*4U)+each;
        }
    }
    return out;
}

} // namespace rloft
