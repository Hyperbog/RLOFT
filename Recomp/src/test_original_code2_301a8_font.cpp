// Source-executed original CODE.2 301A8 font-layout regression.
// Synthetic uncompressed raster isolates the ownership/layout bug; authentic
// RES0 type-2 method-3 data is validated separately by the media runtime.
#include "../../Code2/Code2.cpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using code2::u8;
using code2::u32;
void require(bool ok, const char* problem) {
    if (!ok) throw std::runtime_error(problem);
}
struct Probe {
    std::vector<u8> memory=std::vector<u8>(0x400000u,0u);
    std::vector<u8> raster=std::vector<u8>(128u*8u,0u);
    code2::Machine machine{};
    u32 heap=0x100000u;
    u32 cursor=0u;
    static Probe* active;
    Probe() {
        active=this;
        machine.data=memory.data();
        for(u32 i=0;i<raster.size();++i)
            raster[i]=static_cast<u8>(i);
        const u32 record=0x2B670u, payload=0x80000u;
        memory[record]='E';memory[record+1u]='H';
        memory[record+2u]=2u;
        code2::store_le16(memory.data()+record+3u,136u);
        code2::store_le32(memory.data()+record+5u,payload);
        for(u32 i=0;i<128u;++i)
            memory[payload+i]=static_cast<u8>((i%7u)+1u);
        const u32 descriptor=payload+128u;
        code2::store_le16(memory.data()+descriptor,128u);
        code2::store_le16(memory.data()+descriptor+2u,1u);
        memory[descriptor+4u]=1u;
        memory[descriptor+5u]=0xFFu;
        memory[descriptor+6u]=0u; // Source-supported raw method (test only).
    }
    ~Probe() {active=nullptr;}
    static Probe& self() noexcept {return *active;}
    static u32 allocate(u32 count) noexcept {
        auto& p=self();
        const u32 result=(p.heap+3u)&~3u;
        if(count>p.memory.size()-result)std::terminate();
        p.heap=result+count;
        return result;
    }
    static void free_block(u32) noexcept {}
    static u32 seek(u32 handle,u32 origin,u32 delta) noexcept {
        if(handle!=7u || origin!=1u || delta!=0u)std::terminate();
        return self().cursor;
    }
    static u32 read(u32 destination,u32 count,u32 handle) noexcept {
        auto& p=self();
        if(handle!=7u || destination>p.memory.size() ||
           count>p.memory.size()-destination)std::terminate();
        const u32 remaining=static_cast<u32>(p.raster.size())-p.cursor;
        const u32 n=std::min(count,remaining);
        std::copy_n(p.raster.data()+p.cursor,n,p.memory.data()+destination);
        p.cursor+=n;
        return n;
    }
    void run() {
        code2::Application70Boundary boundary{};
        boundary.call_3104c=&allocate;
        boundary.call_310b0=&free_block;
        boundary.call_3131c=&seek;
        boundary.call_312ec=&read;
        const u32 font=code2::function_301a8(machine,boundary,0x2B670u,7u);
        require(font!=0u,"original 301A8 returned a null font");
        const u32 image=code2::load_le32(memory.data()+font);
        require(image!=0u,"original 301A8 did not retain decoded image pointer");
        require(code2::load_le16(memory.data()+image+2u)==128u,
                "font image lost its original 128-frame count");
        require(code2::load_le16(memory.data()+image+4u)==1u,
                "font image height mismatch");
        require(memory[font+4u+65u]==(65u%7u)+1u,
                "A width at font+4+ASCII is wrong");
        for(u32 glyph=0;glyph<128u;++glyph)
            require(memory[font+4u+glyph]==memory[0x80000u+glyph],
                    "glyph widths are not contiguous after the image pointer");
        const u32 raster_at=image+8u;
        const u8 packed_first[]={0u,4u,1u,5u,2u,6u,3u,7u};
        require(std::equal(std::begin(packed_first),std::end(packed_first),
                           memory.data()+raster_at),
                "compiled 2A20C raster did not pack the first glyph");
        require(cursor==raster.size(),"font source bytes were not fully consumed");
        std::cout<<"CODE2_ORIGINAL_301A8_TYPE2_GLYPH_LAYOUT_PASS"
                 <<" glyphs=128 widths_at=FONT_PLUS_4"
                 <<" image=ORIGINAL_2FD9C_2A20C"
                 <<" raster=SYNTHETIC_METHOD0"
                 <<" retail_method3=NOT_EXERCISED\n";
    }
};
Probe* Probe::active=nullptr;
}
int main() {
    try {Probe probe;probe.run();return 0;}
    catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_301A8_FONT_TEST_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
