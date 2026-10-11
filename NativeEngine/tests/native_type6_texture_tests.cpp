#include "rloft_native/native_type6_texture.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
std::vector<std::uint8_t> make_texture(unsigned columns) {
    const unsigned count=columns*128U;
    std::vector<std::uint8_t> raw(count);
    for(unsigned x=0;x<columns;++x)
        for(unsigned y=0;y<128;++y)
            raw[x*128+y]=std::uint8_t((x*7+y*3)&0xffU);
    std::vector<std::uint8_t> compressed;
    for(unsigned i=0;i<count;i+=8) {
        compressed.push_back(0xff); // 8 original LZSS literal tokens
        for(unsigned j=0;j<8;++j)compressed.push_back(raw[i+j]);
    }
    std::vector<std::uint8_t> record{'E','H',6,0,0,
        std::uint8_t(count),std::uint8_t(count>>8U)};
    record.insert(record.end(),compressed.begin(),compressed.end());
    return record;
}
template<class Fn> void rejects(Fn fn) {
    bool failed=false;
    try{fn();}catch(const std::exception&){failed=true;}
    assert(failed);
}
void run() {
    for(unsigned columns:{32U,64U}) {
        const auto bytes=make_texture(columns);
        const auto bitmap=rloft_native::decode_native_type6_texture(bytes);
        assert(bitmap.frames==1 && bitmap.width==columns);
        assert(bitmap.height==128 && bitmap.pixels.size()==columns*128);
        for(unsigned x=0;x<columns;++x)
            for(unsigned y=0;y<128;++y)
                assert(bitmap.pixels[y*columns+x]==
                    std::uint8_t((x*7+y*3)&0xffU));
        assert(bitmap.transparent_index==255);
        auto truncated=bytes;truncated.pop_back();
        rejects([&]{static_cast<void>(
            rloft_native::decode_native_type6_texture(truncated));});
        auto wrong=bytes;wrong[2]=7;
        rejects([&]{static_cast<void>(
            rloft_native::decode_native_type6_texture(wrong));});
        auto badsize=bytes;badsize[5]=1;
        rejects([&]{static_cast<void>(
            rloft_native::decode_native_type6_texture(badsize));});
        auto badheader=bytes;badheader[3]=1;
        rejects([&]{static_cast<void>(
            rloft_native::decode_native_type6_texture(badheader));});
    }
    rejects([]{static_cast<void>(
        rloft_native::decode_native_type6_texture({}));});
}
}
int main(){run();}
