#include "rloft/original_resource_startup.hpp"
#include <array>
#include <stdexcept>
#include <string_view>

namespace rloft {
namespace {

[[nodiscard]] bool has(const std::vector<std::uint8_t>& bytes,
                       std::uint32_t pos, std::initializer_list<std::uint8_t> pattern) {
    if (pos > bytes.size() || pattern.size() > bytes.size() - pos) return false;
    auto at = pos;
    for (auto b : pattern) if (bytes[at++] != b) return false;
    return true;
}
[[nodiscard]] std::uint32_t rd32(const std::vector<std::uint8_t>& bytes,
                                 std::size_t at) {
    if (at > bytes.size() || bytes.size() - at < 4)
        throw std::invalid_argument("truncated original RES directory");
    return std::uint32_t(bytes[at]) | (std::uint32_t(bytes[at+1]) << 8) |
           (std::uint32_t(bytes[at+2]) << 16) | (std::uint32_t(bytes[at+3]) << 24);
}
[[nodiscard]] bool call_to(const std::vector<std::uint8_t>& code,
                           std::uint32_t at, std::uint32_t target) {
    if (at > code.size() || code.size() - at < 5 || code[at] != 0xe8)
        return false;
    return std::uint32_t(at + 5 + rd32(code, at+1)) == target;
}
[[nodiscard]] bool cstr(const std::vector<std::uint8_t>& data,
                        std::size_t at, std::string_view expected) {
    if (at > data.size() || expected.size()+1 > data.size()-at) return false;
    for (std::size_t i=0;i<expected.size();++i)
        if (data[at+i] != std::uint8_t(expected[i])) return false;
    return data[at+expected.size()] == 0;
}

} // namespace

OriginalStartupCheckpoint execute_code1_media_prefix(
    GameId game, OriginalResourceStartupPort& port) {
    if (game == GameId::Menzoberranzan) {
        (void)port.call(0x6B7C, {0x1F0, 11, 0xC350, 0x186A0}); // "RES"
        (void)port.call(0x6D94, {0x1F4, 1, 254}); // game title
        (void)port.call(0x5204, {3});
        (void)port.call(0xA840, {0xB90, 0, 0x300});
        (void)port.call(0x5340, {0xB90});
        (void)port.call(0x0FE8, {});
        (void)port.call(0x39F0, {});
        (void)port.call(0x0E98, {});
        // 0x10B0..0x10C0: x86 stores AL while EAX increments 0..255.
        for (unsigned i=0;i<256;++i) port.write8(0xE90+i,std::uint8_t(i));
        // An original subsequent store deliberately changes table[0x17].
        port.write8(0xEA7,1);
        (void)port.call(0x0F48, {});
        return {0x10CC,false,false,false};
    }
    if (game == GameId::StoneProphet) {
        (void)port.call(0xC480, {0x84, 11, 0xEA60, 0x186A0}); // "RES"
        (void)port.call(0xC69C, {0x88, 1, 254}); // game title
        (void)port.call(0x677C, {});
        (void)port.call(0x65C0, {});
        (void)port.call(0x8A94, {3});
        (void)port.call(0xF790, {0x6EB8, 0, 2, 100, 256});
        (void)port.call(0x8BD8, {0x6EB8});
        (void)port.call(0xCACC, {});
        (void)port.call(0x63A8, {});
        // 0x6891..0x68B5: a slightly different original 256-byte loop.
        for (unsigned i=0;i<256;++i) port.write8(0x68B8+i,std::uint8_t(i));
        return {0x68B6,false,false,false};
    }
    throw std::invalid_argument("CODE.1 resource prefix not reconstructed for game");
}

OriginalStartupCheckpoint execute_stone_code1_interaction_prefix(
    OriginalResourceStartupPort& port) {
    port.write32(0x8BC,0);
    (void)port.call(0x85D8,{});
    (void)port.call(0xF790,{0x6EB8,0,2,100,256});
    (void)port.call(0x8BD8,{0x6EB8});
    port.write8(0x89FE,std::uint8_t(port.read8(0x8750)^1U));
    (void)port.call(0x7478,{});
    return {0x618B,false,false,false}; // event dispatcher remains unconverted
}

OriginalStartupCheckpoint execute_code2_resource_prefix(
    GameId game, OriginalResourceStartupPort& port) {
    if (game == GameId::Menzoberranzan) {
        (void)port.call(0x3532C,{0x1054,11,0xEA60,0x186A0});
        const auto state=port.read32(0x17DAC);
        const bool need_init=port.read_pointer_field8(state,0x72)==0;
        if (need_init) (void)port.call(0x237EC,{});
        (void)port.call(0x35544,{0x1058,1,254});
        return {0x23977,false,false,need_init};
    }
    if (game == GameId::StoneProphet) {
        (void)port.call(0x412D0,{0x12D0,11,0xF230,0x186A0});
        const auto state=port.read32(0x26B6C);
        const bool need_init=port.read_pointer_field8(state,0x72)==0;
        if (need_init) (void)port.call(0x2D310,{});
        (void)port.call(0x414EC,{0x12D4,1,254});
        const bool chars=port.call(0x43338,{0x12E8,0})!=0;
        return {chars?0x2D4E6U:0x2D4B5U,true,chars,need_init};
    }
    throw std::invalid_argument("CODE.2 resource prefix not reconstructed for game");
}

bool verify_original_resource_startup_witness(
    GameId game, bool code2, const std::vector<std::uint8_t>& code,
    const std::vector<std::uint8_t>& data) {
    if (game != GameId::Menzoberranzan && game != GameId::StoneProphet)
        return false;
    const bool menzo=game == GameId::Menzoberranzan;
    if (code2) {
        if (menzo)
            return cstr(data,0x1054,"RES") && cstr(data,0x1058,"Menzoberranzan") &&
                cstr(data,0x1068,"CHARS") &&
                has(code,0x23939,{0x68,0xA0,0x86,1,0,0x68,0x60,0xEA,0,0,
                                  0x6A,0x0B,0x68,0x54,0x10,0,0}) &&
                call_to(code,0x2394A,0x3532C) && call_to(code,0x2395E,0x237EC) &&
                call_to(code,0x2396F,0x35544) &&
                has(code,0x23958,{0x80,0x7B,0x72,0,0x75,0x05});
        return cstr(data,0x12D0,"RES") && cstr(data,0x12D4,"The Stone Prophet") &&
            cstr(data,0x12E8,"CHARS") &&
            has(code,0x2D461,{0x68,0xA0,0x86,1,0,0x68,0x30,0xF2,0,0,
                              0x6A,0x0B,0x68,0xD0,0x12,0,0}) &&
            call_to(code,0x2D472,0x412D0) && call_to(code,0x2D487,0x2D310) &&
            call_to(code,0x2D498,0x414EC) && call_to(code,0x2D4A7,0x43338) &&
            has(code,0x2D481,{0x80,0x78,0x72,0,0x75,0x05});
    }
    if (menzo)
        return cstr(data,0x1F0,"RES") && cstr(data,0x1F4,"Menzoberranzan") &&
            call_to(code,0x105A,0x6B7C) && call_to(code,0x106E,0x6D94) &&
            call_to(code,0x1099,0x5340) &&
            has(code,0x10B0,{0x31,0xC0,0x88,0x80,0x90,0x0E,0,0,0x40,
                             0x3D,0,1,0,0,0x7C,0xF2}) &&
            has(code,0x10C0,{0xC6,0x05,0xA7,0x0E,0,0,1});
    return cstr(data,0x84,"RES") && cstr(data,0x88,"The Stone Prophet") &&
        call_to(code,0x682A,0xC480) && call_to(code,0x6840,0xC69C) &&
        call_to(code,0x6871,0xF790) &&
        has(code,0x6891,{0xC7,0x45,0xF4,0,0,0,0,0x81,0x7D,0xF4,
                          0,1,0,0,0x7C,0x07,0xEB,0x13}) &&
        has(code,0x68A8,{0x8A,0x45,0xF4,0x8B,0x55,0xF4,
                          0x88,0x82,0xB8,0x68,0,0,0xEB,0xED}) &&
        call_to(code,0x614A,0x85D8) && call_to(code,0x6164,0xF790) &&
        call_to(code,0x6172,0x8BD8) && call_to(code,0x6186,0x7478);
}

OriginalRes0Layout inspect_original_res0(const std::vector<std::uint8_t>& b) {
    if (b.size()<8) throw std::invalid_argument("truncated RES0");
    const auto count=rd32(b,0);
    if (count==0 || count>65535 || std::uint64_t(count)*4+4>b.size())
        throw std::invalid_argument("invalid RES0 index count");
    const auto table_end=4U+count*4U;
    std::uint32_t last=table_end;
    for(std::uint32_t i=0;i<count;++i) {
        const auto off=rd32(b,4U+i*4U);
        if(off<last || off>b.size() || b.size()-off<2 ||
           b[off]!='E' || b[off+1]!='H')
            throw std::invalid_argument("bad RES0 offset or EH prefix");
        last=off;
    }
    if(rd32(b,4)!=table_end)
        throw std::invalid_argument("RES0 first offset != index end");
    return {count,table_end,static_cast<std::uint32_t>(b.size())};
}

} // namespace rloft
