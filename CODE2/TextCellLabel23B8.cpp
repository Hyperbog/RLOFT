#include "TextCellLabel23B8.h"

namespace ravenloft::game {
namespace {
std::int16_t s16(std::span<const std::uint8_t> s,std::size_t p) noexcept {
 return static_cast<std::int16_t>(static_cast<std::uint16_t>(s[p])|
   (static_cast<std::uint16_t>(s[p+1u])<<8u));
}
std::int32_t div8Trunc(std::int32_t v) noexcept {
 return v/8; // C++ signed division truncates toward zero, matching the sequence.
}
}

std::int32_t TextCellLabel23B8::run(const TextCellLabel23B8State& s,
                                    std::int32_t column,
                                    std::int32_t row,
                                    std::uint8_t* out) {
    std::uint8_t first='A';
    std::uint8_t second=' ';
    std::int32_t found=0;
    std::size_t p=0;

    for(;;) {
        const std::int16_t count=s16(s.records5760,p+4u);
        if(count==0) break;

        const std::int32_t xCell=div8Trunc(s16(s.records5760,p));
        const std::int32_t yCell=s16(s.records5760,p+2u)/5;
        if(xCell==column && yCell==row) {
            found=1;
            break;
        }

        if(first=='Z') {
            first='A';
            if(second==' ') second='A';
            else ++second;
        } else {
            ++first;
        }

        p+=6u+static_cast<std::size_t>(count);
    }

    out[0]=first;
    out[1]=second;
    return found;
}

} // namespace ravenloft::game
