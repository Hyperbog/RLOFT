#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_numbered_effect_flags.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>

int main() {
    using rloft::OriginalNumberedEffectFlags;
    OriginalNumberedEffectFlags::Flags flags{};
    const auto mist=OriginalNumberedEffectFlags::inspect_1cd48(
        flags,2);
    assert(mist.should_execute);
    assert(mist.has_persistent_bit);
    assert(mist.persistent_bit_index==0x82U);
    assert(flags.at(0x82U/8U)==0U);
    OriginalNumberedEffectFlags::finish_1cd48(flags,2);
    assert(flags.at(0x82U/8U)==0x04U);
    assert(!OriginalNumberedEffectFlags::inspect_1cd48(
        flags,2).should_execute);
    OriginalNumberedEffectFlags::finish_1cd48(flags,2);
    assert(flags.at(0x82U/8U)==0x04U);

    // The other original event bits are distinct and retained.
    OriginalNumberedEffectFlags::finish_1cd48(flags,10);
    assert(flags.at(0x8AU/8U)==0x04U);
    assert(flags.at(0x82U/8U)==0x04U);
    const auto unnumbered=
        OriginalNumberedEffectFlags::inspect_1cd48(flags,-1);
    assert(unnumbered.should_execute);
    assert(!unnumbered.has_persistent_bit);
    const auto copy=flags;
    OriginalNumberedEffectFlags::finish_1cd48(flags,-1);
    assert(flags==copy);

    // 0xFF is the last representable event+0x80 bit.
    const auto last=OriginalNumberedEffectFlags::inspect_1cd48(
        flags,127);
    assert(last.persistent_bit_index==0xFFU);
    OriginalNumberedEffectFlags::finish_1cd48(flags,127);
    assert(flags.at(31U)==0x80U);

    for(const auto invalid:{-2,128,1000}) {
        bool rejected=false;
        try { (void)OriginalNumberedEffectFlags::inspect_1cd48(
            flags,invalid); }
        catch(const std::out_of_range&) { rejected=true; }
        assert(rejected);
    }
}
