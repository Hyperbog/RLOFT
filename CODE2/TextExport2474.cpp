#include "TextExport2474.h"
#include <array>
#include <vector>

namespace ravenloft::game {
namespace {
std::int16_t s16(std::span<const std::uint8_t> s,std::size_t p) noexcept {
 return static_cast<std::int16_t>(static_cast<std::uint16_t>(s[p])|
  (static_cast<std::uint16_t>(s[p+1u])<<8u));
}
void advance(std::uint8_t& a,std::uint8_t& b) noexcept {
 if(a=='Z'){a='A'; if(b==' ') b='A'; else ++b;} else ++a;
}
}

void TextExport2474::run(const TextExport2474State& s,const TextExport2474Ops& o,
                         std::int32_t handle,std::int32_t alt) {
    std::uint8_t first='A', second=' ';
    std::array<std::uint8_t,0x4F> line{};
    std::size_t p=0;

    while(s16(s.records5760,p+4u)!=0) {
        o.call30D30(line.data(),0x20,0x4F);
        line[0]=first; line[1]=second; line[2]=':';
        advance(first,second);

        const std::int32_t count=s16(s.records5760,p+4u);
        std::size_t out=4;
        bool textSinceBreak=false;
        const std::uint8_t* text=s.records5760.data()+p+6u;

        for(std::int32_t i=0;i<count;++i) {
            const std::uint8_t ch=text[i];
            if(ch==0x0D) {
                if(alt==0) {
                    for(std::size_t q=0;q<0x4F;++q) o.call31107(line[q],handle);
                    o.call31107(s.value5F60,handle);
                    o.call31107(s.value5F61,handle);
                } else {
                    o.call31304(line.data(),0x4F,alt);
                    const std::uint8_t nl[2]={s.value5F60,s.value5F61};
                    o.call31304(nl,2,alt);
                }
                o.call30D30(line.data(),0x20,0x4F);
                out=4;
                textSinceBreak=false;
            } else {
                line[out++]=ch;
                textSinceBreak=true;
            }
        }

        if(alt==0) {
            if(textSinceBreak) {
                for(std::size_t q=0;q<0x4F;++q) o.call31107(line[q],handle);
                o.call31107(s.value5F60,handle);
                o.call31107(s.value5F61,handle);
            }
            o.call31107(s.value5F60,handle);
            o.call31107(s.value5F61,handle);
        } else {
            if(textSinceBreak) {
                o.call31304(line.data(),0x4F,alt);
                const std::uint8_t nl[2]={s.value5F60,s.value5F61};
                o.call31304(nl,2,alt);
            }
            std::uint8_t lf=0x0A;
            o.call31304(&lf,1,alt);
        }
        p+=6u+static_cast<std::size_t>(count);
    }

    if(alt==0) {
        o.call31107(s.value5F60,handle);
        o.call31107(s.value5F61,handle);
    } else {
        const std::uint8_t nl[2]={s.value5F60,s.value5F61};
        o.call31304(nl,2,alt);
    }
}

} // namespace ravenloft::game
