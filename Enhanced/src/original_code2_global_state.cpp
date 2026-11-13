#include "rloft/original_code2_global_state.hpp"
#include <array>
#include <stdexcept>
#include <string>

namespace rloft {
namespace {

using u32 = std::uint32_t;
constexpr std::array<u32,23> menzo_zero{{
    0x3A5E4,0x39D80,0x39D6C,0x39D68,0x39D78,0x39D74,0x39D7C,
    0x39D88,0x39D84,0x39D90,0x39D8C,0x39D98,0x39D94,0x39D70,
    0x39D60,0x8F84,0x8F88,0x8F8C,0x8F90,0x8F94,0x8F98,0x8F9C,0x8FA0
}};
constexpr std::array<u32,23> stone_zero{{
    0x4478C,0x447B8,0x447A4,0x447AC,0x4479C,0x44798,0x44794,
    0x44774,0x44780,0x4476C,0x44770,0x44778,0x44788,0x44784,
    0x4477C,0x16E8C,0x16E90,0x16E94,0x16E98,0x16E9C,0x16EA0,
    0x16EA4,0x16EA8
}};

u32 get32(const std::vector<std::uint8_t>& b,std::size_t p) {
    if(p>b.size() || b.size()-p<4) throw std::out_of_range("CODE.2 witness truncated");
    return u32(b[p])|(u32(b[p+1])<<8)|(u32(b[p+2])<<16)|(u32(b[p+3])<<24);
}
bool match(const std::vector<std::uint8_t>& b, std::size_t p,
           std::initializer_list<std::uint8_t> bytes) {
    if(p>b.size()||bytes.size()>b.size()-p)return false;
    for(auto x:bytes)if(b[p++]!=x)return false;
    return true;
}
bool store32(const std::vector<std::uint8_t>& b,std::size_t p,u32 address,u32 value) {
    return match(b,p,{0xC7,0x05}) && get32(b,p+2)==address && get32(b,p+6)==value;
}
bool store16(const std::vector<std::uint8_t>& b,std::size_t p,u32 address,
             std::uint16_t value) {
    return match(b,p,{0x66,0xC7,0x05}) && get32(b,p+3)==address &&
           match(b,p+7,{std::uint8_t(value),std::uint8_t(value>>8)});
}
bool call_to(const std::vector<std::uint8_t>& b,std::size_t p,u32 target) {
    return p<b.size() && b.size()-p>=5 && b[p]==0xE8 &&
           u32(u32(p+5)+get32(b,p+1))==target;
}
void put16(OriginalResourceStartupPort& p,u32 address,std::uint16_t value) {
    p.write8(address,std::uint8_t(value));
    p.write8(address+1,std::uint8_t(value>>8));
}

template<std::size_t N>
void zero_original(OriginalResourceStartupPort& p,const std::array<u32,N>& a) {
    for(auto addr:a)p.write32(addr,0);
}

template<std::size_t N>
bool check_zeros(const std::vector<std::uint8_t>& code,
                 std::size_t start,const std::array<u32,N>& a) {
    for(auto address:a) {
        if(!store32(code,start,address,0))return false;
        start+=10;
    }
    return true;
}

} // namespace

Code2GlobalCheckpoint execute_code2_post_environment(
    GameId game,const OriginalCode2Environment& env,OriginalResourceStartupPort& port) {
    if(game==GameId::Menzoberranzan) {
        zero_original(port,menzo_zero);
        put16(port,0x8FC4,0x8000); put16(port,0x8FC0,0x8000);
        port.write8(0xF57C,1);port.write32(0x8FBC,0xFFFFFFFFU);
        put16(port,0x8FC6,std::uint16_t(env.esi));
        put16(port,0x8FC2,std::uint16_t(env.ebx));
        port.write32(0x8FC8,u32(env.edi));
        (void)port.call(0x23934,{u32(env.local)});
        const auto first=port.read32(0x3A660);
        port.write32(0x19454,first);
        const auto second=port.read32(0x3A658);
        port.write32(0x15468,1);
        port.write32(0x19450,first+second);
        (void)port.call(0x23500,{});
        (void)port.call(0x23A94,{0xFFFFFFFFU});
        return {0x23CD8,0x23934,0x23500,0x23A94};
    }
    if(game==GameId::StoneProphet) {
        zero_original(port,stone_zero);
        put16(port,0x16ECC,0x8000); put16(port,0x16EC8,0x8000);
        port.write8(0x1DC40,1);
        port.write32(0x16EC4,0xFFFFFFFFU);
        port.write32(0x24E3C,77);
        put16(port,0x16ECE,std::uint16_t(env.esi));
        put16(port,0x16ECA,std::uint16_t(env.ebx));
        port.write32(0x16ED0,u32(env.local));
        (void)port.call(0x2D45C,{u32(env.edi)});
        const auto first=port.read32(0x4812C);
        port.write32(0x281BC,first);
        const auto second=port.read32(0x4815C);
        port.write32(0x281B8,first+second);
        (void)port.call(0x2D010,{});
        (void)port.call(0x2D70C,{0xFFFFFFFFU});
        return {0x2DEF0,0x2D45C,0x2D010,0x2D70C};
    }
    throw std::invalid_argument("CODE.2 global startup not reconstructed for title");
}

bool verify_code2_global_state_witness(
    GameId game,const std::vector<std::uint8_t>& code) {
    if(game!=GameId::Menzoberranzan && game!=GameId::StoneProphet)return false;
    try {
        const bool menzo=game==GameId::Menzoberranzan;
        const std::size_t start=menzo?0x23B7D:0x2DD95;
        if(!check_zeros(code,start,menzo?menzo_zero:stone_zero))return false;
        const std::size_t base=start+230;
        if(menzo) {
            return store16(code,base,0x8FC4,0x8000) &&
              store16(code,base+9,0x8FC0,0x8000) &&
              match(code,base+18,{0xC6,0x05,0x7C,0xF5,0,0,1}) &&
              store32(code,base+25,0x8FBC,0xFFFFFFFFU) &&
              match(code,0x23C86,{0xFF,0x75,0xFC}) &&
              match(code,0x23C89,{0x66,0x89,0x35,0xC6,0x8F,0,0}) &&
              match(code,0x23C90,{0x66,0x89,0x1D,0xC2,0x8F,0,0}) &&
              match(code,0x23C97,{0x89,0x3D,0xC8,0x8F,0,0}) &&
              call_to(code,0x23C9D,0x23934) &&
              match(code,0x23CA2,{0xA1,0x60,0xA6,3,0,0x83,0xC4,4,0xA3,0x54,0x94,1,0}) &&
              match(code,0x23CAF,{0x03,0x05,0x58,0xA6,3,0}) &&
              store32(code,0x23CB5,0x15468,1) &&
              match(code,0x23CBF,{0xA3,0x50,0x94,1,0}) &&
              call_to(code,0x23CC4,0x23500) &&
              match(code,0x23CC9,{0x6A,0xFF}) &&
              call_to(code,0x23CCB,0x23A94);
        }
        return store16(code,base,0x16ECC,0x8000) &&
          store16(code,base+9,0x16EC8,0x8000) &&
          match(code,0x2DE8D,{0x8B,0x45,0xFC}) &&
          match(code,0x2DE90,{0xC6,0x05,0x40,0xDC,1,0,1}) &&
          store32(code,0x2DE97,0x16EC4,0xFFFFFFFFU) &&
          store32(code,0x2DEA1,0x24E3C,77) &&
          match(code,0x2DEAB,{0x66,0x89,0x35,0xCE,0x6E,1,0}) &&
          match(code,0x2DEB2,{0x57}) &&
          match(code,0x2DEB3,{0x66,0x89,0x1D,0xCA,0x6E,1,0}) &&
          match(code,0x2DEBA,{0xA3,0xD0,0x6E,1,0}) &&
          call_to(code,0x2DEBF,0x2D45C) &&
          match(code,0x2DEC4,{0xA1,0x2C,0x81,4,0,0xA3,0xBC,0x81,2,0}) &&
          match(code,0x2DECE,{0x03,0x05,0x5C,0x81,4,0,0x83,0xC4,4}) &&
          match(code,0x2DED7,{0xA3,0xB8,0x81,2,0}) &&
          call_to(code,0x2DEDC,0x2D010) &&
          match(code,0x2DEE1,{0x6A,0xFF}) &&
          call_to(code,0x2DEE3,0x2D70C);
    } catch(const std::out_of_range&) {return false;}
}

std::vector<std::uint8_t> load_shared_original_res(
    GameId game,unsigned number,const OriginalResourceFileReader& reader) {
    if(!game_profile(game))throw std::invalid_argument("unrecognized game profile");
    if(number>12)throw std::out_of_range("RES archive number outside supported set");
    if(!reader)throw std::invalid_argument("missing original RES reader");
    const auto name=std::string("RES")+std::to_string(number);
    auto bytes=reader(name);
    if(number==0) {
        const auto layout=inspect_original_res0(bytes);
        const auto expected=game==GameId::StrahdsPossession?141U:
                            game==GameId::Menzoberranzan?175U:174U;
        if(layout.entries!=expected)
            throw std::invalid_argument("RES0 entry count does not match title");
    }
    return bytes;
}

} // namespace rloft
