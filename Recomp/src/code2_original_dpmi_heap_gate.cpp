// R04 CODE.2 source execution: an initially empty arena chain is grown by
// actual reconstructed 37B42 -> 39D4D -> 39C7B -> DPMI 0501 -> 39BBC.
// Source 39838/398E0 performs allocation, splitting, free and reuse.
// A bounded shared linear aperture permits original Machine.data + guest
// address to access the very same host-owned DPMI pages. Not DOS/4GW parity.
#include "../../Code2/Code2.cpp"
#include "dpmi_linear_memory.hpp"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
struct Session {
    // Both provider and original dg(m,linear) now share exact guest bytes.
    std::vector<u8> guest=std::vector<u8>(0x01100000u,0u);
    rloft_recomp::DpmiLinearMemory memory;
    std::vector<std::pair<u32,u32>> owned; // (handle, linear base)
    code2::Machine machine{};
    code2::Runtime142Boundary dpmi{};
    code2::Runtime39d4dBoundary growth{};
    code2::Runtime129Boundary heap{};
    bool bad=false;
    unsigned grow_calls=0, free_calls=0, resize_calls=0, release_calls=0;
    Session():memory(guest.data(),guest.size()) {
        machine.data=guest.data();
    }
};
thread_local Session* active=nullptr;
u32 read(u32 p) noexcept {return code2::load_le32(active->guest.data()+p);}
void check(bool ok,const char* what) {
    if(!ok)throw std::runtime_error(what);
}
code2::Runtime142Boundary::Dpmi0501Result allocate(u32 bytes) noexcept {
    auto& s=*active;
    const auto r=s.memory.allocate(bytes);
    if(r.carry)return {true,0u,0u,0u};
    const auto* block=s.memory.block(r.handle);
    if(!block || r.linear>=s.guest.size() ||
       block->committed>s.guest.size()-r.linear) {
        s.bad=true;
        (void)s.memory.release(r.handle);
        return {true,0u,0u,0u};
    }
    // Source writes through Machine.data now address host-owned DPMI pages.
    if(s.memory.translate(r.linear,block->committed)!=
       s.guest.data()+r.linear) {
        s.bad=true;
        (void)s.memory.release(r.handle);
        return {true,0u,0u,0u};
    }
    s.owned.emplace_back(r.handle,r.linear);
    return {false,r.linear,static_cast<u16>(r.handle>>16u),
            static_cast<u16>(r.handle)};
}
void release_block(u32 handle) noexcept {
    ++active->release_calls;
    if(active->memory.release(handle).carry)active->bad=true;
}
code2::Runtime39a8aBoundary::Dpmi0503Result resize_source_block(
        u32 bytes,u16 old_hi,u16 old_lo) noexcept {
    ++active->resize_calls;
    const u32 old=(static_cast<u32>(old_hi)<<16u)|old_lo;
    const auto r=active->memory.resize(old,bytes);
    return {r.carry,r.linear,static_cast<u16>(r.handle>>16u),
            static_cast<u16>(r.handle)};
}
u32 source_39c7b(u32 bytes) noexcept {
    return code2::function_39c7b(active->machine,active->dpmi,bytes);
}
u32 source_39bbc(u32 node) noexcept {
    return code2::function_39bbc(active->machine,node);
}
void source_free_new_arena(u32 payload) noexcept {
    const u32 node=payload-0x30u;
    if(node!=read(0x12624u) && read(node+4u)==0u) {
        // Another arena can be inserted ahead of this one.
        if(read(node+0x2Cu)==0u){active->bad=true;return;}
    }
    code2::function_398e0(active->machine,node,payload);
}
u32 grow(u32 bytes) noexcept {
    ++active->grow_calls;
    return code2::function_39d4d(active->machine,active->growth,bytes);
}
u32 allocate_from_chain(u32 root,u32 request) noexcept {
    if(root!=0x12624u){active->bad=true;return 0u;}
    for(u32 node=read(root);node;node=read(node+8u)) {
        const u32 p=code2::function_39838(active->machine,node,request);
        if(p)return p;
    }
    return 0u;
}
void free_from_chain(u32 root,u32 payload) noexcept {
    if(root!=0x12624u){active->bad=true;return;}
    for(u32 node=read(root);node;node=read(node+8u)) {
        const u32 end=node+read(node);
        if(payload>=node+0x30u && payload<end) {
            code2::function_398e0(active->machine,node,payload);
            ++active->free_calls;
            return;
        }
    }
    active->bad=true;
}
void release_resized_tail(u32 payload) noexcept {
    // Only the arena-list lookup is host-owned. Allocation splitting,
    // free-list insertion and coalescing run real 398E0 source.
    free_from_chain(0x12624u,payload);
}
void configure(Session& s) {
    active=&s;
    // Source-required initialization flags; NO heap blocks or arena headers
    // are written by the host. The chain starts at its actual zero value.
    s.guest[0x12662u]=1u; // Watcom extender mode for DPMI 0501
    code2::store_le32(s.guest.data()+0x12708u,1u); // heap enabled
    s.dpmi.dpmi_alloc=&allocate;
    s.dpmi.release_dpmi_block=&release_block;
    s.growth.alloc_39c7b=&source_39c7b;
    s.growth.init_arena_39bbc=&source_39bbc;
    s.growth.free_37c45=&source_free_new_arena;
    s.heap.arena_alloc=&allocate_from_chain;
    s.heap.arena_free=&free_from_chain;
    s.heap.grow_heap=&grow;
}
void test() {
    Session s;
    configure(s);
    check(read(0x12624u)==0u && s.memory.allocation_count()==0u,
          "source allocator began with a seeded arena");
    const u32 small=code2::function_37b42(s.heap,60u);
    check(small!=0u && s.grow_calls==1u &&
          s.memory.allocation_count()==1u,"first original 39D4D growth failed");
    const u32 first_node=read(0x12624u);
    check(first_node==s.owned[0].second+8u &&
          small==first_node+0x30u &&
          read(first_node+0x18u)==1u,
          "original arena/link/first-allocation metadata differs");
    const u32 big=code2::function_37b42(s.heap,7000u);
    check(big!=0u && big!=small && s.grow_calls==2u &&
          s.memory.allocation_count()==2u,
          "source 37B42 failed to grow a second real host arena");
    check(read(first_node+8u)==s.owned[1].second+8u,
          "39BBC failed to link host-owned second arena");
    const u32 root=read(0x12624u);
    s.guest[small]=0xA5u;s.guest[big]=0x5Au;
    u8* live_small=s.memory.translate(small,1u);
    u8* live_big=s.memory.translate(big,1u);
    check(live_small==s.guest.data()+small &&
          live_big==s.guest.data()+big &&
          *live_small==0xA5u && *live_big==0x5Au,
          "original heap writes not visible in live host DPMI pages");
    *live_small=0xB7u;
    check(s.guest[small]==0xB7u,
          "host DPMI write not visible to original CODE.2 source");
    *live_small=0xA5u;
    // OOM must neither invent an arena nor destroy the two live blocks.
    const u32 rejected=code2::function_37b42(s.heap,0x01000000u);
    check(rejected==0u && s.memory.allocation_count()==2u &&
          read(0x12624u)==root && s.guest[small]==0xA5u &&
          s.guest[big]==0x5Au,"out-of-memory rollback corrupted live allocations");
    code2::function_37c37(s.heap,big);
    code2::function_37c37(s.heap,small);
    check(s.free_calls==2u && !s.bad,
          "original 398E0 free callbacks failed");
    const u32 reused=code2::function_37b42(s.heap,60u);
    check(reused==small && s.memory.allocation_count()==2u,
          "original 39838 failed first-fit reuse without new host allocation");
    // Provider and reconstructed CODE.2 must retain one guest byte image.
    // No end-of-test copy-back is permitted.
    for(const auto& [handle,base]:s.owned) {
        const auto* block=s.memory.block(handle);
        check(block!=nullptr && block->linear==base,
              "DPMI handle lost before host-backed snapshot");
        u8* host=s.memory.translate(base,block->committed);
        check(host==s.guest.data()+base,
              "original CODE.2 did not write host-owned DPMI memory");
        check(!s.memory.release(handle).carry,"host release failed");
        check(s.memory.translate(base,1u)==nullptr,
              "released host allocation remained mapped");
    }
    check(s.memory.allocation_count()==0u && !s.bad,
          "host allocator leaked a source-owned block");
    active=nullptr;
    std::cout<<"CODE2_ORIGINAL_37B42_39D4D_39C7B_39BBC=PASS\n"
               "CODE2_HOST_DPMI_0501_TWO_ARENAS=PASS\n"
               "CODE2_SOURCE_FREE_SPLIT_REUSE_OOM=PASS\n"
               "CODE2_SOURCE_39A2C_39A8A_FULL_PARITY=NO\n"
               "CODE2_UNIFIED_GUEST_PAGE_MAPPING=YES\n"
               "R04_ACCEPTED=NO\n";
}
// R04: exact CODE.2 original DPMI 0503 and 0502 heap-lifetime path.
// This is a second isolated stateful guest, with no fabricated arena nodes.
void test_source_resize_and_reclamation() {
    Session s;
    configure(s);
    const u32 original_payload=code2::function_37b42(s.heap,60u);
    check(original_payload!=0u && s.memory.allocation_count()==1u,
          "original CODE.2 source did not establish initial resize arena");
    const u32 old_node=read(0x12624u);
    const u32 old_arena=old_node+0x2Cu;
    const u32 old_base=s.owned[0].second,old_handle=s.owned[0].first;
    check(original_payload==old_arena+4u &&
          read(old_node+0x18u)==1u &&
          read(old_base)==old_handle,
          "39A8A source precondition must be a single allocated block");
    s.guest[original_payload+12u]=0xA6u;
    s.guest[original_payload+13u]=0xB7u;

    code2::Runtime141Boundary cleanup{};
    cleanup.release_dpmi_block=&release_block;
    code2::Runtime39a8aBoundary resizing{};
    resizing.cleanup=&cleanup;
    resizing.dpmi_resize=&resize_source_block;
    resizing.release_payload=&release_resized_tail;
    const u32 new_arena=code2::function_39a8a(
        s.machine,resizing,old_arena,0x1800u);
    const u32 new_node=read(0x12624u);
    check(new_arena!=0u && new_node!=old_node &&
          new_arena==new_node+0x2Cu &&
          s.resize_calls==1u && s.release_calls==0u &&
          s.memory.allocation_count()==1u &&
          s.memory.block(old_handle)==nullptr &&
          s.memory.translate(old_base,1u)==nullptr,
          "original 39A8A did not relocate and retire DPMI 0503 owner");
    const u32 new_base=new_node-8u;
    const u32 new_handle=read(new_base);
    const auto* owned=s.memory.block(new_handle);
    check(owned && owned->linear==new_base &&
          owned->committed==0x3000u &&
          s.memory.translate(new_base,0x3000u)==s.guest.data()+new_base &&
          read(new_base+4u)==0u &&
          read(new_node+0x18u)==1u,
          "original 39A8A new arena/source metadata not backed by host 0503");
    check(s.guest[new_arena+4u+12u]==0xA6u &&
          s.guest[new_arena+4u+13u]==0xB7u &&
          s.free_calls==1u && !s.bad,
          "original 39A8A relocation lost user bytes or failed tail free");
    const u32 marker=new_arena+4u+12u;
    check(s.memory.translate(marker,1u)==s.guest.data()+marker,
          "relocated 39A8A user bytes not visible to DPMI provider");
    code2::function_37c37(s.heap,new_arena+4u);
    check(s.free_calls==2u && read(new_node+0x18u)==0u &&
          !s.bad,"original 398E0 failed to coalesce resized heap");
    code2::function_39a2c(s.machine,cleanup);
    check(s.release_calls==1u &&
          s.memory.allocation_count()==0u &&
          s.memory.translate(new_base,1u)==nullptr &&
          read(0x12624u)==0u && !s.bad,
          "original CODE.2 39A2C failed to reclaim whole DPMI heap");
    active=nullptr;
    std::cout<<"CODE2_SOURCE_39A8A_DPMI_0503_RELOCATION=PASS\\n"
               "CODE2_SOURCE_39A2C_DPMI_0502_RECLAMATION=PASS\\n"
               "CODE2_HEAP_RELOCATION_WITHOUT_PRESEEDED_ARENA=YES\\n"
               "R04_ACCEPTED=NO\\n";
}
} // namespace
int main() {
    try{test();test_source_resize_and_reclamation();return 0;}
    catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_HEAP_GATE_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
