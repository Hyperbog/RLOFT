// Media-free original CODE.2 296C0/29738/2978C/2A6B0 source test.
// Synthetic palette/VGA callbacks are strictly ABI tests, not DOS parity.
#include "../../Code2/Code2.cpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using code2::u8;using code2::u16;using code2::u32;
code2::Machine* active=nullptr;
unsigned unclipped=0,clipped=0,writes=0,port_sets=0;
u32 image_base=0x40000u;
void check(bool ok,const char* why) {
    if(!ok)throw std::runtime_error(why);
}
u32 resource(u32,u32) noexcept {return image_base;}
u32 plain(u32,u32,u32,u32) noexcept {++unclipped;return 0u;}
void out16(u16 port,u16) noexcept {if(port==0x3C4u)++port_sets;}
void write8(u32 addr,u8) noexcept {
    if(addr>=0xA0000u && addr<0xB0000u)++writes;
}
u32 clipped_raster(u32 record,u32 row,u32 x,u32 clip,u32 image) noexcept {
    ++clipped;
    code2::Application61Boundary vga{};
    vga.out16=&out16;
    vga.vga_write8=&write8;
    code2::function_2a6b0(*active,vga,record,row,x,clip,image);
    return 0u;
}
}

int main() {
    try {
        std::vector<u8> data(0x300000u,0u);
        code2::Machine machine{};
        machine.data=data.data();
        active=&machine;
        // Original clip defaults and guest planar video addressing.
        code2::store_le16(data.data()+0xEF3Fu,320u);
        code2::store_le16(data.data()+0xEF41u,400u);
        code2::store_le32(data.data()+0xEFEDu,4u);
        code2::store_le32(data.data()+0xEFF1u,2u);
        code2::store_le32(data.data()+0xEFF9u,80u);
        code2::store_le32(data.data()+0xF001u,0xA0000u);
        // Four planar records, two bytes per row x eight rows = 64
        // source bytes. Valid image metadata is original packed format.
        code2::store_le16(data.data()+image_base,64u);
        code2::store_le16(data.data()+image_base+2u,1u);
        code2::store_le16(data.data()+image_base+4u,8u);
        data[image_base+6u]=1u;
        for(u32 i=0u;i<64u;++i)
            data[image_base+8u+i]=static_cast<u8>(0x20u+i);

        code2::function_296c0(machine,4,32,259,287);
        check(code2::load_le32(data.data()+0xEF55u)==4u &&
              code2::load_le32(data.data()+0xEF59u)==259u &&
              code2::load_le32(data.data()+0xEF5Du)==32u &&
              code2::load_le32(data.data()+0xEF61u)==287u,
              "296C0 source clip rectangle diverged");

        code2::function_29738(machine,0x50000u);
        for(u32 i=0u;i<4u;++i)
            check(code2::load_le32(data.data()+0x50000u+i*4u)==
                  code2::load_le32(data.data()+0xEF55u+i*4u),
                  "29738 failed original 16-byte clip save");
        code2::Application57Boundary draw{};
        draw.call_2f29c=&resource;
        draw.call_2a380=&plain;
        draw.call_2a6b0=&clipped_raster;
        const u32 clean=code2::function_2978c(machine,draw,0u,40u,40u,1u,0u);
        check(clean==2u && unclipped==1u && clipped==0u,
              "2978C original visible image callback missing");
        const u32 partial=code2::function_2978c(machine,draw,0u,256u,40u,1u,0u);
        check(partial==1u && clipped==1u && writes!=0u && port_sets!=0u,
              "2978C did not execute original clipped 2A6B0 VGA writes");
        check(code2::load_le32(data.data()+0xEFD4u)==0u,
              "2978C source clip stack depth leaked");
        std::cout<<"ORIGINAL_CODE2_SHARED_CLIP_OVERLAY_SOURCE_PASS"
                 <<" clip_rectangles=2 clip_saved_dwords=4"
                 <<" clipped_original_2A6B0_calls="<<clipped
                 <<" source_VGA_writes="<<writes
                 <<" source_port_masks="<<port_sets
                 <<" original_media=NOT_INCLUDED DOS_PARITY=NOT_CERTIFIED\n";
        return 0;
    } catch(const std::exception& e){
        std::cerr<<e.what()<<"\n";return 1;
    }
}
