#include "TextInput1CE4.h"

#include <cstddef>

namespace ravenloft::game {
namespace {
std::int16_t get16(std::span<std::uint8_t> s,std::size_t p) noexcept {
 return static_cast<std::int16_t>(static_cast<std::uint16_t>(s[p]) |
   (static_cast<std::uint16_t>(s[p+1u])<<8u));
}
void put16(std::span<std::uint8_t> s,std::size_t p,std::int16_t v) noexcept {
 const auto u=static_cast<std::uint16_t>(v);
 s[p]=static_cast<std::uint8_t>(u);
 s[p+1u]=static_cast<std::uint8_t>(u>>8u);
}
std::size_t endRecord(std::span<std::uint8_t> s) noexcept {
 std::size_t p=0;
 if(get16(s,4)==0) return p;
 do { p+=6u; p=static_cast<std::size_t>(
      static_cast<std::ptrdiff_t>(p)+get16(s,p-2u)); }
 while(get16(s,p+4u)!=0);
 return p;
}
}

void TextInput1CE4::run(TextInputState1CE4& s,const TextInputOps1CE4& o,
                        std::int32_t arg0,std::int32_t arg1,
                        std::int32_t arg2,std::int32_t arg3) {
    std::int32_t key=0;
    std::int32_t active=1;
    std::int32_t lines=0;

    std::size_t rec=endRecord(s.records5760);
    if(rec>=0x7EEu) return;

    const std::int16_t rx=static_cast<std::int16_t>(arg0+
        static_cast<std::int16_t>(s.value12744*8));
    const std::int16_t ry=static_cast<std::int16_t>(arg1+
        static_cast<std::int16_t>(s.value12740*5));
    put16(s.records5760,rec,rx);
    put16(s.records5760,rec+2u,ry);
    put16(s.records5760,rec+4u,0);
    std::size_t text=rec+6u;
    s.records5760[text]=0;

    while(key!=0x1B) {
        const std::int32_t maxLines=arg2-1;
        const std::int32_t alignedY=arg1&~7;
        const std::int32_t invY=0x140-arg1;
        const std::int32_t linePixels=arg2*0x0C;

        if(active!=0) {
            o.call2A380(0,arg0,alignedY,arg3);
            o.call304F4(arg0,arg1);
            o.call1C50(s.records5760.data()+text,1,get16(s.records5760,rec+4u));
            o.call30354(0x7C,1);
            o.call1A8C8(s.valueC050,arg0,arg1,linePixels,invY);
            active=0;
        }

        key=o.call30AEC();
        if(key==8) {
            auto n=get16(s.records5760,rec+4u);
            if(n==0) continue;
            put16(s.records5760,rec+4u,static_cast<std::int16_t>(n-1));
            s.records5760[text+static_cast<std::size_t>(n-1)]=0;
            active=1;
            continue;
        }

        if(key==0x0D) {
            if(endRecord(s.records5760)>=0x7EEu) continue;
            if(lines<maxLines) {
                auto n=get16(s.records5760,rec+4u);
                put16(s.records5760,rec+4u,static_cast<std::int16_t>(n+1));
                s.records5760[text+static_cast<std::size_t>(n)]=0x0D;
                ++lines;
                active=1;
            } else key=0x1B;
            continue;
        }

        const std::uint8_t idx=static_cast<std::uint8_t>(key+1);
        const std::int8_t cls=s.table165C[static_cast<std::size_t>(idx)];
        if(((static_cast<std::uint8_t>(cls)&0xE0u)!=0 ||
            (static_cast<std::uint8_t>(cls)&0x04u)!=0 || key==0x20) &&
           endRecord(s.records5760)<0x7EEu &&
           o.call1CA8(s.records5760.data()+text,1)+arg1<0xE3) {
            auto n=get16(s.records5760,rec+4u);
            put16(s.records5760,rec+4u,static_cast<std::int16_t>(n+1));
            s.records5760[text+static_cast<std::size_t>(n)]=static_cast<std::uint8_t>(key);
            s.records5760[text+static_cast<std::size_t>(n+1)]=0;
            active=1;
        }
    }

    if(get16(s.records5760,rec+4u)==0) {
        s.records5760[text]=0;
        put16(s.records5760,rec,0);
        put16(s.records5760,rec+2u,0);
        put16(s.records5760,rec+4u,0);
    }

    o.call2A380(0,arg0,arg1&~7,arg3);
    o.call304F4(arg0,arg1);
    o.call1C50(s.records5760.data()+text,1,get16(s.records5760,rec+4u));
    o.call1A8C8(s.valueC050,arg0,arg1,arg2*0x0C,0x140-arg1);
}

} // namespace ravenloft::game
