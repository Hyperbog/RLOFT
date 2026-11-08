#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_xp_award_service.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace {
void write16(rloft::PartyImage& p,std::size_t actor,
             std::size_t off,std::uint16_t v) {
    const auto at=actor*rloft::PartyImage::kActorSize+off;
    p.bytes.at(at)=static_cast<std::uint8_t>(v);
    p.bytes.at(at+1U)=static_cast<std::uint8_t>(v>>8U);
}
void write32(rloft::PartyImage& p,std::size_t actor,
             std::size_t off,std::uint32_t v) {
    const auto at=actor*rloft::PartyImage::kActorSize+off;
    for(std::size_t i=0;i<4U;++i)
        p.bytes.at(at+i)=static_cast<std::uint8_t>(v>>(8U*i));
}
std::uint32_t read32(const rloft::PartyImage& p,std::size_t actor,
                     std::size_t off) {
    const auto at=actor*rloft::PartyImage::kActorSize+off;
    std::uint32_t out=0;
    for(unsigned i=0;i<4U;++i)
        out|=static_cast<std::uint32_t>(p.bytes.at(at+i))<<(8U*i);
    return out;
}
rloft::CombatProgressionTables tables() {
    rloft::CombatProgressionTables t{};
    for(auto& cls:t.thresholds_in_thousands) {
        for(std::size_t i=0;i<cls.size();++i)
            cls[i]=static_cast<std::uint16_t>(i*2U);
    }
    for(auto& n:t.attack_numerator)n=1;
    for(auto& d:t.attack_denominator)d=1;
    return t;
}
rloft::PartyImage party() {
    rloft::PartyImage p{};
    p.bytes.fill(0xFFU);
    // One single-class fighter, one two-track character, two
    // blocked actor slots. Original DDA4 F240 actor eligibility.
    for(unsigned a=0;a<2;++a) {
        write16(p,a,0x10U,0U);
        p.bytes[a*rloft::PartyImage::kActorSize+0x15U]=0U;
        p.bytes[a*rloft::PartyImage::kActorSize+0x16U]=0xFFU;
        p.bytes[a*rloft::PartyImage::kActorSize+0x17U]=0xFFU;
        write16(p,a,0x20U,1U);
        write32(p,a,0x26U,100U+a);
    }
    p.bytes[rloft::PartyImage::kActorSize+0x16U]=1U;
    write16(p,1U,0x22U,1U);
    write32(p,1U,0x2AU,301U);
    return p;
}
}

int main() {
    using rloft::CombatXpAwardService;
    const auto table=tables();
    auto p=party();
    const auto before=p;
    // Original two eligible actors each receive (200+1)/2=100;
    // second actor's two classes each receive (100+1)/2=50.
    auto reward=CombatXpAwardService::apply(p,200U,table);
    assert(reward);
    assert(reward->distribution.eligible_party_members==2U);
    assert(reward->distribution.rounded_party_share==100U);
    assert(reward->updated_tracks==3U);
    assert(!reward->level_change_services_pending);
    assert(read32(p,0U,0x26U)==200U);
    assert(read32(p,1U,0x26U)==151U);
    assert(read32(p,1U,0x2AU)==351U);
    assert(p.bytes[0x20U]==1U); // no synthetic class-level change
    // Every non-XP byte must remain identical, including a blocked
    // actor's full record and original live actor condition/effects.
    const auto mutated=p;
    for(std::size_t i=0;i<p.bytes.size();++i) {
        constexpr std::size_t lane0=0x26U;
        constexpr std::size_t lane1=rloft::PartyImage::kActorSize+0x26U;
        constexpr std::size_t lane2=rloft::PartyImage::kActorSize+0x2AU;
        if((i>=lane0 && i<lane0+4U) ||
           (i>=lane1 && i<lane1+4U) ||
           (i>=lane2 && i<lane2+4U))continue;
        assert(mutated.bytes[i]==before.bytes[i]);
    }

    // A party at the original threshold receives its exact class XP
    // while the not-yet-reconstructed 50DC/3EA4 callbacks are
    // advertised as PENDING. No fake HP, levels or base THAC0.
    auto ascending=party();
    write32(ascending,0U,0x26U,1950U);
    reward=CombatXpAwardService::apply(ascending,200U,table);
    assert(reward && reward->level_change_services_pending);
    assert(read32(ascending,0U,0x26U)==2050U);
    assert(ascending.bytes[0x20U]==1U);
    assert(ascending.bytes[0x21U]==0U);

    // Unsupported class bytes reject the WHOLE award BEFORE even the
    // first participating actor has its XP changed.
    auto unsupported=party();
    unsupported.bytes[rloft::PartyImage::kActorSize+0x16U]=0xFEU;
    const auto invalid_before=unsupported.bytes;
    assert(!CombatXpAwardService::apply(unsupported,200U,table));
    assert(unsupported.bytes==invalid_before);

    // DDA4 with zero participating actors is an x86 DIV0 condition;
    // never fabricate an award or change original party bytes.
    auto empty=party();
    write16(empty,0U,0x10U,0x8000U);
    write16(empty,1U,0x10U,0x8000U);
    const auto empty_bytes=empty.bytes;
    assert(!CombatXpAwardService::apply(empty,200U,table));
    assert(empty.bytes==empty_bytes);

    // Original unsigned 32-bit ADD wraps, not C++ signed overflow.
    auto wrapping=party();
    write32(wrapping,0U,0x26U,0xFFFFFFFFU);
    reward=CombatXpAwardService::apply(wrapping,2U,table);
    assert(reward);
    assert(read32(wrapping,0U,0x26U)==0U);
    assert(wrapping.bytes[0x20U]==1U);
}
