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
        return {0x2D54D,handle,handle!=0,result!=0,handle?0x4191CU:0x46600U};
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


Code2GameplayBoundary execute_code2_gameplay_boundary(
    GameId game,const Code2MediaHandoff& prior,
    OriginalResourceStartupPort& p,u32 original_stack_argument) {
    if(prior.has_chars!=(prior.chars_handle!=0))
        throw std::invalid_argument("inconsistent original CHARS handle");
    if(game==GameId::Menzoberranzan) {
        if(prior.source_next!=0x23A20 || prior.upcoming_call!=0x204DC)
            throw std::invalid_argument("Menzo CODE.2 source boundary mismatch");
        // 23A20..23A93, including the original ESI test. At 239F0/239F2,
        // the executable copied the CHARS handle into both EBX and ESI.
        execute_menzo_first_gameplay_callee(p,1);
        (void)p.call(0x35950,{});
        const auto mode=static_cast<u32>(p.read8(0xBEE4));
        (void)p.call(0x314FC,{mode});
        const auto inverse=static_cast<u32>(p.read8(0xBEE4)^1U);
        (void)p.call(0x31514,{inverse});
        p.write32(0xF570,0x00150000);
        if(prior.chars_handle!=0) {
            (void)p.call(0x1E474,{});
            p.write8(0x124E5,static_cast<std::uint8_t>(p.read8(0xBEE4)^1U));
            (void)p.call(0x1EF40,{});
            // Original PUSH 0xD4; PUSH [EBP+0x10] (cdecl source order).
            (void)p.call(0x1D6EC,{original_stack_argument,0xD4});
        }
        p.write32(0x12EAC,1);
        // 23A93 is RET. 23A94 is NOT a fallthrough execution target.
        return {0x23A93,0,true,false};
    }
    if(game==GameId::StoneProphet) {
        if(prior.source_next!=0x2D54D ||
           prior.upcoming_call!=(prior.chars_handle?0x4191CU:0x46600U))
            throw std::invalid_argument("Stone CODE.2 source boundary mismatch");
        // 2D54D..2D5C4: absent-CHARS UI setup, common callback tail,
        // then the original EBX-dependent jump/call decision.
        bool drawn=false;
        if(prior.chars_handle==0 && p.read32(0x24E54)!=0) {
            const auto screen=static_cast<u32>(p.read8(0x44FDD))+4U;
            const auto work=p.read32(0x15C20);
            (void)p.call(0x46600,{0x1A5AC,0,screen,work,0x100});
            (void)p.call(0x3CA58,{0x1A5AC});
            drawn=true;
        }
        (void)p.call(0x4191C,{});
        const auto flag=static_cast<u32>(p.read8(0x1A5A8));
        (void)p.call(0x3C9D4,{flag});
        const auto flipped=static_cast<u32>(p.read8(0x1A5A8)^1U);
        (void)p.call(0x3C9EC,{flipped});
        p.write32(0x1DC34,0x00070000);
        // Present CHARS: 2D5C5 calls 25468; absent: JE -> 2D6CC.
        return prior.chars_handle!=0
            ? Code2GameplayBoundary{0x2D5C5,0x25468,false,drawn}
            : Code2GameplayBoundary{0x2D6CC,0,false,drawn};
    }
    throw std::invalid_argument("no cross-game CODE.2 gameplay handoff for title");
}

bool verify_code2_gameplay_boundary_witness(
    GameId game,const std::vector<std::uint8_t>& c) {
    try {
        if(game==GameId::Menzoberranzan)
            return call(c,0x23A22,0x204DC) &&
                call(c,0x23A2A,0x35950) &&
                call(c,0x23A37,0x314FC) &&
                call(c,0x23A4A,0x31514) &&
                bytes(c,0x23A4F,{0xC7,0x05,0x70,0xF5,0,0,0,0,0x15,0}) &&
                bytes(c,0x23A5C,{0x85,0xF6,0x74,0x26}) &&
                call(c,0x23A60,0x1E474) &&
                bytes(c,0x23A6C,{0xA2,0xE5,0x24,0x01,0}) &&
                call(c,0x23A71,0x1EF40) &&
                call(c,0x23A7E,0x1D6EC) &&
                bytes(c,0x23A86,{0xC7,0x05,0xAC,0x2E,1,0,1,0,0,0}) &&
                bytes(c,0x23A90,{0x5D,0x5E,0x5B,0xC3});
        if(game==GameId::StoneProphet)
            return bytes(c,0x2D54D,{0x85,0xDB,0x75,0x3A}) &&
                bytes(c,0x2D551,{0x83,0x3D,0x54,0x4E,2,0,0,0x74,0x31}) &&
                call(c,0x2D576,0x46600) &&
                call(c,0x2D583,0x3CA58) &&
                call(c,0x2D58B,0x4191C) &&
                call(c,0x2D598,0x3C9D4) &&
                call(c,0x2D5AB,0x3C9EC) &&
                bytes(c,0x2D5B0,{0xC7,0x05,0x34,0xDC,1,0,0,0,7,0}) &&
                bytes(c,0x2D5BD,{0x85,0xDB,0x0F,0x84,7,1,0,0}) &&
                call(c,0x2D5C5,0x25468);
    }catch(const std::out_of_range&) {return false;}
    return false;
}


