// Bounded execution of the *canonical* CODE.2 DOS startup, not an emulated game.
// The only synthetic pieces are explicitly identified DOS/PSP/environment fixtures.
// Every unsupported service stops execution before the original code can consume a
// fabricated result. Source: original CODE.2 353A0 -> 35418 -> 395E2.
#include "../../Code2/Code2.cpp"
#include "original_le_image.h"
#include <algorithm>
#include <array>
#include <csetjmp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
struct Session {
    std::jmp_buf escape;
    std::vector<u8> data, code;
    std::array<u8,256> psp{};
    std::array<u8,512> env{};
    code2::Machine machine{};
    code2::Runtime111Boundary entry{};
    code2::Runtime112Boundary startup{};
    code2::Runtime119Boundary init{};
    const char* stop_reason="NO_BOUNDARY_REACHED";
    u32 first_guest=0, guest_arg=0;
    unsigned version_calls=0, ff00_calls=0, dpmi_calls=0;
};
thread_local Session* current=nullptr;
[[noreturn]] void stop(const char* reason) noexcept {
    current->stop_reason=reason;
    std::longjmp(current->escape,1);
}
u8* segment(u16 selector,u32 off,unsigned size) noexcept {
    Session& s=*current;
    u8* ptr=nullptr;std::size_t available=0;
    if(selector==0x30u){ptr=s.data.data();available=s.data.size();}
    else if(selector==0x17u){ptr=s.code.data();available=s.code.size();}
    else if(selector==0x24u){ptr=s.psp.data();available=s.psp.size();}
    else if(selector==0x2Cu){ptr=s.env.data();available=s.env.size();}
    else stop("UNMAPPED_SELECTOR");
    if(off>available || size>available-off)stop("SEGMENT_BOUNDARY_VIOLATION");
    return ptr+off;
}
u8 seg8(u16 sel,u32 off) noexcept {return *segment(sel,off,1);}
u16 seg16(u16 sel,u32 off) noexcept {return code2::load_le16(segment(sel,off,2));}
u32 seg32(u16 sel,u32 off) noexcept {return code2::load_le32(segment(sel,off,4));}
void put8(u16 sel,u32 off,u8 value) noexcept {*segment(sel,off,1)=value;}
void put16(u16 sel,u32 off,u16 value) noexcept {
    code2::store_le16(segment(sel,off,2),value);
}
// Source-led callsite gate: AH=30h and AX=FF00h only. Values below are
// deterministic DOS 6.22/no-DOSX fixtures; NOT claims of real DOS trace parity.
void dos21(code2::Machine& m) noexcept {
    const u16 ax=static_cast<u16>(m.cpu.eax);
    if(ax==0x3000u) {
        ++current->version_calls;
        m.cpu.eax=0x00001606u;  // AL major 6; AH minor 22
    }else if(ax==0xFF00u) {
        ++current->ff00_calls;
        m.cpu.eax=0u;          // Controlled plain-DOS startup branch
    }else stop("UNSUPPORTED_DOS_INT21");
}
void dpmi31(code2::Machine&) noexcept {
    ++current->dpmi_calls;stop("UNSUPPORTED_DPMI_INT31");
}
void guest(u32 address,u32 arg) noexcept {
    current->first_guest=address; current->guest_arg=arg;
    stop("FIRST_ORIGINAL_INITIALIZER_UNRESOLVED");
}
void initializers(code2::Machine& m) noexcept {
    // This executes the real reconstructed CODE.2 scheduler. Source directly
    // references the original 0x12720 table and invokes original guest addresses.
    code2::function_395e2(m,current->init,m.cpu.edx);
    stop("INITIALIZER_TABLE_EXHAUSTED");
}
void after_initializers(code2::Machine&) noexcept {
    stop("POST_INITIALIZER_RUNTIME_UNRESOLVED");
}
void invoke_startup(code2::Machine& m) noexcept {
    code2::function_35418(m,current->startup);
}
void fail(const char* explanation) {
    throw std::runtime_error(explanation);
}
void require(bool ok,const char* explanation) {
    if(!ok)fail(explanation);
}
rloft_recomp::OriginalLeImage synthetic_image() {
    rloft_recomp::OriginalLeImage img;
    img.entry_object=1;img.entry_offset=0x353A0u;
    img.stack_object=2;img.stack_offset=0x2DD10u;
    img.objects.resize(2);
    img.objects[0].base=0x10000u;
    img.objects[0].virtual_size=0x3A852u;
    img.objects[0].raw.assign(0x3A852u,0);
    img.objects[1].base=0x50000u;
    img.objects[1].virtual_size=0x2DD10u;
    img.objects[1].raw.assign(0x2DD10u,0);
    constexpr std::array<u32,4> init_addresses{0x3968Eu,0x3A1B3u,0x3A2ADu,0x3A16Eu};
    for(std::size_t i=0;i<init_addresses.size();++i){
        const std::size_t off=0x12720u+i*6u;
        img.objects[1].raw[off]=0;
        img.objects[1].raw[off+1]=0x20u;
        code2::store_le32(img.objects[1].raw.data()+off+2,init_addresses[i]);
    }
    return img;
}
std::vector<u8> read_file(const char* name) {
    std::ifstream file(name,std::ios::binary);
    require(bool(file),"cannot open original CODE.2");
    return {std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
}
void run(const rloft_recomp::OriginalLeImage& img,bool original) {
    require(img.objects.size()==2 && img.entry_object==1 &&
            img.entry_offset==0x353A0u && img.stack_object==2 &&
            img.stack_offset==0x2DD10u &&
            img.objects[0].raw.size()==0x3A852u &&
            img.objects[1].raw.size()==0x2DD10u,"CODE.2 LE geometry mismatch");
    Session state;
    state.data=img.objects[1].raw;
    state.code=img.objects[0].raw;
    state.psp[0x2c]=0x2c;state.psp[0x2d]=0;
    // PSP:80h command tail is four bytes ("  /?") and a DOS carriage return.
    state.psp[0x80]=4;
    state.psp[0x81]=' ';state.psp[0x82]=' ';
    state.psp[0x83]='/';state.psp[0x84]='?';
    state.psp[0x85]=13;
    // DOS environment: one NO87 variable, double NUL, word 0001h, full path.
    constexpr char kEnv[]="no87=1\0\0\1\0C:\\RLOFT\\CODE.2";
    static_assert(sizeof(kEnv)<=512,"environment fixture too large");
    std::copy_n(reinterpret_cast<const u8*>(kEnv),sizeof(kEnv),state.env.begin());
    state.machine.data=state.data.data();
    state.machine.cpu.ds=0x30;state.machine.cpu.ss=0x30;
    state.machine.cpu.cs=0x17;state.machine.cpu.esp=0x2DD10;
    state.entry.tail_35418=&invoke_startup;
    state.startup.int21=&dos21;
    state.startup.int31=&dpmi31;
    state.startup.read_segment8=&seg8;
    state.startup.read_segment16=&seg16;
    state.startup.read_segment32=&seg32;
    state.startup.write_segment8=&put8;
    state.startup.write_segment16=&put16;
    state.startup.call_395e2=&initializers;
    state.startup.call_39589=&after_initializers;
    state.init.call_guest=&guest;
    current=&state;
    if(setjmp(state.escape)==0) {
        code2::function_353a0(state.machine,state.entry);
        stop("ORIGINAL_ENTRY_RETURNED_WITHOUT_RUNTIME");
    }
    current=nullptr;
    require(state.version_calls==1 && state.ff00_calls==1 &&
            state.dpmi_calls==0,"DOS service sequence differs from bounded source");
    require(state.machine.cpu.interrupt_enable,"STI was not executed");
    require(code2::load_le16(state.code.data()+0x35675u)==0x30u,
            "source startup did not save original DS through CODE selector");
    require(state.data[0x1266bu]==6u && state.data[0x1266cu]==22u,
            "original DOS version writes not observed");
    require(code2::load_le16(state.data.data()+0x12660u)==1u,
            "source environment NO87 scan mismatch");
    require(state.data[0x2BD10u]=='/' && state.data[0x2BD11u]=='?',
            "original PSP command-tail whitespace handling mismatch");
    require(code2::load_le32(state.data.data()+0x12638u)==0x2BD10u,
            "original argv/command staging pointer mismatch");
    require(state.first_guest==0x3968Eu,
            "original initializer table dispatch did not reach first callback");
    require(state.stop_reason==std::string("FIRST_ORIGINAL_INITIALIZER_UNRESOLVED"),
            "test continued beyond the first unsupported real initializer");
    std::cout<<"CODE2_ORIGINAL_SOURCE_ENTRY=353A0\n"
             <<"CODE2_ORIGINAL_SOURCE_STARTUP=35418..3562E\n"
             <<"DOS21_30H_VERSION_FIXTURE=PASS\n"
             <<"DOS21_FF00_FIXTURE=PASS\n"
             <<"DOS_PSP_COMMAND_AND_NO87_ENVIRONMENT=PASS\n"
             <<"CODE_SELECTOR_SAVED_DS=PASS\n"
             <<"ORIGINAL_INITIALIZER_DISPATCH=395E2\n"
             <<"FIRST_UNRESOLVED_GUEST=0x"<<std::hex<<state.first_guest<<std::dec<<'\n'
             <<"EXECUTION_BOUNDARY=INTENTIONALLY_STOPPED\n"
             <<"ORIGINAL_DOS_STARTUP_ACCEPTED=NO\n"
             <<"GAME_BOOTED=NO\n"
             <<"IMAGE_INPUT="<<(original?"ORIGINAL_FILE_LENGTH_AND_LAYOUT_CHECKED":"SYNTHETIC_FIXTURE")<<'\n';
}
}
int main(int argc,char**argv) {
    try {
        require(argc==1 || argc==2,"usage: rloft_code2_startup_gate [CODE.2]");
        if(argc==1)run(synthetic_image(),false);
        else {
            const auto b=read_file(argv[1]);
            require(b.size()==558689u,"original CODE.2 size is incorrect");
            run(rloft_recomp::OriginalLeImage::parse(b),true);
        }
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_STARTUP_GATE_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
