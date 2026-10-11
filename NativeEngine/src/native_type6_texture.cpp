#include "rloft_native/native_type6_texture.hpp"
#include "rloft_native/native_lzss.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft_native {
NativeType1Raster decode_native_type6_texture(
    const std::vector<std::uint8_t>& bytes) {
    // Original loader 37590: EH type-6 (header span zero) followed by
    // uint16 decoded length and LZSS. The two shipped body shapes are
    // exactly 32*128 and 64*128 bytes. Keep all source values indexed.
    if (bytes.size() < 8 || bytes[0]!='E' || bytes[1]!='H' ||
        bytes[2]!=6 || bytes[3]!=0 || bytes[4]!=0)
        throw std::runtime_error("original resource is not EH type-6 texture");
    const unsigned size=unsigned(bytes[5])|(unsigned(bytes[6])<<8U);
    if(size!=4096 && size!=8192)
        throw std::runtime_error("EH type-6 source texture dimensions invalid");
    const auto columns=size/128U;
    const auto decoded=native_lzss4k(
        bytes.data()+7,bytes.size()-7,size);
    if(decoded.size()!=size)
        throw std::runtime_error("EH type-6 LZSS decoded size mismatch");
    NativeType1Raster result;
    result.frames=1;
    result.width=static_cast<std::uint16_t>(columns);
    result.height=128;
    result.transparent_index=255; // type-6 has no sprite transparency key
    result.pixels.resize(size);
    for(unsigned x=0;x<columns;++x) {
        for(unsigned y=0;y<128;++y) {
            result.pixels[y*columns+x]=decoded[x*128U+y];
        }
    }
    return result;
}
} // namespace rloft_native
