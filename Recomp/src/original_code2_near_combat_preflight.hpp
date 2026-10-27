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
    const auto far=[](std::uint32_t a,std::uint32_t b) noexcept -> bool {
        const std::uint32_t d=a-b;
        const std::uint32_t sign=0u-(d>>31u);
        const std::uint32_t magnitude=(d^sign)-sign;
        return static_cast<std::int32_t>(magnitude)>0x15000;
    };
    if(far(word32(0x255D0u),word32(record+4u)) ||
       far(word32(0x255D4u),word32(record)))
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
}} // namespace rloft::source_near_combat
