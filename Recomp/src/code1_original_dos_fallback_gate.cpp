// Canonical CODE.1 10AA9/109C1 DOS 48h fallback and 107CB DOS 49h
// reclamation, with actual host-owned DOS paragraph memory. The CODE.1
// guest free-list adapter remains explicitly bounded/test-owned.
#include "../../Code1/Code1.cpp"
#include "dos_paragraph_memory.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
struct Session {
    rloft_recomp::DosParagraphMemory memory;
    std::vector<u8> dgroup;
    code1::Machine machine{};
    unsigned allocated=0, released=0, selector_base=0, free_callbacks=0;
    bool invalid_guest=false, invalid_free=false;
};
thread_local Session* current=nullptr;
void need(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
u32 read_guest(u32 address) {
    u8* p=current->memory.translate(address,4u);
    if(!p || (address&3u)!=0u){current->invalid_guest=true;return 0u;}
    return code1::load_le32(p);
}
void write_guest(u32 address,u32 value) {
    u8* p=current->memory.translate(address,4u);
    if(!p || (address&3u)!=0u){current->invalid_guest=true;return;}
    code1::store_le32(p,value);
}
bool dos_alloc(u16 paragraphs,u16& segment) {
    ++current->allocated;
    const auto result=current->memory.allocate(paragraphs);
    if(result.carry)return false;
    segment=result.ax;
    return true;
}
void dos_free(u16 segment) {
    ++current->released;
    if(current->memory.release(segment).carry)current->invalid_free=true;
}
u32 segment_base(u16 segment) {
    ++current->selector_base;
    // The Watcom boundary supplies a selector; in this constrained DOS
    // fallback the DOS 48h returned segment is mapped to its physical base.
    // A full INT31 0006h selector descriptor service remains an R04 blocker.
    return current->memory.physical_base(segment);
}
void free_adapter(u32 pointer) {
    ++current->free_callbacks;
    if(pointer<4u || !current->memory.translate(pointer-4u,4u)){
        current->invalid_guest=true;return;
    }
    // Source DB5B is not yet integrated. Only its released-bit state
    // transition is provided here for original 107CB arena reclamation.
    write_guest(pointer-4u,read_guest(pointer-4u)&~1u);
}
}
int main(){
    try {
        Session s;
        s.dgroup.assign(code1::kDataObjectVirtualSize,0u);
        s.machine.data=s.dgroup.data();
        s.machine.cpu.ds=0x30u;
        code1::store_le32(s.dgroup.data()+0x55ccu,1u);
        s.dgroup[0x531eu]=1u; // original Watcom DOS/4GW mode 1
        code1::HeapCoreBoundaries heap{};
        heap.read32=&read_guest;heap.write32=&write_guest;
        heap.dos_free_49=&dos_free;
        code1::HeapGrowthBoundaries grow{};
        // Missing 0501h forces the *original* 109C1 DOS-memory fallback.
        grow.dos_alloc_48=&dos_alloc;
        grow.dos_free_49=&dos_free;
        grow.segment_base_0006=&segment_base;
        grow.free_db5b=&free_adapter;
        need(s.memory.allocation_count()==0u,"DOS arena preseeded");
        need(code1::function_10aa9(s.machine,heap,grow,0x300u)==1u,
             "original CODE.1 10AA9 DOS fallback failed");
        need(!s.invalid_guest && !s.invalid_free &&
             s.allocated==2u && s.released==1u &&
             s.selector_base==1u && s.free_callbacks==1u,
             "original 109C1 DOS probe/request/release ABI changed");
        need(s.memory.allocation_count()==1u,"DOS fallback leaked probe");
        const u32 arena=code1::load_le32(s.dgroup.data()+0x8e74u);
        need(arena==0x10030u,"DOS-owned source heap arena base");
        const u16 segment=static_cast<u16>(read_guest(arena+0x10u));
        const auto* block=s.memory.block(segment);
        need(segment==0x1003u && block &&
             block->paragraphs==0x100u && read_guest(arena+4u)==0x1000u,
             "source heap DOS ownership header and paragraph count");
        need(read_guest(arena+0x14u)==0x0fe4u &&
             read_guest(arena+0x20u)==0x0fd8u,
             "source 1069B DOS-backed free block mismatch");
        code1::function_107cb(s.machine,heap);
        need(!s.invalid_guest && !s.invalid_free &&
             s.released==2u && s.memory.allocation_count()==0u &&
             code1::load_le32(s.dgroup.data()+0x8e74u)==0u &&
             s.memory.translate(arena,4u)==nullptr,
             "original 107CB failed DOS 49h arena reclamation");
        current=nullptr;
        std::cout<<"ORIGINAL_CODE1_SOURCE_10AA9_109C1_107CB=EXECUTED\n"
                    "HOST_DOS_INT21_AH48_PROBE_REQUEST=2\n"
                    "HOST_DOS_INT21_AH49_RELEASE=2\n"
                    "HOST_DOS_SEGMENT_PHYSICAL_BASE=BOUNDED_ALIAS\n"
                    "ORIGINAL_DOS_PARITY=NO\n"
                    "GAME_BOOTED=NO\n";
        return 0;
    }catch(const std::exception& e) {
        current=nullptr;
        std::cerr<<"CODE1_ORIGINAL_DOS_FALLBACK_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
