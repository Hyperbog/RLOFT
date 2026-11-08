#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

// Original CODE.2 1CD48/1CE2B dispatcher over the mutable bit
// vector DGROUP:22200, using original 1C1DC and 1CE2B tests.
// This models the original event gate only, NOT the audiovisual
// work performed by 1C924/1CA2C/1C77C.
struct NumberedEffectGate final {
    bool should_execute{};
    bool has_persistent_bit{};
    std::uint16_t persistent_bit_index{};
};

class OriginalNumberedEffectFlags final {
public:
    using Flags=std::array<std::uint8_t,32>;

    // CODE.2 1CD48 takes event number, adds 0x80 unless -1,
    // consults 1C1DC, bypasses effect body when the bit is set.
    [[nodiscard]] static NumberedEffectGate inspect_1cd48(
        const Flags& flags, int event_number) {
        if(event_number==-1)
            return {true,false,0U};
        // Authored 0..127 events fit the known 32-byte DGROUP vector;
        // do not emulate unverified out-of-bounds DOS memory aliases.
        if(event_number<0 || event_number>127)
            throw std::out_of_range("1CD48 event bit exceeds original flags");
        const auto bit=static_cast<std::uint16_t>(event_number+0x80);
        const auto mask=static_cast<std::uint8_t>(
            1U << (bit&7U));
        const auto present=
            (flags[static_cast<std::size_t>(bit>>3U)]&mask)!=0U;
        return {!present,true,bit};
    }

    // 1CE2B..1CE63 runs after the dispatch/skip branch, setting
    // original numbered event+0x80. -1 deliberately has no bit.
    static void finish_1cd48(
        Flags& flags, int event_number) {
        const auto gate=inspect_1cd48(flags,event_number);
        if(!gate.has_persistent_bit) return;
        const auto bit=gate.persistent_bit_index;
        flags[static_cast<std::size_t>(bit>>3U)] |=
            static_cast<std::uint8_t>(1U << (bit&7U));
    }
};

} // namespace rloft
