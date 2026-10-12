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
    unsigned grow_calls=0, free_calls=0;
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
    if(active->memory.release(handle).carry)active->bad=true;
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
} // namespace
int main() {
    try{test();return 0;}
    catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_HEAP_GATE_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
