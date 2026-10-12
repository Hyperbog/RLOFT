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
    unsigned allocations=0, resizes=0, releases=0, return_to_free_list=0;
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
code1::DpmiBlockResult linear_resize(u32 handle,u32 bytes) {
    ++active->resizes;
    const auto result=active->memory.resize(handle,bytes);
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
        const u32 rejected=code1::function_109c1(
            session.machine,hb,gb,
            rloft_recomp::DpmiLinearMemory::kBudget+1u);
        need(rejected==0u && session.memory.allocation_count()==0u &&
             code1::load_le32(session.dgroup.data()+0x8e74u)==0u,
             "original 109C1 OOM path mutated heap");
        const u32 result=code1::function_10aa9(
            session.machine,hb,gb,0x300u);
        need(result==1u,"original CODE.1 10AA9 failed to grow the heap");
        need(!session.invalid_memory && !session.invalid_free,
             "original heap touched unmapped guest memory");
        need(session.allocations==2u && session.releases==0u &&
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
             heap_read(raw)==0x0fe4u,
             "original 1069B free-list arena or size mismatch");
        need(session.last_free_pointer==raw+0x10u &&
             heap_read(session.last_free_pointer-4u)>0u,
             "original 10AA9 did not publish new free allocation");
        // Original CODE.1 107CB now owns the 0502 release, not host test cleanup.
        code1::function_107cb(session.machine,hb);
        need(session.releases==1u && !session.invalid_free &&
             !session.invalid_memory,
             "original 107CB failed to release host DPMI allocation");
        need(session.memory.allocation_count()==0u &&
             session.memory.block(handle)==nullptr &&
             code1::load_le32(session.dgroup.data()+0x8e74u)==0u,
             "source 107CB left a stale guest arena or handle");

        // Original 1083F requires an allocated terminal block. The source
        // free-list callback is still a test observer, so only the terminal
        // used bit is installed as an explicit bounded test precondition.
        Session resizing;
        resizing.dgroup.assign(code1::kDataObjectVirtualSize,0u);
        resizing.machine.data=resizing.dgroup.data();
        resizing.machine.cpu.ds=0x30u;
        code1::store_le32(resizing.dgroup.data()+0x55ccu,1u);
        resizing.dgroup[0x531eu]=1u;
        active=&resizing;
        auto growth=gb;
        growth.resize_0503=&linear_resize;
        need(code1::function_10aa9(resizing.machine,hb,growth,0x300u)==1u,
             "source 10AA9 failed to establish resize arena");
        const u32 before=code1::load_le32(resizing.dgroup.data()+0x8e74u);
        const u32 old_handle=heap_read(before);
        need(heap_read(before+0x14u)==0x0fe4u,
             "resize setup source arena size changed");
        heap_write(before+0x20u,heap_read(before+0x20u)|1u);
        const u32 relocated=code1::function_1083f(
            resizing.machine,hb,growth,before+0x20u,0x1200u);
        const u32 after=code1::load_le32(resizing.dgroup.data()+0x8e74u);
        need(relocated==after+0x24u && after!=before &&
             resizing.resizes==1u && resizing.memory.block(old_handle)==nullptr &&
             resizing.memory.translate(before,4u)==nullptr,
             "source 1083F did not perform DPMI 0503 relocation");
        const u32 new_handle=heap_read(after);
        const auto* new_block=resizing.memory.block(new_handle);
        need(new_block && new_block->linear==after &&
             new_block->committed==0x2000u &&
             heap_read(after+4u)==0x2000u &&
             (heap_read(after+0x20u)&1u)==1u,
             "source 1083F did not rebuild relocated arena");
        need(!resizing.invalid_memory && !resizing.invalid_free &&
             resizing.return_to_free_list==2u,
             "source 1083F guest accesses or tail release invalid");
        need(!resizing.memory.release(new_handle).carry,
             "resize regression test cleanup failed");
        active=nullptr;
        std::cout<<"ORIGINAL_CODE1_SOURCE_10AA9_109C1_1069B=EXECUTED\n"
                    "DPMI_0501_HOST_ALLOCATION=YES\n"
                    "SOURCE_HEAP_ARENA_PRESEEDED=NO\n"
                    "DPMI_0502_SOURCE_FREE=EXECUTED_AND_SOURCE_1083F_0503_RELOCATED\n"
                    "ORIGINAL_CODE1_109C1_OOM_REJECTED=YES\\n"
                    "ORIGINAL_DOS_PARITY=NO\n"
                    "GAME_BOOTED=NO\n";
        return 0;
    }catch(const std::exception& e) {
        active=nullptr;
        std::cerr<<"CODE1_ORIGINAL_HEAP_GROWTH_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
