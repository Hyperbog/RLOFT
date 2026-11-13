#include "rloft/original_application_main.hpp"

#include <array>
#include <initializer_list>
#include <stdexcept>

namespace rloft {
namespace {

struct EnvProfile {
    GameId game;
    std::uint32_t getenv_address, atoi_address, join;
    std::array<std::uint32_t, 4> names;
    std::array<std::uint32_t, 5> getenv_sites;
    std::array<std::uint32_t, 4> atoi_sites;
    OriginalCode2Environment defaults;
};

constexpr std::array<std::string_view, 4> kNames{{
    "ALSIU", "KHSSDKJ", "CANLW", "SEPVON"
}};
constexpr std::array<EnvProfile, 2> kEnvironment{{
    {GameId::Menzoberranzan, 0x3CA41, 0x3CA96, 0x23B7D,
     {{0x1074, 0x107C, 0x1084, 0x108C}},
     {{0x23B10, 0x23B21, 0x23B3A, 0x23B52, 0x23B6A}},
     {{0x23B2A, 0x23B43, 0x23B5B, 0x23B73}},
     {4, 6, 180, 1, false, 0x23B7D}},
    {GameId::StoneProphet, 0x4A920, 0x4A97B, 0x2DD95,
     {{0x12F4, 0x12FC, 0x1304, 0x130C}},
     {{0x2DD28, 0x2DD39, 0x2DD51, 0x2DD69, 0x2DD81}},
     {{0x2DD42, 0x2DD5A, 0x2DD72, 0x2DD8A}},
     {27, 232, 41, 45, false, 0x2DD95}},
}};

const EnvProfile& environment(GameId game) {
    for (const auto& profile : kEnvironment)
        if (profile.game == game) return profile;
    throw std::invalid_argument("no CODE.2 environment prefix lifted for title");
}

bool has_bytes(const std::vector<std::uint8_t>& b,
               std::uint32_t pos, std::initializer_list<std::uint8_t> sig) {
    if (pos > b.size() || sig.size() > b.size() - pos) return false;
    std::size_t i = pos;
    for (auto value : sig)
        if (b[i++] != value) return false;
    return true;
}

bool call_to(const std::vector<std::uint8_t>& code,
             std::uint32_t at, std::uint32_t target) {
    if (at >= code.size() || code.size() - at < 5 || code[at] != 0xE8)
        return false;
    const auto raw = std::uint32_t(code[at+1]) |
        (std::uint32_t(code[at+2]) << 8) |
        (std::uint32_t(code[at+3]) << 16) |
        (std::uint32_t(code[at+4]) << 24);
    return static_cast<std::uint32_t>(at + 5 + raw) == target;
}

bool c_string_at(const std::vector<std::uint8_t>& data,
                 std::uint32_t off, std::string_view expected) {
    if (off >= data.size() || expected.size() + 1 > data.size() - off)
        return false;
    for (std::size_t i = 0; i < expected.size(); ++i)
        if (data[off+i] != static_cast<std::uint8_t>(expected[i])) return false;
    return data[off+expected.size()] == 0;
}

} // namespace

const OriginalMainIdentity& original_main(GameId game, OriginalAppModule module) {
    for (const auto& item : kOriginalMains)
        if (item.game == game && item.module == module) return item;
    throw std::invalid_argument("no verified original application entry");
}

bool verify_original_application_main(GameId game, OriginalAppModule module,
                                      const std::vector<std::uint8_t>& code,
                                      const std::vector<std::uint8_t>& data) {
    const auto& entry = original_main(game, module);
    if (!call_to(code, entry.crt_call, entry.main_offset) ||
        !has_bytes(code, entry.crt_call+5, {0x83,0xC4,0x08,0x50})) return false;
    if (game == GameId::StrahdsPossession)
        return true; // canonical Strahd application code is in Code1/Code2
    if (module == OriginalAppModule::Code1) {
        if (game == GameId::Menzoberranzan) {
            if (!has_bytes(code, 0x127C, {0x55,0x89,0xE5}) ||
                !has_bytes(code, 0x129D, {0x6A,0x0A,0xE9,0xEA,0xB9,0,0})) return false;
            constexpr std::array<std::uint32_t,6> sites{{
                0x127F,0x1284,0x1289,0x128E,0x1293,0x1298}};
            constexpr std::array<std::uint32_t,6> dests{{
                0x1044,0x0C24,0x4280,0x6E1C,0x3D54,0x1198}};
            for(std::size_t i=0;i<sites.size();++i)
                if(!call_to(code,sites[i],dests[i])) return false;
            return true;
        }
        return has_bytes(code,0x6C58,{0x53,0x56,0x57,0x55,0x89,0xE5,
                                     0x81,0xEC,0,0,0,0}) &&
               call_to(code,0x6C64,0x680C) &&
               call_to(code,0x6C69,0x6134) &&
               has_bytes(code,0x6C6E,{0x50}) &&
               call_to(code,0x6C6F,0x6BB0) &&
               has_bytes(code,0x6C74,{0x83,0xC4,0x04,0x5D,0x5F,0x5E,0x5B,0xC3});
    }
    const auto& profile=environment(game);
    if (!has_bytes(code,entry.main_offset,{0x53,0x56,0x57,0x55,0x89,0xE5,0x83,0xEC,0x04}))
        return false;
    // Verify the literal fallback values, not merely the call addresses.
    if (game == GameId::Menzoberranzan) {
        if (!has_bytes(code,0x23AF5,{0xC7,0x45,0xFC,1,0,0,0,
                0xBE,6,0,0,0,0x68,0x74,0x10,0,0,
                0xBB,4,0,0,0,0xBF,0xB4,0,0,0})) return false;
    } else if (!has_bytes(code,0x2DD0D,{0xBF,0x29,0,0,0,
                0xBE,0xE8,0,0,0,0x68,0xF4,0x12,0,0,
                0xBB,0x1B,0,0,0,0xC7,0x45,0xFC,0x2D,0,0,0})) {
        return false;
    }
    for(std::size_t i=0;i<kNames.size();++i)
        if(!c_string_at(data,profile.names[i],kNames[i])) return false;
    for(auto site:profile.getenv_sites)
        if(!call_to(code,site,profile.getenv_address)) return false;
    for(auto site:profile.atoi_sites)
        if(!call_to(code,site,profile.atoi_address)) return false;
    const auto test_site=profile.getenv_sites[0]+8;
    // getenv call+5, add esp,4 (3 bytes), test eax,eax / je +0x61
    return has_bytes(code,test_site,{0x85,0xC0,0x74,0x61});
}

OriginalCode1Outcome execute_original_code1_main(
    GameId game, OriginalApplicationBoundary& boundary) {
    if(game==GameId::Menzoberranzan) {
        for (auto target : {0x1044U,0x0C24U,0x4280U,
                            0x6E1CU,0x3D54U,0x1198U})
            (void)boundary.call0(target);
        // Original `push 10; jmp 0xCC8E`; the tail consumes that status.
        return {OriginalCode1Outcome::Kind::TailExit,10,0xCC8E};
    }
    if(game==GameId::StoneProphet) {
        (void)boundary.call0(0x680C);
        const auto arg=boundary.call0(0x6134);
        return {OriginalCode1Outcome::Kind::Returned,
                boundary.call1(0x6BB0,arg),0};
    }
    throw std::invalid_argument("use canonical Strahd Code1.cpp for this title");
}

OriginalCode2Environment execute_original_code2_env_prefix(
    GameId game, OriginalApplicationBoundary& boundary) {
    const auto& p=environment(game);
    auto state=p.defaults;
    if(!boundary.getenv_call(p.getenv_address,kNames[0])) return state;
    state.gate_present=true;
    std::array<std::int32_t,4> parsed{};
    for(std::size_t i=0;i<kNames.size();++i) {
        const auto value=boundary.getenv_call(p.getenv_address,kNames[i]);
        if(!value) throw std::runtime_error("original environment name disappeared between calls");
        parsed[i]=boundary.atoi_call(p.atoi_address,*value);
    }
    if(game==GameId::Menzoberranzan) {
        state.local=parsed[0]; state.esi=parsed[1];
        state.ebx=parsed[2]; state.edi=parsed[3];
    } else {
        state.edi=parsed[0]; state.esi=parsed[1];
        state.ebx=parsed[2]; state.local=parsed[3];
    }
    return state;
}

} // namespace rloft
