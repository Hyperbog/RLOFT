// Compiled original CODE.1 6F10/6FC0/6FF4/709C/705C/6F74 source path.
// Service success comes from stateful host-owned DPMI allocations, NOT fixtures.
// This does not execute DOS hardware or constitute DOS/4GW runtime parity.
#include "../../Code1/Code1.cpp"
#include "dpmi_dos_memory.hpp"
#include "dpmi_linear_memory.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace {
using u8=std::uint8_t;using u16=std::uint16_t;using u32=std::uint32_t;
struct Session {
    rloft_recomp::DpmiSelectorDescriptors descriptors;
    rloft_recomp::DpmiDosMemory dos{&descriptors};
    rloft_recomp::DpmiLinearMemory linear;
    std::array<u16,8> observed_selectors{};
    unsigned selectors=0,alloc_dos=0,release_dos=0,lookups=0;
    unsigned alloc_linear=0,release_linear=0,header_writes=0;
    bool bad_access=false;
};
thread_local Session* active=nullptr;
void need(bool ok,const char* msg){if(!ok)throw std::runtime_error(msg);}
u32 source_dos_allocate(u32 paragraphs,u16* output_selector) {
    ++active->alloc_dos;
    if(paragraphs==0u || paragraphs>0xffffu || !output_selector)return 0u;
    const auto r=active->dos.allocate(static_cast<u16>(paragraphs));
    if(r.carry)return 0u;
    if(active->selectors>=active->observed_selectors.size()) {
        active->bad_access=true;return 0u;
    }
    *output_selector=r.dx;
    active->observed_selectors[active->selectors++]=r.dx;
    return r.ax;
}
code1::s32 source_dos_release(u16 selector) {
    ++active->release_dos;
    return active->dos.release(selector).carry?-1:0;
}
u32 source_selector_base(u16 selector) {
    ++active->lookups;
    const auto r=active->dos.selector_base(selector);
    return r.carry?0u:r.base();
}
u32 source_linear_allocate(u32 bytes,u32* output_handle) {
    ++active->alloc_linear;
    const auto r=active->linear.allocate(bytes);
    if(r.carry || !output_handle)return 0u;
    *output_handle=r.handle;
    return r.linear;
}
code1::s32 source_linear_release(u32 handle) {
    ++active->release_linear;
    return active->linear.release(handle).carry?-1:0;
}
u8* guest_access(u32 base,std::size_t count) {
    if(u8* ptr=active->linear.translate(base,count))return ptr;
    for(unsigned i=0;i<active->selectors;++i) {
        const auto selector=active->observed_selectors[i];
        const auto* block=active->dos.block(selector);
        if(!block)continue;
        const u32 address=static_cast<u32>(block->segment)*16u;
        const u32 length=static_cast<u32>(block->paragraphs)*16u;
        if(base>=address && base-address<=length &&
           count<=length-(base-address))
            return active->dos.translate(selector,base-address,count);
    }
    active->bad_access=true;
    return nullptr;
}
void source_write16(u32 address,u16 value) {
    auto* p=guest_access(address,2u);
    if(!p)return;
    p[0]=static_cast<u8>(value);
    p[1]=static_cast<u8>(value>>8u);
    ++active->header_writes;
}
void source_write32(u32 address,u32 value) {
    auto* p=guest_access(address,4u);
    if(!p)return;
    for(unsigned i=0;i<4u;++i)p[i]=static_cast<u8>(value>>(i*8u));
    ++active->header_writes;
}
u16 word(const u8* p){return static_cast<u16>(p[0]|(u16(p[1])<<8u));}
u32 dword(const u8* p) {
    return u32(p[0])|(u32(p[1])<<8u)|(u32(p[2])<<16u)|(u32(p[3])<<24u);
}
u32 largest_dos_bytes(){return static_cast<u32>(active->dos.largest_free())*16u;}
} // namespace

