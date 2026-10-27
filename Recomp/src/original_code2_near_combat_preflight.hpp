#pragma once
// Preflight for a genuine 0D550 near-action that exhausts ORIGINAL 0D4E8
// target eligibility without entering still-unconnected hit/effect callbacks.
// Validation is performed before original 1BDAC/0D550 can mutate guest state.
// This checks a source-proven branch, not a substitute combat decision.
#include <cstddef>
#include <cstdint>

namespace rloft { namespace source_near_combat {
struct Validation {
    bool accepted;
    const char* reason;
};

inline Validation exhausted_targets(const std::uint8_t* data,
                                    std::size_t size,
                                    std::uint32_t record,
                                    std::uint32_t definition) noexcept {
    constexpr std::size_t needed=0x258C4u;
    if(!data || size<needed ||
       std::uint64_t(record)+0x16u>size ||
       std::uint64_t(definition)+0x3Bu>size)
        return {false,"ORIGINAL_0D550_NEAR_SOURCE_BOUNDS"};

    const auto word16=[&](std::uint32_t p)->std::uint32_t {
        return std::uint32_t(data[p]) | (std::uint32_t(data[p+1u])<<8u);
    };
    const auto word32=[&](std::uint32_t p)->std::uint32_t {
        return word16(p) | (word16(p+2u)<<16u);
    };
    const std::uint32_t type=word16(record+0x0Eu);
    const std::uint32_t state=word16(record+0x14u);
    if(type==15u || (type==8u && (state&0xC000u)==0x8000u))
        return {false,"ORIGINAL_0D550_NEAR_SPECIAL_ACTION_OPEN"};

    // Mirror 0D550's unsigned subtraction / sign-bit absolute difference,
    // followed by its signed JG comparison; don't reinterpret INT_MIN.
    const auto is_distant=[](std::uint32_t a,std::uint32_t b) noexcept -> bool {
        const std::uint32_t d=a-b;
        const std::uint32_t sign=0u-(d>>31u);
        const std::uint32_t magnitude=(d^sign)-sign;
        return static_cast<std::int32_t>(magnitude)>0x15000;
    };
    if(is_distant(word32(0x255D0u),word32(record+4u)) ||
       is_distant(word32(0x255D4u),word32(record)))
        return {false,"ORIGINAL_0D550_NEAR_ACTOR_DISTANCE_CULLED"};

    // 0D550 indexes the original 8-byte orientation-to-party table.
    // The original first maps through 0..7, then optionally resolves to
    // one of four party slots using the original 30CA2 random generator.
    for(unsigned i=0;i<8u;++i)
        if(data[0x687Eu+i]>7u)
            return {false,"ORIGINAL_0D550_NEAR_SOURCE_SLOT_MAP_INVALID"};

    // All possible final slots must be rejected by the EXACT three
    // short-circuit conditions in original function_0d4e8. This guarantees
    // no entry to 14A60/0C9A0/0CD44/0CBC8 on the admitted source path.
    for(std::uint32_t slot=0u;slot<4u;++slot) {
        const std::uint32_t base=slot*0x248u;
        const bool blocked=
            word16(0x218F0u+base)>=0x4000u ||
            (data[0x21A12u+base]&0x20u)!=0u ||
            ((data[0x21A13u+base]&0x02u)!=0u &&
             data[definition+0x1Du]<=4u &&
             (data[definition+0x3Au]&0x20u)!=0u);
        if(!blocked)
            return {false,"ORIGINAL_0D550_NEAR_ELIGIBLE_TARGET_CALLBACKS_OPEN"};
    }
    return {true,"ORIGINAL_0D550_NEAR_EXHAUSTED_TARGETS_SOURCE"};
}

struct EligibleHit {
    bool admitted;
    const char* reason;
    std::uint32_t slot;
    std::uint32_t d20;
};
// Read-only guard for original 0D550 -> 0C9A0. Replays ONLY the exact
// CODE.2 seed/dice arithmetic to predict whether downstream services
// are required. Live gameplay calls compiled 0D550/0C9A0/0EFA0/0CD44.
inline EligibleHit eligible_hit(const std::uint8_t* mem,std::size_t bytes,
                                std::uint32_t rec,std::uint32_t desc) noexcept {
    const auto reject=[](const char* reason) noexcept -> EligibleHit {
        return {false,reason,0u,0u};
    };
    if(!mem || bytes<0x258C4u ||
       std::uint64_t(rec)+0x16u>bytes ||
       std::uint64_t(desc)+0x45u>bytes)
        return reject("ORIGINAL_0D550_HIT_BOUNDS");
    const auto u16=[&](std::uint32_t p)->std::uint32_t {
        return std::uint32_t(mem[p])|(std::uint32_t(mem[p+1u])<<8u);
    };
    const auto u32=[&](std::uint32_t p)->std::uint32_t {
        return u16(p)|(u16(p+2u)<<16u);
    };
    const std::uint32_t type=u16(rec+0x0Eu), flags=u16(rec+0x14u);
    if(type==15u || (type==8u&&(flags&0xC000u)==0x8000u) ||
       std::uint64_t(u32(0x22230u))+std::uint64_t(type)*0x45u!=desc)
        return reject("ORIGINAL_0D550_HIT_SPECIAL_OR_DESCRIPTOR");
    const auto outside=[](std::uint32_t a,std::uint32_t b) noexcept {
        const std::uint32_t v=a-b, sign=0u-(v>>31u);
        return static_cast<std::int32_t>((v^sign)-sign)>0x15000;
    };
    if(outside(u32(0x255D0u),u32(rec+4u)) ||
       outside(u32(0x255D4u),u32(rec)))
        return reject("ORIGINAL_0D550_HIT_ACTOR_CULLED");

    const std::int32_t view=static_cast<std::int32_t>(u32(0xC658u))/45;
    const std::uint32_t dir=((flags<<10u)&0xFFFFu)>>13u;
    std::uint32_t slot=mem[0x687Eu+
        ((static_cast<std::uint32_t>(view)+dir)&7u)];
    if(slot>7u)return reject("ORIGINAL_0D550_HIT_SLOT_MAP");
    std::uint32_t rng=u32(0xF58Cu);
    const auto next_rng=[&]() noexcept -> std::uint32_t {
        rng=rng*0x41C64E6Du+0x3039u;
        return (rng>>16u)&0x7FFFu;
    };
    if(slot>=4u) {
        const std::uint32_t r=next_rng(), selector=slot-4u;
        slot=(r*2u)>>15u;
        if(selector==0u||selector==2u)++slot;
    }
    const auto blocked=[&](std::uint32_t s) noexcept {
        const std::uint32_t off=s*0x248u;
        return u16(0x218F0u+off)>=0x4000u ||
            (mem[0x21A12u+off]&0x20u)!=0u ||
            ((mem[0x21A13u+off]&2u)!=0u &&
             mem[desc+0x1Du]<=4u &&
             (mem[desc+0x3Au]&0x20u)!=0u);
    };
    if(mem[0x258A8u]!=0xFFu) {
        const std::uint32_t first=slot;
        while(blocked(slot)) {
            slot=(slot+3u)%4u;
            if(slot==first)return reject("ORIGINAL_0D550_HIT_ALL_TARGETS_BLOCKED");
        }
    } else {
        if(u16(0x218F0u)>=0x4000u&&u16(0x21B38u)>=0x4000u&&slot<2u)
            slot+=2u;
        if(blocked(slot)) {
            slot^=1u;
            if(blocked(slot))
                return reject("ORIGINAL_0D550_HIT_PAIRED_TARGETS_BLOCKED");
        }
    }
    if(slot>=4u)return reject("ORIGINAL_0D550_HIT_SLOT_RANGE");
    const std::uint32_t off=slot*0x248u;
    if((u32(0x21A12u+off)&0x40u)!=0u)
        return reject("ORIGINAL_0C9A0_0871C_MODAL_EFFECT_PENDING");
    if(u16(0x2193Cu+off)!=0xFFFFu || u16(0x2193Eu+off)!=0xFFFFu)
        return reject("ORIGINAL_0EFA0_EQUIPPED_HAND_CALLBACK_PENDING");
    if((mem[desc+0x39u]&~1u)!=0u ||
       (mem[desc+0x3Au]&~0x10u)!=0u)
        return reject("ORIGINAL_0D550_COMPLEX_STATUS_CALLBACK_PENDING");

    // 0EFA0 and original 0FE44 may follow item pointers. No made-up
    // inventory weights or callback results: exclude nested containers.
    const std::uint32_t items=u32(0x255D8u);
    for(std::uint32_t i=0u;i<0x2Au;i+=2u) {
        const std::int32_t id=static_cast<std::int16_t>(u16(0x21920u+off+i));
        if(id==-1)continue;
        if(id<0 || std::uint64_t(items)+std::uint64_t(id)*65u+0x41u>bytes)
            return reject("ORIGINAL_0EFA0_INVENTORY_OUT_OF_BOUNDS");
        if(mem[items+std::uint32_t(id)*65u+0x2Fu]==1u)
            return reject("ORIGINAL_0FE44_NESTED_CONTAINER_PENDING");
    }
    const std::uint32_t state=mem[0x21913u+off];
    if(state<2u || state>0x20u ||
       std::uint64_t(0x7E8Cu)+(std::uint64_t(state)+3u)*10u>bytes)
        return reject("ORIGINAL_0FE44_BURDEN_TABLE_INVALID");

    const auto valid_name=[&](std::uint32_t p) noexcept {
        if(std::uint64_t(p)+16u>bytes)return false;
        const std::size_t limit=(bytes-p-15u<0x400u)?bytes-p-15u:0x400u;
        for(std::size_t i=0u;i<limit;++i)
            if(mem[p+i]==0u)return true;
        return false;
    };
    if(!valid_name(0x584u))
        return reject("ORIGINAL_0D550_PARTY_SELECTOR_TOKEN_INVALID");
    for(std::uint32_t i=0u;i<4u;++i)
        if(!valid_name(0x218E0u+i*0x248u))
            return reject("ORIGINAL_0D550_PARTY_SELECTOR_NAME_INVALID");

    if((mem[desc+0x39u]&1u)!=0u || (mem[desc+0x3Au]&0x10u)!=0u) {
        const std::int32_t cls=static_cast<std::int8_t>(mem[0x218F5u+off]);
        const std::int32_t lvl=static_cast<std::int16_t>(u16(0x21900u+off));
        const std::int64_t table=0x6888ll+std::int64_t(cls)*0x5C+
                                 std::int64_t(lvl)*4ll;
        if(cls<0 || lvl<0 || table<0 || std::uint64_t(table)+4u>bytes)
            return reject("ORIGINAL_0CEE4_SAVE_CLASS_LEVEL_INVALID");
    }
    // A conservative upper bound prevents a callback-free lethal
    // 0CD44 -> 0AD24 -> 0CBC8 transition on this admitted path.
    const std::uint64_t count=mem[desc+0x1Eu];
    const std::uint64_t sides=mem[desc+0x1Fu];
    const std::uint64_t bonus=mem[desc+0x20u];
    const std::uint64_t maximum=count==0u?1u:
        (count*(sides+bonus)==0u?1u:count*(sides+bonus));
    const std::int32_t hp=static_cast<std::int16_t>(u16(0x218FCu+off));
    if(hp<=0 || maximum>=static_cast<std::uint64_t>(hp))
        return reject("ORIGINAL_0CD44_LETHAL_DAMAGE_PENDING");

    const std::uint32_t die=((next_rng()*20u)>>15u)+1u;
    return {true,"ORIGINAL_0D550_0C9A0_ELIGIBLE_SOURCE_HIT",slot,die};
}

}} // namespace rloft::source_near_combat
