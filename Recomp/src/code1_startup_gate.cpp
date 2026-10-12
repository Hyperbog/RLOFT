// Original CODE.1 CC1C DOS startup witness (not the separate native engine).
// Compile the exact canonical Code1.cpp and halt at genuine original DOS calls.
// There are no fake screen/menu/character-creation results here.
#include "../../Code1/Code1.cpp"
#include "original_le_image.h"
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
enum class Mode { BeforeVersion, Dos6ToFf00, ExtenderToResize };
struct Session {
    std::jmp_buf escape{};
    std::vector<u8> dgroup;
    code1::Machine machine{};
    unsigned int21_calls=0;
    u32 original_stack=0x9E90u;
    Mode mode=Mode::BeforeVersion;
    const char* boundary="NONE";
    bool dos6_branch=false,extender_branch=false;
};
thread_local Session* current=nullptr;
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
        m.cpu.eax=(s.mode==Mode::ExtenderToResize)?
            0x44581606u:0x00001606u;
        return;
    }
    if(s.int21_calls==2u) {
        if(s.mode==Mode::Dos6ToFf00 &&
           (m.cpu.eax&0xFFFFu)==0xFF00u &&
           s.dos6_branch)
            halt("STOP_BEFORE_CODE1_DOS_INT21_FF00");
        if(s.mode==Mode::ExtenderToResize &&
           ((m.cpu.eax>>8)&0xFFu)==0x4Au &&
           s.extender_branch)
            halt("STOP_BEFORE_CODE1_DOS_INT21_AH4A_RESIZE");
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
            if(mode!=Mode::ExtenderToResize)
                halt("CODE1_STARTUP_EXTENDER_BRANCH_CHANGED");
            (void)code1::translate_ccde_cd16(s.machine,0x5000u);
        } else halt("CODE1_ORIGINAL_ENTRY_BRANCH_INVALID");
        halt("CODE1_SOURCE_RETURNED_BEFORE_EXPECTED_DOS_BOUNDARY");
    }
    current=nullptr;
    const char* expected=mode==Mode::BeforeVersion?
        "STOP_BEFORE_CODE1_DOS_INT21_AH30_VERSION":
        mode==Mode::Dos6ToFf00?
        "STOP_BEFORE_CODE1_DOS_INT21_FF00":
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
