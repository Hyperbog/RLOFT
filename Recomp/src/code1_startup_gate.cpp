// Original CODE.1 CC1C DOS startup witness (not the separate native engine).
// Compile the exact canonical Code1.cpp and halt at genuine original DOS calls.
// There are no fake screen/menu/character-creation results here.
#include "../../Code1/Code1.cpp"
#include "original_le_image.h"
#include "extender_data_break.hpp"
#include "dpmi_linear_memory.hpp"
#include <array>
#include <csetjmp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using u8=std::uint8_t;using u16=std::uint16_t;using u32=std::uint32_t;
enum class Mode { BeforeVersion, Dos6ToFf00, ExtenderToResize, ExtenderHostResize,
                  ExtenderHostResizeHeap, ExtenderCd97Startup, ExtenderCd97BreakHeap,
                  ExtenderCd97Constructors };
struct Session {
    std::jmp_buf escape{};
    std::vector<u8> dgroup;
    code1::Machine machine{};
    unsigned int21_calls=0;
    u32 original_stack=0x9E90u;
    Mode mode=Mode::BeforeVersion;
    rloft_recomp::ExtenderDataBreak host_break{0x30u,code1::kDataObjectVirtualSize};
    const char* boundary="NONE";
    bool dos6_branch=false,extender_branch=false;
    rloft_recomp::DpmiLinearMemory source_heap;
    unsigned source_0501=0,source_0502=0,source_db5b=0;
    bool bad_heap_access=false;
    std::array<u8,256> source_psp{},source_env{};
    std::array<u32,3> source_ctor_targets{};
    unsigned source_ctor_calls=0,source_runtime_calls=0,source_main_calls=0;
    unsigned source_exit_calls=0,source_break_calls=0;
    unsigned source_constructor_allocs=0,source_nodes=0;
    bool bad_cd97_access=false,source_cd97_saved_ds=false;
};
thread_local Session* current=nullptr;
u8* heap_dword(u32 guest) noexcept {
    auto& s=*current;
    if(guest&3u){s.bad_heap_access=true;return nullptr;}
    if(guest<s.dgroup.size()) {
        if(s.dgroup.size()-guest<4u){s.bad_heap_access=true;return nullptr;}
        return s.dgroup.data()+guest;
    }
    auto* ptr=s.source_heap.translate(guest,4u);
    if(!ptr)s.bad_heap_access=true;
    return ptr;
}
u32 heap_read32(u32 guest) noexcept {
    const auto* p=heap_dword(guest);
    return p?code1::load_le32(p):0u;
}
void heap_write32(u32 guest,u32 value) noexcept {
    auto* p=heap_dword(guest);
    if(p)code1::store_le32(p,value);
}
code1::DpmiBlockResult heap_dpmi_0501(u32 bytes) {
    ++current->source_0501;
    const auto r=current->source_heap.allocate(bytes);
    return {r.carry,r.linear,r.handle};
}
void heap_dpmi_0502(u32 handle) {
    ++current->source_0502;
    if(current->source_heap.release(handle).carry)
        current->bad_heap_access=true;
}
void heap_source_10590(u32 payload,u16 ds,u32 arena) {
    if(ds!=current->machine.cpu.ds || arena!=0x30B8u) {
        current->bad_heap_access=true;
        return;
    }
    code1::HeapCoreBoundaries h{};
    h.read32=&heap_read32;
    h.write32=&heap_write32;
    code1::function_10590(h,payload,arena);
}
void heap_source_clear8(u32 address,u8 value) {
    if(address!=0x8E78u || address>=current->dgroup.size()) {
        current->bad_heap_access=true;
        return;
    }
    current->dgroup[address]=value;
}
void heap_source_db5b(u32 payload) {
    ++current->source_db5b;
    code1::AllocBoundary a{};
    a.ds=current->machine.cpu.ds;
    a.call_10590=&heap_source_10590;
    a.write_linear8=&heap_source_clear8;
    code1::function_db5b(a,payload);
}
void source_heap_post_extender_handoff(bool original_loaded) {
    auto& s=*current;
    // These four root DWORDs are exact authenticated original CODE.1
    // static LE data bytes. The asset-free variant supplies them as an
    // explicit fixture; original-media mode must already contain them.
    const auto initialize=[&](u32 offset,u32 value) {
        if(original_loaded) {
            if(code1::load_le32(s.dgroup.data()+offset)!=value)
                throw std::runtime_error(
                    "authenticated CODE.1 original static heap roots changed");
        } else code1::store_le32(s.dgroup.data()+offset,value);
    };
    initialize(0x30C0u,0x30D4u);
    initialize(0x30C8u,0xFFFFFFFFu);
    initialize(0x30D8u,0x30D4u);
    initialize(0x30DCu,0x30D4u);
    // These two original CODE.1 Watcom allocator preconditions are host-
    // supplied here; the runtime has not yet reached natural game setup.
    initialize(0x55CCu,1u);
    s.dgroup[0x531Eu]=1u;
    code1::HeapCoreBoundaries hb{};
    hb.read32=&heap_read32;
    hb.write32=&heap_write32;
    hb.dpmi_free_0502=&heap_dpmi_0502;
    code1::HeapGrowthBoundaries gb{};
    gb.alloc_0501=&heap_dpmi_0501;
    gb.free_db5b=&heap_source_db5b;
    if(code1::function_10aa9(s.machine,hb,gb,0x300u)!=1u)
        throw std::runtime_error("original CODE.1 10AA9 failed after CCDE");
    const u32 arena=code1::load_le32(s.dgroup.data()+0x8E74u);
    if(arena!=rloft_recomp::DpmiLinearMemory::kFirstLinear ||
       s.source_heap.allocation_count()!=1u ||
       s.source_0501!=1u || s.source_db5b!=1u ||
       heap_read32(0x30D8u)!=arena+0x20u ||
       heap_read32(arena+0x20u)!=0x0FD8u || s.bad_heap_access)
        throw std::runtime_error("CCDE original DPMI heap metadata mismatch");
    const u32 payload=code1::function_104e0(hb,0x80u,0x30B8u);
    if(payload!=arena+0x24u || !(heap_read32(payload-4u)&1u))
        throw std::runtime_error("CODE.1 original 104E0 did not split heap");
    heap_source_db5b(payload);
    if(heap_read32(arena+0x20u)!=0x0FD8u ||
       heap_read32(0x30C0u)!=arena+0x20u || s.bad_heap_access)
        throw std::runtime_error("CODE.1 original DB5B/10590 coalescing changed");
    code1::function_107cb(s.machine,hb);
    if(s.source_0502!=1u || s.source_db5b!=2u ||
       s.source_heap.allocation_count()!=0u ||
       code1::load_le32(s.dgroup.data()+0x8E74u)!=0u ||
       heap_read32(0x30C0u)!=0x30D4u || s.bad_heap_access)
        throw std::runtime_error("CODE.1 original 107CB leaked heap after CCDE");
}
// The following source-backed callbacks continue original CODE.1 CD97
// through the actual FCE8 prioritized runtime dispatcher and FC98 launch
// trampoline. Guest constructors are explicitly STOP/CANDIDATE boundaries;
// they are not silently replaced with successful gameplay calls.
u8 cd97_read8(u16 selector,u32 offset) noexcept {
    auto& s=*current;
    const std::array<u8,256>* bytes=selector==0x24u?
        &s.source_psp:(selector==0x2Cu?&s.source_env:nullptr);
    if(!bytes||offset>=bytes->size()) {
        s.bad_cd97_access=true;
        return 0u;
    }
    return (*bytes)[offset];
}
u32 cd97_read32(u16 selector,u32 offset) noexcept {
    u32 result=0u;
    for(u32 i=0;i<4u;++i)
        result|=static_cast<u32>(cd97_read8(selector,offset+i))<<(i*8u);
    return result;
}
void cd97_write16(u16 selector,u32 offset,u16 value) noexcept {
    auto& s=*current;
    if(selector!=0x30u || offset!=0xCC90u || value!=0x30u)
        s.bad_cd97_access=true;
    else s.source_cd97_saved_ds=true;
}
// Original FCE8 source constructors can opt into actual source allocator
// dispatch. No native malloc allocation is substituted for Watcom 104E0.
u32 source_cd97_extend_break(u32 target) noexcept;
u32 cd97_guest_allocate(u32 requested) noexcept {
    auto& s=*current;
    code1::HeapCoreBoundaries h{};
    h.read32=&heap_read32;
    h.write32=&heap_write32;
    code1::HeapGrowthBoundaries g{};
    g.extend_10dd7=&source_cd97_extend_break;
    g.free_db5b=&heap_source_db5b;
    u32 p=code1::function_104e0(h,requested,0x30B8u);
    if(!p) {
        if(code1::function_10aa9(s.machine,h,g,requested)!=1u) {
            s.bad_cd97_access=true;
            return 0u;
        }
        p=code1::function_104e0(h,requested,0x30B8u);
    }
    if(!p || p>=s.dgroup.size() || s.bad_heap_access)
        s.bad_cd97_access=true;
    else ++s.source_constructor_allocs;
    return p;
}
u32 cd97_alloc_node8() noexcept {
    ++current->source_nodes;
    return cd97_guest_allocate(8u);
}
void cd97_guest_write32(u32 address,u32 value) noexcept {
    auto& s=*current;
    // CODE.1's original FD6E argv strings can place dwords at
    // unaligned offsets. Source heap *metadata* is still aligned.
    if(address>s.dgroup.size() || s.dgroup.size()-address<4u) {
        s.bad_cd97_access=true;
        return;
    }
    code1::store_le32(s.dgroup.data()+address,value);
}
u32 cd97_tokenize_empty(u32 src,u32,u32* end) noexcept {
    auto& s=*current;
    if(!end || src>=s.dgroup.size()) {
        s.bad_cd97_access=true;
        return 0u;
    }
    // Delegate lexical decisions to the reconstructed CODE.1 FE21
    // scanner. This opt-in fixture has an empty PSP argument tail.
    u8* limit=nullptr;
    const u32 argc=code1::function_fe21(s.dgroup.data()+src,nullptr,&limit);
    if(argc!=0u || !limit || limit!=s.dgroup.data()+src)
        s.bad_cd97_access=true;
    *end=src+static_cast<u32>(limit-(s.dgroup.data()+src));
    return argc;
}
void cd97_guest_copy(u32 destination,u32 source,u32 count) noexcept {
    auto& s=*current;
    if(destination>s.dgroup.size() || count>s.dgroup.size()-destination ||
       source>s.dgroup.size() || count>s.dgroup.size()-source) {
        s.bad_cd97_access=true;
        return;
    }
    std::memmove(s.dgroup.data()+destination,s.dgroup.data()+source,count);
}
void cd97_ctor_boundary(u32 addr) noexcept {
    auto& s=*current;
    if(s.source_ctor_calls>=s.source_ctor_targets.size()) {
        s.bad_cd97_access=true;
        return;
    }
    s.source_ctor_targets[s.source_ctor_calls++]=addr;
    if(s.mode!=Mode::ExtenderCd97Constructors)return;
    if(addr==0x10E81u) {
        code1::RuntimeIoBoundaries b{};
        b.alloc_node8=&cd97_alloc_node8;
        b.write32=&cd97_guest_write32;
        code1::function_10e81(s.machine,b);
    } else if(addr==0x10438u)
        code1::function_10438(s.machine);
    else if(addr==0xFD6Eu) {
        code1::ArgvBuildBoundaries b{};
        b.tokenize_fe21=&cd97_tokenize_empty;
        b.alloc_dac9=&cd97_guest_allocate;
        b.copy_d9ee=&cd97_guest_copy;
        b.write32=&cd97_guest_write32;
        code1::function_fd6e(s.machine,b);
    } else s.bad_cd97_access=true;
}
u32 cd97_stack_probe() noexcept {return 0u;}
void cd97_source_10d1c() noexcept {
    code1::function_10d1c(current->machine);
}
code1::s32 cd97_game_entry_boundary(u32,u32) noexcept {
    ++current->source_main_calls;
    return 0;
}
void cd97_exit_boundary(code1::s32) noexcept {
    ++current->source_exit_calls;
}
void cd97_runtime_call(u32 target) noexcept {
    auto& s=*current;
    ++s.source_runtime_calls;
    if(target==0xFCE8u) {
        code1::RuntimeTailBoundaries b{};
        b.call_guest_near=&cd97_ctor_boundary;
        code1::function_fce8(s.machine,b);
    } else if(target==0xFC98u) {
        code1::LaunchBoundaries b{};
        b.stack_probe_1042d=&cd97_stack_probe;
        b.init_10d1c=&cd97_source_10d1c;
        b.main_508c=&cd97_game_entry_boundary;
        b.exit_cbf0=&cd97_exit_boundary;
        code1::function_fc98(s.machine,b);
    } else s.bad_cd97_access=true;
}
void source_cd97_followup(bool original_loaded) {
    auto& s=*current;
    // These values are verified against the owner's original object-2
    // bytes: 55CC=1, 55D0=0x10 and three six-byte FCE8 ctor records.
    // The synthetic asset-free fixture encodes only this bounded evidence.
    const auto verify_or_initialize=[&](u32 offset,u32 value) {
        if(original_loaded) {
            if(code1::load_le32(s.dgroup.data()+offset)!=value)
                throw std::runtime_error("authentic CODE.1 runtime table differs");
        } else code1::store_le32(s.dgroup.data()+offset,value);
    };
    verify_or_initialize(0x55CCu,1u);
    verify_or_initialize(0x55D0u,0x10u);
    for(unsigned i=0;i<3u;++i) {
        const u32 addr=0x55E0u+i*6u;
        if(original_loaded) {
            if(s.dgroup[addr]!=0u || s.dgroup[addr+1u]!=0x20u)
                throw std::runtime_error("authentic CODE.1 ctor priority differs");
        } else {
            s.dgroup[addr]=0u;
            s.dgroup[addr+1u]=0x20u;
        }
        verify_or_initialize(addr+2u,
            i==0u?0xFD6Eu:i==1u?0x10438u:0x10E81u);
    }
    if(s.mode==Mode::ExtenderCd97Constructors) {
        // Five nonzero original static DOS stdio stream flag records
        // 539C..5404, and the terminal zero stream at 541E.
        constexpr u32 flags[6]={1u,2u,2u,3u,2u,0u};
        for(unsigned i=0;i<6u;++i)
            verify_or_initialize(0x539Cu+i*0x1Au+0x0Cu,flags[i]);
    }
    s.source_psp[0x80u]=0u; // owner-controlled empty PSP command tail
    s.source_env[0u]=0u; s.source_env[1u]=0u;
    s.source_env[4u]=0u; // empty environment and executable-path tail
    code1::StartupRegs12 regs{};
    regs.eax=s.machine.cpu.eax;
    regs.ebx=s.machine.cpu.ebx;
    regs.ecx=s.machine.cpu.ecx;
    regs.esi=s.machine.cpu.esi;
    regs.edi=s.machine.cpu.edi;
    regs.esp=s.machine.cpu.esp;
    regs.ds=s.machine.cpu.ds;
    regs.es=s.machine.cpu.es;
    if(static_cast<u8>(regs.eax)!=2u)
        throw std::runtime_error("CODE.1 CCDE did not derive Phar Lap mode 2 from ASCII BL");
    code1::Startup12Boundary b{};
    b.read8=&cd97_read8;
    b.read32=&cd97_read32;
    b.write16=&cd97_write16;
    b.runtime_call=&cd97_runtime_call;
    code1::function_cd97(s.machine,b,regs);
    if(s.mode==Mode::ExtenderCd97Constructors &&
       (s.source_nodes!=5u || s.source_constructor_allocs!=6u ||
        s.source_break_calls!=1u ||
        code1::load_le16(s.dgroup.data()+0x55B0u)!=s.machine.cpu.ss ||
        code1::load_le32(s.dgroup.data()+0x8E84u)==0u ||
        code1::load_le32(s.dgroup.data()+0x30A0u)==0u ||
        code1::load_le32(s.dgroup.data()+0x309Cu)!=1u ||
        code1::load_le32(s.dgroup.data()+0x8E80u)!=1u ||
        s.source_heap.allocation_count()!=0u || s.bad_heap_access))
        throw std::runtime_error(
            "CODE.1 original FCE8 constructors did not allocate guest stdio/argv state");
    if(s.dgroup[0x531Eu]!=2u ||
       code1::load_le32(s.dgroup.data()+0x55CCu)!=1u ||
       code1::load_le32(s.dgroup.data()+0x55D0u)!=0x8000u ||
       code1::load_le16(s.dgroup.data()+0x5324u)!=0x2Cu ||
       s.source_runtime_calls!=2u || s.source_ctor_calls!=3u ||
       s.source_ctor_targets[0]!=0x10E81u ||
       s.source_ctor_targets[1]!=0x10438u ||
       s.source_ctor_targets[2]!=0xFD6Eu ||
       s.source_main_calls!=1u || s.source_exit_calls!=1u ||
       !s.source_cd97_saved_ds || s.bad_cd97_access)
        throw std::runtime_error("CODE.1 CD97/FCE8/FC98 startup state differs");
}
bool source_cd97_resize_break(u32 pages) noexcept {
    auto& s=*current;
    if(pages==0u || pages>0xFFFFu) {
        s.bad_heap_access=true;
        return false;
    }
    const auto r=s.host_break.resize(0x30u,static_cast<u16>(pages),s.dgroup);
    if(r.carry)return false;
    s.machine.data=s.dgroup.data();
    ++s.source_break_calls;
    return true;
}
u32 source_cd97_extend_break(u32 target) noexcept {
    code1::FileBoundaries files{};
    code1::RuntimeIoBoundaries io{};
    io.resize_break_other=&source_cd97_resize_break;
    return code1::function_10dd7(current->machine,files,io,target);
}
void source_break_heap_followup(bool original_loaded) {
    auto& s=*current;
    // CODE.1 CD97 itself publishes the true mode=2. The FCE8 ctor
    // dispatcher and FC98 source initializer produce min_heap=0x8000.
    if(s.dgroup[0x531Eu]!=2u ||
       code1::load_le32(s.dgroup.data()+0x55D0u)!=0x8000u ||
       code1::load_le32(s.dgroup.data()+0x52F0u)!=0xA000u)
        throw std::runtime_error("CODE.1 source break heap startup preconditions differ");
    const auto root=[&](u32 off,u32 original_value) {
        if(original_loaded) {
            if(code1::load_le32(s.dgroup.data()+off)!=original_value)
                throw std::runtime_error("authenticated CODE.1 free-list roots differ");
        } else code1::store_le32(s.dgroup.data()+off,original_value);
    };
    root(0x30C0u,0x30D4u);
    root(0x30C8u,0xFFFFFFFFu);
    root(0x30D8u,0x30D4u);
    root(0x30DCu,0x30D4u);
    code1::HeapCoreBoundaries hb{};
    hb.read32=&heap_read32;
    hb.write32=&heap_write32;
    code1::HeapGrowthBoundaries gb{};
    gb.extend_10dd7=&source_cd97_extend_break;
    gb.free_db5b=&heap_source_db5b;
    if(code1::function_10aa9(s.machine,hb,gb,0x300u)!=1u ||
       s.source_break_calls!=1u || s.dgroup.size()!=0x12000u ||
       code1::load_le32(s.dgroup.data()+0x52F0u)!=0x12000u ||
       s.source_0501!=0u || s.source_heap.allocation_count()!=0u ||
       s.source_db5b!=1u || s.bad_heap_access)
        throw std::runtime_error("CODE.1 natural mode2 10AA9 failed to extend DS break");
    const u32 payload=code1::function_104e0(hb,0x80u,0x30B8u);
    if(!payload || payload<0xA000u || payload>=0x12000u ||
       !(heap_read32(payload-4u)&1u))
        throw std::runtime_error("CODE.1 source 104E0 mode2 allocation failed");
    heap_source_db5b(payload);
    if(s.source_db5b!=2u || s.source_0502!=0u ||
       code1::load_le32(s.dgroup.data()+0x8E74u)!=0u ||
       s.bad_heap_access)
        throw std::runtime_error("CODE.1 source 10590 mode2 break free failed");
}
[[noreturn]] void halt(const char* reason) noexcept {
    current->boundary=reason;
    std::longjmp(current->escape,1);
}
void assert_true(bool yes,const char* reason) {
    if(!yes)throw std::runtime_error(reason);
}
void original_dos_int21(code1::Machine& m,u8 number) noexcept {
    Session& s=*current;
    ++s.int21_calls;
    if(number!=0x21u)halt("CODE1_UNEXPECTED_INTERRUPT");
    if(s.int21_calls==1u) {
        if((m.cpu.eax&0xFFFFu)!=0x3000u ||
           m.cpu.ebx!=0x50484152u ||
           code1::load_le32(s.dgroup.data()+0x5304u)!=s.original_stack ||
           code1::load_le16(s.dgroup.data()+0x52FCu)!=0x24u)
            halt("CODE1_ORIGINAL_AH30_ABI_CHANGED");
        if(s.mode==Mode::BeforeVersion)
            halt("STOP_BEFORE_CODE1_DOS_INT21_AH30_VERSION");
        // Controlled DOS responses for *source-branch observation only*.
        // 4458h is the original 32-bit extender marker; no extender ran.
        m.cpu.eax=(s.mode==Mode::ExtenderToResize ||
                    s.mode==Mode::ExtenderHostResize ||
                    s.mode==Mode::ExtenderHostResizeHeap ||
                    s.mode==Mode::ExtenderCd97Startup ||
                    s.mode==Mode::ExtenderCd97BreakHeap ||
                    s.mode==Mode::ExtenderCd97Constructors)?
            0x44581606u:0x00001606u;
        if(s.mode==Mode::ExtenderToResize ||
           s.mode==Mode::ExtenderHostResize ||
           s.mode==Mode::ExtenderHostResizeHeap ||
           s.mode==Mode::ExtenderCd97Startup ||
           s.mode==Mode::ExtenderCd97BreakHeap ||
                    s.mode==Mode::ExtenderCd97Constructors)
            m.cpu.ebx=(m.cpu.ebx&0xFFFFFF00u)|0x32u; // ASCII '2' (not PSP probe 'R')
        return;
    }
    if(s.int21_calls==2u) {
        if(s.mode==Mode::Dos6ToFf00 &&
           (m.cpu.eax&0xFFFFu)==0xFF00u &&
           s.dos6_branch)
            halt("STOP_BEFORE_CODE1_DOS_INT21_FF00");
        if((s.mode==Mode::ExtenderToResize ||
            s.mode==Mode::ExtenderHostResize ||
            s.mode==Mode::ExtenderHostResizeHeap ||
            s.mode==Mode::ExtenderCd97Startup ||
            s.mode==Mode::ExtenderCd97BreakHeap ||
                    s.mode==Mode::ExtenderCd97Constructors) &&
           ((m.cpu.eax>>8)&0xFFu)==0x4Au &&
           s.extender_branch) {
            if(s.mode==Mode::ExtenderToResize)
                halt("STOP_BEFORE_CODE1_DOS_INT21_AH4A_RESIZE");
            // Source CCDE rounds the high-water mark to 4 KiB and passes
            // BX in 4 KiB units, with ES=DS, not the DOS PSP segment.
            if(m.cpu.es!=0x30u || (m.cpu.ebx&0xFFFFu)!=10u)
                halt("ORIGINAL_CODE1_EXTENDER_AH4A_ABI_CHANGED");
            const auto resized=s.host_break.resize(
                m.cpu.es,static_cast<u16>(m.cpu.ebx),s.dgroup);
            if(resized.carry)
                halt("HOST_CODE1_EXTENDER_AH4A_RESIZE_FAILED");
            m.data=s.dgroup.data();
            return;
        }
        halt("CODE1_UNEXPECTED_SECOND_DOS_SERVICE");
    }
    halt("CODE1_UNEXPECTED_THIRD_DOS_SERVICE");
}
std::vector<u8> read_file(const char* path) {
    std::ifstream f(path,std::ios::binary);
    assert_true(bool(f),"cannot open supplied original CODE.1");
    return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
}
void execute(Mode mode,const std::vector<u8>* original) {
    Session s;
    s.mode=mode;
    if(original) {
        assert_true(original->size()==code1::kSourceFileSize,
                    "CODE.1 original file size differs");
        const auto image=rloft_recomp::OriginalLeImage::parse(*original);
        assert_true(image.objects.size()==2 &&
                    image.entry_object==1 &&
                    image.entry_offset==code1::kEntryOffset &&
                    image.objects[0].virtual_size==code1::kCodeObjectVirtualSize &&
                    image.objects[1].virtual_size==code1::kDataObjectVirtualSize &&
                    image.objects[0].base==code1::kCodeObjectBase &&
                    image.objects[1].base==code1::kDataObjectBase &&
                    image.stack_object==2,
                    "original CODE.1 memory image or entry differs");
        s.dgroup=image.objects[1].raw;
    } else {
        s.dgroup.assign(code1::kDataObjectVirtualSize,0u);
    }
    s.machine.data=s.dgroup.data();
    s.machine.cpu.esp=s.original_stack;
    s.machine.cpu.ds=0x30u;
    s.machine.cpu.ss=0x30u;
    s.machine.cpu.es=0x24u;
    s.machine.protected_interrupt=&original_dos_int21;
    current=&s;
    if(setjmp(s.escape)==0) {
        const auto branch=code1::translate_cc1c_ccdd(s.machine);
        if(branch==code1::StartupPath::BranchCd17) {
            s.dos6_branch=true;
            if(mode!=Mode::Dos6ToFf00)
                halt("CODE1_STARTUP_BRANCH_CHANGED");
            const auto next=code1::translate_cd17_cd4f(s.machine);
            if(next!=code1::Cd17Path::BranchCd50)
                halt("CODE1_ORIGINAL_CD17_BRANCH_CHANGED");
            (void)code1::translate_cd50_cd96(s.machine);
        } else if(branch==code1::StartupPath::FallThroughCcde) {
            s.extender_branch=true;
            if(mode!=Mode::ExtenderToResize &&
               mode!=Mode::ExtenderHostResize &&
               mode!=Mode::ExtenderHostResizeHeap &&
               mode!=Mode::ExtenderCd97Startup &&
               mode!=Mode::ExtenderCd97BreakHeap &&
               mode!=Mode::ExtenderCd97Constructors)
                halt("CODE1_STARTUP_EXTENDER_BRANCH_CHANGED");
            (void)code1::translate_ccde_cd16(
                s.machine,(mode==Mode::ExtenderHostResize ||
                           mode==Mode::ExtenderHostResizeHeap ||
                           mode==Mode::ExtenderCd97Startup ||
                           mode==Mode::ExtenderCd97BreakHeap ||
                           mode==Mode::ExtenderCd97Constructors)?0xA000u:0x5000u);
            if(mode==Mode::ExtenderHostResize ||
               mode==Mode::ExtenderHostResizeHeap ||
               mode==Mode::ExtenderCd97Startup ||
               mode==Mode::ExtenderCd97BreakHeap ||
               mode==Mode::ExtenderCd97Constructors)
                halt("STOP_AFTER_CODE1_ORIGINAL_AH4A_HOST_RESIZE");
        } else halt("CODE1_ORIGINAL_ENTRY_BRANCH_INVALID");
        halt("CODE1_SOURCE_RETURNED_BEFORE_EXPECTED_DOS_BOUNDARY");
    }
    if(mode==Mode::ExtenderHostResizeHeap &&
       s.boundary==std::string("STOP_AFTER_CODE1_ORIGINAL_AH4A_HOST_RESIZE"))
        source_heap_post_extender_handoff(original!=nullptr);
    if((mode==Mode::ExtenderCd97Startup ||
        mode==Mode::ExtenderCd97BreakHeap ||
        mode==Mode::ExtenderCd97Constructors) &&
       s.boundary==std::string("STOP_AFTER_CODE1_ORIGINAL_AH4A_HOST_RESIZE")) {
        source_cd97_followup(original!=nullptr);
        if(mode==Mode::ExtenderCd97BreakHeap)
            source_break_heap_followup(original!=nullptr);
    }
    current=nullptr;
    const char* expected=mode==Mode::BeforeVersion?
        "STOP_BEFORE_CODE1_DOS_INT21_AH30_VERSION":
        mode==Mode::Dos6ToFf00?
        "STOP_BEFORE_CODE1_DOS_INT21_FF00":
        (mode==Mode::ExtenderHostResize ||
         mode==Mode::ExtenderHostResizeHeap ||
         mode==Mode::ExtenderCd97Startup ||
         mode==Mode::ExtenderCd97BreakHeap ||
         mode==Mode::ExtenderCd97Constructors)?
        "STOP_AFTER_CODE1_ORIGINAL_AH4A_HOST_RESIZE":
        "STOP_BEFORE_CODE1_DOS_INT21_AH4A_RESIZE";
    assert_true(s.boundary==std::string(expected),"CODE1 DOS entry boundary incorrect");
    assert_true(s.int21_calls==(mode==Mode::BeforeVersion?1u:2u),
                "CODE1 DOS interrupt count differs");
    assert_true(s.machine.cpu.interrupt_enable &&
                code1::load_le32(s.dgroup.data()+0x5304u)==s.original_stack &&
                code1::load_le16(s.dgroup.data()+0x52FCu)==0x24u,
                "original CC1C stack/alignment/PSP setup differs");
    if(mode!=Mode::BeforeVersion)
        assert_true(s.dgroup[0x5326u]==6u && s.dgroup[0x5327u]==22u,
                    "original CC1C DOS major/minor publication differs");
    if(mode==Mode::ExtenderHostResize ||
       mode==Mode::ExtenderHostResizeHeap ||
       mode==Mode::ExtenderCd97Startup ||
       mode==Mode::ExtenderCd97BreakHeap ||
       mode==Mode::ExtenderCd97Constructors)
        assert_true(s.host_break.successful_resizes()>=1u &&
                    s.dgroup.size()==
                        (mode==Mode::ExtenderCd97BreakHeap?0x12000u:
                         mode==Mode::ExtenderCd97Constructors?0xB000u:0xA000u) &&
                    code1::load_le32(s.dgroup.data()+0x52f0u)==
                        (mode==Mode::ExtenderCd97BreakHeap?0x12000u:
                         mode==Mode::ExtenderCd97Constructors?0xB000u:0xA000u) &&
                    s.machine.cpu.es==0x30u,
                    "original CODE.1 CCDE host-backed 4KiB resize mismatch");
    if(mode==Mode::ExtenderCd97Startup ||
       mode==Mode::ExtenderCd97BreakHeap ||
       mode==Mode::ExtenderCd97Constructors)
        std::cout<<"CODE1_ORIGINAL_CD97_EXTENDER_MODE=2\\n"
                    "CODE1_FCE8_ORIGINAL_CTOR_DISPATCH=3_SOURCE_RECORDS\\n"
                    "CODE1_FC98_HEAP_MINIMUM=SOURCE_10D1C\\n"
                    "CODE1_GAME_ENTRY_508C=HOST_STOP_BOUNDARY\\n";
    if(mode==Mode::ExtenderCd97Constructors)
        std::cout<<"CODE1_FCE8_10E81_10438_FD6E_SOURCE_CONSTRUCTORS=PASS\\n"
                    "CODE1_STARTUP_STDIO_RECORDS=5_ORIGINAL_SOURCE_ALLOCATED\\n"
                    "CODE1_STARTUP_ARGV_ALLOCATION=SOURCE_104E0\\n"
                    "CODE1_STARTUP_GUEST_BREAK=SOURCE_10DD7\\n";
    if(mode==Mode::ExtenderCd97BreakHeap)
        std::cout<<"CODE1_CD97_MODE2_10DD7_DS_BREAK_ALLOC_FREE=PASS\\n"
                    "CODE1_SOURCE_HEAP_PRESEEDED_ARENA=NO\\n";
    if(mode==Mode::ExtenderHostResizeHeap)
        std::cout<<"CODE1_CCDE_10AA9_DB5B_107CB_SHARED_SESSION=PASS\\n"
                    "CODE1_SOURCE_HEAP_PRESEEDED_ARENA=NO\\n"
                    "CODE1_POST_STARTUP_HEAP_ENABLE=HOST_TRIGGERED\\n";
    std::cout<<"ORIGINAL_CODE1_SOURCE_ENTRY=CC1C\n"
             <<"SOURCE_BRANCH="<<(mode==Mode::BeforeVersion?"FIRST_DOS_INTERRUPT":
                      mode==Mode::Dos6ToFf00?"CD17_TO_CD50":"CCDE")<<"\n"
             <<"ORIGINAL_DOS_CALL_COUNT="<<s.int21_calls<<"\n"
             <<"EXECUTION_BOUNDARY="<<s.boundary<<"\n"
             <<"ORIGINAL_MEDIA="<<(original?"LE_IMAGE_VERIFIED":"SYNTHETIC_DGROUP")<<"\n"
             <<"ORIGINAL_DOS_PARITY=NO\n"
             <<"CHARACTER_CREATION_BOOTED=NO\n";
}
} // namespace
int main(int argc,char** argv) {
    try {
        Mode mode=Mode::BeforeVersion;
        int index=1;
        if(argc>1) {
            const std::string cmd(argv[1]);
            if(cmd=="--dos6-to-ff00") {mode=Mode::Dos6ToFf00;index=2;}
            else if(cmd=="--extender-to-resize") {mode=Mode::ExtenderToResize;index=2;}
            else if(cmd=="--extender-host-resize") {mode=Mode::ExtenderHostResize;index=2;}
            else if(cmd=="--extender-host-resize-to-source-heap") {mode=Mode::ExtenderHostResizeHeap;index=2;}
            else if(cmd=="--extender-host-resize-to-cd97") {mode=Mode::ExtenderCd97Startup;index=2;}
            else if(cmd=="--extender-host-resize-to-cd97-break-heap") {mode=Mode::ExtenderCd97BreakHeap;index=2;}
            else if(cmd=="--extender-host-resize-to-cd97-constructors") {mode=Mode::ExtenderCd97Constructors;index=2;}
            else if(cmd=="--stop-before-version") {index=2;}
        }
        assert_true(argc==index || argc==index+1,
                    "usage: rloft_code1_startup_gate [--stop-before-version|--dos6-to-ff00|--extender-to-resize] [CODE.1]");
        std::vector<u8> source;
        if(argc==index+1)source=read_file(argv[index]);
        execute(mode,argc==index+1?&source:nullptr);
        return 0;
    } catch(const std::exception& ex) {
        std::cerr<<"CODE1_STARTUP_GATE_FAIL: "<<ex.what()<<'\n';
        return 1;
    }
}
