#include "rloft/original_code2_handoff.hpp"
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {
using u32=std::uint32_t;
[[nodiscard]] u32 rd32(const std::vector<std::uint8_t>& c, std::size_t at) {
    if(at>c.size()||c.size()-at<4)throw std::out_of_range("truncated CODE.2 opcode");
    return u32(c[at])|(u32(c[at+1])<<8)|(u32(c[at+2])<<16)|(u32(c[at+3])<<24);
}
[[nodiscard]] bool call(const std::vector<std::uint8_t>& c,
                        std::size_t at,u32 target) {
    return at<c.size() && c.size()-at>=5 && c[at]==0xE8 &&
        u32(u32(at+5)+rd32(c,at+1))==target;
}
[[nodiscard]] bool bytes(const std::vector<std::uint8_t>& c,
                         std::size_t at,std::initializer_list<std::uint8_t> v) {
    if(at>c.size()||v.size()>c.size()-at)return false;
    for(auto b:v)if(c[at++]!=b)return false;
    return true;
}
}

Code2MediaHandoff execute_code2_media_handoff(
    GameId game,const OriginalStartupCheckpoint& prefix,
    OriginalResourceStartupPort& p) {
    if(game==GameId::Menzoberranzan) {
        if(prefix.next_code_offset!=0x23977 || prefix.chars_examined)
            throw std::invalid_argument("Menzo CODE.2 prefix/handoff mismatch");
        (void)p.call(0x31444,{3});
        (void)p.call(0x37110,{0xBEE8,0,0x300});
        (void)p.call(0x31580,{0xBEE8});
        (void)p.call(0x23790,{});
        (void)p.call(0x6510,{});
        (void)p.call(0x31168,{0x5A});
        (void)p.call(0x37110,{0xBEE8,0,0x300});
        (void)p.call(0x31580,{0xBEE8});
        (void)p.call(0x23790,{});
        const auto handle=p.call(0x37248,{0x1068,0}); // original "CHARS" open
        const auto config=p.call(0x372E4,{0x1070});   // original "t"
        p.write32(0x39D58,config);
        (void)p.call(0x1B6C4,{});
        (void)p.call(0x38788,{});
        if(handle==0) (void)p.call(0x1DF34,{});
        else {
            (void)p.call(0x1B68C,{});
            (void)p.call(0x23790,{});
        }
        return {0x23A20,handle,handle!=0,false,0x204DC};
    }
    if(game==GameId::StoneProphet) {
        if(!prefix.chars_examined || prefix.chars_present!=(prefix.chars_handle!=0) ||
           prefix.next_code_offset!=(prefix.chars_handle?0x2D4E6U:0x2D4B5U))
            throw std::invalid_argument("Stone CODE.2 prefix/handoff mismatch");
        const auto handle=prefix.chars_handle;
        if(handle==0) {
            (void)p.call(0x3C914,{3});
            (void)p.call(0x39CD0,{0x1A5AC,0,0x300});
            (void)p.call(0x3CA58,{0x1A5AC});
            (void)p.call(0x2D2B4,{});
        } else (void)p.call(0x3C914,{0x83});
        (void)p.call(0x6E60,{});
        p.write32(0x24E54,1);
        (void)p.call(0x3C640,{0x5A});
        const auto value=p.call(0x40584,{0x12F0}); // "t" config
        p.write32(0x44764,value);
        (void)p.call(0x216E8,{});
        (void)p.call(0x4683C,{});
        u32 result=0;
        if(handle==0) result=p.call(0x24F14,{});
        else (void)p.call(0x216A0,{});
        if(result!=0) {
            (void)p.call(0x25EA8,{});
            (void)p.call(0x2D2B4,{});
            (void)p.call(0x25F54,{});
        }
        return {0x2D54D,handle,handle!=0,result!=0,0x46600};
    }
    throw std::invalid_argument("no original CODE.2 resource handoff for title");
}

bool verify_code2_media_handoff_witness(
    GameId game,const std::vector<std::uint8_t>& c) {
    try {
        if(game==GameId::Menzoberranzan)
            return call(c,0x23979,0x31444) && call(c,0x2398D,0x37110) &&
                call(c,0x2399A,0x31580) && call(c,0x239A2,0x23790) &&
                call(c,0x239A7,0x6510) && call(c,0x239AE,0x31168) &&
                call(c,0x239C2,0x37110) && call(c,0x239CF,0x31580) &&
                call(c,0x239D7,0x23790) && call(c,0x239E3,0x37248) &&
                call(c,0x239F4,0x372E4) && bytes(c,0x239FC,{0xA3,0x58,0x9D,3,0}) &&
                call(c,0x23A01,0x1B6C4) && call(c,0x23A06,0x38788) &&
                bytes(c,0x23A0B,{0x85,0xDB,0x75,0x07}) &&
                call(c,0x23A0F,0x1DF34) && call(c,0x23A16,0x1B68C) &&
                call(c,0x23A1B,0x23790) && call(c,0x23A22,0x204DC);
        if(game==GameId::StoneProphet)
            return bytes(c,0x2D4B1,{0x85,0xC0,0x75,0x31}) &&
                call(c,0x2D4B7,0x3C914) && call(c,0x2D4CA,0x39CD0) &&
                call(c,0x2D4D7,0x3CA58) && call(c,0x2D4DF,0x2D2B4) &&
                call(c,0x2D4EB,0x3C914) && call(c,0x2D4F3,0x6E60) &&
                bytes(c,0x2D4FA,{0xC7,0x05,0x54,0x4E,2,0,1,0,0,0}) &&
                call(c,0x2D504,0x3C640) && call(c,0x2D511,0x40584) &&
                bytes(c,0x2D519,{0xA3,0x64,0x47,4,0}) &&
                call(c,0x2D51E,0x216E8) && call(c,0x2D523,0x4683C) &&
                call(c,0x2D52C,0x24F14) && call(c,0x2D535,0x216A0) &&
                bytes(c,0x2D53A,{0x85,0xF6,0x74,0x0F}) &&
                call(c,0x2D53E,0x25EA8) && call(c,0x2D543,0x2D2B4) &&
                call(c,0x2D548,0x25F54) && bytes(c,0x2D54D,{0x85,0xDB});
    }catch(const std::out_of_range&) {return false;}
    return false;
}

OriginalNativeMediaSession::OriginalNativeMediaSession(
    GameId game,OriginalResourceFileReader resource,OriginalCharsReader chars)
  :game_(game),resource_(std::move(resource)),chars_(std::move(chars)) {
    if(!game_profile(game_) || !resource_ || !chars_)
        throw std::invalid_argument("missing original title/media reader");
}
const std::vector<std::uint8_t>& OriginalNativeMediaSession::res(unsigned number) {
    if(number>12) throw std::out_of_range("RES index outside 0..12");
    const auto found=archives_.find(number);
    if(found!=archives_.end())return found->second;
    auto data=load_shared_original_res(game_,number,resource_);
    // Non-RES0 archives may have different internal semantics; existing
    // ResArchive handles extraction and does not assume a common type table.
    return archives_.emplace(number,std::move(data)).first->second;
}
std::optional<std::vector<std::uint8_t>> OriginalNativeMediaSession::chars() const {
    // Never cache: CODE.1 can create CHARS between consecutive CODE.2 runs.
    return chars_();
}

} // namespace rloft
