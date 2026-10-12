// Original CODE.1 CC1C DOS startup witness (not the separate native engine).
// Compile the exact canonical Code1.cpp and halt at genuine original DOS calls.
// There are no fake screen/menu/character-creation results here.
#include "../../Code1/Code1.cpp"
#include "original_le_image.h"
#include "extender_data_break.hpp"
#include "dpmi_linear_memory.hpp"
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
                  ExtenderHostResizeHeap };
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
void source_heap_post_extender_handoff() {
    auto& s=*current;
    // These four root DWORDs are exact authenticated original CODE.1
    // static LE data bytes. The asset-free variant supplies them as an
    // explicit fixture; original-media mode must already contain them.
    const auto initialize=[&](u32 offset,u32 value) {
        code1::store_le32(s.dgroup.data()+offset,value);
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
                    s.mode==Mode::ExtenderHostResizeHeap)?
            0x44581606u:0x00001606u;
        return;
    }
    if(s.int21_calls==2u) {
        if(s.mode==Mode::Dos6ToFf00 &&
           (m.cpu.eax&0xFFFFu)==0xFF00u &&
           s.dos6_branch)
            halt("STOP_BEFORE_CODE1_DOS_INT21_FF00");
        if((s.mode==Mode::ExtenderToResize ||
            s.mode==Mode::ExtenderHostResize ||
            s.mode==Mode::ExtenderHostResizeHeap) &&
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
               mode!=Mode::ExtenderHostResizeHeap)
                halt("CODE1_STARTUP_EXTENDER_BRANCH_CHANGED");
            (void)code1::translate_ccde_cd16(
                s.machine,(mode==Mode::ExtenderHostResize ||
                           mode==Mode::ExtenderHostResizeHeap)?0xA000u:0x5000u);
            if(mode==Mode::ExtenderHostResize ||
               mode==Mode::ExtenderHostResizeHeap)
                halt("STOP_AFTER_CODE1_ORIGINAL_AH4A_HOST_RESIZE");
        } else halt("CODE1_ORIGINAL_ENTRY_BRANCH_INVALID");
        halt("CODE1_SOURCE_RETURNED_BEFORE_EXPECTED_DOS_BOUNDARY");
    }
    if(mode==Mode::ExtenderHostResizeHeap &&
       s.boundary==std::string("STOP_AFTER_CODE1_ORIGINAL_AH4A_HOST_RESIZE"))
        source_heap_post_extender_handoff();
    current=nullptr;
    const char* expected=mode==Mode::BeforeVersion?
        "STOP_BEFORE_CODE1_DOS_INT21_AH30_VERSION":
        mode==Mode::Dos6ToFf00?
        "STOP_BEFORE_CODE1_DOS_INT21_FF00":
        (mode==Mode::ExtenderHostResize ||
         mode==Mode::ExtenderHostResizeHeap)?
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
       mode==Mode::ExtenderHostResizeHeap)
        assert_true(s.host_break.successful_resizes()==1u &&
                    s.dgroup.size()==0xA000u &&
                    code1::load_le32(s.dgroup.data()+0x52f0u)==0xA000u &&
                    s.machine.cpu.es==0x30u,
                    "original CODE.1 CCDE host-backed 4KiB resize mismatch");
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
