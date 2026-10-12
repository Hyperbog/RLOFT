// Bounded execution of the *canonical* CODE.2 DOS startup, not an emulated game.
// The only synthetic pieces are explicitly identified DOS/PSP/environment fixtures.
// Every unsupported service stops execution before the original code can consume a
// fabricated result. Source: original CODE.2 353A0 -> 35418 -> 395E2.
#include "../../Code2/Code2.cpp"
#include "original_le_image.h"
#include "dos_readonly_files.hpp"
#include "dpmi_dos_memory.hpp"
#include <algorithm>
#include <array>
#include <csetjmp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <map>
#include <filesystem>
#include <string>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
enum class GameConfig { Unset, Overrides, SignedWrap };
// Hypothetical mouse hardware outcomes, NEVER a claim of DOS observation.
enum class MouseResetBranch { Stop, Present, Absent, MemoryHandoff, FreeBoundary, RangeX, RangeY, DiskSpace };
struct GuestVector final { u32 offset{}; u16 selector{}; };
struct Session {
    std::jmp_buf escape;
    std::vector<u8> data, code;
    std::array<u8,256> psp{};
    std::array<u8,512> env{};
    code2::Machine machine{};
    code2::Runtime111Boundary entry{};
    code2::Application46Boundary game{};
    code2::Application69Boundary game_audio{};
    code2::Runtime126Boundary dos_irq{};
    code2::Application71Boundary keyboard_irq{};
    code2::Application72Boundary dos_file_io{};
    code2::Runtime131Boundary dos_file_open{};
    code2::Runtime132Boundary dos_file_read{};
    code2::Runtime131CloseBoundary dos_file_close{};
    code2::Runtime100Boundary mouse_init{};
    code2::Runtime101Boundary mouse_range{};
    code2::Application56Boundary audio_seed{};
    std::array<GuestVector,256> ivt{};
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
    bool advance_to_sound_file=false;
    bool advance_to_mouse=false;
    bool sound_no_arguments=false;
    GameConfig game_config=GameConfig::Unset;
    MouseResetBranch mouse_branch=MouseResetBranch::Stop;
    bool arena_seeded=false;
    unsigned getenv_calls=0, atoi_calls=0, audio_init_calls=0, far_install_calls=0;
    u32 active_sound_base=0, active_sound_count=0, far_target=0;
    u8 vector_number=0; u16 vector_selector=0; u32 vector_handler=0;
    unsigned dos_vector_callbacks=0; bool observe_vector_only=false;
    unsigned irq_vector_reads=0, irq_vector_sets=0, seed_calls=0;
    unsigned audio_atoi_calls=0;
    u32 audio_atoi_text=0;
    unsigned checked_alloc_calls=0, checked_fill_calls=0, file_open_calls=0;
    u32 sound_config_path=0, sound_config_mode=0;
    unsigned dos_ah3d_open_calls=0;
    u32 original_open_create_slot=0xFFFFFFFFu;
    u8 open_access_share=0xFFu;
    unsigned dos_file_reads=0, dos_file_closes=0, mouse_reset_calls=0;
    unsigned dpmi_mouse_simulation_calls=0, mouse_range_calls=0;
    unsigned mouse_x_range_returned=0, dpmi_mouse_free_calls=0, disk_space_calls=0;
    std::size_t audio_file_cursor=0;
    u32 audio_descriptor_flags=0, audio_published_flags=0;
    std::array<u8,59> audio_file_fixture{};
    // Optional genuine host file I/O. Existing DOS/mouse/vector fixtures
    // remain explicit; this is not a complete DOS runtime.
    std::unique_ptr<rloft_recomp::ReadOnlyDosFiles> host_files;
    std::map<u32,u32> host_descriptor_flags;
    // Opt-in stateful DPMI 0100/0101 host service; never original DOS parity.
    std::unique_ptr<rloft_recomp::DpmiDosMemory> dpmi_dos_memory;

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
// Original 375FE selects two different interrupt installation mechanisms
// depending on the DOS extender mode; actual installation remains a STOP.
void original_vector_install(u8 vector,u16 selector,u32 handler) noexcept {
    ++current->dos_vector_callbacks;
    current->vector_number=vector;
    current->vector_selector=selector;
    current->vector_handler=handler;
    if(current->advance_to_sound_file) {
        current->ivt[vector]={handler,selector};
        return; // test-only IVT: never touch the host DOS/Windows vectors
    }
    if(!current->observe_vector_only)
        stop("STOP_BEFORE_ORIGINAL_DOS_VECTOR_INSTALL_375FE");
}
void original_far_install(u32 target,u16 selector) noexcept {
    ++current->far_install_calls;
    current->far_target=target;
    if(target!=0x2F2E0u || selector!=current->machine.cpu.cs)
        stop("ORIGINAL_375FE_FAR_INSTALL_CONTRACT_CHANGED");
    // Original 3761E uses DOS/4GW byte 12662; 105FC is argc
    // published by original 3968E, not an extender selector mode.
    const u8 guest_extender=current->machine.data[0x12662u];
    // Execute the *canonical reconstructed function_375fe*, including its
    // original mode-dependent vector/selector dispatch; fail closed at the
    // actual DOS vector write rather than before this source routine.
    (void)code2::function_375fe(current->machine,current->dos_irq,
                                target,selector,guest_extender);
    if(current->advance_to_sound_file && current->dos_vector_callbacks==1u)
        return;
    stop("ORIGINAL_375FE_RETURNED_WITHOUT_VECTOR_INSTALL");
}
// Bounded in-memory IVT models only the exact DOS get/set service boundary
// used by original 30BB4. This is NOT host interrupt or IRQ emulation.
void guest_get_vector(u8 vector,u32* offset,u16* selector) noexcept {
    ++current->irq_vector_reads;
    const auto& entry=current->ivt[vector];
    if(offset)*offset=entry.offset;
    if(selector)*selector=entry.selector;
}
void guest_set_vector(u8 vector,u32 offset,u16 selector) noexcept {
    ++current->irq_vector_sets;
    current->ivt[vector]={offset,selector};
}
void original_keyboard_vectors() noexcept {
    code2::function_30bb4(current->machine,current->keyboard_irq);
}
u32 original_random_seed(u32 value) noexcept {
    ++current->seed_calls;
    code2::function_30cc4(current->machine,value);
    return 0u;
}
u32 original_checked_allocation_header(u32 amount) noexcept {
    return code2::function_37b34(current->heap,amount);
}
u32 original_checked_allocation(u32 amount) noexcept {
    ++current->checked_alloc_calls;
    return code2::function_3104c(current->machine,current->dos_file_io,amount);
}
void original_linear_dword_fill(u32 dst,u32 packed,u32 count) noexcept {
    code2::function_379d0(segment(0x30u,dst,count),count,packed);
}
void original_checked_memset(u32 dst,u32 byte,u32 count) noexcept {
    ++current->checked_fill_calls;
    (void)code2::function_30d30(current->dos_file_io,dst,byte,count);
}
u8 original_dos_open_guest_read8(u32 addr) noexcept {
    return *segment(0x30u,addr,1u);
}
std::string guest_dos_basename(u32 off) {
    // A DOS 8.3 path is at most 12 characters, plus its NUL byte.
    // segment() verifies each guest byte before it reaches the host.
    std::string name;
    for(unsigned i=0;i<=12u;++i) {
        const auto ch=seg8(0x30u,off+i);
        if(ch==0u)return name;
        name.push_back(static_cast<char>(ch));
    }
    stop("GUEST_DOS_PATH_UNTERMINATED_OR_OVERLONG");
}
code2::s32 stop_at_real_dos_ah3d_open(u32 path,u8 access_share) noexcept {
    ++current->dos_ah3d_open_calls;
    current->open_access_share=access_share;
    if(access_share!=0u || (!current->host_files && path!=0x143Cu))
        stop("ORIGINAL_DOS_AH3D_OPEN_ARGUMENTS_DIFFER");
    if(current->host_files) {
        const auto name=guest_dos_basename(path);
        const auto result=current->host_files->open(name,access_share);
        if(result.carry)stop("HOST_DOS_AH3D_GUEST_PATH_OPEN_FAILED");
        current->host_descriptor_flags[result.ax]=0u;
        return static_cast<code2::s32>(result.ax);
    }
    if(current->advance_to_mouse)return 5; // EXPLICIT test-only DOS handle
    stop("STOP_BEFORE_ORIGINAL_DOS_AH3D_OPEN");
}
// Read-only synthetic 59-byte SOUND.CFG transport for testing the *original*
// reconstructed file wrappers. These callbacks do not read/modify game
// installation files or pretend that the 59 bytes are authentic media.
u32 original_read_descriptor_flags(u32 h) noexcept {
    if(current->host_files) {
        const auto it=current->host_descriptor_flags.find(h);
        if(it==current->host_descriptor_flags.end())
            stop("HOST_DOS_DESCRIPTOR_BAD_HANDLE");
        return it->second;
    }
    if(h!=5u)stop("ORIGINAL_DOS_DESCRIPTOR_UNEXPECTED_HANDLE");
    return current->audio_descriptor_flags;
}
bool original_descriptor_is_device(u32 h) noexcept {
    if(current->host_files && current->host_descriptor_flags.count(h)==0u)
        stop("HOST_DOS_DEVICE_PROBE_BAD_HANDLE");
    if(!current->host_files && h!=5u)
        stop("ORIGINAL_DOS_DEVICE_PROBE_BAD_HANDLE");
    return false;
}
void original_publish_descriptor_flags(u32 h,u32 flags) noexcept {
    if(current->host_files) {
        auto it=current->host_descriptor_flags.find(h);
        if(it==current->host_descriptor_flags.end())
            stop("HOST_DOS_DESCRIPTOR_PUBLISH_BAD_HANDLE");
        it->second=flags;
    } else if(h!=5u)stop("ORIGINAL_DOS_DESCRIPTOR_PUBLISH_BAD_HANDLE");
    if(h==5u) {
        current->audio_descriptor_flags=flags;
        current->audio_published_flags=flags;
    }
}
code2::s32 original_dos_read_fixture(u32 h,u32 destination,u32 count) noexcept {
    if(current->host_files) {
        if(h>0xFFFFu || current->host_descriptor_flags.count(h)==0u)
            stop("HOST_DOS_AH3F_BAD_DESCRIPTOR");
        ++current->dos_file_reads;
        const auto response=current->host_files->read(
            static_cast<u16>(h),segment(0x30u,destination,count),count);
        if(response.carry)stop("HOST_DOS_AH3F_READ_FAILED");
        if(h==5u)current->audio_file_cursor+=response.ax;
        return static_cast<code2::s32>(response.ax);
    }
    if(h!=5u || count!=59u || current->audio_file_cursor!=0u)
        stop("ORIGINAL_DOS_SOUND_CFG_READ_ABI_CHANGED");
    ++current->dos_file_reads;
    std::memcpy(segment(0x30u,destination,count),
                current->audio_file_fixture.data(),count);
    current->audio_file_cursor=count;
    return static_cast<code2::s32>(count);
}
code2::s32 original_dos_close_fixture(u32 h) noexcept {
    if(current->host_files) {
        if(h>0xFFFFu || current->host_descriptor_flags.count(h)==0u)
            stop("HOST_DOS_AH3E_BAD_DESCRIPTOR");
        const auto response=current->host_files->close(static_cast<u16>(h));
        if(response.carry)stop("HOST_DOS_AH3E_CLOSE_FAILED");
        current->host_descriptor_flags.erase(h);
        ++current->dos_file_closes;
        if(h==5u)current->audio_descriptor_flags=0u;
        return 0;
    }
    if(h!=5u || current->audio_file_cursor!=59u)
        stop("ORIGINAL_DOS_SOUND_CFG_CLOSE_ABI_CHANGED");
    ++current->dos_file_closes;
    current->audio_descriptor_flags=0u;
    return 0;
}
void original_sound_config_read(u32 destination,u32 count,u32 h) noexcept {
    (void)code2::function_312ec(current->dos_file_io,destination,count,h);
}
u32 original_binary_read(u32 handle,u32 dest,u32 n) noexcept {
    return static_cast<u32>(code2::function_38106(
        current->dos_file_read,handle,dest,n));
}
void original_sound_config_close(u32 handle) noexcept {
    (void)code2::function_312dc(current->dos_file_io,handle);
}
u32 original_source_descriptor_close(u32 handle) noexcept {
    return static_cast<u32>(code2::function_380f8(
        current->dos_file_close,handle));
}
code2::Runtime100MouseResetResult controlled_mouse_reset() noexcept {
    ++current->mouse_reset_calls;
    if(current->mouse_branch==MouseResetBranch::Stop)
        stop("STOP_BEFORE_DOS_INT33_MOUSE_RESET");
    if(current->mouse_branch!=MouseResetBranch::Absent)
        return {0xFFFFu,3u};
    return {0u,0u};
}
void original_dpmi_register_context(u8* dst) noexcept {
    // Original CODE.2 37A73 copies six segment words in ES,CS,SS,DS,FS,GS
    // order into the private 30D52 register-call context.
    code2::Runtime128Boundary ctx{};
    const auto& cpu=current->machine.cpu;
    ctx.es=cpu.es;ctx.cs=cpu.cs;ctx.ss=cpu.ss;
    ctx.ds=cpu.ds;ctx.fs=cpu.fs;ctx.gs=cpu.gs;
    std::array<u16,6> words{};
    code2::function_37a73(ctx,words.data());
    for(unsigned i=0;i<words.size();++i)
        code2::store_le16(dst+2u*i,words[i]);
}
bool wants_synthetic_mouse_dpmi_allocation() noexcept {
    const auto b=current->mouse_branch;
    return b==MouseResetBranch::MemoryHandoff ||
           b==MouseResetBranch::FreeBoundary ||
           b==MouseResetBranch::RangeX || b==MouseResetBranch::RangeY ||
           b==MouseResetBranch::DiskSpace;
}
u32 stop_at_original_dpmi31(u32 n,u8* in,u8* out,u8* ctx) noexcept {
    ++current->dpmi_calls;
    if(n!=0x31u)stop("ORIGINAL_DPMI_INTERRUPT_CHANGED");
    const auto& cpu=current->machine.cpu;
    const std::array<u16,6> expected{
        cpu.es,cpu.cs,cpu.ss,cpu.ds,cpu.fs,cpu.gs};
    for(unsigned i=0;i<expected.size();++i) {
        if(code2::load_le16(ctx+2u*i)!=expected[i])
            stop("ORIGINAL_DPMI_SEGMENT_SNAPSHOT_CHANGED");
    }
    const u32 ax=code2::load_le32(in);
    if(ax==0x0100u) {
        if(in==out || code2::load_le32(in+4u)!=5u)
            stop("ORIGINAL_DPMI_0100_REQUEST_CHANGED");
        if(!wants_synthetic_mouse_dpmi_allocation())
            stop("STOP_BEFORE_DOS_INT31_0100_ALLOCATION");

        if(current->dpmi_dos_memory) {
            const auto result=current->dpmi_dos_memory->allocate(
                static_cast<u16>(code2::load_le32(in+4u)));
            if(result.carry)stop("HOST_DPMI_0100_DOS_MEMORY_EXHAUSTED");
            // Source 30D52 register-context ABI: low AX -> byte 0,
            // low DX -> +0C, carry flag -> +18.
            code2::store_le32(out,result.ax);
            code2::store_le16(out+0x0Cu,result.dx);
            out[0x18u]=0u;
            return 0u;
        }
        // Legacy opt-in fixture retained for differential test continuity.
        code2::store_le32(out,0x2D60u);
        code2::store_le16(out+0x0Cu,0x0060u);
        out[0x18u]=0u;
        return 0u;
    }
    if(ax==0x0101u) {
        ++current->dpmi_mouse_free_calls;
        if(in!=out || code2::load_le16(in+0x0Cu)!=0x0060u ||
           !wants_synthetic_mouse_dpmi_allocation() ||
           current->dpmi_mouse_simulation_calls!=1u)
            stop("ORIGINAL_DPMI_0101_RELEASE_CHANGED");
        if(current->mouse_branch==MouseResetBranch::FreeBoundary)
            stop("STOP_BEFORE_DOS_INT31_0101_MOUSE_FREE");
        if(current->mouse_branch!=MouseResetBranch::RangeX &&
           current->mouse_branch!=MouseResetBranch::RangeY &&
           current->mouse_branch!=MouseResetBranch::DiskSpace)
            stop("ORIGINAL_DPMI_0101_UNEXPECTED_RELEASE");

        if(current->dpmi_dos_memory) {
            const auto result=current->dpmi_dos_memory->release(
                code2::load_le16(in+0x0Cu));
            if(result.carry)stop("HOST_DPMI_0101_INVALID_SELECTOR");
            out[0x18u]=0u;
            return 0u;
        }
        // Legacy opt-in fixture, not the real tracked DPMI service.
        out[0x18u]=0u;
        return 0u;
    }
    stop("ORIGINAL_DPMI_UNRECOGNIZED_FUNCTION");
}
u32 stop_at_original_int31_real_mode_mouse(u32 interrupt,u32* in,
                                           u32* out,u16* segs) noexcept {
    ++current->dpmi_mouse_simulation_calls;
    if(interrupt!=0x31u || in!=out || in[0]!=0x0300u ||
       in[1]!=0x0033u || in[5]!=0x2B770u ||
       segs[0]!=current->machine.cpu.ds ||
       code2::load_le32(segment(0x30u,0x2B770u+0x1Cu,4))!=0x53C1u ||
       code2::load_le16(segment(0x30u,0x2B770u+0x22u,2))!=0x2D60u)
        stop("ORIGINAL_33AA0_DPMI_0300_FRAME_CHANGED");
    if(current->mouse_branch==MouseResetBranch::MemoryHandoff)
        stop("STOP_BEFORE_DOS_INT31_0300_MOUSE_SIMULATION");
    if(current->mouse_branch!=MouseResetBranch::FreeBoundary &&
       current->mouse_branch!=MouseResetBranch::RangeX &&
       current->mouse_branch!=MouseResetBranch::RangeY &&
       current->mouse_branch!=MouseResetBranch::DiskSpace)
        stop("ORIGINAL_DPMI_0300_UNEXPECTED_MODE");
    // No driver probe: frame+1C remains 53C1, not the required 1.
    // The real 33B44 code must then release the allocated DOS selector.
    return 0u;
}
u32 original_mouse_33aa0(u32 frame) noexcept {
    if(frame!=0x2B770u || !wants_synthetic_mouse_dpmi_allocation())
        stop("ORIGINAL_33AA0_FRAME_OR_MODE_CHANGED");
    code2::Runtime99Boundary b{};
    b.ds_selector=current->machine.cpu.ds;
    b.call_33a7c=&stop_at_original_int31_real_mode_mouse;
    return code2::function_33aa0(b,frame);
}
u32 original_mouse_free_via_30de8(u32 selector) noexcept {
    if(selector!=0x0060u)stop("ORIGINAL_MOUSE_SELECTOR_TO_FREE_CHANGED");
    return code2::function_30de8(current->dos_file_io,selector);
}
code2::Runtime101MouseRegs source_mouse_present_2e340() noexcept {
    // Canonical 2E340 reads low AX from DGROUP:[F04C], as established
    // by the actual completed 33B44 after the fake DPMI release.
    return {static_cast<u32>(code2::function_2e340(current->machine)),0u,0u,0u};
}
code2::Runtime101MouseRegs interrupt_mouse_range_boundary(
    u32 command,u32 ebx,u32 ecx,u32 edx) noexcept {
    ++current->mouse_range_calls;
    if(ebx!=0u || ecx!=0u ||
       (command==7u && edx!=0x027Eu) ||
       (command==8u && edx!=0x00C7u) ||
       (command!=7u && command!=8u))
        stop("ORIGINAL_MOUSE_RANGE_ARGUMENTS_CHANGED");
    if(current->mouse_branch==MouseResetBranch::RangeX &&
       command==7u && current->mouse_range_calls==1u)
        stop("STOP_BEFORE_DOS_INT33_X_RANGE");
    if((current->mouse_branch==MouseResetBranch::RangeY ||
        current->mouse_branch==MouseResetBranch::DiskSpace) &&
       command==7u && current->mouse_range_calls==1u) {
        // Explicit test-only successful X range result, no host mouse.
        ++current->mouse_x_range_returned;
        return {0xFFFFu,0u,0u,0u};
    }
    if((current->mouse_branch==MouseResetBranch::RangeY ||
        current->mouse_branch==MouseResetBranch::DiskSpace) &&
       command==8u && current->mouse_range_calls==2u) {
        if(current->mouse_branch==MouseResetBranch::RangeY)
            stop("STOP_BEFORE_DOS_INT33_Y_RANGE");
        // Test-only successful Y range; original CODE.2 2F2EC returns.
        return {0xFFFFu,0u,0u,0u};
    }
    stop("UNEXPECTED_MOUSE_RANGE_COMMAND_ORDER");
}
void original_mouse_range_x(u32 lo,u32 hi) noexcept {
    (void)code2::function_33d18(current->mouse_range,lo,hi);
}
void original_mouse_range_y(u32 lo,u32 hi) noexcept {
    (void)code2::function_33d38(current->mouse_range,lo,hi);
}
bool stop_at_dos_disk_free(u8 drive,u16&,u16&,u16&,u16&) noexcept {
    ++current->disk_space_calls;
    if(drive!=0u || current->mouse_branch!=MouseResetBranch::DiskSpace ||
       current->mouse_range_calls!=2u)
        stop("ORIGINAL_DOS_DISK_FREE_SERVICE_ORDER_CHANGED");
    stop("STOP_BEFORE_DOS_INT21_AH36_DISK_FREE");
}
u32 original_disk_free_from_35286(u32 drive,u16& first,
                                  u16& second,u16& third) noexcept {
    u16 out[4]{};
    // Execute actual canonical 35286 and its first external DOS AH36
    // callback, after 33B44, 33D18 and 33D38 have all returned.
    const u32 ok=code2::function_35286(current->entry,drive,out);
    first=out[0];second=out[1];third=out[2];
    return ok;
}
u32 original_mouse_memory_via_30d80(u32 bytes,u32 a,u32 b) noexcept {
    if(bytes!=0x40u || a!=0xFD14u || b!=0xFD18u)
        stop("ORIGINAL_MOUSE_MEMORY_ARGUMENTS_DIFFER");
    u32 packed_segment=0u;
    u16 selector=0u;
    // Execute canonical 30D80 and 30D52 and reach 33A7C's DOS boundary.
    // Output pointers are the original destination locations, represented
    // as typed local storage; they cannot be written before INT31 succeeds.
    const auto value=code2::function_30d80(
        current->dos_file_io,static_cast<code2::s32>(bytes),
        &packed_segment,&selector);
    code2::store_le32(segment(0x30u,a,4),packed_segment);
    code2::store_le16(segment(0x30u,b,2),selector);
    return value;
}
void stop_at_dos_mouse_warning_ah09(u32 text) noexcept {
    if(text!=0x15C8u)stop("ORIGINAL_MOUSE_DOS_WARNING_POINTER_CHANGED");
    // DOS text call is external. Verify original 311E8's terminator state.
    unsigned n=0u;
    for(;n<256u;++n)if(seg8(0x30u,text+n)==0x24u)break;
    if(n==256u)stop("ORIGINAL_MOUSE_DOS_WARNING_NOT_TERMINATED");
    stop("STOP_BEFORE_DOS_INT21_AH09_MOUSE_WARNING");
}
void original_mouse_warning_via_311e8(u32 msg) noexcept {
    if(msg!=0x15C8u)stop("ORIGINAL_MOUSE_WARNING_ARGUMENT_DIFFERS");
    code2::function_311e8(current->machine,current->dos_file_io,msg);
}
u32 original_mouse_initialization() noexcept {
    return code2::function_33b44(current->machine,current->mouse_init);
}
u32 original_checked_source_open(u32 path,u32 mode) noexcept {
    ++current->file_open_calls;
    current->sound_config_path=path;
    current->sound_config_mode=mode;
    if(mode!=0x200u)stop("ORIGINAL_SOURCE_READONLY_MODE_UNSUPPORTED");
    if(!current->host_files &&
       (path!=0x143Cu ||
        std::memcmp(segment(0x30u,path,10),"sound.cfg",10)!=0))
        stop("ORIGINAL_SOUND_CFG_OPEN_ABI_CHANGED");
    // Actual reconstructed 312A0 uses two stack slots; 37E77 reads an
    // additional inherited third slot. In read-only mode 0x200, 37E99
    // never consumes this create-only value, so a marked dummy is safe
    // for this *bounded* original DOS open witness only.
    return static_cast<u32>(code2::function_37e77(
        current->dos_file_open,path,mode,current->original_open_create_slot));
}
u32 original_sound_file(u32 filename) noexcept {
    return code2::function_312a0(current->dos_file_io,filename);
}
u32 original_sound_pointer_from_argv(u32 text) noexcept {
    ++current->audio_atoi_calls;
    current->audio_atoi_text=text;
    if(text==0u || text>=current->data.size())
        stop("ORIGINAL_AUDIO_ARGV_TEXT_OUT_OF_BOUNDS");
    return code2::function_35349(current->machine,text);
}
void original_audio_init(u32 base,u32 count) noexcept {
    ++current->audio_init_calls;
    current->active_sound_base=base; current->active_sound_count=count;
    if(base!=0x1130u || count!=0x0Bu)
        stop("ORIGINAL_GAME_RESOURCE_INITIALIZER_ABI_CHANGED");
    code2::function_2f2ec(current->machine,current->game_audio,base,count);
    // After the original 33B44 and both source 33D18/33D38 calls
    // actually return, only the explicitly named disk-query probe may
    // proceed into the canonical 20440 -> 202F0 -> 35286 call chain.
    if(current->mouse_branch==MouseResetBranch::DiskSpace)return;
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
    // Original CODE.2 DGROUP sound.cfg literal (2F2EC -> 312A0).
    constexpr char cfg_literal[]="sound.cfg";
    std::memcpy(img.objects[1].raw.data()+0x143Cu,cfg_literal,sizeof(cfg_literal));
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
             <<"IMAGE_INPUT="<<(original?"ORIGINAL_LE_GEOMETRY_VERIFIED":"SYNTHETIC_FIXTURE")<<"\n"
             <<'\n';
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
// Exhaustively execute the original 375FE mode switch (all 256 byte modes).
// This is a source-branch oracle, NOT real DOS IVT/interrupt installation.
void interrupt_vector_matrix_probe() {
    Session state;
    state.observe_vector_only=true;
    state.data.assign(0x2DD10u,0u);
    state.machine.data=state.data.data();
    state.dos_irq.install_interrupt=&original_vector_install;
    current=&state;
    for(unsigned mode=0;mode<256;mode++) {
        state.dos_vector_callbacks=0;
        state.vector_number=0;state.vector_selector=0;state.vector_handler=0;
        state.data[0x12662u]=static_cast<u8>(mode);
        const u32 result=code2::function_375fe(
            state.machine,state.dos_irq,0x2F2E0u,0x17u,
            state.data[0x12662u]);
        const bool extended=(mode>=2u && mode<=6u);
        require(result==0u && state.dos_vector_callbacks==1u &&
                state.vector_number==(extended?6u:0x24u) &&
                state.vector_selector==(extended?0x24u:0u) &&
                state.vector_handler==0x37642u &&
                code2::load_le32(state.data.data()+0x105F2u)==0x2F2E0u &&
                code2::load_le16(state.data.data()+0x105F6u)==0x17u,
                "original 375FE vector choice and callback save differs from CODE.2");
    }
    current=nullptr;
    std::cout<<"ORIGINAL_375FE_MODE_SWITCH=256/256 PASS\n"
             <<"DOS_VECTOR_INSTALLED=NO\nORIGINAL_DOS_PARITY=NO\n";
}
void run(const rloft_recomp::OriginalLeImage& img,bool original,
         bool extended,bool execute_application=false,
         GameConfig game_config=GameConfig::Unset,
         bool advance_to_sound_file=false,bool sound_no_arguments=false,
         bool advance_to_mouse=false,
         const std::vector<u8>* original_sound_cfg=nullptr,
         MouseResetBranch mouse_branch=MouseResetBranch::Stop,
         const std::filesystem::path* host_root=nullptr,
         bool use_host_dpmi=false,bool multi_resource_probe=false) {
    require(img.objects.size()==2 && img.entry_object==1 &&
            img.entry_offset==0x353A0u && img.stack_object==2 &&
            img.stack_offset==0x2DD10u &&
            img.objects[0].raw.size()==0x3A852u &&
            img.objects[1].raw.size()==0x2DD10u,"CODE.2 LE geometry mismatch");
    Session state;
    state.extended=extended;
    state.execute_application=execute_application;
    state.advance_to_sound_file=advance_to_sound_file;
    state.advance_to_mouse=advance_to_mouse;
    state.mouse_branch=mouse_branch;
    if(use_host_dpmi) {
        require(advance_to_mouse &&
                (mouse_branch==MouseResetBranch::MemoryHandoff ||
                 mouse_branch==MouseResetBranch::DiskSpace),
                "host DPMI 0100/0101 requires original mouse source path");
        state.dpmi_dos_memory=std::make_unique<rloft_recomp::DpmiDosMemory>();
    }
    if(multi_resource_probe)require(host_root!=nullptr,
            "multi-resource source probe requires a host installation root");
    if(host_root) {
        require(advance_to_mouse && original_sound_cfg!=nullptr,
                "host DOS file mode needs explicit 59-byte comparison input");
        state.host_files=std::make_unique<rloft_recomp::ReadOnlyDosFiles>(*host_root);
    }
    if(original_sound_cfg) {
        require((original || host_root) && advance_to_mouse,
                "original or host SOUND.CFG requires mouse-boundary mode");
        require(original_sound_cfg->size()==state.audio_file_fixture.size(),
                "original SOUND.CFG must contain exactly 59 bytes");
        std::copy(original_sound_cfg->begin(),original_sound_cfg->end(),
                  state.audio_file_fixture.begin());
    } else if(advance_to_mouse) {
        // Explicit synthetic fallback; never label this as original media.
        for(std::size_t i=0;i<state.audio_file_fixture.size();++i)
            state.audio_file_fixture[i]=static_cast<u8>((0x31u+3u*i)&0xFFu);
    }
    state.sound_no_arguments=sound_no_arguments;
    state.game_config=game_config;
    // Original DOS interrupt vectors are external system state. The test
    // fixture assigns three distinct sentinel pre-existing vectors, which
    // canonical 30BB4 must save exactly before replacing them.
    state.ivt[9u]={0x00009009u,0x0020u};
    state.ivt[8u]={0x00009008u,0x0020u};
    state.ivt[0u]={0x00009000u,0x0020u};
    state.data=img.objects[1].raw;
    state.code=img.objects[0].raw;
    state.psp[0x2c]=0x2c;state.psp[0x2d]=0;
    // PSP:80h command tail is four bytes ("  /?") and a DOS carriage return.
    state.psp[0x80]=4;
    state.psp[0x81]=' ';state.psp[0x82]=' ';
    state.psp[0x83]='/';state.psp[0x84]='?';
    state.psp[0x85]=13;
    if(advance_to_sound_file) {
        if(sound_no_arguments) {
            // Original CODE.2 3968E parses zero extra DOS argv arguments
            // and 2F2EC follows its checked allocator/zero-fill path.
            state.psp[0x80]=0u;
            state.psp[0x81]=13;
        } else {
            // A decimal guest pointer supplied as an explicit DOS launcher
            // fixture: original 3968E parses it, original 35349 converts it.
            // 186496 decimal is DGROUP:2D880; this descriptor is TEST-ONLY.
            constexpr char numeric_arg[]=" 186496";
            state.psp[0x80]=static_cast<u8>(sizeof(numeric_arg)-1u);
            std::memcpy(state.psp.data()+0x81,numeric_arg,
                        sizeof(numeric_arg)-1u);
            state.psp[0x81+sizeof(numeric_arg)-1u]=13;
            // Guest audio descriptor storage is outside the bounded allocator.
            // Original LE media are not rewritten on disk.
            std::fill_n(state.data.begin()+0x2D880u,0x80u,0u);
        }
    }
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
    state.game.call_35286=&original_disk_free_from_35286;
    state.entry.int21_36=&stop_at_dos_disk_free;
    state.game_audio.call_375fe_far=&original_far_install;
    state.dos_irq.install_interrupt=&original_vector_install;
    state.keyboard_irq.get_vector=&guest_get_vector;
    state.keyboard_irq.set_vector=&guest_set_vector;
    state.audio_seed.call_30cc4=&original_random_seed;
    state.game_audio.call_30bb4=&original_keyboard_vectors;
    state.game_audio.app56=&state.audio_seed;
    state.game_audio.call_3104c=&original_checked_allocation;
    state.game_audio.call_30d30=&original_checked_memset;
    state.game_audio.call_312a0=&original_sound_file;
    state.game_audio.call_35349=&original_sound_pointer_from_argv;
    state.dos_file_io.call_37b34=&original_checked_allocation_header;
    state.dos_file_io.call_379d0=&original_linear_dword_fill;
    state.dos_file_io.call_37e77=&original_checked_source_open;
    state.dos_file_open.read8=&original_dos_open_guest_read8;
    state.dos_file_open.dos_open=&stop_at_real_dos_ah3d_open;
    state.dos_file_open.descriptor_flags=&original_read_descriptor_flags;
    state.dos_file_open.descriptor_is_device=&original_descriptor_is_device;
    state.dos_file_open.descriptor_publish=&original_publish_descriptor_flags;
    state.dos_file_read.descriptor_flags=&original_read_descriptor_flags;
    state.dos_file_read.dos_read=&original_dos_read_fixture;
    state.dos_file_close.descriptor_close=&original_dos_close_fixture;
    state.dos_file_io.call_38106=&original_binary_read;
    state.dos_file_io.call_380f8=&original_source_descriptor_close;
    state.game_audio.call_312ec=&original_sound_config_read;
    state.game_audio.call_312dc=&original_sound_config_close;
    state.mouse_init.int33_reset=&controlled_mouse_reset;
    state.mouse_init.call_30d80=&original_mouse_memory_via_30d80;
    state.mouse_init.call_33aa0=&original_mouse_33aa0;
    state.mouse_init.call_30de8=&original_mouse_free_via_30de8;
    state.mouse_range.call_2e340=&source_mouse_present_2e340;
    state.mouse_range.int33=&interrupt_mouse_range_boundary;
    state.game_audio.call_33d18=&original_mouse_range_x;
    state.game_audio.call_33d38=&original_mouse_range_y;
    state.dos_file_io.call_37a73=&original_dpmi_register_context;
    state.dos_file_io.call_33a7c=&stop_at_original_dpmi31;
    state.mouse_init.call_311e8=&original_mouse_warning_via_311e8;
    state.dos_file_io.dos_ah09=&stop_at_dos_mouse_warning_ah09;
    state.game_audio.call_33b44=&original_mouse_initialization;
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
            state.dpmi_calls==(mouse_branch==MouseResetBranch::Present ||
                              mouse_branch==MouseResetBranch::MemoryHandoff?1u:
                              mouse_branch==MouseResetBranch::FreeBoundary ||
                              mouse_branch==MouseResetBranch::RangeX ||
                              mouse_branch==MouseResetBranch::RangeY ||
                              mouse_branch==MouseResetBranch::DiskSpace?2u:0u),
            "DOS service sequence differs from bounded original source");
    require(state.machine.cpu.interrupt_enable,"STI was not executed");
    require(code2::load_le16(state.code.data()+0x35675u)==0x30u,
            "source startup did not save original DS through CODE selector");
    require(state.data[0x1266bu]==6u && state.data[0x1266cu]==22u,
            "original DOS version writes not observed");
    require(code2::load_le16(state.data.data()+0x12660u)==1u,
            "source environment NO87 scan mismatch");
    if(advance_to_sound_file) {
        if(sound_no_arguments) {
            require(state.data[0x2BD10u]==0u,
                    "original empty DOS command-tail staging differs");
        } else {
            require(std::memcmp(state.data.data()+0x2BD10u,
                                "186496",7u)==0,
                    "original numeric DOS command-tail staging differs");
        }
    } else {
        require(state.data[0x2BD10u]=='/' && state.data[0x2BD11u]=='?',
                "original PSP command-tail whitespace handling mismatch");
    }
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
        const char* required_stop=advance_to_mouse
            ? mouse_branch==MouseResetBranch::DiskSpace
                ? "STOP_BEFORE_DOS_INT21_AH36_DISK_FREE"
                : mouse_branch==MouseResetBranch::RangeY
                    ? "STOP_BEFORE_DOS_INT33_Y_RANGE"
                : mouse_branch==MouseResetBranch::RangeX
                    ? "STOP_BEFORE_DOS_INT33_X_RANGE"
                : mouse_branch==MouseResetBranch::FreeBoundary
                    ? "STOP_BEFORE_DOS_INT31_0101_MOUSE_FREE"
                : mouse_branch==MouseResetBranch::MemoryHandoff
                    ? "STOP_BEFORE_DOS_INT31_0300_MOUSE_SIMULATION"
                : mouse_branch==MouseResetBranch::Present
                    ? "STOP_BEFORE_DOS_INT31_0100_ALLOCATION"
                : mouse_branch==MouseResetBranch::Absent
                    ? "STOP_BEFORE_DOS_INT21_AH09_MOUSE_WARNING"
                    : "STOP_BEFORE_DOS_INT33_MOUSE_RESET"
            : advance_to_sound_file ? "STOP_BEFORE_ORIGINAL_DOS_AH3D_OPEN"
            : execute_application ? "STOP_BEFORE_ORIGINAL_DOS_VECTOR_INSTALL_375FE"
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
        const bool source_audio_allocates=
            advance_to_sound_file && sound_no_arguments;
        if(state.stream_nodes!=5u || state.allocation_calls!=8u ||
           state.aligned_allocations!=state.allocation_calls+
               (source_audio_allocates?1u:0u)) {
            std::cerr<<"SOURCE_ALLOCATOR_ACCOUNTING_DIAGNOSTIC"
                     <<" stop="<<state.stop_reason
                     <<" stream_nodes="<<state.stream_nodes
                     <<" allocation_calls="<<state.allocation_calls
                     <<" aligned_allocations="<<state.aligned_allocations
                     <<" argc="<<state.game_argc
                     <<" original_argc="<<code2::load_le32(
                         state.data.data()+0x105FCu)
                     <<" checked_alloc_calls="<<state.checked_alloc_calls
                     <<" file_open_calls="<<state.file_open_calls
                     <<" original_375fe_callback="<<state.dos_vector_callbacks
                     <<'\n';
            fail("five original stream descriptors plus env/table/argv allocations not observed");
        }
        require(state.data[0xF73Du]==0x04u &&
                code2::load_le32(state.data.data()+0x2B74Cu)==0u,
                "original stream initializer state mismatch");
        const u32 expected_argc=sound_no_arguments?1u:2u;
        require(state.game_argc==expected_argc && state.game_argv!=0u &&
                code2::load_le32(state.data.data()+0x2BD00u)==expected_argc &&
                code2::load_le32(state.data.data()+0x2BD04u)==state.game_argv &&
                code2::load_le32(state.data.data()+0x105FCu)==expected_argc,
                "original 3968E argc/argv publication differs");
        const u32 argv1=code2::load_le32(state.data.data()+state.game_argv+4u);
        if(sound_no_arguments) {
            require(argv1==0u,"original argc=1 must terminate argv");
        } else {
            require(argv1>=0x2C000u && argv1<0x2D500u &&
                    std::memcmp(state.data.data()+argv1,
                                advance_to_sound_file?"186496":"/?",
                                advance_to_sound_file?7u:3u)==0,
                    "original DOS argv text differs");
            require(code2::load_le32(state.data.data()+state.game_argv+8u)==0u,
                    "original argc=2 argv terminator missing");
        }
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
                    state.dos_vector_callbacks==1u &&
                    code2::load_le32(state.data.data()+0x105F2u)==0x2F2E0u &&
                    code2::load_le16(state.data.data()+0x105F6u)==
                        state.machine.cpu.cs &&
                    state.vector_handler==0x37642u &&
                    state.vector_number==
                        (state.data[0x12662u]>=2u &&
                         state.data[0x12662u]<=6u?6u:0x24u) &&
                    state.vector_selector==
                        (state.vector_number==6u?0x24u:0u) &&
                    state.active_sound_base==0x1130u &&
                    state.active_sound_count==0x0Bu,
                    "real CODE.2 getenv/atoi/resource/ISR callback chain differs");
            if(advance_to_sound_file) {
                require(state.irq_vector_reads==3u && state.irq_vector_sets==3u &&
                        state.ivt[9u].offset==0x308F8u &&
                        state.ivt[8u].offset==0x30708u &&
                        state.ivt[0u].offset==0x30B54u &&
                        state.ivt[9u].selector==state.machine.cpu.cs &&
                        state.ivt[8u].selector==state.machine.cpu.cs &&
                        state.ivt[0u].selector==state.machine.cpu.cs &&
                        state.data[0xF575u]==1u,
                        "original 30BB4 vector save/install chain not executed");
                require(rd32(0xF576u)==0x9009u &&
                        rd16(0xF57Au)==0x20u &&
                        rd32(0xF582u)==0x9008u &&
                        rd16(0xF586u)==0x20u &&
                        rd32(0xF57Cu)==0x9000u &&
                        rd16(0xF580u)==0x20u,
                        "original keyboard/PIT vector backups were not written");
                require(state.seed_calls==1u &&
                        rd32(0xF6F4u)==rd16(0x046Cu),
                        "canonical 290D0 -> 30CC4 RNG seed path was not executed");
                const u32 env=rd32(0xF050u);
                if(sound_no_arguments) {
                    require(state.checked_alloc_calls==1u &&
                            state.checked_fill_calls==1u &&
                            state.audio_atoi_calls==0u &&
                            env>=state.arena_start+0x0Au &&
                            env<state.arena_end-0x100u &&
                            rd32(env-4u)==0x12345678u &&
                            state.data[env+0x64u]==(advance_to_mouse?1u:0u),
                            "canonical argc=1 checked source audio buffer init mismatch");
                } else {
                    require(state.checked_alloc_calls==0u &&
                            state.checked_fill_calls==0u &&
                            state.audio_atoi_calls==1u &&
                            state.audio_atoi_text==argv1 &&
                            env==0x2D880u &&
                            state.data[env+0x64u]==(advance_to_mouse?1u:0u),
                            "canonical argc=2 original argv audio descriptor path differs");
                }
                require(state.file_open_calls==1u &&
                        state.sound_config_path==0x143Cu &&
                        state.sound_config_mode==0x200u &&
                        state.dos_ah3d_open_calls==1u &&
                        state.open_access_share==0u,
                        "original 312A0->37E77->37E99 DOS AH=3D open not executed");
                if(advance_to_mouse) {
                    const auto base=rd32(0xF050u);
                    // Original 312EC reads *0x3B bytes directly into env*,
                    // not env+0x3B. Source 2F2EC then sets env+0x64 and
                    // env+0x66, both OUTSIDE the 59-byte file image.
                    // Verify all 59 transported bytes exactly, and verify
                    // the two later original application-state writes below.
                    const bool untouched_file_bytes_match=
                        std::memcmp(state.data.data()+base,
                                    state.audio_file_fixture.data(),
                                    state.audio_file_fixture.size())==0;
                    const bool mouse_source_ok=
                        state.audio_published_flags==0x41u &&
                        state.audio_descriptor_flags==0u &&
                        state.dos_file_reads==1u &&
                        state.dos_file_closes==1u &&
                        state.audio_file_cursor==59u &&
                        state.mouse_reset_calls==1u &&
                        state.data[base+0x64u]==1u &&
                        state.data[base+0x66u]==1u &&
                        untouched_file_bytes_match;
                    if(!mouse_source_ok) {
                        std::cerr<<"ORIGINAL_MOUSE_BOUNDARY_DIAGNOSTIC"
                                 <<" stop="<<state.stop_reason
                                 <<" descriptor_flags=0x"<<std::hex
                                 <<state.audio_published_flags
                                 <<" final_flags="<<state.audio_descriptor_flags
                                 <<" loaded_base="<<base<<std::dec
                                 <<" reads="<<state.dos_file_reads
                                 <<" closes="<<state.dos_file_closes
                                 <<" cursor="<<state.audio_file_cursor
                                 <<" mouse="<<state.mouse_reset_calls
                                 <<" overwrites64="<<unsigned(state.data[base+0x64u])
                                 <<" overwrites66="<<unsigned(state.data[base+0x66u])
                                 <<" other_bytes="<<untouched_file_bytes_match<<'\n';
                    }
                    require(mouse_source_ok,
                            "original 37E99/312EC/38106/312DC/380F8/33B44 source chain differs");
                    if(mouse_branch!=MouseResetBranch::Stop) {
                        const u16 present=mouse_branch==MouseResetBranch::Absent?0u:0xFFFFu;
                        const u16 buttons=mouse_branch==MouseResetBranch::Absent?0u:3u;
                        require(rd16(0x2B7A2u)==present &&
                                rd16(0x2B7A4u)==buttons &&
                                rd16(0x2B7A6u)==0u,
                                "canonical 33B44 source register stores differ");
                        if(mouse_branch==MouseResetBranch::MemoryHandoff ||
                           mouse_branch==MouseResetBranch::FreeBoundary ||
                           mouse_branch==MouseResetBranch::RangeX ||
                           mouse_branch==MouseResetBranch::RangeY ||
                           mouse_branch==MouseResetBranch::DiskSpace) {
                            require(state.dpmi_mouse_simulation_calls==1u &&
                                    rd32(0xFD14u)==0x2D600000u &&
                                    rd16(0xFD18u)==0x0060u &&
                                    rd32(0xFD10u)==0x2D600u &&
                                    rd32(0x2B770u+0x1Cu)==0x53C1u &&
                                    rd16(0x2B770u+0x22u)==0x2D60u,
                                "canonical 30D80/33AA0 DPMI handoff differs");
                            require(state.dpmi_mouse_free_calls==
                                (mouse_branch==MouseResetBranch::MemoryHandoff?0u:1u),
                                "original DPMI mouse release count changed");
                            require(state.mouse_range_calls==
                                (mouse_branch==MouseResetBranch::RangeX?1u:
                                 mouse_branch==MouseResetBranch::RangeY ||
                                 mouse_branch==MouseResetBranch::DiskSpace?2u:0u),
                                "original mouse coordinate range call sequence differs");
                            require(state.mouse_x_range_returned==
                                (mouse_branch==MouseResetBranch::RangeY ||
                                 mouse_branch==MouseResetBranch::DiskSpace?1u:0u),
                                "test-only mouse X range handoff changed");
                        } else require(state.dpmi_mouse_simulation_calls==0u &&
                                       state.dpmi_mouse_free_calls==0u &&
                                       state.mouse_range_calls==0u,
                            "unexpected DPMI real-mode mouse simulation");
                    }
                    require(state.disk_space_calls==
                        (mouse_branch==MouseResetBranch::DiskSpace?1u:0u),
                        "source DOS disk free query unexpectedly bypassed");
                }
            }
        }
        require(state.arena_seeded &&
                code2::load_le32(state.data.data()+0x12624u)==state.arena_header &&
                code2::load_le32(state.data.data()+state.arena_header+0x18u)==
                    (source_audio_allocates?9u:8u) &&
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
             <<"SOURCE_GAME_AUDIO_INIT_2F2EC="<<(advance_to_sound_file?(advance_to_mouse?"EXECUTED_37E77_38106_380F8_TO_INT33":"EXECUTED_37E77_TO_DOS_AH3D"):execute_application?"EXECUTED_TO_ORIGINAL_375FE":"NOT_EXECUTED")<<"\n"
             <<"ORIGINAL_GETENV_352EE_CALLS="<<state.getenv_calls<<"\n"
             <<"ORIGINAL_ATOI_35349_CALLS="<<state.atoi_calls<<"\n"
             <<"EXECUTION_BOUNDARY="<<state.stop_reason<<"\n"
             <<"ORIGINAL_DOS_PARITY=NO\nGAME_BOOTED=NO\n"
             <<"IMAGE_INPUT="<<(original?"ORIGINAL_LE_GEOMETRY_VERIFIED":"SYNTHETIC_FIXTURE")<<"\n"
             <<"SOUND_CFG_INPUT="<<(host_root?"HOST_READ_ONLY_SERVICE":
                   original_sound_cfg?"ORIGINAL_FILE":"SYNTHETIC_FIXTURE")<<'\n';
    if(multi_resource_probe) {
        // Independent source-wrapper postlude AFTER the authentic compiled
        // startup reaches its real external mouse boundary. Not a claim
        // that original game flow naturally opens this second asset here.
        constexpr u32 path=0x2D900u, buf1=0x2DA00u,buf2=0x2DA80u;
        constexpr char name[]="objects.dat";
        // Independent expected host bytes: works with owner-original
        // OBJECTS.DAT as well as CI's generated synthetic fixture.
        const auto reference=read_file(((*host_root)/"OBJECTS.DAT").string().c_str());
        require(reference.size()>=50u,"OBJECTS.DAT too short for source probe");
        std::memcpy(state.data.data()+path,name,sizeof(name));
        current=&state;
        const auto fd1=code2::function_37e77(
            state.dos_file_open,path,0x200u,0xFFFFFFFFu);
        const auto fd2=code2::function_37e77(
            state.dos_file_open,path,0x200u,0xFFFFFFFFu);
        require(fd1==5 && fd2==6,
                "original 37E77 two-descriptor source open failed");
        require(code2::function_38106(state.dos_file_read,5u,buf1,17u)==17,
                "original 38106 first descriptor partial read failed");
        require(code2::function_38106(state.dos_file_read,6u,buf2,50u)==50,
                "original 38106 independent descriptor read failed");
        const auto pos1=state.host_files->seek(5u,-17,1u);
        const auto pos2=state.host_files->seek(6u,0,1u);
        require(!pos1.carry && pos1.offset()==0u &&
                !pos2.carry && pos2.offset()==50u,
                "host AH42 cursor independence after original reads failed");
        require(code2::function_38106(state.dos_file_read,5u,buf1,32u)==32,
                "original 38106 read after host seek failed");
        for(unsigned i=0;i<32u;++i)
            require(state.data[buf1+i]==reference[i],
                    "first original CODE.2 multi-file read bytes differ");
        for(unsigned i=0;i<50u;++i)
            require(state.data[buf2+i]==reference[i],
                    "second original CODE.2 multi-file read bytes differ");
        (void)code2::function_380f8(state.dos_file_close,5u);
        (void)code2::function_380f8(state.dos_file_close,6u);
        current=nullptr;
        std::cout<<"ORIGINAL_CODE2_MULTIFILE_WRAPPERS=37E77_38106_380F8\\n"
                    "SOURCE_HOST_INDEPENDENT_DESCRIPTORS=2\\n"
                    "SOURCE_HOST_SEEK_AFTER_READ=PASS\\n";
    }
    if(host_root)require(state.host_files->open_handles()==0u &&
                         state.host_descriptor_flags.empty(),
                         "host DOS descriptors remained open");
    if(use_host_dpmi) {
        require(state.dpmi_dos_memory->allocation_count()==
                (mouse_branch==MouseResetBranch::MemoryHandoff?1u:0u),
                "actual DOS-memory descriptor state differs after original code");
        // This descriptor was created by host-owned 0100 through original
        // CODE.2 and must vanish after source 30DE8 -> 0101 on DiskSpace.
        const auto live_base=state.dpmi_dos_memory->selector_base(0x0060u);
        require(mouse_branch==MouseResetBranch::MemoryHandoff?
                    (!live_base.carry && live_base.base()==0x0002d600u):
                    (live_base.carry && live_base.ax==0x8022u),
                "original CODE.2 DPMI selector base/lifetime mismatch");
        std::cout<<"HOST_DPMI_DOS_MEMORY_0100=TRACKED_SERVICE\\n"
                    "HOST_DPMI_DOS_MEMORY_0101="
                 <<(mouse_branch==MouseResetBranch::DiskSpace?
                    "RELEASED":"NOT_REACHED")<<"\\n";
    }

}
}
int main(int argc,char**argv) {
    try {
        bool extended=false,termination=false,arena_probe=false,failure_probe=false,vector_matrix=false;
        bool execute_application=false,advance_to_sound_file=false,
             sound_no_arguments=false,advance_to_mouse=false,
             host_sound_file=false,host_dpmi=false,multi_host_resource=false;
        MouseResetBranch mouse_branch=MouseResetBranch::Stop;
        GameConfig game_config=GameConfig::Unset;
        int original_index=1;
        if(argc>=2) {
            const std::string mode(argv[1]);
            if(mode=="--through-initializers" || mode=="--termination-probe" ||
               mode=="--arena-roundtrip-probe" || mode=="--env-rollback-probe" ||
               mode=="--enter-game" || mode=="--enter-game-config" ||
               mode=="--enter-game-signed-wrap" || mode=="--vector-mode-matrix" ||
               mode=="--to-sound-config" || mode=="--to-sound-config-configured" ||
               mode=="--to-sound-config-no-args" ||
               mode=="--to-sound-config-no-args-configured" ||
               mode=="--to-mouse-reset" ||
               mode=="--to-mouse-reset-host-file" ||
               mode=="--to-mouse-reset-host-multifile" ||
               mode=="--to-mouse-reset-configured" ||
               mode=="--to-mouse-reset-no-args" ||
               mode=="--to-mouse-reset-no-args-configured" ||
               mode=="--mouse-dpmi-to-real-mode" ||
               mode=="--mouse-dpmi-to-disk-query" ||
               mode=="--mouse-present-to-memory" ||
               mode=="--mouse-present-to-real-mode" ||
               mode=="--mouse-no-driver-to-dpmi-free" ||
               mode=="--mouse-no-driver-to-x-range" ||
               mode=="--mouse-no-driver-to-y-range" ||
               mode=="--mouse-no-driver-to-disk-query" ||
               mode=="--mouse-absent-to-warning") {
                advance_to_mouse=(mode=="--to-mouse-reset-host-file" ||
                    mode=="--to-mouse-reset-host-multifile" ||
                    mode=="--mouse-no-driver-to-dpmi-free" ||
                    mode=="--mouse-no-driver-to-x-range" ||
                    mode=="--mouse-no-driver-to-y-range" ||
                    mode=="--mouse-no-driver-to-disk-query" ||
                    mode=="--mouse-dpmi-to-real-mode" ||
                    mode=="--mouse-dpmi-to-disk-query" ||
                    mode=="--mouse-present-to-real-mode" ||
                    mode=="--mouse-present-to-memory" ||
                    mode=="--mouse-absent-to-warning" ||
                    mode=="--to-mouse-reset" ||
                    mode=="--to-mouse-reset-configured" ||
                    mode=="--to-mouse-reset-no-args" ||
                    mode=="--to-mouse-reset-no-args-configured");
                advance_to_sound_file=(mode=="--to-sound-config" ||
                    mode=="--to-sound-config-configured" ||
                    mode=="--to-sound-config-no-args" ||
                    mode=="--to-sound-config-no-args-configured" ||
                    advance_to_mouse);
                sound_no_arguments=(mode=="--to-sound-config-no-args" ||
                    mode=="--to-sound-config-no-args-configured" ||
                    mode=="--to-mouse-reset-no-args" ||
                    mode=="--to-mouse-reset-no-args-configured");
                execute_application=(mode=="--enter-game" ||
                    mode=="--enter-game-config" || mode=="--enter-game-signed-wrap" ||
                    advance_to_sound_file);
                if(mode=="--enter-game-config" ||
                   mode=="--to-sound-config-configured" ||
                   mode=="--to-sound-config-no-args-configured" ||
                   mode=="--to-mouse-reset-configured" ||
                   mode=="--to-mouse-reset-no-args-configured")
                    game_config=GameConfig::Overrides;
                if(mode=="--enter-game-signed-wrap")game_config=GameConfig::SignedWrap;
                if(mode=="--mouse-present-to-memory")mouse_branch=MouseResetBranch::Present;
                if(mode=="--mouse-present-to-real-mode" ||
                   mode=="--mouse-dpmi-to-real-mode")mouse_branch=MouseResetBranch::MemoryHandoff;
                if(mode=="--mouse-no-driver-to-dpmi-free")mouse_branch=MouseResetBranch::FreeBoundary;
                if(mode=="--mouse-no-driver-to-x-range")mouse_branch=MouseResetBranch::RangeX;
                if(mode=="--mouse-no-driver-to-y-range")mouse_branch=MouseResetBranch::RangeY;
                if(mode=="--mouse-no-driver-to-disk-query" ||
                   mode=="--mouse-dpmi-to-disk-query")mouse_branch=MouseResetBranch::DiskSpace;
                host_dpmi=(mode=="--mouse-dpmi-to-real-mode" ||
                           mode=="--mouse-dpmi-to-disk-query");
                if(mode=="--mouse-absent-to-warning")mouse_branch=MouseResetBranch::Absent;
                host_sound_file=(mode=="--to-mouse-reset-host-file" ||
                                 mode=="--to-mouse-reset-host-multifile");
                multi_host_resource=(mode=="--to-mouse-reset-host-multifile");
                extended=(mode=="--through-initializers" || execute_application);
                termination=(mode=="--termination-probe");
                arena_probe=(mode=="--arena-roundtrip-probe");
                failure_probe=(mode=="--env-rollback-probe");
                vector_matrix=(mode=="--vector-mode-matrix");
                original_index=2;
            }
        }
        require(argc>=original_index && argc<=original_index+2,
                "usage: rloft_code2_startup_gate [mode] [CODE.2 [SOUND.CFG]]");
        if(argc==original_index+2)
            require(advance_to_mouse && !termination && !arena_probe &&
                    !failure_probe && !vector_matrix,
                    "original SOUND.CFG only valid for original mouse-reset source modes");
        if(host_sound_file) {
            require(argc==original_index+1,
                    "usage: --to-mouse-reset-host-file INSTALLATION_ROOT");
            const std::filesystem::path root(argv[original_index]);
            // Read a separate expected 59-byte snapshot solely for checking
            // original compiled source's DGROUP writes. Actual AH=3D/3F/3E
            // callbacks use ReadOnlyDosFiles, not this copy.
            const auto expected=read_file((root/"SOUND.CFG").string().c_str());
            require(expected.size()==59u,"host SOUND.CFG must contain 59 bytes");
            run(synthetic_image(),false,true,true,GameConfig::Unset,
                true,false,true,&expected,MouseResetBranch::Stop,&root,
                false,multi_host_resource);
        } else if(vector_matrix) {
            require(argc==original_index,"vector-mode probe does not use original data");
            interrupt_vector_matrix_probe();
        }else if(arena_probe || failure_probe) {
            require(argc==original_index,"allocator probes use synthetic-only guest memory");
            if(arena_probe)arena_split_coalesce_probe();
            else environment_alloc_failure_probe();
        }else if(argc==original_index) {
            const auto fixture=synthetic_image();
            if(termination)termination_probe(fixture,false);
            else run(fixture,false,extended,execute_application,game_config,
                     advance_to_sound_file,sound_no_arguments,advance_to_mouse,
                     nullptr,mouse_branch,nullptr,host_dpmi);
        }else{
            const auto b=read_file(argv[original_index]);
            require(b.size()==558689u,"original CODE.2 size is incorrect");
            const auto image=rloft_recomp::OriginalLeImage::parse(b);
            std::vector<u8> sound_bytes;
            const std::vector<u8>* sound_ptr=nullptr;
            if(argc==original_index+2) {
                sound_bytes=read_file(argv[original_index+1]);
                require(sound_bytes.size()==59u,
                        "original SOUND.CFG must contain exactly 59 bytes");
                sound_ptr=&sound_bytes;
            }
            if(termination)termination_probe(image,true);
            else run(image,true,extended,execute_application,game_config,
                     advance_to_sound_file,sound_no_arguments,
                     advance_to_mouse,sound_ptr,mouse_branch,nullptr,host_dpmi);
        }
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_STARTUP_GATE_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
