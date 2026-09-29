#include "games/ravenloft/data/TrigTable.h"
#include "games/ravenloft/data/Binary.h"
#include "engine/core/FileSystem.h"
#include <span>
#include <stdexcept>

namespace ravenloft::data {
int TrigTable::normalize(int d){ d%=360; if(d<0)d+=360; return d; }
TrigTable::TrigTable(const std::filesystem::path& path){
    auto bytes=ssi::core::readFile(path); if(bytes.size()<450*4) throw std::runtime_error("SINTAB too small");
    std::span<const std::uint8_t> b(bytes.data(),bytes.size()); sine_.reserve(450);
    for(std::size_t i=0;i<450;++i)sine_.push_back(u32(b,i*4));

    // CODE.2 SINTAB loader 0x3838..0x393E reads 450 sine DWORDs, 900
    // tangent DWORDs, two 128-WORD projection tables, then these eight
    // DWORD renderer constants in this exact order.
    constexpr std::size_t planeTablesOffset=450u*4u + 900u*4u;
    requireRange(b,planeTablesOffset,128u*2u*2u,"SINTAB plane tables");
    // Loader 0x38A0 reads the first 256 bytes to DS:19660, then the second
    // 256 bytes to DS:193E0. Both are signed WORD tables consumed by 0x34998.
    for(std::size_t i=0;i<128;++i) planeDistance_[i]=i16(b,planeTablesOffset+i*2u);
    for(std::size_t i=0;i<128;++i) planeStart_[i]=i16(b,planeTablesOffset+128u*2u+i*2u);

    constexpr std::size_t rendererConstantsOffset=planeTablesOffset + 128u*2u + 128u*2u;
    requireRange(b,rendererConstantsOffset,8u*4u,"SINTAB renderer constants");
    rendererConstants_.tableWidthA=u32(b,rendererConstantsOffset+0);
    rendererConstants_.tableWidthB=u32(b,rendererConstantsOffset+4);
    rendererConstants_.screenCenterX=u32(b,rendererConstantsOffset+8);
    rendererConstants_.viewportConstantA=u32(b,rendererConstantsOffset+12);
    rendererConstants_.viewportConstantB=u32(b,rendererConstantsOffset+16);
    rendererConstants_.cameraOffset=u32(b,rendererConstantsOffset+20);
    rendererConstants_.perspectiveScale=u32(b,rendererConstantsOffset+24);
    rendererConstants_.depthScaleNumerator=u32(b,rendererConstantsOffset+28);

    // The loader then consumes a 0x400-byte renderer lookup at DS:C73C, followed
    // by three DWORD geometry values used by CODE.2 0x31A14 lighting falloff.
    constexpr std::size_t lightingConstantsOffset=rendererConstantsOffset+8u*4u+0x400u;
    requireRange(b,lightingConstantsOffset,3u*4u,"SINTAB lighting constants");
    lightingPivot_=static_cast<int>(u32(b,lightingConstantsOffset+0));
    lightingTableExtent_=static_cast<int>(u32(b,lightingConstantsOffset+4));
    lightingAux_=static_cast<int>(u32(b,lightingConstantsOffset+8));
}
std::uint32_t TrigTable::rawSine(int degrees) const { return sine_.at(normalize(degrees)); }
std::int32_t TrigTable::fixedMulSignMagnitude(std::int32_t value,std::uint32_t sm){
    // CODE.2 0x35680: the table stores a 16-bit fractional magnitude in the low word
    // and the sign in bit 31. The other operand is ordinary signed 16.16.
    bool neg=(sm&0x80000000u)!=0;
    std::uint32_t mag=sm&0xFFFFu;
    std::int64_t v=value;
    if(v<0){ v=-v; neg=!neg; }
    std::int64_t product=(v*mag)>>16;
    if(neg) product=-product;
    return static_cast<std::int32_t>(product);
}
std::int32_t TrigTable::sinMul16_16(std::int32_t m,int degrees) const { return fixedMulSignMagnitude(m,sine_.at(normalize(degrees))); }
std::int32_t TrigTable::cosMul16_16(std::int32_t m,int degrees) const { return fixedMulSignMagnitude(m,sine_.at(normalize(degrees)+90)); }
}

std::int16_t ravenloft::data::TrigTable::sin14(int degrees) const {
    const auto raw=rawSine(degrees);
    auto v=static_cast<std::int32_t>(((raw&0xFFFFu)+1u)>>2u);
    if(raw&0x80000000u)v=-v;
    return static_cast<std::int16_t>(v);
}
std::int16_t ravenloft::data::TrigTable::cos14(int degrees) const { return sin14(degrees+90); }
