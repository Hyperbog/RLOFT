// CODE.1 original Watcom near-heap growth with a genuine host-owned
// DPMI linear allocation. No guest arena is preseeded by this test.
// Scope: source 10AA9 -> 109C1 -> DPMI 0501 -> 1069B, not game startup.
#include "../../Code1/Code1.cpp"
#include "dpmi_linear_memory.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using u8=std::uint8_t; using u32=std::uint32_t;
struct Session {
    rloft_recomp::DpmiLinearMemory memory;
    std::vector<u8> dgroup;
    code1::Machine machine{};
    bool invalid_memory=false, invalid_free=false;
    unsigned allocations=0, releases=0, return_to_free_list=0;
    u32 last_free_pointer=0;
};
thread_local Session* active=nullptr;
void need(bool yes,const char* reason) {
    if(!yes)throw std::runtime_error(reason);
}
u32 heap_read(u32 guest) {
    u8* ptr=active->memory.translate(guest,4);
    if(!ptr || (guest&3u)!=0u) {
        active->invalid_memory=true;
        return 0;
    }
    return code1::load_le32(ptr);
}
void heap_write(u32 guest,u32 value) {
    u8* ptr=active->memory.translate(guest,4);
    if(!ptr || (guest&3u)!=0u) {
        active->invalid_memory=true;
        return;
    }
    code1::store_le32(ptr,value);
}
code1::DpmiBlockResult linear_allocate(u32 bytes) {
    ++active->allocations;
    const auto result=active->memory.allocate(bytes);
    return {result.carry,result.linear,result.handle};
}
void linear_free(u32 handle) {
    ++active->releases;
    if(active->memory.release(handle).carry)active->invalid_free=true;
}
void observe_original_free(u32 guest_ptr) {
    ++active->return_to_free_list;
    active->last_free_pointer=guest_ptr;
    if(guest_ptr<4u || !active->memory.translate(guest_ptr-4u,4))
        active->invalid_memory=true;
}
}
int main() {
    try {
        Session session;
        session.dgroup.assign(code1::kDataObjectVirtualSize,0u);
        session.machine.data=session.dgroup.data();
        session.machine.cpu.ds=0x30u;
        active=&session;
        // Source 10AA9 requires the near-heap growth enable flag; mode 1
        // denotes the DOS/4GW-backed original runtime branch.
        code1::store_le32(session.dgroup.data()+0x55ccu,1u);
        session.dgroup[0x531eu]=1u;
        code1::HeapCoreBoundaries hb{};
        hb.read32=&heap_read;
        hb.write32=&heap_write;
        hb.dpmi_free_0502=&linear_free;
        code1::HeapGrowthBoundaries gb{};
        gb.alloc_0501=&linear_allocate;
        gb.free_db5b=&observe_original_free;
        need(session.memory.allocation_count()==0u,
             "CODE.1 heap started from synthetic preseeded arena");
        const u32 result=code1::function_10aa9(
            session.machine,hb,gb,0x300u);
        need(result==1u,"original CODE.1 10AA9 failed to grow the heap");
        need(!session.invalid_memory && !session.invalid_free,
             "original heap touched unmapped guest memory");
        need(session.allocations==1u && session.releases==0u &&
             session.return_to_free_list==1u,
             "original growth did not traverse 109C1 and free-list callback");
        need(session.memory.allocation_count()==1u,
             "original growth did not retain a live DPMI allocation");
        const u32 arena=code1::load_le32(session.dgroup.data()+0x8e74u);
        need(arena==rloft_recomp::DpmiLinearMemory::kFirstLinear,
             "original 109C1 arena head not stored from host DPMI 0501");
        const u32 handle=heap_read(arena);
        const auto* block=session.memory.block(handle);
        need(block && block->linear==arena && block->committed==0x1000u,
             "CODE.1 arena does not own actual DPMI pages");
        need(heap_read(arena+4u)==block->requested &&
             block->requested==0x1000u &&
             heap_read(arena+0x10u)==0u,
             "original 109C1 ownership header mismatch");
        const u32 raw=code1::load_le32(session.dgroup.data()+0x30b0u);
        need(raw==arena+0x14u &&
             code1::load_le32(session.dgroup.data()+0x30b4u)==raw &&
             heap_read(raw)==0x0fd8u,
             "original 1069B free-list arena or size mismatch");
        need(session.last_free_pointer==raw+0x10u &&
             heap_read(session.last_free_pointer-4u)>0u,
             "original 10AA9 did not publish new free allocation");
        // The final lifecycle release here is test cleanup; it is NOT
        // evidence that original CODE.1 heap eviction has run.
        need(!session.memory.release(handle).carry,
             "DPMI host-backed cleanup failed");
        need(session.memory.allocation_count()==0u,
             "DPMI block leaked at end of source execution");
        active=nullptr;
        std::cout<<"ORIGINAL_CODE1_SOURCE_10AA9_109C1_1069B=EXECUTED\n"
                    "DPMI_0501_HOST_ALLOCATION=YES\n"
                    "SOURCE_HEAP_ARENA_PRESEEDED=NO\n"
                    "DPMI_0502_SOURCE_FREE=NOT_YET\n"
                    "ORIGINAL_DOS_PARITY=NO\n"
                    "GAME_BOOTED=NO\n";
        return 0;
    }catch(const std::exception& e) {
        active=nullptr;
        std::cerr<<"CODE1_ORIGINAL_HEAP_GROWTH_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