int main() {
    try {
        Session s;
        need(s.descriptors.install(0x30u,0x00050000u,0x00020000u),
             "CODE.1 loader DGROUP descriptor installation failed");
        active=&s;
        code1::MidDpmiBoundaries b{};
        b.alloc_dos=&source_dos_allocate;
        b.free_dos=&source_dos_release;
        b.get_base=&source_selector_base;
        b.alloc_linear=&source_linear_allocate;
        b.free_linear=&source_linear_release;
        b.write16_guest=&source_write16;
        b.write32_guest=&source_write32;
        b.query_largest_dos_bytes=&largest_dos_bytes;
        need(s.dos.allocation_count()==0u &&
             s.linear.allocation_count()==0u &&
             s.selectors==0u,"source DPMI test preseeded backing memory");

        u32 retained_far_word=0u;u16 retained_selector=0u;
        const u32 real_linear=code1::function_6f10(
            b,64,&retained_far_word,&retained_selector);
        need(real_linear==0x2d600u && retained_selector==0x60u &&
             retained_far_word==0x2d600000u &&
             s.dos.allocation_count()==1u,
             "original CODE.1 6F10 0100 register/result ABI changed");
        need(code1::function_6fc0(b,retained_selector)==real_linear &&
             code1::function_6fc0(b,0x88u)==0u,
             "original CODE.1 6FC0 did not use DPMI 0006 descriptor lookup");

        // Original CODE.1 6FF4 builds a six-byte DOS ownership header
        // using a separate live selector -> linear-base lookup.
        const u32 dos_owned=code1::function_6ff4(b,17);
        need(dos_owned==0x2d650u && s.dos.allocation_count()==2u,
             "original CODE.1 6FF4 allocated incorrect DOS block");
        const u16 owner_selector=s.observed_selectors[1];
        const auto owned_base=s.dos.selector_base(owner_selector);
        u8* owned=guest_access(dos_owned,6u);
        need(!owned_base.carry && owned_base.base()==dos_owned &&
             owned && word(owned)==owner_selector &&
             dword(owned+2u)==0u,
             "original source 6FF4 did not publish DOS selector header");

        const u32 before=s.linear.committed_bytes();
        const u32 linear_owned=code1::function_709c(b,0x1200u);
        need(linear_owned==rloft_recomp::DpmiLinearMemory::kFirstLinear &&
             s.linear.committed_bytes()==before+0x2000u &&
             s.linear.allocation_count()==1u,
             "original CODE.1 709C 0501 did not allocate linear pages");
        u8* linear_header=guest_access(linear_owned,6u);
        need(linear_header && word(linear_header)==0u &&
             dword(linear_header+2u)!=0u &&
             s.linear.block(dword(linear_header+2u))!=nullptr,
             "original CODE.1 709C did not publish opaque 0501 handle");
        const u32 live_handle=dword(linear_header+2u);
        need(code1::function_705c(b,live_handle)==0 &&
             s.linear.block(live_handle)==nullptr &&
             code1::function_705c(b,live_handle)!=0,
             "original CODE.1 705C 0502 handle release/stale behavior");
        need(code1::function_6f74(b,retained_selector)==0 &&
             s.dos.selector_base(retained_selector).carry &&
             code1::function_6fc0(b,retained_selector)==0u &&
             code1::function_6f74(b,retained_selector)!=0,
             "original 6F74 0101 failed to invalidate descriptor");
        need(code1::function_6f74(b,owner_selector)==0 &&
             s.dos.allocation_count()==0u &&
             s.linear.allocation_count()==0u &&
             s.descriptors.size()==1u &&
             !s.descriptors.get_base(0x30u).carry &&
             s.descriptors.get_base(0x30u).base()==0x00050000u &&
             s.descriptors.get_base(retained_selector).carry &&
             !s.bad_access && s.header_writes==4u,
             "source DPMI header or lifetime invariants failed");
        need(s.alloc_dos==2u && s.release_dos==3u &&
             s.alloc_linear==1u && s.release_linear==2u &&
             s.lookups>=4u,"original CODE.1 wrapper call accounting changed");
        need(code1::function_70fc(b)==
                 rloft_recomp::DpmiDosMemory::kLastParagraph*16u-
                 rloft_recomp::DpmiDosMemory::kFirstParagraph*16u,
             "original CODE.1 70FC largest DOS free-space query incorrect");
        active=nullptr;
        std::cout<<"ORIGINAL_CODE1_DPMI_6F10_6FC0_6FF4_709C_705C_6F74=EXECUTED\n"
                    "DPMI_0006_SOURCE_CX_DX_DESCRIPTOR=LIVE_AND_RETIRED\n"
                    "ORIGINAL_CODE1_OWNERSHIP_HEADERS=2_SOURCE_WRITTEN\n"
                    "HOST_DPMI_MEMORY_LIFECYCLE=NO_PRESEEDED_ALLOCATION\n"
                    "ORIGINAL_DOS_PARITY=NO\n"
                    "GAME_BOOTED=NO\n";
        return 0;
    }catch(const std::exception& e) {
        active=nullptr;
        std::cerr<<"CODE1_DPMI_SOURCE_MEMORY_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
