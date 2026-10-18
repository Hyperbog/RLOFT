#pragma once
// CODE.2 object-1 0x37700..0x3780D: instruction-derived bounded recovery.
//
// Crucial correction: 3772E / 3774D / 37785 / 37797 test *guest byte*
// [2B6C2h], NOT a value passed to the function. Source 2FA30 sets this
// flag when the declared output has been collected. The decoder returns on
// the same four checks. Strict byte/dictionary bounds are added for host
// safety; the genuine DOS source has no explicit malformed-input outcome.
//
// No original copyrighted bytes included.
#include <array>
#include <cstddef>
#include <cstdint>

namespace rloft::code2source {
enum class Decode37700 {
    completed,
    truncated_stream,
    uninitialized_dictionary,
    output_limit,
};
struct Decoded37700 {
    Decode37700 result=Decode37700::truncated_stream;
    std::size_t bytes_in=0, bytes_out=0;
};

template <typename Input, typename Output, typename Stop>
Decoded37700 function_37700_stream(Input&& input,Output&& output,Stop&& stopped,
                                   std::size_t max_output) {
    std::array<std::int16_t,4096> dictionary{};
    dictionary.fill(-1);
    for(unsigned i=0;i<0xFEEu;++i) dictionary[i]=0xFE;
    unsigned write_pos=0xFEEu, flags=0u;
    Decoded37700 result{};
    auto in=[&](std::uint8_t& value)->bool {
        if(!input(value))return false;
        ++result.bytes_in;
        return true;
    };
    auto out=[&](std::uint8_t value)->bool {
        // 2FA30 may set guest 2B6C2 on this emitted byte.
        if(result.bytes_out>=max_output)return false;
        output(value);
        ++result.bytes_out;
        return true;
    };
    while(true) {
        flags>>=1u;                             // 37724
        if((flags&0x100u)==0u) {                 // 37727
            if(stopped()) {result.result=Decode37700::completed;return result;} // 3772E
            std::uint8_t byte=0;
            if(!in(byte))return result;
            flags=unsigned(byte)|0xFF00u;
        }
        if((flags&1u)!=0u) {
            if(stopped()) {result.result=Decode37700::completed;return result;} // 3774D
            std::uint8_t value=0;
            if(!in(value))return result;
            if(!out(value)){result.result=Decode37700::output_limit;return result;}
            dictionary[write_pos]=value;
            write_pos=(write_pos+1u)&0xFFFu;
        } else {
            if(stopped()) {result.result=Decode37700::completed;return result;} // 37785
            std::uint8_t lo=0,hi=0;
            if(!in(lo))return result;
            if(stopped()) {result.result=Decode37700::completed;return result;} // 37797
            if(!in(hi))return result;
            const unsigned start=unsigned(lo)|((unsigned(hi)&0xF0u)<<4u);
            const unsigned final=(unsigned(hi)&0x0Fu)+2u;
            // 377C3..37803: CX is zero and JLE compares inclusive.
            for(unsigned i=0;i<=final;++i) {
                const std::int16_t v=dictionary[(start+i)&0xFFFu];
                if(v<0){result.result=Decode37700::uninitialized_dictionary;return result;}
                if(!out(static_cast<std::uint8_t>(v))){
                    result.result=Decode37700::output_limit;return result;
                }
                dictionary[write_pos]=v;
                write_pos=(write_pos+1u)&0xFFFu;
            }
        }
    }
}
} // namespace rloft::code2source
