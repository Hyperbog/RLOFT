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
    bool invalid_guest=false, invalid_free=false, invalid_dispatch=false;
};
thread_local Session* current=nullptr;
void need(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
// Original DB5B/10590 also accesses static DGROUP circular-list roots,
// which must not alias DOS conventional paragraphs.
u8* guest_word(u32 address) {
    if((address&3u)!=0u){current->invalid_guest=true;return nullptr;}
    if(address<current->dgroup.size()) {
        if(current->dgroup.size()-address>=4u)
            return current->dgroup.data()+address;
        current->invalid_guest=true;
        return nullptr;
    }
    u8* p=current->memory.translate(address,4u);
    if(!p)current->invalid_guest=true;
    return p;
}
u32 read_guest(u32 address) {
    u8* p=guest_word(address);
    return p?code1::load_le32(p):0u;
}
void write_guest(u32 address,u32 value) {
    u8* p=guest_word(address);
    if(p)code1::store_le32(p,value);
}
void original_heap_roots(Session& s) {
    // Authenticated original CODE.1 LE object-2 bytes, four DWORD roots.
    u8* d=s.dgroup.data();
    code1::store_le32(d+0x30c0u,0x30d4u);
    code1::store_le32(d+0x30c8u,0xffffffffu);
    code1::store_le32(d+0x30d8u,0x30d4u);
    code1::store_le32(d+0x30dcu,0x30d4u);
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
void free_core_10590(u32 pointer,u16 ds,u32 arena) {
    if(ds!=current->machine.cpu.ds || arena!=0x30b8u) {
        current->invalid_dispatch=true;return;
    }
    code1::HeapCoreBoundaries hb{};
    hb.read32=&read_guest;
    hb.write32=&write_guest;
    code1::function_10590(hb,pointer,arena);
}
void source_clear_8e78(u32 address,u8 value) {
    if(address!=0x8e78u || address>=current->dgroup.size()) {
        current->invalid_dispatch=true;return;
    }
    current->dgroup[address]=value;
}
void free_adapter(u32 pointer) {
    ++current->free_callbacks;
    if(pointer<4u || !current->memory.translate(pointer-4u,4u)){
        current->invalid_guest=true;return;
    }
    code1::AllocBoundary ab{};
    ab.ds=current->machine.cpu.ds;
    ab.call_10590=&free_core_10590;
    ab.write_linear8=&source_clear_8e78;
    code1::function_db5b(ab,pointer);
}
}
int main(){
    try {
        Session s;
        current=&s;
        s.dgroup.assign(code1::kDataObjectVirtualSize,0u);
        s.machine.data=s.dgroup.data();
        s.machine.cpu.ds=0x30u;
        original_heap_roots(s);
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
        need(!s.invalid_guest && !s.invalid_free && !s.invalid_dispatch &&
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
             read_guest(arena+0x20u)==0x0fd8u &&
             read_guest(0x30c0u)==0x30d4u &&
             read_guest(0x30d8u)==arena+0x20u &&
             read_guest(0x30dcu)==arena+0x20u &&
             read_guest(arena+0x24u)==0x30d4u &&
             read_guest(arena+0x28u)==0x30d4u,
             "source 1069B/DB5B/10590 DOS-backed free list mismatch");
        const u32 allocated=code1::function_104e0(heap,0x60u,0x30b8u);
        need(allocated==arena+0x24u &&
             (read_guest(allocated-4u)&1u)!=0u,
             "original 104E0 did not split DOS-backed arena");
        free_adapter(allocated);
        need(read_guest(arena+0x20u)==0x0fd8u &&
             read_guest(0x30c0u)==arena+0x20u &&
             read_guest(0x30d0u)==1u,
             "original DB5B/10590 did not coalesce DOS-backed blocks");
        code1::function_107cb(s.machine,heap);
        need(!s.invalid_guest && !s.invalid_free && !s.invalid_dispatch &&
             s.free_callbacks==2u &&
             s.released==2u && s.memory.allocation_count()==0u &&
             code1::load_le32(s.dgroup.data()+0x8e74u)==0u &&
             s.memory.translate(arena,4u)==nullptr &&
             read_guest(0x30c0u)==0x30d4u &&
             read_guest(0x30d8u)==0x30d4u &&
             read_guest(0x30dcu)==0x30d4u &&
             read_guest(0x30d0u)==0u,
             "original 107CB failed DOS 49h arena reclamation");
        current=nullptr;
        std::cout<<"ORIGINAL_CODE1_SOURCE_10AA9_109C1_107CB=EXECUTED\n"
                    "HOST_DOS_INT21_AH48_PROBE_REQUEST=2\n"
                    "HOST_DOS_INT21_AH49_RELEASE=2\n"
                    "ORIGINAL_CODE1_DB5B_10590_DOS_SPLIT_COALESCE=EXECUTED\n"
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