namespace {
[[nodiscard]] std::uint16_t port_word(
    OriginalResourceStartupPort& p, std::uint32_t at) {
    return static_cast<std::uint16_t>(
        p.read8(at) | (std::uint16_t(p.read8(at+1U))<<8U));
}
void put_port_word(OriginalResourceStartupPort& p, u32 at, std::uint16_t v) {
    p.write8(at,static_cast<std::uint8_t>(v));
    p.write8(at+1U,static_cast<std::uint8_t>(v>>8U));
}
[[nodiscard]] std::uint16_t pointer_word(
    OriginalResourceStartupPort& p,u32 pointer,u32 offset) {
    return static_cast<std::uint16_t>(
        p.read_pointer_field8(pointer,offset) |
        (std::uint16_t(p.read_pointer_field8(pointer,offset+1U))<<8U));
}
} // namespace

OriginalCharsLayout original_chars_layout(GameId game) {
    switch(game) {
    case GameId::StrahdsPossession: return {4,0x248,0x920,0x10};
    case GameId::Menzoberranzan:   return {4,0x41C,0x1070,0x10};
    case GameId::StoneProphet:     return {4,0x41C,0x1070,0x10};
    }
    throw std::invalid_argument("unsupported original CHARS game");
}
OriginalCharsImage::OriginalCharsImage(GameId game,std::vector<std::uint8_t> bytes)
    :game_(game),bytes_(std::move(bytes)) {
    const auto schema=original_chars_layout(game_);
    if(schema.record_count*schema.record_bytes!=schema.image_bytes ||
       bytes_.size()!=schema.image_bytes)
        throw std::invalid_argument("original CHARS byte length mismatch");
}
OriginalCharsLayout OriginalCharsImage::layout() const {
    return original_chars_layout(game_);
}
std::vector<std::uint8_t> OriginalCharsImage::record(std::size_t index) const {
    const auto schema=layout();
    if(index>=schema.record_count)throw std::out_of_range("CHARS actor index");
    const auto first=bytes_.begin()+static_cast<std::ptrdiff_t>(index*schema.record_bytes);
    return {first,first+static_cast<std::ptrdiff_t>(schema.record_bytes)};
}
std::uint16_t OriginalCharsImage::status_word(std::size_t index) const {
    const auto schema=layout();
    if(index>=schema.record_count)throw std::out_of_range("CHARS actor index");
    const auto at=index*schema.record_bytes+schema.status_word_offset;
    return static_cast<std::uint16_t>(
        bytes_[at] | (std::uint16_t(bytes_[at+1U])<<8U));
}
bool verify_cross_game_chars_geometry_witness(
    GameId game,const std::vector<std::uint8_t>& c1,
    const std::vector<std::uint8_t>& c2) {
    try {
        if(game==GameId::Menzoberranzan)
            return bytes(c1,0x1209,{0x68,0x10,0x02,0,0}) &&
                bytes(c1,0x11E2,{0x69,0xF3,0x1C,0x04,0,0}) &&
                bytes(c1,0x123A,{0x68,0x70,0x10,0,0}) &&
                bytes(c1,0x123F,{0x68,0x70,0xB6,0,0}) &&
                call(c1,0x1246,0x9714) &&
                bytes(c2,0x1B690,{0x68,0x08,0x10,0,0}) &&
                bytes(c2,0x1B69E,{0x68,0x70,0x10,0,0}) &&
                call(c2,0x1B6AA,0x3733C) &&
                bytes(c2,0x204F8,{0x69,0xF3,0x1C,0x04,0,0});
        if(game==GameId::StoneProphet)
            return bytes(c1,0x6B22,{0xB8,0x9C,0,0,0}) &&
                bytes(c1,0x6AC6,{0x69,0x45,0xF8,0x1C,0x04,0,0}) &&
                bytes(c1,0x6B71,{0x68,0x1C,0x04,0,0}) &&
                bytes(c1,0x6B8A,{0x68,0x70,0x10,0,0}) &&
                call(c1,0x6B95,0x8884) &&
                bytes(c2,0x216A4,{0x68,0x64,0x12,0,0}) &&
                bytes(c2,0x216B9,{0x68,0x70,0x10,0,0}) &&
                call(c2,0x216C3,0x4340C);
    }catch(const std::out_of_range&) {return false;}
    return false;
}

