#pragma once
// Exact original CODE.2 object-1 0x5E38..0x5EC2 control/data flow,
// reconstructed from the immutable retail CODE.2 LE image.
// Missing GAME.OPT is an ordinary early return, not an error or fake
// successful file open. Source address arguments and encoded order match
// retail calls: 312A0,312EC,312DC,325B0,29234,28098,290A8.
// Include original Code2.cpp before this header.
#include <cstdint>
#include <stdexcept>
namespace rloft { namespace original_code2_options {
using u32=std::uint32_t;
struct Boundary {
    std::int32_t (*open_read)(u32)=nullptr;
    void (*read_guest)(u32,u32,u32)=nullptr;
    void (*close)(u32)=nullptr;
    void (*palette_bank)(u32,u32,u32,u32,u32)=nullptr;
    void (*palette_dac)(u32)=nullptr;
    void (*volume_28098)(u32)=nullptr;
    void (*sound_enable_290a8)(u32)=nullptr;
};
struct Report {
    bool found=false;
    bool applied=false;
    u32 handle=0;
};
inline Report function_5e38(code2::Machine& m,Boundary& b) {
    if(!m.data||!b.open_read)
        throw std::invalid_argument("Original CODE2 5E38 requires guest and DOS open");
    const u32 path=code2::load_le32(m.data+0x6138u);
    // This path is the original pointer, 0374h -> GAME.OPT.
    if(path!=0x374u)
        throw std::runtime_error("Original 5E38 game-option filename pointer differs");
    const std::int32_t h=b.open_read(path);
    if(h==-1)return {}; // retail 5E4C JE 5EC1, EBX restored and RET
    if(h<0 || !b.read_guest || !b.close || !b.palette_bank ||
       !b.palette_dac || !b.volume_28098 || !b.sound_enable_290a8)
        throw std::runtime_error("Original 5E38 GAME.OPT exists without complete DOS and palette host services");
    const u32 handle=static_cast<u32>(h);
    // Retail 5E4E..5E66: original 312EC stack permutation reads
    // 000Eh bytes at DGROUP C025, then 312DC closes the handle.
    b.read_guest(0xC025u,0x0Eu,handle);
    b.close(handle);
    const u32 actor=code2::load_le32(m.data+0xF050u);
    if(actor==0u)
        throw std::runtime_error("Original 5E38 character state pointer is null");
    m.data[actor+0x6Cu]=m.data[0xC02Eu];
    b.palette_bank(0xC054u,0u,code2::load_le32(m.data+0xC034u),0x390u,0x100u);
    b.palette_dac(0xC054u);
    b.volume_28098(m.data[0xC02Bu]);
    b.sound_enable_290a8(m.data[0xC02Cu]);
    return {true,true,handle};
}
}} // namespace rloft::original_code2_options
