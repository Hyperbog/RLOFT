#include "TextRecords1C08.h"

namespace ravenloft::game {
namespace {
std::int16_t s16(std::span<std::uint8_t> s,std::size_t o) noexcept {
 return static_cast<std::int16_t>(
   static_cast<std::uint16_t>(s[o]) |
   (static_cast<std::uint16_t>(s[o+1u])<<8u));
}
}

std::size_t TextRecords1C08::endOffset(const TextRecords1C08State& s) {
    std::size_t p=0;
    if(s16(s.records5760,4)==0) return p;
    do {
        p+=6u;
        p=static_cast<std::size_t>(
            static_cast<std::ptrdiff_t>(p)+s16(s.records5760,p-2u));
    } while(s16(s.records5760,p+4u)!=0);
    return p;
}

std::size_t TextRecords1C08::usedBytes(const TextRecords1C08State& s) {
    return endOffset(s);
}

void TextRecords1C08::draw(const TextRecords1C08Ops& o,
                           const std::uint8_t* text,
                           std::int32_t arg1,
                           std::int32_t count) {
    const std::int32_t x0=o.call3058C();
    const std::int32_t y0=o.call30594();
    if(text==nullptr || *text==0 || count==0) return;

    while(*text!=0 && count!=0) {
        const std::int8_t ch=static_cast<std::int8_t>(*text);
        if(ch==0x0D) {
            o.call304F4(x0+0x0C,y0);
        } else {
            o.call30354(ch,arg1);
        }
        ++text;
        --count;
    }
}

std::int32_t TextRecords1C08::width(const TextRecords1C08Ops& o,
                                    const std::uint8_t* text,
                                    std::int32_t arg1) {
    std::int32_t total=0;
    if(text==nullptr) return total;
    while(*text!=0) {
        const std::uint8_t ch=*text++;
        if(ch==0x0D) {
            total=0;
        } else {
            total+=o.call30318(static_cast<std::int8_t>(ch),arg1);
        }
    }
    return total;
}

} // namespace ravenloft::game