void execute_menzo_first_gameplay_callee(
    OriginalResourceStartupPort& p,u32 redraw_flag) {
    // CODE.2 204DC..205D7: four actor records, source signed/unsigned
    // status comparisons and short-circuit predicates, then UI resource draw.
    const auto bank=static_cast<u32>(p.read8(0x3A5AD));
    u32 selector=bank+9U;
    bool matched=false;
    for(u32 actor=0;actor<4U;++actor) {
        const u32 base=0x38CE8U+actor*0x41CU;
        const auto status=port_word(p,base+0x10U);
        if(status>=0x4000U || (status&0x20U)!=0)continue;
        bool eligible=true;
        for(const u32 flag:{0x0FU,0x0AU,0x11U,0xA5U}) {
            if(p.call(0x115C0,{flag,actor})!=0) {
                eligible=false;break;
            }
        }
        if(eligible && p.call(0x37510,{base,0x13F40})!=0)matched=true;
    }
    if(matched)for(u32 actor=0;actor<4U;++actor) {
        if(p.call(0x29098,{actor,0})==0)selector=bank+0x1AU;
    }
    (void)p.call(0x3854C,{0xBEE8,0,selector,p.read32(0x8DC8),0x100});
    if(redraw_flag!=0)(void)p.call(0x31580,{0xBEE8});
}

bool execute_stone_first_gameplay_callee(OriginalResourceStartupPort& p) {
    // CODE.2 25468..25586. A zero 400E0 result exits immediately.
    const auto active=static_cast<std::uint16_t>(p.call(0x400E0,{}));
    put_port_word(p,0x1F710,active);
    if(active==0)return false;
    (void)p.call(0x498E0,{0,0x18F});
    (void)p.call(0x498C0,{0,0x27E});
    (void)p.call(0x49838,{0xC8,0xA0});
    const auto ptr=p.call(0x49858,{});
    if(ptr==0)throw std::runtime_error("Stone 49858 returned null layout");
    for(const u32 at:{0x21F54U,0x227D6U,0x21F52U})put_port_word(p,at,0);
    put_port_word(p,0x1F712,0xFFFF);
    put_port_word(p,0x1F714,0xFFFF);
    p.write8(0x1F716,0);p.write8(0x1F717,0);
    const auto x=pointer_word(p,ptr,4U),y=pointer_word(p,ptr,6U);
    put_port_word(p,0x227D2,y);
    put_port_word(p,0x227D4,x);
    put_port_word(p,0x21F50,x);
    put_port_word(p,0x21F4E,y);
    p.write8(0x21F56,p.read8(0x26A26));
    for(u32 row=0;row<2U;++row)
        for(u32 col=0;col<4U;++col)
            put_port_word(p,0x21F3EU+row*8U+col*2U,0xFFFF);
    const auto asset=p.call(0x4121C,{p.read32(0x48114)+0x2FU,0});
    (void)p.call(0x42BC9,{0x1F922,asset,0x261C});
    (void)p.call(0x3CEC8,{0,0,0x18F,0x13F});
    (void)p.call(0x2541C,{0xFFFF});
    return true;
}
Code2GameplayBoundary continue_stone_code2_present_startup(
    const Code2GameplayBoundary& prior,OriginalResourceStartupPort& p) {
    if(prior.source_boundary!=0x2D5C5 || prior.pending_original_call!=0x25468)
        throw std::invalid_argument("Stone next callee requires CHARS-present path");
    (void)execute_stone_first_gameplay_callee(p);
    return {0x2D5CA,0,false,prior.opening_ui_drawn};
}
bool verify_first_gameplay_callee_witness(
    GameId game,const std::vector<std::uint8_t>& c) {
    try {
        if(game==GameId::Menzoberranzan)
            return bytes(c,0x204EC,{0x0F,0xB6,0x3D,0xAD,0xA5,3,0}) &&
                bytes(c,0x204F8,{0x69,0xF3,0x1C,0x04,0,0}) &&
                bytes(c,0x204FE,{0x66,0x81,0xBE,0xF8,0x8C,3,0,0,0x40}) &&
                call(c,0x20519,0x115C0) &&
                call(c,0x20528,0x115C0) &&
                call(c,0x20537,0x115C0) &&
                call(c,0x20549,0x115C0) &&
                call(c,0x20561,0x37510) &&
                call(c,0x20589,0x29098) &&
                call(c,0x205B8,0x3854C) &&
                call(c,0x205CB,0x31580);
        if(game==GameId::StoneProphet)
            return call(c,0x2546B,0x400E0) &&
                bytes(c,0x25470,{0x66,0xA3,0x10,0xF7,1,0}) &&
                call(c,0x25486,0x498E0) &&
                call(c,0x25495,0x498C0) &&
                call(c,0x254A7,0x49838) &&
                call(c,0x254AF,0x49858) &&
                bytes(c,0x25527,{0x66,0xC7,0x84,0xC1,0x3E,0x1F,2,0,0xFF,0xFF}) &&
                call(c,0x2554C,0x4121C) &&
                call(c,0x2555A,0x42BC9) &&
                call(c,0x25570,0x3CEC8) &&
                call(c,0x2557D,0x2541C);
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
std::optional<OriginalCharsImage> OriginalNativeMediaSession::validated_chars() const {
    auto raw=chars_(); // must recheck each access after CODE.1 handoff
    if(!raw)return std::nullopt;
    return OriginalCharsImage(game_,std::move(*raw));
}

} // namespace rloft
