// R04: execute original CODE.2 39C7B/39D4D DOS-memory fallback and
// source 39A2C DOS selector reclamation, plus original 39E1C break growth.
// No heap headers are preseeded, and no commercial media is needed.
#include "../../Code2/Code2.cpp"
#include "dos_paragraph_memory.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using u8=std::uint8_t;
using u16=std::uint16_t;
using u32=std::uint32_t;
using s32=std::int32_t;
constexpr u32 kDosGuestAlias=0x300000u;
constexpr u32 kGuestSize=0x400000u;

struct Session {
    // Original CODE.2 DGROUP and DOS-owned guest physical segments must not
    // collide. Here a checked selector-base alias maps conventional memory
    // into a disjoint region of the one guest byte image.
    std::vector<u8> guest=std::vector<u8>(kGuestSize,0u);
    rloft_recomp::DosParagraphMemory dos;
    code2::Machine machine{};
    code2::Runtime142Boundary memory{};
    code2::Runtime39d4dBoundary growth{};
    code2::Runtime129Boundary heap{};
    code2::Runtime141Boundary cleanup{};
    u32 break_end=0x30000u;
    unsigned dpmi_denials=0,dos_attempts=0,dos_allocations=0;
    unsigned dos_frees=0,selector_queries=0,arena_free_calls=0;
    unsigned grow_calls=0,break_calls=0;
    bool fail_second_dos_request=false,bad=false;
    Session():dos(guest.data()+kDosGuestAlias,guest.size()-kDosGuestAlias) {
        machine.data=guest.data();
    }
};
thread_local Session* active=nullptr;
void need(bool yes,const char* why){if(!yes)throw std::runtime_error(why);}
u32 word(u32 address) noexcept {
    if(address>active->guest.size()-4u){active->bad=true;return 0u;}
    return code2::load_le32(active->guest.data()+address);
}
code2::Runtime142Boundary::Dpmi0501Result fail_dpmi(u32) noexcept {
    ++active->dpmi_denials;
    return {true,0u,0u,0u};
}
s32 dos_allocate(u16 paragraphs) noexcept {
    ++active->dos_attempts;
    if(active->fail_second_dos_request && active->dos_attempts==2u)
        return -1;
    const auto result=active->dos.allocate(paragraphs);
    if(result.carry)return -1;
    ++active->dos_allocations;
    return result.ax;
}
void dos_free(u16 selector) noexcept {
    ++active->dos_frees;
    if(active->dos.release(selector).carry)active->bad=true;
}
u32 selector_base(u16 selector) noexcept {
    ++active->selector_queries;
    const auto physical=active->dos.physical_base(selector);
    if(!physical){active->bad=true;return 0u;}
    return kDosGuestAlias+physical;
}
u32 original_39c7b(u32 bytes) noexcept {
    return code2::function_39c7b(active->machine,active->memory,bytes);
}
u32 original_39bbc(u32 node) noexcept {
    return code2::function_39bbc(active->machine,node);
}
void original_free_chain(u32 root,u32 payload) noexcept {
    if(root!=0x12624u){active->bad=true;return;}
    for(u32 node=word(root);node;node=word(node+8u)){
        const u32 extent=word(node);
        if(extent>0x2Cu && payload>=node+0x30u &&
           payload<node+extent) {
            code2::function_398e0(active->machine,node,payload);
            ++active->arena_free_calls;
            return;
        }
    }
    active->bad=true;
}
void original_release_first(u32 payload) noexcept {
    original_free_chain(0x12624u,payload);
}
u32 original_39d4d(u32 bytes) noexcept {
    ++active->grow_calls;
    return code2::function_39d4d(active->machine,active->growth,bytes);
}
u32 original_39e1c(u32 bytes) noexcept {
    ++active->grow_calls;
    return code2::function_39e1c(active->machine,active->growth,bytes);
}
u32 original_alloc_chain(u32 root,u32 request) noexcept {
    if(root!=0x12624u){active->bad=true;return 0u;}
    for(u32 node=word(root);node;node=word(node+8u)){
        const u32 payload=code2::function_39838(active->machine,node,request);
        if(payload)return payload;
    }
    return 0u;
}
u32 host_limit() noexcept {return active->break_end-1u;}
u32 host_extend(u32 requested) noexcept {
    ++active->break_calls;
    if(requested>active->guest.size() || requested>0x37000u ||
       requested<=active->break_end)return 0xFFFFFFFFu;
    const u32 previous=active->break_end;
    active->break_end=requested;
    return previous;
}
void configure(Session& s,u8 mode) {
    active=&s;
    s.guest[0x12662u]=mode;
    (void)code2::function_3a395(s.machine,1u);
    s.memory.dpmi_alloc=&fail_dpmi;
    s.memory.dos_alloc_paragraphs=&dos_allocate;
    s.memory.dos_free_selector=&dos_free;
    s.memory.selector_base=&selector_base;
    s.memory.release_dos_block=&dos_free;
    s.cleanup.release_dos_block=&dos_free;
    s.growth.alloc_39c7b=&original_39c7b;
    s.growth.init_arena_39bbc=&original_39bbc;
    s.growth.free_37c45=&original_release_first;
    s.growth.lsl_ds=&host_limit;
    s.growth.limit_3a467=&host_extend;
    s.heap.arena_alloc=&original_alloc_chain;
    s.heap.arena_free=&original_free_chain;
    s.heap.grow_heap=mode==1u?&original_39d4d:&original_39e1c;
    need(word(0x12624u)==0u &&
         s.dos.allocation_count()==0u,
         "original CODE.2 heap must start without seeded arena");
}
void test_dos_fallback() {
    Session s;
    configure(s,1u);
    const u32 payload=code2::function_37b42(s.heap,60u);
    need(payload && !s.bad && s.grow_calls==1u &&
         s.dpmi_denials==1u && s.dos_attempts==2u &&
         s.dos_allocations==2u && s.dos_frees==1u &&
         s.selector_queries==1u && s.arena_free_calls==1u &&
         s.dos.allocation_count()==1u,
         "original CODE.2 39C7B DOS fallback/probe path failed");
    const u32 node=word(0x12624u);
    const u16 selector=static_cast<u16>(word(node-4u));
    const u32 physical=s.dos.physical_base(selector);
    need(selector==0x1003u && node==kDosGuestAlias+physical+8u &&
         payload==node+0x30u && word(node+0x18u)==1u &&
         word(node-4u)==static_cast<u32>(selector) &&
         word(node-8u)==0u &&
         s.dos.translate(physical,0x1000u)==s.guest.data()+
                                               kDosGuestAlias+physical &&
         s.guest[0x12624u]==static_cast<u8>(node),
         "original 39BBC arena lacks real host DOS ownership metadata");
    s.guest[payload+12u]=0x7Au;
    need(s.dos.translate(physical+payload-(kDosGuestAlias+physical)+12u,1u)[0]==0x7Au,
         "source CODE.2 payload not visible through DOS allocation");
    // DOS allocation failure of the second request must release the
    // original one-paragraph probe without publishing an arena.
    code2::function_37c37(s.heap,payload);
    need(word(node+0x18u)==0u && !s.bad,
         "original 398E0 did not coalesce DOS-backed arena");
    code2::function_39a2c(s.machine,s.cleanup);
    need(word(0x12624u)==0u && s.dos_frees==2u &&
         s.dos.allocation_count()==0u &&
         s.dos.translate(physical,1u)==nullptr && !s.bad,
         "original 39A2C DOS 49h reclamation leaked selector");
    // Large allocations over the 39C7B 64-KiB fallback ceiling fail
    // without consuming a DOS block, and never invent a success receipt.
    const unsigned attempts=s.dos_attempts;
    const u32 denied=code2::function_37b42(s.heap,0x12000u);
    need(denied==0u && word(0x12624u)==0u &&
         s.dos_attempts==attempts && s.dos.allocation_count()==0u,
         "39C7B invalid DOS-sized request published heap pages");
    active=nullptr;

    Session injected;
    configure(injected,1u);
    injected.fail_second_dos_request=true;
    const u32 fail=code2::function_37b42(injected.heap,60u);
    need(fail==0u && injected.dos_attempts==2u &&
         injected.dos_allocations==1u && injected.dos_frees==1u &&
         injected.dos.allocation_count()==0u &&
         word(0x12624u)==0u && !injected.bad,
         "original 39C7B DOS probe rollback leaked on second request");
    active=nullptr;
}
void test_source_39e1c_break_growth() {
    Session s;
    configure(s,2u); // original extender-memory DS-limit branch
    const u32 first=code2::function_37b42(s.heap,60u);
    need(first && s.grow_calls==1u &&
         s.break_calls==1u && s.break_end==0x31000u &&
         s.dos_attempts==0u && s.dpmi_denials==0u &&
         word(0x12624u)==0x30000u &&
         word(0x30000u+0x18u)==1u && !s.bad,
         "original 39E1C did not extend host-backed guest data break");
    s.guest[first+13u]=0xA6u;
    const u32 second=code2::function_37b42(s.heap,7000u);
    if(!(second && second!=first && s.grow_calls==2u &&
         s.break_calls==2u && s.break_end==0x33000u &&
         s.guest[first+13u]==0xA6u && !s.bad)) {
        std::cerr<<"39E1C_ADJACENT_GROWTH_DIAGNOSTIC"
                 <<" first="<<first<<" second="<<second
                 <<" grows="<<s.grow_calls
                 <<" breaks="<<s.break_calls
                 <<" end="<<s.break_end
                 <<" head="<<word(0x12624u)
                 <<" size="<<word(0x30000u)
                 <<" alloccount="<<word(0x30018u)
                 <<" freelist="<<word(0x3000Cu)
                 <<" free_callbacks="<<s.arena_free_calls
                 <<" marker="<<static_cast<unsigned>(s.guest[first+13u])
                 <<" bad="<<s.bad<<'\\n';
    }
    need(second && second!=first && s.grow_calls==2u &&
         s.break_calls==2u && s.break_end==0x33000u &&
         s.guest[first+13u]==0xA6u && !s.bad,
         "original 39E1C adjacent growth lost original guest allocation");
    const u32 frozen=s.break_end;
    const u32 before=word(0x12624u);
    const u32 fail=code2::function_37b42(s.heap,0x80000u);
    need(fail==0u && s.break_end==frozen &&
         word(0x12624u)==before &&
         s.guest[first+13u]==0xA6u && !s.bad,
         "original 39E1C rejected extension corrupted heap state");
    code2::function_37c37(s.heap,second);
    code2::function_37c37(s.heap,first);
    const u32 full=word(0x30000u+0x2Cu);
    need(s.arena_free_calls>=3u &&
         word(0x30000u+0x18u)==0u &&
         word(0x30000u+0x1Cu)==1u &&
         word(0x30000u+0x24u)==0x3002Cu &&
         word(0x30000u+0x0Cu)==0x3002Cu &&
         full==word(0x30000u)-0x2Cu && !s.bad,
         "source 398E0 failed to coalesce grown 39E1C guest arena");
    const unsigned previous_growth=s.grow_calls;
    const u32 reused=code2::function_37b42(s.heap,7000u);
    need(reused==first && s.grow_calls==previous_growth &&
         s.break_end==frozen && !s.bad,
         "source 39838 did not reuse fully coalesced 39E1C arena");
    code2::function_37c37(s.heap,reused);
    need(word(0x30000u+0x18u)==0u && !s.bad,
         "source break-growth reused block not released");
    active=nullptr;
}
} // namespace

int main(){
    try {
        test_dos_fallback();
        test_source_39e1c_break_growth();
        std::cout<<"CODE2_SOURCE_39C7B_DOS48_FALLBACK=PASS\n"
                    "CODE2_SOURCE_39A2C_DOS49_RECLAIM=PASS\n"
                    "CODE2_SOURCE_39E1C_BREAK_GROWTH=PASS\n"
                    "HOST_DOS_SELECTOR_ALIAS=EXPLICIT_TEST_POLICY\n"
                    "R04_ACCEPTED=NO\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_DOS_FALLBACK_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
