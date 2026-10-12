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
#include <string>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
enum class GameConfig { Unset, Overrides, SignedWrap };
struct Session {
    std::jmp_buf escape;
    std::vector<u8> data, code;
    std::array<u8,256> psp{};
    std::array<u8,512> env{};
    code2::Machine machine{};
    code2::Runtime111Boundary entry{};
    code2::Application46Boundary game{};
    code2::Application69Boundary game_audio{};
    code2::Runtime112Boundary startup{};
    code2::Runtime119Boundary init{};
    code2::Runtime119Boundary post{};
    code2::Runtime129Boundary heap{};
    code2::RuntimeTail36Boundary environment{};
    code2::Runtime3a1b3Boundary streams{};
    code2::Runtime139Boundary argv{};
    // A separate execution gate opts into all original initializer callbacks.
    // This is a controlled guest-memory arena, NOT the DOS/WATCOM heap driver.
    bool extended=false;
    bool execute_application=false;
    GameConfig game_config=GameConfig::Unset;
    bool arena_seeded=false;
    unsigned getenv_calls=0, atoi_calls=0, audio_init_calls=0, far_install_calls=0;
    u32 active_sound_base=0, active_sound_count=0, far_target=0;

    u32 arena_start=0x2C000u,arena_end=0x2D500u;
    u32 arena_header=0x2B800u;
    unsigned allocation_calls=0, aligned_allocations=0, env_copies=0;
    unsigned arena_frees=0, injected_alloc_attempts=0;
    unsigned stream_nodes=0, argv_initializations=0;
    u32 game_argc=0, game_argv=0;
    const char* stop_reason="NO_BOUNDARY_REACHED";
    u32 first_guest=0, guest_arg=0;
    unsigned version_calls=0, ff00_calls=0, dpmi_calls=0, completed_initializers=0;
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
// The *arena storage and first free block* are a deterministic test fixture.
// All allocation choice/split/metadata linkage below is performed by original
// reconstructed CODE.2 function_39838, reached through function_37b42.
// The DOS/4GW heap provider and original arena-growth callbacks are still absent.
void initialize_guest_arena(Session& s) noexcept {
    if(s.arena_seeded)return;
    const u32 a=s.arena_header, first=s.arena_start, sentinel=a+0x20u;
    if((first&3u)!=0u || (a&3u)!=0u || first<a+0x2Cu ||
       s.arena_end<=first || s.arena_end>s.data.size())
        stop("GUEST_SOURCE_ARENA_GEOMETRY_INVALID");
    // The real allocator's chain walk obtains the first arena from [12624].
    code2::store_le32(segment(0x30u,0x12624u,4),a);
    code2::store_le32(segment(0x30u,0x12628u,4),a);
    code2::store_le32(segment(0x30u,a+0x0Cu,4),first);
    code2::store_le32(segment(0x30u,a+0x10u,4),0u);
    code2::store_le32(segment(0x30u,a+0x14u,4),s.arena_end-first);
    code2::store_le32(segment(0x30u,a+0x18u,4),0u);
    code2::store_le32(segment(0x30u,a+0x1Cu,4),1u);
    code2::store_le32(segment(0x30u,sentinel+0u,4),0u);
    code2::store_le32(segment(0x30u,sentinel+4u,4),first);
    code2::store_le32(segment(0x30u,sentinel+8u,4),first);
    code2::store_le32(segment(0x30u,first+0u,4),s.arena_end-first);
    code2::store_le32(segment(0x30u,first+4u,4),sentinel);
    code2::store_le32(segment(0x30u,first+8u,4),sentinel);
    s.arena_seeded=true;
}
u32 fixture_arena_alloc(u32 root,u32 request) noexcept {
    Session& s=*current;
    if(root!=0x12624u || request==0u || request>0xFFFFFFD4u)
        stop("GUEST_ARENA_ABI_MISMATCH");
    initialize_guest_arena(s);
    const u32 arena=code2::load_le32(segment(0x30u,root,4));
    if(arena!=s.arena_header)stop("GUEST_ARENA_ROOT_POINTER_CHANGED");
    const u32 p=code2::function_39838(s.machine,arena,request);
    if(p==0u)return 0u; // real source allocator's out-of-space result
    if(p<s.arena_start+4u || p>=s.arena_end || (p&3u)!=0u)
        stop("ORIGINAL_ARENA_ALLOCATION_OUT_OF_BOUNDS");
    const u32 encoded=code2::load_le32(segment(0x30u,p-4u,4));
    if((encoded&1u)==0u || (encoded&~1u)<request+4u ||
       (encoded&~1u)>s.arena_end-(p-4u))
        stop("ORIGINAL_ARENA_USED_BLOCK_METADATA_INVALID");
    (void)segment(0x30u,p,request);
    ++s.aligned_allocations;
    return p;
}
u32 fixture_alloc(u32 bytes) noexcept {
    ++current->allocation_calls;
    const u32 p=code2::function_37b42(current->heap,bytes);
    if(p==0u)stop("GUEST_ALLOCATOR_RETURNED_NULL");
    return p;
}
void fixture_fill(u32 off,u8 value,u32 count) noexcept {
    u8* p=segment(0x30u,off,count);
    std::fill_n(p,count,value);
}
void fixture_arena_free(u32 root,u32 payload) noexcept {
    Session& s=*current;
    if(!s.arena_seeded || root!=0x12624u ||
       payload<s.arena_start+4u || payload>=s.arena_end)
        stop("SOURCE_ARENA_FREE_OUT_OF_BOUNDS");
    const u32 arena=code2::load_le32(segment(0x30u,root,4));
    if(arena!=s.arena_header)stop("SOURCE_ARENA_FREE_ROOT_CHANGED");
    code2::function_398e0(s.machine,arena,payload);
    ++s.arena_frees;
}
void fixture_free(u32 payload) noexcept {
    code2::function_37c37(current->heap,payload);
}
u32 injected_fail_second_alloc(u32 bytes) noexcept {
    if(++current->injected_alloc_attempts==2u)return 0u;
    return fixture_alloc(bytes);
}
u32 rejected_fallback(u32) noexcept {stop("UNIMPLEMENTED_SECONDARY_HEAP_PATH");}
void rejected_fatal(u32,u32) noexcept {stop("GUEST_FATAL_3A536");}
void guest(u32 address,u32 arg) noexcept {
    Session& s=*current;
    constexpr std::array<u32,4> order{0x3A16Eu,0x3A2ADu,0x3A1B3u,0x3968Eu};
    if(s.completed_initializers>=order.size() ||
       address!=order[s.completed_initializers])
        stop("ORIGINAL_PRIORITY_CALLBACK_ORDER_CHANGED");
    if(arg!=0xFFu)stop("ORIGINAL_PRIORITY_CALLBACK_ARGUMENT_CHANGED");
    if(address==0x3A16Eu) {
        code2::function_3a16e(s.machine,s.machine.cpu.ss);
        ++s.completed_initializers;
        return;
    }
    if(!s.extended) {
        s.first_guest=address;s.guest_arg=arg;
        stop("NEXT_ORIGINAL_INITIALIZER_REQUIRES_ALLOCATOR");
    }
    if(address==0x3A2ADu) {
        const u16 selector=code2::load_le16(s.data.data()+0x12669u);
        const u32 source=code2::load_le32(s.data.data()+0x12665u);
        if(selector!=0x2Cu || source>=s.env.size())
            stop("ORIGINAL_ENVIRONMENT_SEGMENT_NOT_AVAILABLE");
        // The canonical source function_3a2ad currently receives a DGROUP
        // offset instead of ES:FS-qualified source. Copy *only* its bounded
        // source string list into a scratch DGROUP region for this fixture.
        // The different address-space model is explicit, not DOS parity.
        constexpr u32 dst=0x2D700u;
        u32 copied=0;
        for(;;) {
            if(copied>=0x200u)stop("ENVIRONMENT_STRING_LIST_UNTERMINATED");
            const u8 ch=seg8(selector,source+copied);
            put8(0x30u,dst+copied,ch);
            ++copied;
            if(ch==0u) {
                if(source+copied>=s.env.size())stop("ENVIRONMENT_TRAILING_NUL_MISSING");
                if(seg8(selector,source+copied)==0u) {
                    put8(0x30u,dst+copied,0u);
                    break;
                }
            }
        }
        s.env_copies=copied;
        code2::function_3a2ad(s.machine,s.environment,dst);
    } else if(address==0x3A1B3u) {
        code2::function_3a1b3(s.machine,s.streams);
        u32 p=code2::load_le32(s.data.data()+0x2BCF0u);
        while(p!=0u) {
            if(++s.stream_nodes>8u)stop("ORIGINAL_STREAM_CHAIN_UNEXPECTED");
            p=code2::load_le32(segment(0x30u,p,8));
        }
    } else if(address==0x3968Eu) {
        code2::function_3968e(s.machine,s.argv);
        ++s.argv_initializations;
    }
    ++s.completed_initializers;
}
void initializers(code2::Machine& m) noexcept {
    // This executes the real reconstructed CODE.2 scheduler. Source directly
    // references the original 0x12720 table and invokes original guest addresses.
    code2::function_395e2(m,current->init,m.cpu.eax);
    // In extended mode the original scheduler's successful RET is followed
    // by source function_35418's post-initializer 39589 call. The default
    // first-boundary gate never reaches this return path.
    if(!current->extended)stop("INITIALIZER_TABLE_EXHAUSTED");
}
u32 post_stack_capacity() noexcept {
    return code2::function_3a165(current->machine,current->machine.cpu.esp);
}
u32 post_stack_alloc(u32 bytes) noexcept {
    if(bytes==0u)return current->machine.cpu.esp;
    if(bytes>post_stack_capacity())stop("GUEST_STACK_ALLOCATION_UNBOUNDED");
    current->machine.cpu.esp-=bytes;
    return current->machine.cpu.esp;
}
void post_protection() noexcept {
    code2::function_3a38a(current->machine);
}
// These are *compiled canonical CODE.2* functions. The test-only DOS
// environment is produced by original 3A2AD before 20608 can read it.
u32 original_getenv(u32 query) noexcept {
    ++current->getenv_calls;
    if(query!=0x114Cu && query!=0x1154u && query!=0x115Cu && query!=0x1164u)
        stop("ORIGINAL_GAME_UNEXPECTED_ENVIRONMENT_QUERY");
    return code2::function_352ee(current->machine,current->entry,query);
}
code2::s32 original_atoi(u32 text) noexcept {
    ++current->atoi_calls;
    return static_cast<code2::s32>(code2::function_35349(current->machine,text));
}
// 2F2EC's first guest-visible side effects execute for real: source AUDIO
// global initialization. Its first external dependency is original 375FE's
// far callback/interrupt installation; no fake BIOS or IRQ is installed.
void stop_before_original_far_install(u32 target,u16 selector) noexcept {
    ++current->far_install_calls;
    current->far_target=target;
    if(target!=0x2F2E0u || selector!=current->machine.cpu.cs)
        stop("ORIGINAL_375FE_FAR_INSTALL_CONTRACT_CHANGED");
    stop("STOP_BEFORE_DOS_FAR_CALLBACK_INSTALL_375FE");
}
void original_audio_init(u32 base,u32 count) noexcept {
    ++current->audio_init_calls;
    current->active_sound_base=base; current->active_sound_count=count;
    if(base!=0x1130u || count!=0x0Bu)
        stop("ORIGINAL_GAME_RESOURCE_INITIALIZER_ABI_CHANGED");
    code2::function_2f2ec(current->machine,current->game_audio,base,count);
    stop("ORIGINAL_AUDIO_INITIALIZER_RETURNED_UNEXPECTEDLY");
}
u32 enter_game(u32 argc,u32 argv) noexcept {
    current->game_argc=argc;current->game_argv=argv;
    if(!current->execute_application)
        stop("STOP_BEFORE_ORIGINAL_GAME_ENTRY_20608");
    // Original main is executed, not a synthetic lift or a new game engine.
    // Every external call needed before the first hardware boundary is
    // source-backed; stop at the *actual* unimplemented DOS/4GW ISR install.
    code2::function_20608(current->machine,current->game);
    stop("ORIGINAL_MAIN_RETURNED_WITHOUT_375FE_BREAKPOINT");
}
void unexpected_exit(u32) noexcept {stop("GAME_ENTRY_RETURNED_WITHOUT_EXECUTION");}
void after_initializers(code2::Machine& m) noexcept {
    if(!current->extended)stop("POST_INITIALIZER_RUNTIME_UNRESOLVED");
    if(current->completed_initializers!=4u)
        stop("INIT_SCHEDULER_TERMINATED_PREMATURELY");
    code2::function_39589(m,current->post);
    stop("POST_INITIALIZER_RETURNED_UNEXPECTEDLY");
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
    // CODE.2 original DGROUP string literals at 114C/1154/115C/1164,
    // used by the *actual* compiled getenv and decimal parser in 20608.
    // These fixture bytes are independently certified against original CODE.2.
    constexpr std::array<const char*,4> names{
        "ALSIU","KHSSDKJ","CANLW","SEPVON"};
    constexpr std::array<u32,4> offsets{0x114Cu,0x1154u,0x115Cu,0x1164u};
    for(std::size_t i=0;i<names.size();++i) {
        const std::size_t len=std::strlen(names[i])+1u;
        std::memcpy(img.objects[1].raw.data()+offsets[i],names[i],len);
    }
    // Original CODE.2 DGROUP ctype table 165C uses bit 20h for ASCII
    // decimal digits and bit 02h for ASCII whitespace. This restricted
    // synthetic table enables only the exact character classes exercised.
    for(unsigned ch='0';ch<='9';++ch)
        img.objects[1].raw[0x165Cu+ch+1u]=0x20u;
    img.objects[1].raw[0x165Cu+unsigned(' ')+1u]=0x02u;
    // Five original-backed 26-byte stream descriptors (original CODE.2
    // DGROUP F6FC/F716/F730/F74A/F764), terminated by a zero descriptor.
    constexpr std::array<u32,5> stream_flags{1u,2u,2u,3u,2u};
    for(std::size_t i=0;i<stream_flags.size();++i)
        code2::store_le32(img.objects[1].raw.data()+0xF6FCu+i*0x1Au+0xCu,
                          stream_flags[i]);
    // Original shutdown table at 12738 has five physical bytes and one LE
    // zero-fill byte; the original callback address is therefore 3A228.
    img.objects[1].raw[0x12738u]=0u;
    img.objects[1].raw[0x12739u]=0x20u;
    code2::store_le32(img.objects[1].raw.data()+0x1273Au,0x3A228u);
    return img;
}
std::vector<u8> read_file(const char* name) {
    std::ifstream file(name,std::ios::binary);
    require(bool(file),"cannot open original CODE.2");
    return {std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
}
void terminator_exit() noexcept {
    stop("STOP_BEFORE_DOS_EXIT_3A63E");
}
void unexpected_terminator_close(u32,u32) noexcept {
    stop("TERMINATION_STREAM_CLOSE_REQUIRES_HOST_SERVICES");
}
void termination_guest(u32 target,u32 arg) noexcept {
    if(target!=0x3A228u || arg!=0xFF00u)
        stop("ORIGINAL_TERMINATION_CALLBACK_UNEXPECTED");
    current->first_guest=target;
    current->guest_arg=arg;
    code2::Runtime3a1b3Boundary bounds{};
    bounds.close_3a689=&unexpected_terminator_close;
    bounds.exit_3a63e=&terminator_exit;
    // A zeroed guest stream list tests the no-open-stream exit path only.
    code2::function_3a228(current->machine,bounds);
    stop("TERMINATION_GUEST_RETURNED_WITHOUT_EXIT");
}
void termination_probe(const rloft_recomp::OriginalLeImage& img,bool original) {
    require(img.objects.size()==2 &&
            img.objects[1].raw.size()==0x2DD10u,
            "CODE.2 termination data object unavailable");
    Session s;
    s.data=img.objects[1].raw;
    s.machine.data=s.data.data();
    require(code2::load_le32(s.data.data()+0x2BCF0u)==0u,
            "termination probe requires original zero-initialized stream list");
    require(s.data[0x12738u]==0u && s.data[0x12739u]==0x20u &&
            code2::load_le32(s.data.data()+0x1273Au)==0x3A228u,
            "authentic original termination table value missing");
    s.init.call_guest=&termination_guest;
    current=&s;
    if(setjmp(s.escape)==0) {
        code2::function_3962d(s.machine,s.init,0u,0xFFu);
        stop("ORIGINAL_TERMINATION_SCHEDULER_RETURNED_WITHOUT_EXIT");
    }
    current=nullptr;
    require(s.stop_reason==std::string("STOP_BEFORE_DOS_EXIT_3A63E") &&
            s.first_guest==0x3A228u && s.guest_arg==0xFF00u,
            "original CODE.2 3962D to 3A228 source termination edge failed");
    std::cout<<"ORIGINAL_TERMINATION_SCHEDULER=3962D\n"
             <<"ORIGINAL_ZERO_FILLED_CALLBACK_TABLE=12738..1273D\n"
             <<"EXECUTED_TERMINATION_HANDLER=3A228\n"
             <<"DOS_EXIT_BOUNDARY_3A63E=STOPPED_BEFORE_OS\n"
             <<"ORIGINAL_DOS_SHUTDOWN_PARITY=NO\n"
             <<"IMAGE_INPUT="<<(original?"ORIGINAL_LE_GEOMETRY_VERIFIED":"SYNTHETIC_FIXTURE")<<'\n';
}
void arena_split_coalesce_probe() {
    Session s;
    s.data.assign(0x2DD10u,0u);
    s.machine.data=s.data.data();
    current=&s;
    initialize_guest_arena(s);
    constexpr u32 sentinel=0x2B820u;
    const u32 first=code2::function_39838(s.machine,s.arena_header,0x20u);
    const u32 second=code2::function_39838(s.machine,s.arena_header,0x10u);
    require(first==s.arena_start+4u &&
            second>first &&
            code2::load_le32(s.data.data()+s.arena_header+0x18u)==2u &&
            code2::load_le32(s.data.data()+s.arena_header+0x1Cu)==1u,
            "original arena splitter did not allocate two contiguous blocks");
    require(code2::function_39838(s.machine,s.arena_header,0u)==0u &&
            code2::function_39838(s.machine,s.arena_header,0xFFFFFFF0u)==0u,
            "original arena allocator size rejection differs");
    // Each allocated block has its low size bit set. Reverse-order original
    // source free 398E0 must combine the blocks with the initial free tail.
    require((code2::load_le32(s.data.data()+first-4u)&1u)!=0u &&
            (code2::load_le32(s.data.data()+second-4u)&1u)!=0u,
            "original allocated-block headers are not marked used");
    code2::function_398e0(s.machine,s.arena_header,second);
    require(code2::load_le32(s.data.data()+s.arena_header+0x18u)==1u,
            "original arena free did not decrement allocated count");
    code2::function_398e0(s.machine,s.arena_header,first);
    const u32 at=code2::load_le32(s.data.data()+s.arena_header+0x0Cu);
    require(at==s.arena_start &&
            code2::load_le32(s.data.data()+at)==s.arena_end-s.arena_start &&
            code2::load_le32(s.data.data()+s.arena_header+0x18u)==0u &&
            code2::load_le32(s.data.data()+s.arena_header+0x1Cu)==1u &&
            code2::load_le32(s.data.data()+sentinel+4u)==at &&
            code2::load_le32(s.data.data()+sentinel+8u)==at &&
            code2::load_le32(s.data.data()+at+4u)==sentinel &&
            code2::load_le32(s.data.data()+at+8u)==sentinel,
            "original arena source free/adjacent-block coalescing mismatch");
    const u32 reused=code2::function_39838(s.machine,s.arena_header,0x20u);
    require(reused==first,"original arena first-fit reuse changed");
    // Original MOV EAX,EDI at 37BC2 passes raw request 7 (not cached 12).
    // Original 39838 rounds 7+7 down to 12 total bytes, not 16.
    s.heap.arena_alloc=&fixture_arena_alloc;
    const u32 tiny=code2::function_37b42(s.heap,7u);
    require(tiny>reused && tiny<s.arena_end &&
            code2::load_le32(s.data.data()+tiny-4u)==(12u|1u),
            "original 37B42->39838 raw request was rounded twice");
    current=nullptr;
    std::cout<<"CODE2_ORIGINAL_ARENA_39838_398E0=PASS\\n"
             <<"GUEST_SOURCE_ALLOCATE_FREE_REUSE=PASS\\n"
             <<"DOS_REAL_HEAP_PARITY=NO\\n";
}
void environment_alloc_failure_probe() {
    // Original 3A2AD must free already-copied environment string storage when
    // pointer-table allocation fails. The *failure injection* is test-only.
    // Both the first allocation and the rollback/free run through actual
    // reconstructed 37B42 -> 39838 and 37C37 -> 398E0 source.
    Session s;
    s.data.assign(0x2DD10u,0u);
    s.machine.data=s.data.data();
    s.heap.arena_alloc=&fixture_arena_alloc;
    s.heap.arena_free=&fixture_arena_free;
    s.environment.alloc=&injected_fail_second_alloc;
    s.environment.free_37c37=&fixture_free;
    s.environment.memset_30d30=&fixture_fill;
    constexpr u32 env_source=0x2D700u;
    constexpr char input[]="no87=1";
    std::copy_n(reinterpret_cast<const u8*>(input),sizeof(input),
                s.data.data()+env_source);
    s.data[env_source+sizeof(input)]=0u; // double NUL
    current=&s;
    code2::function_3a2ad(s.machine,s.environment,env_source);
    const u32 a=s.arena_header;
    const u32 sentinel=a+0x20u;
    const u32 block=code2::load_le32(s.data.data()+a+0x0Cu);
    require(s.injected_alloc_attempts==2u && s.allocation_calls==1u &&
            s.aligned_allocations==1u && s.arena_frees==1u &&
            code2::load_le32(s.data.data()+0x12688u)==0u &&
            code2::load_le32(s.data.data()+0x1268Cu)==0u &&
            code2::load_le32(s.data.data()+a+0x18u)==0u &&
            code2::load_le32(s.data.data()+a+0x1Cu)==1u &&
            block==s.arena_start &&
            code2::load_le32(s.data.data()+block)==s.arena_end-s.arena_start &&
            code2::load_le32(s.data.data()+sentinel+4u)==block &&
            code2::load_le32(s.data.data()+sentinel+8u)==block,
            "original environment allocation failure did not free its guest arena block");
    current=nullptr;
    std::cout<<"ORIGINAL_3A2AD_ALLOC_FAILURE_ROLLBACK=PASS\n"
             <<"ORIGINAL_37C37_398E0_FREE_COALESCE=PASS\n"
             <<"FAILED_ALLOCATION_INJECTED=YES\n"
             <<"DOS_HEAP_PARITY=NO\n";
}
void run(const rloft_recomp::OriginalLeImage& img,bool original,
         bool extended,bool execute_application=false,
         GameConfig game_config=GameConfig::Unset) {
    require(img.objects.size()==2 && img.entry_object==1 &&
            img.entry_offset==0x353A0u && img.stack_object==2 &&
            img.stack_offset==0x2DD10u &&
            img.objects[0].raw.size()==0x3A852u &&
            img.objects[1].raw.size()==0x2DD10u,"CODE.2 LE geometry mismatch");
    Session state;
    state.extended=extended;
    state.execute_application=execute_application;
    state.game_config=game_config;
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
    if(game_config==GameConfig::Overrides) {
        // Testing a complete source-backed original getenv/atoi path with
        // four explicit DOS environment variables; these values are *not*
        // substituted into CODE.2 globals by the host.
        constexpr char kOverrides[]=
            "no87=1\0ALSIU=0\0KHSSDKJ=7\0CANLW=5\0SEPVON=359\0\0\1\0C:\\RLOFT\\CODE.2";
        std::copy_n(reinterpret_cast<const u8*>(kOverrides),sizeof(kOverrides),
                    state.env.begin());
    } else if(game_config==GameConfig::SignedWrap) {
        constexpr char kSigned[]=
            "no87=1\0ALSIU=22\0KHSSDKJ=-1\0CANLW=65536\0SEPVON=-90\0\0\1\0C:\\RLOFT\\CODE.2";
        std::copy_n(reinterpret_cast<const u8*>(kSigned),sizeof(kSigned),
                    state.env.begin());
    }
    state.machine.data=state.data.data();
    state.machine.cpu.ds=0x30;state.machine.cpu.ss=0x30;
    state.machine.cpu.cs=0x17;state.machine.cpu.esp=0x2DD10;
    state.entry.tail_35418=&invoke_startup;
    // The canonical 352EE implementation delegates case-folded comparison
    // to the real canonical 39526, not a host std::getenv/strtol substitute.
    state.entry.call_39526=&code2::function_39526;
    state.game.call_352ee=&original_getenv;
    state.game.call_35349=&original_atoi;
    state.game.call_2f2ec=&original_audio_init;
    state.game_audio.call_375fe_far=&stop_before_original_far_install;
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
    if(extended) {
        state.heap.arena_alloc=&fixture_arena_alloc;
        state.environment.alloc=&fixture_alloc;
        state.environment.memset_30d30=&fixture_fill;
        state.environment.free_37c37=&fixture_free;
        state.streams.alloc_primary=&fixture_alloc;
        state.streams.alloc_fallback=&rejected_fallback;
        state.streams.fatal_3a536=&rejected_fatal;
        state.argv.alloc=&fixture_alloc;
        state.post.call_3a165=&post_stack_capacity;
        state.post.alloca_guest=&post_stack_alloc;
        state.post.call_3a38a=&post_protection;
        state.post.call_20608=&enter_game;
        state.post.call_352be=&unexpected_exit;
    }
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
    require(code2::load_le16(state.data.data()+0x126ECu)==0x30u,
            "canonical SS initializer at 3A16E did not run");
    if(!extended) {
        require(state.completed_initializers==1u &&
                state.first_guest==0x3A2ADu && state.guest_arg==0xFFu,
                "original scheduler stopped at the wrong first unimplemented callback");
        require(state.stop_reason==std::string("NEXT_ORIGINAL_INITIALIZER_REQUIRES_ALLOCATOR"),
                "default test unexpectedly passed original allocator boundary");
    }else{
        const char* required_stop=execute_application
            ? "STOP_BEFORE_DOS_FAR_CALLBACK_INSTALL_375FE"
            : "STOP_BEFORE_ORIGINAL_GAME_ENTRY_20608";
        if(state.completed_initializers!=4u ||
           state.stop_reason!=std::string(required_stop)) {
            std::cerr<<"EXTENDED_STARTUP_DIAGNOSTIC stop="<<state.stop_reason
                     <<" completed="<<state.completed_initializers
                     <<" allocations="<<state.allocation_calls
                     <<" stream_nodes="<<state.stream_nodes
                     <<" env_copies="<<state.env_copies
                     <<" argv_init="<<state.argv_initializations
                     <<" game_argc="<<state.game_argc
                     <<" last_guest=0x"<<std::hex<<state.first_guest<<std::dec<<'\n';
        }
        require(state.completed_initializers==4u &&
                state.stop_reason==std::string(required_stop),
                "original initializer to game-entry/ISR-install source path differs");
        const u32 expected_env_size=game_config==GameConfig::Unset?7u:
            game_config==GameConfig::Overrides?44u:50u;
        require(state.env_copies==expected_env_size &&
                state.argv_initializations==1u,
                "original environment or argv initializer did not run");
        const u32 table=code2::load_le32(state.data.data()+0x12688u);
        const u32 expected_env_entries=game_config==GameConfig::Unset?1u:5u;
        require(table>=0x2C000u && table<0x2D500u &&
                code2::load_le32(state.data.data()+table+4u*expected_env_entries)==0u,
                "original environment pointer-table terminator differs");
        const u32 item=code2::load_le32(state.data.data()+table);
        require(item>=0x2C000u && item<0x2D500u &&
                std::memcmp(state.data.data()+item,"no87=1",7)==0,
                "original environment string was not copied by 3A2AD");
        require(state.stream_nodes==5u && state.allocation_calls==8u &&
                state.aligned_allocations==state.allocation_calls,
                "five original stream descriptors plus env/table/argv allocations not observed");
        require(state.data[0xF73Du]==0x04u &&
                code2::load_le32(state.data.data()+0x2B74Cu)==0u,
                "original stream initializer state mismatch");
        require(state.game_argc==2u && state.game_argv!=0u &&
                code2::load_le32(state.data.data()+0x2BD00u)==2u &&
                code2::load_le32(state.data.data()+0x2BD04u)==state.game_argv,
                "original argv initializer did not publish argc/argv");
        const u32 argv1=code2::load_le32(state.data.data()+state.game_argv+4u);
        require(argv1>=0x2C000u && argv1<0x2D500u &&
                std::memcmp(state.data.data()+argv1,"/?",3)==0,
                "original command-line token /? differs");
        require(code2::load_le32(state.data.data()+state.game_argv+8u)==0u,
                "argv final null pointer was not written");
        require(code2::load_le32(state.data.data()+0x1270Cu)==0x8000u,
                "post-initialization flag set by original 3A38A is absent");
        if(execute_application) {
            // Written by the *compiled canonical source* main 20608,
            // followed by 20440 and 2F2EC; no substitute startup model.
            const auto rd16=[&](u32 off){return code2::load_le16(state.data.data()+off);};
            const auto rd32=[&](u32 off){return code2::load_le32(state.data.data()+off);};
            const u16 expected_x=game_config==GameConfig::Overrides?7u:
                game_config==GameConfig::SignedWrap?0xFFFFu:4u;
            const u16 expected_y=game_config==GameConfig::Overrides?5u:
                game_config==GameConfig::SignedWrap?0u:4u;
            const u32 expected_facing=game_config==GameConfig::Overrides?359u:
                game_config==GameConfig::SignedWrap?u32(-90):270u;
            require(rd16(0x255D0u)==0x8000u && rd16(0x255D4u)==0x8000u &&
                    rd16(0x255D2u)==expected_x && rd16(0x255D6u)==expected_y &&
                    rd32(0xC658u)==expected_facing,
                    "real CODE.2 20608 initial position/facing state differs");
            for(u32 off=0x255E0u;off<=0x25610u;off+=4u)
                require(rd32(off)==0u,"original 20608 actor timer reset missed");
            for(u32 off=0x22200u;off<=0x2220Cu;off+=4u)
                require(rd32(off)==0u,"original 20608 counters not reset");
            require(rd32(0x248E8u)==0u && rd32(0x248E4u)==0u,
                    "original 20608 link reset differs");
            require(rd16(0xEF53u)==0x0182u &&
                    state.data[0xEF3Eu]==0x0Cu &&
                    state.data[0xEF68u]==1u && state.data[0xEF3Cu]==0xFFu &&
                    rd32(0xF0C0u)==0xFFFFFFFFu,
                    "original CODE.2 2F2EC source audio/resource prelude differs");
            const unsigned expected_queries=game_config==GameConfig::Unset?1u:5u;
            const unsigned expected_conversions=game_config==GameConfig::Unset?0u:4u;
            require(state.getenv_calls==expected_queries &&
                    state.atoi_calls==expected_conversions &&
                    state.audio_init_calls==1u && state.far_install_calls==1u &&
                    state.far_target==0x2F2E0u &&
                    state.active_sound_base==0x1130u &&
                    state.active_sound_count==0x0Bu,
                    "real CODE.2 getenv/atoi/resource/ISR callback chain differs");
        }
        require(state.arena_seeded &&
                code2::load_le32(state.data.data()+0x12624u)==state.arena_header &&
                code2::load_le32(state.data.data()+state.arena_header+0x18u)==8u &&
                code2::load_le32(state.data.data()+state.arena_header+0x1Cu)==1u,
                "actual original 39838 arena allocation/split metadata differs");
        const u32 freeptr=code2::load_le32(state.data.data()+state.arena_header+0x0Cu);
        const u32 sent=state.arena_header+0x20u;
        require(freeptr>=state.arena_start && freeptr<state.arena_end &&
                code2::load_le32(state.data.data()+freeptr+4u)==sent &&
                code2::load_le32(state.data.data()+freeptr+8u)==sent &&
                code2::load_le32(state.data.data()+sent+4u)==freeptr &&
                code2::load_le32(state.data.data()+sent+8u)==freeptr &&
                code2::load_le32(state.data.data()+freeptr)>12u,
                "real 39838 original arena free-list split/link invariants differ");
    }
    std::cout<<"CODE2_ORIGINAL_SOURCE_ENTRY=353A0\n"
             <<"ORIGINAL_PRIORITY_INITIALIZERS_EXECUTED="<<state.completed_initializers<<"/4\n"
             <<"TEST_ONLY_HEAP_ALLOCATIONS="<<state.allocation_calls<<"\n"
             <<"EXECUTED_ORIGINAL_ARENA_SPLITTER_39838="<<(extended?"YES":"NO")<<"\n"
             <<"REAL_GAME_ENTRY_20608_CALLED="<<(execute_application?"EXECUTED_SOURCE":extended?"REACHED_CALLBACK_NOT_EXECUTED":"NO")<<"\n"
             <<"SOURCE_GAME_AUDIO_INIT_2F2EC="<<(execute_application?"EXECUTED_TO_ORIGINAL_375FE":"NOT_EXECUTED")<<"\n"
             <<"ORIGINAL_GETENV_352EE_CALLS="<<state.getenv_calls<<"\n"
             <<"ORIGINAL_ATOI_35349_CALLS="<<state.atoi_calls<<"\n"
             <<"EXECUTION_BOUNDARY="<<state.stop_reason<<"\n"
             <<"ORIGINAL_DOS_PARITY=NO\nGAME_BOOTED=NO\n"
             <<"IMAGE_INPUT="<<(original?"ORIGINAL_LE_GEOMETRY_VERIFIED":"SYNTHETIC_FIXTURE")<<'\n';

}
}
int main(int argc,char**argv) {
    try {
        bool extended=false,termination=false,arena_probe=false,failure_probe=false;
        bool execute_application=false;
        GameConfig game_config=GameConfig::Unset;
        int original_index=1;
        if(argc>=2) {
            const std::string mode(argv[1]);
            if(mode=="--through-initializers" || mode=="--termination-probe" ||
               mode=="--arena-roundtrip-probe" || mode=="--env-rollback-probe" ||
               mode=="--enter-game" || mode=="--enter-game-config" ||
               mode=="--enter-game-signed-wrap") {
                execute_application=(mode=="--enter-game" ||
                    mode=="--enter-game-config" || mode=="--enter-game-signed-wrap");
                if(mode=="--enter-game-config")game_config=GameConfig::Overrides;
                if(mode=="--enter-game-signed-wrap")game_config=GameConfig::SignedWrap;
                extended=(mode=="--through-initializers" || execute_application);
                termination=(mode=="--termination-probe");
                arena_probe=(mode=="--arena-roundtrip-probe");
                failure_probe=(mode=="--env-rollback-probe");
                original_index=2;
            }
        }
        require(argc==original_index || argc==original_index+1,
                "usage: rloft_code2_startup_gate [--through-initializers|--enter-game|--enter-game-config|--enter-game-signed-wrap|--termination-probe|--arena-roundtrip-probe|--env-rollback-probe] [CODE.2]");
        if(arena_probe || failure_probe) {
            require(argc==original_index,"allocator probes use synthetic-only guest memory");
            if(arena_probe)arena_split_coalesce_probe();
            else environment_alloc_failure_probe();
        }else if(argc==original_index) {
            const auto fixture=synthetic_image();
            if(termination)termination_probe(fixture,false);
            else run(fixture,false,extended,execute_application,game_config);
        }else{
            const auto b=read_file(argv[original_index]);
            require(b.size()==558689u,"original CODE.2 size is incorrect");
            const auto image=rloft_recomp::OriginalLeImage::parse(b);
            if(termination)termination_probe(image,true);
            else run(image,true,extended,execute_application,game_config);
        }
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_STARTUP_GATE_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
