// Bounded execution of the *canonical* CODE.2 DOS startup, not an emulated game.
// The only synthetic pieces are explicitly identified DOS/PSP/environment fixtures.
// Every unsupported service stops execution before the original code can consume a
// fabricated result. Source: original CODE.2 353A0 -> 35418 -> 395E2.
#include "../../Code2/Code2.cpp"
#include "original_le_image.h"
#include "original_media_identity.hpp"
#include "canonical_code2_runtime_owner.hpp"
#include "original_guest_continuation.hpp"
#include "original_code2_video_boot.hpp"
#include "original_code2_5e38_game_options.hpp"
#include "original_dpmi_real_mode_mouse.hpp"
#include "host_dos_disk_space.hpp"
#include "dos_readonly_files.hpp"
#include "dpmi_dos_memory.hpp"
#include "dpmi_linear_memory.hpp"
#include "dpmi_selector_descriptors.hpp"
#include "dos_paragraph_memory.hpp"
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
    rloft::guest_continuation::Worker* live_continuation=nullptr;
    std::jmp_buf escape;
    // A borrowed host heap call uses its own live setjmp frame; it must
    // never overwrite the suspended CODE2 worker's escape target.
    std::jmp_buf* active_escape=&escape;
    std::vector<u8> data, code;
    std::array<u8,256> psp{};
    std::array<u8,512> env{};
    code2::Machine machine{};
    code2::Runtime111Boundary entry{};
    code2::Application46Boundary game{};
    code2::Application45Boundary presentation{};
    rloft::graphics::OriginalVgaHost* original_vga=nullptr;
    unsigned original_source_video_modes=0;
    unsigned original_source_dac_streams=0;
    unsigned original_source_page_pushes=0;
    unsigned original_source_page_pops=0;
    unsigned original_source_page_selects=0;
    unsigned original_source_planar_fills=0;
    unsigned original_5e38_calls=0;
    unsigned original_5e38_absent=0;
    unsigned original_28f58_calls=0;
    unsigned original_31218_checks=0;
    unsigned original_312a0_calls=0;
    unsigned original_16d04_entries=0;
    unsigned original_16d04_completed=0;
    unsigned original_03838_completed=0;
    unsigned original_16cd0_completed=0;
    unsigned original_181b4_completed=0;
    unsigned original_325b0_completed=0;
    unsigned original_boot_output_page_selects=0;
    unsigned original_boot_scanout_page_selects=0;
    unsigned original_20440_res_image_loads=0;
    unsigned original_20440_res_image_retains=0;
    unsigned original_20440_mouse_inits=0;
    unsigned original_20440_mouse_cursor_loads=0;
    unsigned original_20440_mouse_capture_calls=0;
    unsigned original_20440_mouse_callback_installs=0;
    unsigned original_20440_mouse_draws=0;
    bool original_20440_mouse_started=false;
    code2::Application33Boundary original_mouse_resources{};
    code2::Application34Boundary original_mouse_init_callbacks{};
    code2::Application35Boundary original_mouse_draw_callbacks{};
    code2::Application30Boundary* original_16d04_callbacks=nullptr;
    std::filesystem::path original_installation_root;
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
    std::unique_ptr<rloft_recomp::DpmiLinearMemory> dpmi_linear_memory;
    std::unique_ptr<rloft_recomp::DosParagraphMemory> dos_heap_memory;
    bool source_dos_fallback=false,source_break_growth=false;
    u32 source_break_end=0x30000u;
    unsigned source_break_resizes=0,source_break_allocs=0;
    unsigned source_dos_requests=0,source_dos_releases=0;
    unsigned source_dpmi_denials=0;
    rloft_recomp::DpmiSelectorDescriptors host_startup_descriptors;
    code2::Runtime142Boundary heap_provider{};
    code2::Runtime39d4dBoundary heap_growth{};
    bool source_host_heap=false;
    unsigned host_heap_allocs=0,host_heap_growths=0,host_heap_releases=0;

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
thread_local bool tracing_original_136_byte_heap=false;
[[noreturn]] void stop(const char* reason) noexcept {
    current->stop_reason=reason;
    std::longjmp(*current->active_escape,1);
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
        if(current->source_break_growth) {
            m.cpu.eax=0x44581606u; // Phar Lap extender identity
            m.cpu.ebx=(m.cpu.ebx&0xFFFFFF00u)|0x32u; // BL='2' -> mode 2
        } else m.cpu.eax=0x00001606u;  // DOS 6.22 fixture
    }else if(current->source_break_growth &&
              (ax&0xFF00u)==0x4A00u) {
        // Original Phar Lap 35418: AH=4Ah requests BX 4KiB pages,
        // not standard DOS AH=4Ah paragraph-resize semantics.
        auto& s=*current;
        if(static_cast<u16>(m.cpu.es)!=0x30u ||
           (static_cast<u32>(static_cast<u16>(m.cpu.ebx))<<12u)!=
             s.source_break_end)
            stop("SOURCE_BREAK_STARTUP_AH4A_INVALID_RESIZE");
        ++s.source_break_resizes;
        m.cpu.eax&=0xFFFF00FFu;
    }else if(ax==0xFF00u) {
        ++current->ff00_calls;
        // R04 host-DPMI scenario advertises a real 0006 selector-base
        // service. All historical tests retain the plain-DOS result.
        m.cpu.eax=current->source_host_heap?1u:0u;
    }else stop("UNSUPPORTED_DOS_INT21");
}
void dpmi31(code2::Machine& m) noexcept {
    ++current->dpmi_calls;
    if(current->source_host_heap &&
       static_cast<u16>(m.cpu.eax)==0x0006u) {
        const auto r=current->host_startup_descriptors.get_base(
            static_cast<u16>(m.cpu.ebx));
        if(r.carry)stop("SOURCE_STARTUP_DPMI_0006_INVALID_SELECTOR");
        m.cpu.ecx=(m.cpu.ecx&0xFFFF0000u)|r.cx;
        m.cpu.edx=(m.cpu.edx&0xFFFF0000u)|r.dx;
        return;
    }
    stop("UNSUPPORTED_DPMI_INT31");
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
// R04 live host-backed heap mode: the original source owns all arena headers,
// allocations, free-list splits and coalescing. The host supplies only a
// checked 0501 memory service and bounded arena-chain dispatch.
code2::Runtime142Boundary::Dpmi0501Result host_heap_0501(u32 bytes) noexcept {
    auto& s=*current;
    // Explicit 0501 failure drives the original CODE.2 39C7B DOS fallback.
    if(s.source_dos_fallback) {
        ++s.source_dpmi_denials;
        return {true,0u,0u,0u};
    }
    const auto r=s.dpmi_linear_memory->allocate(bytes);
    if(r.carry)return {true,0u,0u,0u};
    const auto* b=s.dpmi_linear_memory->block(r.handle);
    if(!b || s.dpmi_linear_memory->translate(r.linear,b->committed)!=
             s.data.data()+r.linear)
        stop("SOURCE_HEAP_DPMI_PAGE_MAPPING_FAILED");
    ++s.host_heap_allocs;
    return {false,r.linear,static_cast<u16>(r.handle>>16u),
            static_cast<u16>(r.handle)};
}
void host_heap_0502(u32 handle) noexcept {
    if(current->dpmi_linear_memory->release(handle).carry)
        stop("SOURCE_HEAP_DPMI_FREE_INVALID_HANDLE");
    ++current->host_heap_releases;
}
// Conventional DOS guest paragraphs map to their own verified aperture,
// never over the original static DGROUP. Selectors and release are stateful.
constexpr u32 kHostDosAlias=0x300000u;
std::int32_t host_heap_dos_allocate(u16 paragraphs) noexcept {
    auto& s=*current;
    if(!s.source_dos_fallback || !s.dos_heap_memory)
        stop("SOURCE_HEAP_DOS_FALLBACK_NOT_CONFIGURED");
    ++s.source_dos_requests;
    const auto r=s.dos_heap_memory->allocate(paragraphs);
    if(r.carry)return -1;
    const u32 physical=s.dos_heap_memory->physical_base(r.ax);
    // The original 39C7B wrapper passes the returned DOS identity through
    // selector-base translation. Register it with the SAME table as DS:30h.
    // Using a real-mode segment as a selector remains explicit host policy.
    if(!physical ||
       !s.host_startup_descriptors.install(
           r.ax,kHostDosAlias+physical,static_cast<u32>(paragraphs)*16u)) {
        (void)s.dos_heap_memory->release(r.ax);
        stop("SOURCE_HEAP_DOS_SELECTOR_REGISTRATION_FAILURE");
    }
    return static_cast<std::int32_t>(r.ax);
}
void host_heap_dos_release(u16 selector) noexcept {
    auto& s=*current;
    if(!s.dos_heap_memory ||
       !s.host_startup_descriptors.descriptor(selector) ||
       s.dos_heap_memory->release(selector).carry ||
       !s.host_startup_descriptors.retire(selector))
        stop("SOURCE_HEAP_DOS_RELEASE_INVALID_DESCRIPTOR_OR_BLOCK");
    ++s.source_dos_releases;
}
u32 host_heap_dos_selector_base(u16 selector) noexcept {
    auto& s=*current;
    if(!s.dos_heap_memory)stop("SOURCE_HEAP_DOS_SELECTOR_UNAVAILABLE");
    const u32 physical=s.dos_heap_memory->physical_base(selector);
    const auto base=s.host_startup_descriptors.get_base(selector);
    const auto* block=s.dos_heap_memory->block(selector);
    if(!physical || base.carry || !block ||
       !s.host_startup_descriptors.check(selector,0u,4u) ||
       !s.host_startup_descriptors.check(
           selector,0u,static_cast<u32>(block->paragraphs)*16u) ||
       base.base()!=kHostDosAlias+physical ||
       s.dos_heap_memory->translate(physical,4u)!=s.data.data()+base.base())
        stop("SOURCE_HEAP_DOS_SELECTOR_UNOWNED");
    return base.base();
}
bool host_heap_owned(Session& s,u32 linear,std::size_t size) noexcept {
    if(linear>s.data.size() || size>s.data.size()-linear)return false;
    if(s.source_break_growth)
        return linear>=0x30000u && linear<=s.source_break_end &&
               size<=s.source_break_end-linear;
    if(s.source_dos_fallback) {
        if(linear<kHostDosAlias || !s.dos_heap_memory)return false;
        return s.dos_heap_memory->translate(linear-kHostDosAlias,size)==
               s.data.data()+linear;
    }
    return s.dpmi_linear_memory &&
           s.dpmi_linear_memory->translate(linear,size)==s.data.data()+linear;
}
bool host_heap_owned(u32 linear,std::size_t size) noexcept {
    return host_heap_owned(*current,linear,size);
}
u32 host_heap_39c7b(u32 n) noexcept {
    if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=39C7B_ENTER requested="<<n<<"\n"<<std::flush;
    const auto result=code2::function_39c7b(current->machine,current->heap_provider,n);
    if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=39C7B_RETURN ptr="<<result<<"\n"<<std::flush;
    return result;
}
u32 host_heap_39bbc(u32 n) noexcept {
    if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=39BBC_ENTER offset="<<n<<"\n"<<std::flush;
    const auto result=code2::function_39bbc(current->machine,n);
    if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=39BBC_RETURN value="<<result<<"\n"<<std::flush;
    return result;
}
void host_heap_free(u32 root,u32 payload) noexcept {
    auto& s=*current;
    if(root!=0x12624u || payload<4u || payload>=s.data.size())
        stop("SOURCE_HEAP_FREE_GUEST_BOUNDS");
    for(u32 node=code2::load_le32(s.data.data()+root);node;) {
        const u32 size=code2::load_le32(segment(0x30u,node,4u));
        const u32 next=code2::load_le32(segment(0x30u,node+8u,4u));
        if(size>0x2Cu && payload>=node+0x30u && payload<node+size) {
            if(!host_heap_owned(payload,1u))
                stop("SOURCE_HEAP_FREE_OUTSIDE_TRACKED_GUEST_PAGES");
            code2::function_398e0(s.machine,node,payload);
            ++s.arena_frees;
            return;
        }
        node=next;
    }
    stop("SOURCE_HEAP_FREE_ARENA_MISSING");
}
void host_heap_free_initial(u32 p) noexcept {
    host_heap_free(0x12624u,p);
}
u32 host_break_limit() noexcept {return current->source_break_end-1u;}
u32 host_break_extend(u32 target) noexcept {
    auto& s=*current;
    ++s.source_break_resizes;
    if(!s.source_break_growth || target<=s.source_break_end ||
       target>s.data.size() || target>0x200000u)return 0xFFFFFFFFu;
    const u32 previous=s.source_break_end;
    s.source_break_end=target;
    ++s.source_break_allocs;
    return previous;
}
u32 host_heap_39e1c(u32 n) noexcept {
    ++current->host_heap_growths;
    if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=39E1C_GROW_ENTER bytes="<<n<<"\n"<<std::flush;
    const auto result=code2::function_39e1c(current->machine,current->heap_growth,n);
    if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=39E1C_GROW_RETURN ptr="<<result<<"\n"<<std::flush;
    return result;
}
u32 host_heap_39d4d(u32 n) noexcept {
    ++current->host_heap_growths;
    if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=39D4D_GROW_ENTER bytes="<<n<<"\n"<<std::flush;
    const auto result=code2::function_39d4d(current->machine,current->heap_growth,n);
    if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=39D4D_GROW_RETURN ptr="<<result<<"\n"<<std::flush;
    return result;
}
u32 host_heap_alloc(u32 root,u32 req) noexcept {
    auto& s=*current;
    if(root!=0x12624u || !req || req>0xFFFFFFD4u)
        stop("SOURCE_HEAP_ALLOC_ABI_CHANGED");
    if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=ARENA_ALLOC_ENTER bytes="<<req
                         <<" arena_head="<<code2::load_le32(s.data.data()+root)<<"\n"<<std::flush;
    unsigned visited=0u;
    for(u32 node=code2::load_le32(s.data.data()+root);node;) {
        // Original heap chain cannot be cyclic. Detect damage to the
        // reconstructed archive/guest heap instead of spinning forever.
        if(++visited>4096u)
            stop("SOURCE_HEAP_ARENA_CHAIN_CYCLE_OR_EXCESSIVE_LENGTH");
        const u32 next=code2::load_le32(segment(0x30u,node+8u,4u));
        if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=39838_ENTER node="<<node
                             <<" next="<<next<<"\n"<<std::flush;
        const u32 p=code2::function_39838(s.machine,node,req);
        if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=39838_RETURN ptr="<<p<<"\n"<<std::flush;
        if(p) {
            if(!host_heap_owned(p,req))
                stop("SOURCE_HEAP_ALLOC_OUTSIDE_TRACKED_GUEST_PAGES");
            ++s.aligned_allocations;
            return p;
        }
        node=next;
    }
    return 0u;
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
    if(current->live_continuation) {
        rloft::guest_continuation::Reply answer{};
        const rloft::guest_continuation::Request request{
            rloft::guest_continuation::Service::DosMouseReset,0,0,0,0,0};
        if(!current->live_continuation->suspend(request,answer))
            stop("LIVE_ORIGINAL_CODE2_GUEST_SHUTDOWN");
        // Response originates exclusively from an explicit owner-host
        // DOS Int33 reset implementation; no synthetic success here.
        if(answer.ax!=0u && answer.ax!=0xFFFFu)
            stop("LIVE_ORIGINAL_MOUSE_RESET_INVALID_AX");
        return {answer.ax,answer.bx};
    }
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
        // The same tracked real DPMI DOS memory service used in our
        // original-source tests serves a live guest. No fixed segment
        // or selector is returned to the suspended source execution.
        if(!wants_synthetic_mouse_dpmi_allocation() &&
           !current->live_continuation)
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
           (!wants_synthetic_mouse_dpmi_allocation() &&
            !current->live_continuation) ||
           current->dpmi_mouse_simulation_calls!=1u)
            stop("ORIGINAL_DPMI_0101_RELEASE_CHANGED");
        if(current->mouse_branch==MouseResetBranch::FreeBoundary)
            stop("STOP_BEFORE_DOS_INT31_0101_MOUSE_FREE");
        if(current->mouse_branch!=MouseResetBranch::RangeX &&
           current->mouse_branch!=MouseResetBranch::RangeY &&
           current->mouse_branch!=MouseResetBranch::DiskSpace &&
           !current->live_continuation)
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
    if(current->live_continuation) {
        // DPMI 0300 passes the original 33AA0 real-mode register
        // image by offset. Honor the tracked DOS AX=0100 segment,
        // and update EAX/EBX/ECX/EDX at the actual DPMI frame slots.
        // A host must explicitly answer this INT33 service; the
        // execution is paused here until it does.
        const auto* block=current->dpmi_dos_memory?
            current->dpmi_dos_memory->block(0x0060u):nullptr;
        if(!block || block->segment!=
           code2::load_le16(segment(0x30u,0x2B770u+0x22u,2)))
            stop("LIVE_ORIGINAL_DPMI_0300_TRACKED_SEGMENT_MISMATCH");
        const auto before=rloft::dpmi_real_mode_mouse::before(
            current->data.data(),current->data.size(),0x2B770u,block->segment);
        if(before.eax!=0x53C1u)
            stop("LIVE_ORIGINAL_DPMI_0300_SOURCE_MOUSE_FUNCTION_CHANGED");
        rloft::guest_continuation::Reply response{};
        const rloft::guest_continuation::Request request{
            rloft::guest_continuation::Service::DpmiRealModeInt33,
            in[0],in[1],in[5],before.eax,0u};
        if(!current->live_continuation->suspend(request,response))
            stop("LIVE_ORIGINAL_DPMI_MOUSE_SHUTDOWN");
        rloft::dpmi_real_mode_mouse::apply(
            current->data.data(),current->data.size(),0x2B770u,
            block->segment,{response.ax,response.bx,response.cx,response.dx});
        return 0u; // DPMI itself succeeded; 33B44 checks returned EAX.
    }
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
    if(frame!=0x2B770u ||
       (!wants_synthetic_mouse_dpmi_allocation() &&
        !current->live_continuation))
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
    if(current->live_continuation && current->original_20440_mouse_started) {
        const bool new_y=(command==8u && ebx==0u &&
                          ecx==0u && edx==0x18Fu);
        const bool new_x=(command==7u && ebx==0u &&
                          ecx==0u && edx==0x27Eu);
        const bool set_pos=(command==4u && ebx==0u &&
                            ecx==0x140u && edx==0xC8u);
        const bool read_pos=(command==3u && ebx==0u &&
                             ecx==0u && edx==0u);
        if(!new_y && !new_x && !set_pos && !read_pos)
            stop("ORIGINAL_20440_INT33_MOUSE_COMMAND_UNEXPECTED");
        if((new_x||new_y) && current->original_20440_mouse_inits!=0u)
            stop("ORIGINAL_20440_INT33_MOUSE_RANGE_AFTER_INIT");
        rloft::guest_continuation::Reply reply{};
        const rloft::guest_continuation::Request request{
            (new_y||new_x)?
                 rloft::guest_continuation::Service::DosMouseRange:
                 rloft::guest_continuation::Service::Original20440MousePointer,
            command,ebx,ecx,edx,0u};
        if(!current->live_continuation->suspend(request,reply))
            stop("ORIGINAL_20440_INT33_MOUSE_HOST_SHUTDOWN");
        if(new_y||new_x)++current->mouse_range_calls;
        return {reply.ax,reply.bx,reply.cx,reply.dx};
    }
    ++current->mouse_range_calls;
    if(current->live_continuation) {
        if((command!=7u && command!=8u) || ebx!=0u || ecx!=0u ||
           (command==7u && edx!=0x027Eu) ||
           (command==8u && edx!=0x00C7u))
            stop("LIVE_ORIGINAL_INT33_MOUSE_RANGE_ARGUMENTS_CHANGED");
        // DOS INT33 0007/0008: set mouse horizontal/vertical range.
        // The virtual host must acknowledge each actual source call.
        rloft::guest_continuation::Reply response{};
        const rloft::guest_continuation::Request request{
            rloft::guest_continuation::Service::DosMouseRange,
            command,ebx,ecx,edx,0u};
        if(!current->live_continuation->suspend(request,response))
            stop("LIVE_ORIGINAL_INT33_MOUSE_RANGE_SHUTDOWN");
        return {response.ax,response.bx,response.cx,response.dx};
    }
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
bool stop_at_dos_disk_free(u8 drive,u16& sectors_per_cluster,
                           u16& free_clusters,u16& bytes_per_sector,
                           u16& total_clusters) noexcept {
    ++current->disk_space_calls;
    if(current->live_continuation) {
        if(drive!=0u || current->mouse_range_calls!=2u ||
           current->dpmi_mouse_free_calls!=1u ||
           current->dpmi_dos_memory->allocation_count()!=0u)
            stop("LIVE_ORIGINAL_DOS_AH36_SERVICE_ORDER_CHANGED");
        // DOS INT21 AH36 is the next original startup dependency.
        // Do not assume disk geometry/available clusters from a modern
        // host filesystem. Stop at the exact service and demand a
        // separately implemented DOS response from the host.
        rloft::guest_continuation::Reply reply{};
        const rloft::guest_continuation::Request request{
            rloft::guest_continuation::Service::DosDiskFree,
            drive,0u,0u,0u,0u};
        if(!current->live_continuation->suspend(request,reply))
            stop("LIVE_ORIGINAL_DOS_AH36_SHUTDOWN");
        if(reply.ax==0u || reply.ax==0xFFFFu || reply.cx==0u ||
           reply.bx>0xFFFFu || reply.dx>0xFFFFu)
            stop("LIVE_ORIGINAL_DOS_AH36_INVALID_GEOMETRY");
        sectors_per_cluster=static_cast<u16>(reply.ax);
        free_clusters=static_cast<u16>(reply.bx);
        bytes_per_sector=static_cast<u16>(reply.cx);
        total_clusters=static_cast<u16>(reply.dx);
        return true;
    }
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
void original_source_resource_selection(u32 context,u32 item,u32 display_byte) noexcept {
    if(context!=0x1134u || item!=1u || display_byte!=0x3Cu)
        stop("ORIGINAL_2F468_RESOURCE_SELECTION_ARGUMENTS_CHANGED");
    if(!current->live_continuation)
        stop("STOP_BEFORE_ORIGINAL_2F468_RESOURCE_SELECTION");
    rloft::guest_continuation::Reply response{};
    const rloft::guest_continuation::Request request{
        rloft::guest_continuation::Service::Original2F468ResourceSelection,
        context,item,display_byte,0u,0u};
    if(!current->live_continuation->suspend(request,response))
        stop("LIVE_ORIGINAL_2F468_RESOURCE_SELECTION_SHUTDOWN");
    if(response.ax!=1u || !current->game_audio.app68 ||
       code2::load_le32(current->data.data()+0xF0B4u)!=11u ||
       code2::load_le32(current->data.data()+0xF0B8u)==0u)
        stop("LIVE_ORIGINAL_2F468_RESOURCE_CACHE_NOT_CONNECTED");
    // The original source selector, not an invented table/index update.
    std::cerr<<"CODE2_ORIGINAL_2F468_STAGE=SOURCE_BODY_BEGIN"
             <<" context="<<context<<" item="<<item
             <<" display_byte="<<display_byte<<"\n"<<std::flush;
    code2::function_2f468(current->machine,current->game_audio,
                           context,item,display_byte);
    std::cerr<<"CODE2_ORIGINAL_2F468_STAGE=SOURCE_BODY_RETURNED\n"<<std::flush;
}
void original_video_mode_290f4(u32 mode) noexcept {
    std::cerr<<"CODE2_ORIGINAL_LIVE_VGA_TRACE=290F4_ENTRY\n"<<std::flush;
    if(mode!=3u)stop("ORIGINAL_290F4_VIDEO_MODE_CHANGED");
    if(!current->live_continuation)
        stop("STOP_BEFORE_ORIGINAL_290F4_DISPLAY_BACKEND");
    rloft::guest_continuation::Reply response{};
    const rloft::guest_continuation::Request request{
        rloft::guest_continuation::Service::Original290F4VideoMode,
        mode,0u,0u,0u,0u};
    if(!current->live_continuation->suspend(request,response))
        stop("LIVE_ORIGINAL_290F4_DISPLAY_SHUTDOWN");
    if(response.ax!=1u || !current->original_vga)
        stop("LIVE_ORIGINAL_290F4_DISPLAY_DEVICE_NOT_ATTACHED");
    std::cerr<<"CODE2_ORIGINAL_LIVE_VGA_TRACE=290F4_RESUMED\n"<<std::flush;
    // Execute the ORIGINAL canonical CODE.2 mode initializer, including
    // 29930 VGA mode/BIOS, 2AB98 planar clear, 2ABF4/2AC10 CRTC,
    // 29FEC/29FFC pages and 296C0 clipping on the same guest and VGA
    // device that already carries its earlier source COLORS palette.
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_290f4(mode);
        std::cerr<<"CODE2_ORIGINAL_LIVE_VGA_TRACE=290F4_SOURCE_RETURNED\n"<<std::flush;
        if(bridge.source_mode_calls!=1u)
            stop("LIVE_ORIGINAL_290F4_SOURCE_NOT_EXECUTED");
        ++current->original_source_video_modes;
    }catch(const std::exception&) {
        stop("LIVE_ORIGINAL_290F4_VGA_BACKEND_REJECTED_SOURCE_IO");
    }
}
void original_video_palette_29234(u32 palette_buffer) noexcept {
    std::cerr<<"CODE2_ORIGINAL_LIVE_VGA_TRACE=29234_ENTRY\n"<<std::flush;
    if(palette_buffer!=0xC054u || !current->original_vga ||
       current->original_source_video_modes!=1u)
        stop("ORIGINAL_29234_VGA_SOURCE_PREREQUISITES_MISSING");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.stream_original_29234(palette_buffer);
        std::cerr<<"CODE2_ORIGINAL_LIVE_VGA_TRACE=29234_SOURCE_RETURNED\n"<<std::flush;
        if(bridge.source_palette_stream_calls!=1u)
            stop("ORIGINAL_29234_FULL_PALETTE_DAC_NOT_EXECUTED");
        ++current->original_source_dac_streams;
    }catch(const std::exception&) {
        stop("ORIGINAL_29234_FULL_PALETTE_DAC_FAILED");
    }
}
// Source 20294 calls 2E590 (push output-page stack), 291B0 (select
// page), two original 29B40 rectangular planar fills, then 2E5D4 (pop).
// These are original CODE.2 functions on the SAME running CODE.2 stack.
void original_291b0_source_page(u32 page) noexcept {
    if(page>1u || !current->original_vga ||
       current->original_source_video_modes!=1u)
        stop("ORIGINAL_291B0_PAGE_UNSUPPORTED");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_291b0(page);
        if(bridge.source_page_select_calls!=1u)
            stop("ORIGINAL_291B0_SOURCE_PAGE_NOT_EXECUTED");
        ++current->original_source_page_selects;
    }catch(const std::exception&) {
        stop("ORIGINAL_291B0_SOURCE_PAGE_VGA_IO_FAILED");
    }
}
void original_2e590_presentation_reset(u32 page) noexcept {
    std::cerr<<"CODE2_ORIGINAL_LIVE_VGA_TRACE=20294_2E590_SOURCE_PAGE_PUSH\n"<<std::flush;
    if(page!=0u || !current->live_continuation ||
       !current->original_vga || current->original_source_video_modes!=1u ||
       current->original_source_dac_streams!=1u ||
       current->original_source_page_pushes!=current->original_source_page_pops ||
       current->data[0xF0D0u]!=0u)
        stop("ORIGINAL_20294_PAGE_STACK_PUSH_PREREQUISITES");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_2e590(page);
        if(bridge.source_page_push_calls!=1u ||
           bridge.source_page_select_calls!=1u ||
           current->data[0xF0D0u]!=1u)
            stop("ORIGINAL_20294_PAGE_STACK_PUSH_INCOMPLETE");
        ++current->original_source_page_pushes;
        ++current->original_source_page_selects;
    }catch(const std::exception&) {
        stop("ORIGINAL_20294_PAGE_STACK_SOURCE_VGA_FAILED");
    }
}
void original_29b40_source_rectangle(u32 row,u32 x,u32 rows,u32 pixels,
                                    u32 color) noexcept {
    if(row!=0u || x!=0u || rows!=400u || pixels!=320u ||
       color!=0u || !current->original_vga ||
       current->original_source_page_pushes!=current->original_source_page_pops+1u ||
       current->original_source_planar_fills<
           2u*current->original_source_page_pops ||
       current->original_source_planar_fills>=
           2u*current->original_source_page_pushes)
        stop("ORIGINAL_20294_29B40_CLEAR_PARAMETERS_CHANGED");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_29b40(row,x,rows,pixels,color);
        if(bridge.source_rectangle_fill_calls!=1u)
            stop("ORIGINAL_20294_29B40_SOURCE_FILL_MISSING");
        ++current->original_source_planar_fills;
        std::cerr<<"CODE2_ORIGINAL_LIVE_VGA_TRACE=20294_29B40_PAGE_CLEARED number="
                 <<current->original_source_planar_fills<<"\n"<<std::flush;
    }catch(const std::exception&) {
        stop("ORIGINAL_20294_29B40_PLANAR_DEVICE_FAILED");
    }
}
void original_2e5d4_source_page_pop() noexcept {
    if(!current->original_vga ||
       current->original_source_page_pushes!=current->original_source_page_pops+1u ||
       current->original_source_planar_fills!=2u*current->original_source_page_pushes ||
       current->data[0xF0D0u]!=1u)
        stop("ORIGINAL_20294_2E5D4_PAGE_STACK_IMBALANCED");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_2e5d4();
        if(bridge.source_page_pop_calls!=1u ||
           bridge.source_page_select_calls!=1u ||
           current->data[0xF0D0u]!=0u)
            stop("ORIGINAL_20294_2E5D4_SOURCE_POP_INCOMPLETE");
        ++current->original_source_page_pops;
        ++current->original_source_page_selects;
        std::cerr<<"CODE2_ORIGINAL_LIVE_VGA_TRACE=20294_TWO_PAGES_SOURCE_CLEARED\n"<<std::flush;
    }catch(const std::exception&) {
        stop("ORIGINAL_20294_2E5D4_SOURCE_POP_FAILED");
    }
}
std::int32_t original_5e38_open_game_opt(u32 path) noexcept {
    if(path!=0x374u ||
       std::memcmp(current->data.data()+path,"GAME.OPT",9u)!=0 ||
       current->original_installation_root.empty())
        stop("ORIGINAL_5E38_GAME_OPT_SOURCE_PATH_CHANGED");
    std::error_code ec;
    const bool present=std::filesystem::exists(
        current->original_installation_root/"GAME.OPT",ec);
    if(ec)stop("ORIGINAL_5E38_GAME_OPT_FILE_STAT_FAILED");
    if(present)stop("ORIGINAL_5E38_PRESENT_GAME_OPT_REQUIRES_ORIGINAL_DOS_READER");
    return -1; // original 312A0 DOS open: file missing, AX=FFFFh
}
void original_5e38_boot_options() noexcept {
    if(!current->live_continuation || current->original_source_page_pushes!=1u ||
       current->original_source_page_pops!=1u ||
       current->original_source_planar_fills!=2u)
        stop("ORIGINAL_5E38_BOOT_OPTION_PREREQUISITES_CHANGED");
    rloft::original_code2_options::Boundary b{};
    b.open_read=&original_5e38_open_game_opt;
    const auto result=rloft::original_code2_options::function_5e38(
        current->machine,b);
    ++current->original_5e38_calls;
    if(result.found)stop("ORIGINAL_5E38_UNEXPECTED_FILE_OPEN");
    ++current->original_5e38_absent;
    std::cerr<<"CODE2_ORIGINAL_LIVE_INIT=5E38_GAME_OPT_ABSENT_SOURCE_RETURN\n"<<std::flush;
}
void original_28f58_volume(u32 value) noexcept {
    if(value!=0x5Au || current->original_5e38_calls!=1u)
        stop("ORIGINAL_28F58_AUDIO_ENTRY_CHANGED");
    // Do not silently claim active audio hardware. The original
    // source 28F58 returns after updating EF30 when EE78==0
    // or EF2C is FFFFFFFF; both are observed retail branches.
    if(code2::load_le32(current->data.data()+0xEE78u)!=0u &&
       code2::load_le32(current->data.data()+0xEF2Cu)!=0xFFFFFFFFu)
        stop("ORIGINAL_28F58_REAL_AUDIO_DRIVER_PENDING");
    code2::function_28f58(current->machine,current->audio_seed,value);
    ++current->original_28f58_calls;
    std::cerr<<"CODE2_ORIGINAL_LIVE_INIT=28F58_SOURCE_VOLUME_RETURN\n"<<std::flush;
}
u32 original_31218_host_access(u32 path,u32 raw_mode) noexcept {
    const u16 mode=static_cast<u16>(raw_mode);
    ++current->original_31218_checks;
    if(path!=0x1140u || mode!=0u ||
       std::memcmp(current->data.data()+path,"CHARS",6u)!=0 ||
       current->original_installation_root.empty())
        stop("ORIGINAL_31218_CHARS_FILENAME_OR_MODE_CHANGED");
    std::error_code ec;
    const bool exists=std::filesystem::is_regular_file(
        current->original_installation_root/"CHARS",ec);
    if(ec)stop("ORIGINAL_31218_DOS_FILE_ACCESS_FAILED");
    std::cerr<<"CODE2_ORIGINAL_LIVE_INIT=31218_SOURCE_CHARS_FOUND_"
             <<(exists?"YES":"NO")<<"\n"<<std::flush;
    return exists?0u:1u; // original 37E39 OS status, 0=exists
}
u32 original_31218_chars_probe(u32 path,u32 mode) noexcept {
    code2::Application72Boundary b{};
    b.call_37e39=&original_31218_host_access;
    return code2::function_31218(b,path,mode);
}
u32 original_312a0_next_dos_open(u32 path) noexcept {
    if(path!=0x1148u || current->original_28f58_calls!=1u ||
       current->original_31218_checks!=1u || !current->host_files)
        stop("ORIGINAL_312A0_NEXT_SOURCE_FILE_PATH_CHANGED");
    // Run genuine reconstructed 312A0 -> 37E77 (DOS 21h/3D00)
    // through the same case-insensitive bounded read-only host transport
    // used by SOUND.CFG. Never invent a handle or success status.
    const auto handle=code2::function_312a0(current->dos_file_io,path);
    ++current->original_312a0_calls;
    std::cerr<<"CODE2_ORIGINAL_LIVE_INIT=312A0_SOURCE_DOS_READONLY_OPEN_RETURN status="
             <<handle<<"\n"<<std::flush;
    return handle;
}
void original_16d04_after_dos_open() noexcept {
    if(current->original_312a0_calls!=1u ||
       current->original_31218_checks!=1u)
        stop("ORIGINAL_16D04_INIT_PRECONDITION_NOT_MET");
    ++current->original_16d04_entries;
    std::cerr<<"CODE2_ORIGINAL_LIVE_INIT=16D04_SECOND_INIT_SOURCE_BOUNDARY\n"<<std::flush;
    rloft::guest_continuation::Reply response{};
    const rloft::guest_continuation::Request request{
        rloft::guest_continuation::Service::Original16D04SecondInit,
        0u,0u,0u,0u,0u};
    if(!current->live_continuation->suspend(request,response))
        stop("ORIGINAL_16D04_SOURCE_SHUTDOWN");
    if(response.ax!=1u || !current->original_16d04_callbacks)
        stop("ORIGINAL_16D04_SOURCE_CALLBACKS_NOT_CONNECTED");
    // Execute the CERTIFIED original source on its actual 20440
    // application call path, using the SAME Watcom heap and DGROUP.
    // The original file/seek/read callbacks are the exact initialized
    // resource-owner Application30 table, not another guest or stub.
    std::cerr<<"CODE2_ORIGINAL_LIVE_INIT=16D04_SOURCE_ENTRY_AFTER_DOS_OPEN\n"<<std::flush;
    code2::function_16d04(current->machine,*current->original_16d04_callbacks);
    ++current->original_16d04_completed;
    std::cerr<<"CODE2_ORIGINAL_LIVE_INIT=16D04_SOURCE_COMPLETED_ON_GUEST_STACK\n"<<std::flush;
}
// The original 20608 application calls the noreturn 1FC14 gameplay
// loop after 20440 returns. Source 1FC14 reads keyboard via 30B40.
// Without this callback the reconstructed optional function pointer
// yields key 0 in a busy loop, hiding the real live-gameplay frontier.
u32 original_30b40_keyboard_poll() noexcept {
    if(!current->live_continuation ||
       current->original_16d04_completed!=1u ||
       current->original_03838_completed!=1u ||
       current->original_16cd0_completed!=1u ||
       current->original_181b4_completed!=1u ||
       current->original_325b0_completed!=1u ||
       current->original_boot_output_page_selects!=1u ||
       current->original_boot_scanout_page_selects!=1u ||
       current->original_20440_res_image_loads!=1u ||
       current->original_20440_res_image_retains!=1u ||
       current->original_20440_mouse_inits!=1u ||
       current->original_20440_mouse_cursor_loads!=1u ||
       current->original_20440_mouse_callback_installs!=1u ||
       current->original_20440_mouse_draws!=1u ||
       current->original_source_page_pushes!=3u ||
       current->original_source_page_pops!=3u ||
       current->original_source_dac_streams!=2u ||
       code2::load_le32(current->data.data()+0xBF44u)!=0x270Fu ||
       code2::load_le32(current->data.data()+0xA7E8u)!=1u)
        stop("LIVE_ORIGINAL_1FC14_KEYBOARD_ENTRY_PREREQUISITES");
    std::cerr<<"CODE2_ORIGINAL_LIVE_GAMEPLAY=1FC14_30B40_KEYBOARD_ENTRY\n"<<std::flush;
    rloft::guest_continuation::Reply response{};
    const rloft::guest_continuation::Request request{
        rloft::guest_continuation::Service::Original1FC14KeyboardEntry,
        0u,0u,0u,0u,0u};
    if(!current->live_continuation->suspend(request,response))
        stop("LIVE_ORIGINAL_1FC14_KEYBOARD_ENTRY_SHUTDOWN");
    return response.ax & 0xFFFFu; // original 1FC14 consumes the low 16 bits
}
// Original 20440 direct dependencies. All file bytes arrive through the
// already installed read-only DOS transport; no second guest/heap is created.
u32 original_boot_file_open(u32 path) noexcept {
    if(!current->host_files)
        stop("ORIGINAL_20440_READONLY_DOS_ROOT_MISSING");
    return code2::function_312a0(current->dos_file_io,path);
}
u32 original_boot_file_read(u32 dst,u32 size,u32 handle) noexcept {
    const u32 got=code2::function_312ec(current->dos_file_io,dst,size,handle);
    if(got!=size)stop("ORIGINAL_20440_DOS_SHORT_READ");
    return got;
}
void original_boot_file_read_void(u32 dst,u32 size,u32 handle) noexcept {
    (void)original_boot_file_read(dst,size,handle);
}
void original_boot_file_close(u32 handle) noexcept {
    (void)code2::function_312dc(current->dos_file_io,handle);
}
u32 original_boot_heap_alloc(u32 bytes) noexcept {
    return original_checked_allocation(bytes);
}
u32 original_boot_read_word(u32 size,u32 handle) noexcept {
    if(size!=2u || !current->host_files || handle>0xFFFFu ||
       !current->host_descriptor_flags.count(handle))
        stop("ORIGINAL_03838_LOCAL_WORD_ABI_INVALID");
    u8 raw[2]{};
    const auto result=current->host_files->read(
        static_cast<u16>(handle),raw,2u);
    if(result.carry || result.ax!=2u)
        stop("ORIGINAL_03838_LOCAL_WORD_READ_FAILED");
    return code2::load_le16(raw);
}
void original_03838_sintab_load() noexcept {
    if(!current->live_continuation ||
       current->original_16d04_completed!=1u ||
       std::memcmp(current->data.data()+0x24u,"SINTAB",7u)!=0)
        stop("ORIGINAL_03838_SOURCE_STARTUP_PRECONDITION");
    code2::Application06Boundary b{};
    b.call_312a0=&original_boot_file_open;
    b.call_3104c=&original_boot_heap_alloc;
    b.call_312ec=&original_boot_file_read_void;
    b.call_312ec_local=&original_boot_read_word;
    b.call_312dc=&original_boot_file_close;
    code2::function_03838(current->machine,b);
    if(code2::load_le32(current->data.data()+0xC6D8u)!=256u ||
       code2::load_le32(current->data.data()+0xC738u)!=127u ||
       code2::load_le32(current->data.data()+0xC730u)!=508u ||
       code2::load_le32(current->data.data()+0x6090u)!=324989u ||
       code2::load_le32(current->data.data()+0xC65Cu)==0u ||
       code2::load_le32(current->data.data()+0xC660u)==0u ||
       code2::load_le32(current->data.data()+0x608Cu)==0u)
        stop("ORIGINAL_03838_AUTHENTIC_SINTAB_SOURCE_MISMATCH");
    ++current->original_03838_completed;
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=03838_SINTAB_SOURCE_COMPLETED\n"<<std::flush;
}
void original_boot_divide_error() noexcept {
    stop("ORIGINAL_16ADC_CHARACTER_DIVIDE_BY_ZERO");
}
u32 original_boot_dbd4(u32 slot) noexcept {
    code2::Application19Boundary b{};
    b.divide_error=&original_boot_divide_error;
    return static_cast<u32>(code2::function_0dbd4(current->machine,b,slot));
}
void original_boot_03ea4(u32 slot,code2::s32 level) noexcept {
    code2::function_03ea4(current->machine,slot,static_cast<u32>(level));
}
void original_boot_03de4(u32 slot,code2::s32 level) noexcept {
    code2::function_03de4(current->machine,slot,static_cast<u32>(level));
}
void original_16cd0_chars_load() noexcept {
    if(!current->live_continuation || current->original_03838_completed!=1u ||
       code2::load_le32(current->data.data()+0xC65Cu)==0u ||
       std::memcmp(current->data.data()+0x710u,"CHARS",6u)!=0)
        stop("ORIGINAL_16CD0_CHARACTERS_SOURCE_PRECONDITION");
    code2::Application30Boundary b{};
    b.call_312a0=&original_boot_file_open;
    b.call_312ec=&original_boot_file_read;
    b.call_312dc=&original_boot_file_close;
    b.call_30d30=&original_checked_memset;
    b.call_dbd4=&original_boot_dbd4;
    b.call_03ea4=&original_boot_03ea4;
    b.call_03de4=&original_boot_03de4;
    code2::function_16cd0(current->machine,b);
    ++current->original_16cd0_completed;
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=16CD0_CHARS_16ADC_SOURCE_COMPLETED\n"<<std::flush;
}
// Original 20440 -> 181B4: preserved archive fallback and unchanged
// CODE.2 Machine/DGROUP/Watcom allocations. WORK saved-level writes are
// intentionally unsupported here; a missing WORK file is a DOS miss.
u32 original_level_missing_standalone(u32 name) noexcept {
    if(name!=0x819Cu || !current->host_files)
        stop("ORIGINAL_181B4_STANDALONE_FILENAME_POINTER");
    // Original WORK\\LEVnnn.DAT contains a directory component and is
    // longer than a DOS 8.3 basename; do not use guest_dos_basename.
    // The original source itself already filled the three level digits.
    const char* const guest=reinterpret_cast<const char*>(
        segment(0x30u,name,16u));
    if(std::memcmp(guest,"work\\lev022.dat",16u)!=0 &&
       std::memcmp(guest,"WORK\\LEV022.DAT",16u)!=0)
        stop("ORIGINAL_181B4_STANDALONE_NAME_UNEXPECTED");
    // The default original installation has no mutable WORK level.
    // If one exists, this read-only archive gate cannot skip that branch.
    const auto filename=current->original_installation_root/
        "WORK"/"LEV022.DAT";
    std::error_code ec;
    const auto status=std::filesystem::symlink_status(filename,ec);
    if(!ec && std::filesystem::exists(status))
        stop("ORIGINAL_181B4_SAVED_LEVEL_BRANCH_NOT_CONNECTED");
    if(ec && ec!=std::errc::no_such_file_or_directory)
        stop("ORIGINAL_181B4_WORK_STAT_FAILED");
    return 0xFFFFFFFFu;
}
u32 original_level_archive_open_local(const u8* name) noexcept {
    if(!name || std::memcmp(name,"levels.dat",11u)!=0 ||
       std::memcmp(current->data.data()+0x170A0u,name,11u)!=0)
        stop("ORIGINAL_181B4_ARCHIVE_NAME_UNINITIALIZED");
    return original_boot_file_open(0x170A0u);
}
u32 original_level_file_seek_host(u32 handle,u32 offset,u32 origin) noexcept {
    if(!current->host_files || handle>0xFFFFu || origin>2u ||
       offset>0x7FFFFFFFu ||
       !current->host_descriptor_flags.count(handle))
        stop("ORIGINAL_181B4_FILE_SEEK_ABI_CHANGED");
    const auto result=current->host_files->seek(
        static_cast<u16>(handle),static_cast<std::int32_t>(offset),
        static_cast<u8>(origin));
    if(result.carry)stop("ORIGINAL_181B4_FILE_SEEK_FAILED");
    return result.offset();
}
u32 original_level_source_seek(u32 offset,u32 origin,u32 handle) noexcept {
    return code2::function_3131c(current->dos_file_io,offset,origin,handle);
}
u32 original_level_read_local(u8* dst,u32 size,u32 handle) noexcept {
    if(!current->host_files || !dst || size>0xFFFFu || handle>0xFFFFu ||
       !current->host_descriptor_flags.count(handle))
        stop("ORIGINAL_181B4_LOCAL_READ_ABI_CHANGED");
    const auto result=current->host_files->read(
        static_cast<u16>(handle),dst,size);
    if(result.carry || result.ax!=size)
        stop("ORIGINAL_181B4_LOCAL_READ_INCOMPLETE");
    return result.ax;
}
u32 original_level_sound_bank() noexcept {
    return code2::function_29050(current->machine);
}
u32 original_level_sound_mode() noexcept {
    return code2::function_29048(current->machine);
}
u32 original_level_heap_release_header(u32 header) noexcept {
    code2::function_37c37(current->heap,header);
    return 0u;
}
void original_level_heap_release(u32 payload) noexcept {
    code2::function_310b0(current->machine,current->dos_file_io,payload);
}
// Original startup asset: 20440 computes [258DC]+2Ch and loads it via
// 2ECB4. The original type-1 RES0 record 59 is independently inspected
// by the owner original-media gate; the cached image is decoded by the
// canonical original 2FD9C callback on the same retained guest.
void original_20440_res0_image_load(u32 item,u32 slot) noexcept {
    if(!current->live_continuation || !current->game_audio.app68 ||
       current->original_boot_scanout_page_selects!=1u ||
       current->original_20440_res_image_loads!=0u || slot!=0u ||
       item!=code2::load_le32(current->data.data()+0x258DCu)+0x2Cu)
        stop("ORIGINAL_20440_2ECB4_ASSET_ORDER_OR_ID_CHANGED");
    const u32 catalog=code2::load_le32(current->data.data()+0xF0B8u);
    if(std::uint64_t(catalog)+16u>current->data.size())
        stop("ORIGINAL_20440_2ECB4_ARCHIVE_CACHE_UNMAPPED");
    const u32 count=code2::load_le32(current->data.data()+catalog+8u);
    const u32 records=code2::load_le32(current->data.data()+catalog+12u);
    if(item>=count ||
       std::uint64_t(records)+std::uint64_t(count)*9u>current->data.size())
        stop("ORIGINAL_20440_2ECB4_RESOURCE_ENTRY_OUT_OF_RANGE");
    const u32 cache=records+item*9u;
    if(current->data[cache+2u]!=0u)
        stop("ORIGINAL_20440_2ECB4_RESOURCE_NOT_UNLOADED");
    code2::function_2ecb4(
        current->machine,*current->game_audio.app68,item,slot);
    const u32 image=code2::load_le32(current->data.data()+cache+5u);
    if(current->data[cache+2u]!=1u || image==0u ||
       std::uint64_t(image)+8u>current->data.size() ||
       code2::load_le16(current->data.data()+image)==0u ||
       code2::load_le16(current->data.data()+image+2u)==0u)
        stop("ORIGINAL_20440_2ECB4_IMAGE_DECODE_NOT_VERIFIED");
    ++current->original_20440_res_image_loads;
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=2ECB4_RES0_TYPE1_SOURCE_COMPLETED"
             <<" entry="<<item<<" decoded_ptr="<<image
             <<" original_guest=PRESERVED\n"<<std::flush;
}
void original_20440_res0_image_retain(u32 item,u32 slot) noexcept {
    if(!current->live_continuation ||
       current->original_20440_res_image_loads!=1u ||
       current->original_20440_res_image_retains!=0u ||
       slot!=0u ||
       item!=code2::load_le32(current->data.data()+0x258DCu)+0x2Cu)
        stop("ORIGINAL_20440_2F184_RETAIN_ORDER_CHANGED");
    code2::function_2f184(current->machine,item,slot);
    const u32 catalog=code2::load_le32(current->data.data()+0xF0B8u);
    const u32 records=code2::load_le32(current->data.data()+catalog+12u);
    const u32 cache=records+item*9u;
    if(current->data[cache+2u]!=1u ||
       (current->data[cache]&0x10u)==0u ||
       code2::load_le32(current->data.data()+cache+5u)==0u)
        stop("ORIGINAL_20440_2F184_RETAIN_FLAG_NOT_SET");
    ++current->original_20440_res_image_retains;
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=2F184_RES0_RESOURCE_PINNED"
             <<" entry="<<item<<" retained_bit=0x10\n"<<std::flush;
}
// Original 20440 runs 1A0B0 and, after switching 9E39 to the
// opposite page, runs 1A288. Retain canonical INT33 host suspension
// instead of inferring real mouse movement from static test fixtures.
u32 original_20440_mouse_probe() noexcept {
    return static_cast<u32>(code2::function_2e340(current->machine));
}
void original_20440_mouse_set_y(u32 lo,u32 hi) noexcept {
    if(lo!=0u || hi!=0x18Fu)stop("ORIGINAL_1A0B0_MOUSE_Y_RANGE_CHANGED");
    (void)code2::function_33d38(current->mouse_range,lo,hi);
}
void original_20440_mouse_set_x(u32 lo,u32 hi) noexcept {
    if(lo!=0u || hi!=0x27Eu)stop("ORIGINAL_1A0B0_MOUSE_X_RANGE_CHANGED");
    (void)code2::function_33d18(current->mouse_range,lo,hi);
}
void original_20440_mouse_set_position(u32 x,u32 y) noexcept {
    if(x!=0xC8u || y!=0xA0u)
        stop("ORIGINAL_1A0B0_CENTER_MOUSE_POSITION_CHANGED");
    (void)code2::function_33c90(current->mouse_range,x,y);
}
u32 original_20440_mouse_get_position() noexcept {
    return code2::function_33cb0(current->machine,current->mouse_range);
}
u32 original_20440_mouse_image(u32 item,u32 slot) noexcept {
    if(!current->game_audio.app68 || slot!=0u ||
       item!=code2::load_le32(current->data.data()+0x258DCu)+0x2Fu ||
       current->original_20440_mouse_cursor_loads!=0u)
        stop("ORIGINAL_1A0B0_CURSOR_SOURCE_ID_CHANGED");
    const u32 image=code2::function_2f29c(
        current->machine,current->game_audio,item,slot);
    if(!image || std::uint64_t(image)+0x1614u>current->data.size() ||
       code2::load_le16(current->data.data()+image+2u)==0u)
        stop("ORIGINAL_1A0B0_RES0_CURSOR_IMAGE_NOT_DECODED");
    ++current->original_20440_mouse_cursor_loads;
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1A0B0_RES0_CURSOR_SOURCE_DECODED"
             <<" entry="<<item<<" image="<<image<<"\n"<<std::flush;
    return image;
}
void original_20440_mouse_copy(u32 dest,u32 src,u32 count) noexcept {
    if(dest!=0x880Du || count!=0x1614u ||
       current->original_20440_mouse_cursor_loads!=1u ||
       src==0u || std::uint64_t(src)+count>current->data.size())
        stop("ORIGINAL_1A0B0_CURSOR_FRAME_COPY_CHANGED");
    (void)code2::function_30cd4(current->machine,dest,src,count);
}
void original_20440_mouse_clip(u32 y0,u32 x0,u32 y1,u32 x1) noexcept {
    if(y0!=0u || x0!=0u || y1!=0x18Fu || x1!=0x13Fu)
        stop("ORIGINAL_1A0B0_CLIPPING_BOUNDS_CHANGED");
    code2::function_296c0(current->machine,static_cast<code2::s32>(y0),
        static_cast<code2::s32>(x0),static_cast<code2::s32>(y1),
        static_cast<code2::s32>(x1));
}
void original_20440_mouse_interrupt_39148(u32 intr,u32 regs,
                                           u32 output,u32 segments) noexcept {
    if(intr!=0x33u || regs!=0xA7ACu || output!=0xA7C8u ||
       segments!=0xA79Eu ||
       code2::load_le16(current->data.data()+regs)!=0x000Cu ||
       code2::load_le16(current->data.data()+regs+8u)!=0xFFFFu ||
       code2::load_le32(current->data.data()+regs+12u)!=0x27894u)
        stop("ORIGINAL_1A064_INT33_CALLBACK_FRAME_CHANGED");
    rloft::guest_continuation::Reply reply{};
    const rloft::guest_continuation::Request request{
        rloft::guest_continuation::Service::Original20440MouseCallback,
        intr,regs,output,segments,0u};
    if(!current->live_continuation->suspend(request,reply))
        stop("ORIGINAL_1A064_INT33_CALLBACK_SHUTDOWN");
    if(reply.ax!=0u)stop("ORIGINAL_1A064_INT33_CALLBACK_HOST_FAILED");
    code2::store_le32(current->data.data()+output,reply.ax);
    ++current->original_20440_mouse_callback_installs;
}
u32 original_20440_mouse_interrupt_result(u32 out) noexcept {
    if(out!=0xA7C8u)stop("ORIGINAL_1A064_INT33_RESPONSE_POINTER_CHANGED");
    return code2::load_le32(current->data.data()+out);
}
void original_20440_mouse_irq(u32 intr,u32 in,u32 out,u32 seg) noexcept {
    code2::Runtime98Boundary boundary{};
    boundary.call_39148=&original_20440_mouse_interrupt_39148;
    boundary.read_output_eax=&original_20440_mouse_interrupt_result;
    (void)code2::function_33a7c(boundary,intr,in,out,seg);
}
void original_20440_mouse_init() noexcept {
    if(!current->live_continuation ||
       current->original_20440_res_image_retains!=1u ||
       current->original_20440_mouse_started ||
       current->original_20440_mouse_inits!=0u ||
       code2::function_2e340(current->machine)!=-1)
        stop("ORIGINAL_20440_1A0B0_MOUSE_PRECONDITIONS_CHANGED");
    current->original_20440_mouse_started=true;
    code2::function_1a0b0(
        current->machine,current->original_mouse_init_callbacks);
    if(current->original_20440_mouse_cursor_loads!=1u ||
       current->original_20440_mouse_callback_installs!=1u ||
       code2::load_le16(current->data.data()+0x85FCu)!=0xFFFFu ||
       code2::load_le16(current->data.data()+0x9E31u)!=160u ||
       code2::load_le16(current->data.data()+0x9E33u)!=200u)
        stop("ORIGINAL_20440_1A0B0_MOUSE_GUEST_STATE_WRONG");
    ++current->original_20440_mouse_inits;
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1A0B0_MOUSE_SOURCE_COMPLETED"
             <<" virtual_host_position=160_200 cursor=RES0_62"
             <<" int33_0C=HOST_ACKNOWLEDGED\n"<<std::flush;
}
void original_20440_mouse_page_push(u32 page) noexcept {
    if(page>1u || current->original_20440_mouse_inits!=1u)
        stop("ORIGINAL_1A288_SOURCE_PAGE_PUSH_ORDER");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_2e590(page);
        ++current->original_source_page_pushes;
        ++current->original_source_page_selects;
    }catch(const std::exception&){
        stop("ORIGINAL_1A288_CURSOR_PAGE_PUSH_FAILED");
    }
}
void original_20440_mouse_page_pop() noexcept {
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_2e5d4();
        ++current->original_source_page_pops;
        ++current->original_source_page_selects;
    }catch(const std::exception&){
        stop("ORIGINAL_1A288_CURSOR_PAGE_POP_FAILED");
    }
}
void original_20440_mouse_capture(u32 dst,code2::s32 x,code2::s32 y,
                                   u32 width,u32 height) noexcept {
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_292bc_cursor_capture(dst,x,y,width,height);
        ++current->original_20440_mouse_capture_calls;
    }catch(const std::exception&){
        stop("ORIGINAL_1A288_SOURCE_CURSOR_VGA_CAPTURE_FAILED");
    }
}
u32 original_20440_mouse_hit(code2::s32 x,code2::s32 y) noexcept {
    code2::Application62Boundary b{};
    const u32 hit=code2::function_2ae30(current->machine,b,
                         static_cast<u32>(x),static_cast<u32>(y));
    if(hit!=0u)stop("ORIGINAL_1A288_HIT_HANDLER_NOT_CONNECTED");
    return hit;
}
void original_20440_mouse_draw(u32,code2::s32,code2::s32,u32) noexcept {
    stop("ORIGINAL_1A288_MASKED_CURSOR_DRAW_NOT_CONNECTED");
}
void original_20440_mouse_render() noexcept {
    if(current->original_20440_mouse_inits!=1u ||
       current->original_20440_mouse_draws!=0u ||
       !current->original_vga)
        stop("ORIGINAL_1A288_SOURCE_CURSOR_PREREQUISITES");
    if(code2::load_le16(current->data.data()+0xA6C0u)!=0u)
        stop("ORIGINAL_1A288_VISIBLE_CURSOR_SPRITE_NOT_CONNECTED");
    code2::function_1a288(
        current->machine,current->original_mouse_draw_callbacks);
    if(current->original_20440_mouse_capture_calls!=1u ||
       current->original_source_page_pushes!=3u ||
       current->original_source_page_pops!=3u ||
       code2::load_le16(current->data.data()+0x9E37u)!=1u)
        stop("ORIGINAL_1A288_SOURCE_CURSOR_CAPTURE_INCOMPLETE");
    ++current->original_20440_mouse_draws;
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=1A288_CURSOR_SOURCE_CAPTURE_COMPLETED"
             <<" source=292BC_2A114"
             <<" sprite_visibility=ORIGINAL_DISABLED\n"<<std::flush;
}
void original_181b4_level_load(u32 requested) noexcept {
    if(requested!=0x16u || current->original_16cd0_completed!=1u ||
       current->original_03838_completed!=1u || !current->host_files)
        stop("ORIGINAL_181B4_SOURCE_BOOT_ORDER");
    // Reconstructed 181B4 copies this 11-byte buffer from guest BSS.
    // The missing C-runtime literal relocation is isolated here:
    // restore ONLY the original CODE.2-authenticated 'levels.dat\0'
    // pathname, never synthetic map/header/actor contents.
    u8* const name=current->data.data()+0x170A0u;
    const bool empty=std::all_of(name,name+11u,[](u8 b){return b==0u;});
    if(empty) {
        std::ifstream original(
            current->original_installation_root/"CODE.2",std::ios::binary);
        if(!original)stop("ORIGINAL_181B4_FILE_PROVENANCE_UNAVAILABLE");
        original.seekg(0x51FC4u,std::ios::beg);
        char source_name[11]{};
        original.read(source_name,11);
        if(!original || std::memcmp(source_name,"levels.dat",11u)!=0)
            stop("ORIGINAL_181B4_LITERAL_PROVENANCE_MISMATCH");
        std::memcpy(name,source_name,11u);
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=181B4_ORIGINAL_FILENAME_LITERAL_ADAPTER\n"
                 <<std::flush;
    } else if(std::memcmp(name,"levels.dat",11u)!=0)
        stop("ORIGINAL_181B4_GUEST_FILENAME_CHANGED");
    code2::Application32Boundary b{};
    b.call_312a0=&original_level_missing_standalone;
    b.call_312a0_local=&original_level_archive_open_local;
    b.call_3131c=&original_level_source_seek;
    b.call_312ec=&original_boot_file_read;
    b.call_312ec_local=&original_level_read_local;
    b.call_312dc=&original_boot_file_close;
    b.call_30d30=&original_checked_memset;
    b.call_3104c=&original_boot_heap_alloc;
    b.call_310b0=&original_level_heap_release;
    b.call_29050=&original_level_sound_bank;
    b.call_29048=&original_level_sound_mode;
    code2::function_181b4(current->machine,b,static_cast<code2::s32>(requested));
    if(code2::load_le32(current->data.data()+0xC6C8u)!=requested ||
       code2::load_le16(current->data.data()+0x25619u)==0u ||
       code2::load_le16(current->data.data()+0x2561Bu)==0u ||
       code2::load_le32(current->data.data()+0xCF44u)==0u)
        stop("ORIGINAL_181B4_GUEST_LEVEL_NOT_INITIALIZED");
    ++current->original_181b4_completed;
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=181B4_LEVEL22_ARCHIVE_SOURCE_COMPLETED\n"
             <<std::flush;
}
void original_325b0_live_palette(u32 palette,u32 first,u32 bank,
                                 u32 resource,u32 colors) noexcept {
    // This is the actual compiled original 325B0 -> 32588 -> 312A0,
    // 3131C, 312EC, 32150 and 312DC, on the running 20440 stack.
    if(!current->live_continuation || !current->host_files ||
       !current->original_vga || current->original_16cd0_completed!=1u ||
       current->original_03838_completed!=1u ||
       current->original_325b0_completed!=0u ||
       palette!=0xC054u || first!=0u || resource!=0x1128u ||
       colors!=0x100u || bank>=15u ||
       bank!=code2::load_le32(current->data.data()+0xC034u) ||
       std::memcmp(current->data.data()+resource,"COLORS",7u)!=0)
        stop("ORIGINAL_325B0_SOURCE_PALETTE_ABI_CHANGED");
    const std::size_t handles_before=current->host_files->open_handles();
    const std::size_t descriptors_before=current->host_descriptor_flags.size();
    code2::Application81Boundary callbacks{};
    callbacks.app72=&current->dos_file_io;
    (void)code2::function_325b0(
        current->machine,callbacks,palette,first,bank,resource,colors);
    // Independent read-only media witness. The existing source startup
    // staged zero brightness, so the original 32150 leaves RGB6 unchanged.
    const u32 profile=code2::load_le32(current->data.data()+0xF050u);
    if(profile>current->data.size()-0x6Du ||
       current->data[profile+0x6Cu]!=0u)
        stop("ORIGINAL_325B0_NONZERO_BRIGHTNESS_NEEDS_SOURCE_COMPARISON");
    std::ifstream original(current->original_installation_root/"COLORS",
                           std::ios::in|std::ios::binary);
    if(!original)stop("ORIGINAL_325B0_COLORS_MEDIA_NOT_FOUND");
    original.seekg(static_cast<std::streamoff>(1u+bank*768u),std::ios::beg);
    std::array<u8,768u> expected{};
    original.read(reinterpret_cast<char*>(expected.data()),
                  static_cast<std::streamsize>(expected.size()));
    if(!original ||
       !std::equal(expected.begin(),expected.end(),
                   current->data.begin()+palette) ||
       std::any_of(expected.begin(),expected.end(),[](u8 value){
           return value>63u;
       }))
        stop("ORIGINAL_325B0_PALETTE_DIFFERS_FROM_AUTHENTIC_COLORS");
    if(current->host_files->open_handles()!=handles_before ||
       current->host_descriptor_flags.size()!=descriptors_before)
        stop("ORIGINAL_325B0_DOS_HANDLE_LEAK");
    ++current->original_325b0_completed;
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=325B0_SOURCE_COLORS_COMPLETED"
             <<" bank="<<bank<<" RGB6=768 original_guest=PRESERVED\n"
             <<std::flush;
}
// Source 20440 calls the render-page and scanout-page selectors in
// sequence. Both update original DGROUP globals on the same VGA device.
void original_291b0_boot_page(u32 page) noexcept {
    if(!current->live_continuation ||
       current->original_325b0_completed!=1u ||
       current->original_boot_output_page_selects!=0u ||
       current->data[0xF0D0u]!=0u ||
       page!=current->data[0xC050u])
        stop("ORIGINAL_20440_291B0_PAGE_ORDER_CHANGED");
    original_291b0_source_page(page);
    ++current->original_boot_output_page_selects;
    std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=291B0_OUTPUT_PAGE_SOURCE_COMPLETED page="
             <<page<<"\n"<<std::flush;
}
void original_291c8_boot_scanout(u32 page) noexcept {
    if(!current->live_continuation || !current->original_vga ||
       current->original_325b0_completed!=1u ||
       current->original_boot_output_page_selects!=1u ||
       current->original_boot_scanout_page_selects!=0u ||
       page>1u || page!=(static_cast<u32>(current->data[0xC050u])^1u))
        stop("ORIGINAL_20440_291C8_SCANOUT_PAGE_ORDER_CHANGED");
    try {
        rloft::source_video_boot::Bridge bridge(
            current->machine,*current->original_vga,current->data.size());
        bridge.run_original_291c8(page);
        ++current->original_boot_scanout_page_selects;
        std::cerr<<"CODE2_ORIGINAL_LIVE_BOOT=291C8_SCANOUT_PAGE_SOURCE_COMPLETED page="
                 <<page<<" CRTC_start="<<current->original_vga->crtc_start()
                 <<"\n"<<std::flush;
    }catch(const std::exception&) {
        stop("ORIGINAL_20440_291C8_SCANOUT_VGA_REJECTED");
    }
}
void original_2f844_source_enable() noexcept {
    code2::function_2f844(current->machine);
}
void original_audio_init(u32 base,u32 count) noexcept {
    ++current->audio_init_calls;
    current->active_sound_base=base; current->active_sound_count=count;
    if(base!=0x1130u || count!=0x0Bu)
        stop("ORIGINAL_GAME_RESOURCE_INITIALIZER_ABI_CHANGED");
    code2::function_2f2ec(current->machine,current->game_audio,base,count);
    // The canonical 2F2EC source has now actually returned through
    // 33B44, 33D18 and 33D38. Retain the historical bounded fixture
    // stop, but allow the genuine suspended CODE2 source call stack
    // to execute the following 20440 -> 202F0 -> 35286 chain.
    if(current->live_continuation ||
       current->mouse_branch==MouseResetBranch::DiskSpace)return;
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
         bool use_host_dpmi=false,bool multi_resource_probe=false,
         bool source_host_heap=false,bool source_dos_fallback=false,
         bool source_break_growth=false,
         std::unique_ptr<Session>* retained_owner=nullptr,
         rloft::guest_continuation::Worker* stackful_worker=nullptr,
         Session** suspended_guest=nullptr) {
    require(img.objects.size()==2 && img.entry_object==1 &&
            img.entry_offset==0x353A0u && img.stack_object==2 &&
            img.stack_offset==0x2DD10u &&
            img.objects[0].raw.size()==0x3A852u &&
            img.objects[1].raw.size()==0x2DD10u,"CODE.2 LE geometry mismatch");
    auto owner=std::make_unique<Session>();
    Session& state=*owner;
    state.live_continuation=stackful_worker;
    if(host_root)state.original_installation_root=*host_root;
    state.extended=extended;
    state.source_host_heap=source_host_heap;
    state.source_dos_fallback=source_dos_fallback;
    state.source_break_growth=source_break_growth;
    require(!(source_dos_fallback && source_break_growth) &&
            (!source_dos_fallback || source_host_heap) &&
            (!source_break_growth || source_host_heap),
            "choose one source-backed original memory mode");
    require(!source_host_heap || extended,"host heap requires initializers");
    state.execute_application=execute_application;
    state.advance_to_sound_file=advance_to_sound_file;
    state.advance_to_mouse=advance_to_mouse;
    state.mouse_branch=mouse_branch;
    if(use_host_dpmi) {
        require(advance_to_mouse &&
                (mouse_branch==MouseResetBranch::MemoryHandoff ||
                 mouse_branch==MouseResetBranch::DiskSpace ||
                 stackful_worker!=nullptr),
                "host DPMI 0100/0101 requires original mouse source path");
        state.dpmi_dos_memory=std::make_unique<rloft_recomp::DpmiDosMemory>(
            &state.host_startup_descriptors);
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
    if(source_host_heap) {
        require(code2::load_le32(state.data.data()+0x12624u)==0u,
                "original heap chain was preseeded");
        // The original LE static DGROUP bytes remain intact. Only the
        // opt-in host aperture is extended to house guest linear DPMI pages.
        state.data.resize(0x01100000u,0u);
        state.dpmi_linear_memory=std::make_unique<
            rloft_recomp::DpmiLinearMemory>(state.data.data(),state.data.size());
        if(source_dos_fallback)
            state.dos_heap_memory=std::make_unique<
                rloft_recomp::DosParagraphMemory>(
                    state.data.data()+kHostDosAlias,
                    state.data.size()-kHostDosAlias);
        require(state.host_startup_descriptors.install(
                    0x30u,0x00050000u,0x2DD10u),
                "CODE.2 original DGROUP selector installation failed");
        // 39D4D checks the original runtime's 12708h enable flag.
        // The actual reconstructed 3A395 setter is invoked below after
        // Machine.data binds the final, non-reallocating host aperture.
    }
    state.psp[0x2c]=0x2c;state.psp[0x2d]=0;
    if(source_break_growth)
        code2::store_le32(state.psp.data()+0x5Cu,state.source_break_end);
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
    if(source_host_heap) {
        // Original retail CODE.2 object-2 byte 0x12708 is initially 1,
        // whereas the deliberately zeroed synthetic fixture starts at 0.
        // Canonical 3A395 returns the PRIOR value. The prior-state check
        // must follow authenticated loaded DGROUP bytes, not assume 0.
        const u32 prior=code2::load_le32(state.data.data()+0x12708u);
        require(prior==(original?1u:0u),
                "CODE.2 heap-enable initial value differs from source image");
        require(code2::function_3a395(state.machine,1u)==prior &&
                code2::load_le32(state.data.data()+0x12708u)==1u,
                "original 3A395 heap enable previous-state contract changed");
    }
    state.machine.cpu.ds=0x30;state.machine.cpu.ss=0x30;
    if(source_host_heap)state.machine.cpu.es=0x24u;
    state.machine.cpu.cs=0x17;state.machine.cpu.esp=0x2DD10;
    state.entry.tail_35418=&invoke_startup;
    // The canonical 352EE implementation delegates case-folded comparison
    // to the real canonical 39526, not a host std::getenv/strtol substitute.
    state.entry.call_39526=&code2::function_39526;
    state.game.call_352ee=&original_getenv;
    state.game.call_35349=&original_atoi;
    state.game.call_2f2ec=&original_audio_init;
    state.game.call_35286=&original_disk_free_from_35286;
    if(stackful_worker) {
        // Explicitly represent source 20440 dependencies; the older
        // null callbacks silently skipped 2F468/290F4 and made 20608
        // return without ever reaching its 1FC14 game loop.
        state.game.call_2f468=&original_source_resource_selection;
        state.game.call_290f4=&original_video_mode_290f4;
        state.game.call_30d30=&original_checked_memset;
        state.game.call_29234=&original_video_palette_29234;
    }
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
    state.dos_file_io.call_38407=&original_level_file_seek_host;
    state.dos_file_io.call_37c37=&original_level_heap_release_header;
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
        if(source_host_heap) {
            state.heap_provider.dpmi_alloc=&host_heap_0501;
            if(source_dos_fallback) {
                state.heap_provider.dos_alloc_paragraphs=&host_heap_dos_allocate;
                state.heap_provider.dos_free_selector=&host_heap_dos_release;
                state.heap_provider.selector_base=&host_heap_dos_selector_base;
                state.heap_provider.release_dos_block=&host_heap_dos_release;
            }
            state.heap_provider.release_dpmi_block=&host_heap_0502;
            state.heap_growth.alloc_39c7b=&host_heap_39c7b;
            state.heap_growth.init_arena_39bbc=&host_heap_39bbc;
            state.heap_growth.free_37c45=&host_heap_free_initial;
            state.heap.arena_alloc=&host_heap_alloc;
            state.heap.arena_free=&host_heap_free;
            if(source_break_growth) {
                state.heap_growth.lsl_ds=&host_break_limit;
                state.heap_growth.limit_3a467=&host_break_extend;
                state.heap.grow_heap=&host_heap_39e1c;
            }else state.heap.grow_heap=&host_heap_39d4d;
        }else state.heap.arena_alloc=&fixture_arena_alloc;
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
    if(suspended_guest)*suspended_guest=&state;
    current=&state;
    if(setjmp(state.escape)==0) {
        code2::function_353a0(state.machine,state.entry);
        stop("ORIGINAL_ENTRY_RETURNED_WITHOUT_RUNTIME");
    }
    current=nullptr;
    // Stackful source resumed beyond the original bounded stop; legacy
    // fixture counter expectations apply only to the historical single-
    // boundary checks. Report the actual original callback that stopped,
    // not the unrelated fixture-service count mismatch.
    if(stackful_worker)
        throw std::runtime_error(
            std::string("LIVE_CODE2_SOURCE_STOP=")+state.stop_reason+
            " dpmi_calls="+std::to_string(state.dpmi_calls)+
            " range_calls="+std::to_string(state.mouse_range_calls)+
            " free_calls="+std::to_string(state.dpmi_mouse_free_calls)+
            " disk_calls="+std::to_string(state.disk_space_calls));
    require(state.version_calls==1 &&
            state.ff00_calls==(source_break_growth?0u:1u) &&
            state.dpmi_calls==(source_host_heap&&!source_break_growth?1u:0u)+
                (mouse_branch==MouseResetBranch::Present ||
                              mouse_branch==MouseResetBranch::MemoryHandoff?1u:
                              mouse_branch==MouseResetBranch::FreeBoundary ||
                              mouse_branch==MouseResetBranch::RangeX ||
                              mouse_branch==MouseResetBranch::RangeY ||
                              mouse_branch==MouseResetBranch::DiskSpace?2u:0u),
            "DOS service sequence differs from bounded original source");
    require(state.machine.cpu.interrupt_enable,"STI was not executed");
    require(code2::load_le16(
                (source_host_heap?state.data.data():state.code.data())+
                 0x35675u)==0x30u,
            "source startup did not save original DS through returned selector");
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
        require((source_host_heap?
                    host_heap_owned(state,table,4u):
                    (table>=0x2C000u && table<0x2D500u)) &&
                code2::load_le32(state.data.data()+table+4u*expected_env_entries)==0u,
                "original environment pointer-table terminator differs");
        const u32 item=code2::load_le32(state.data.data()+table);
        require((source_host_heap?
                    host_heap_owned(state,item,7u):
                    (item>=0x2C000u && item<0x2D500u)) &&
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
            require((source_host_heap?
                        host_heap_owned(state,argv1,7u):
                        (argv1>=0x2C000u && argv1<0x2D500u)) &&
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
                            (source_host_heap?
                                host_heap_owned(state,env,0x101u):
                                (env>=state.arena_start+0x0Au &&
                                 env<state.arena_end-0x100u)) &&
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
        if(source_host_heap) {
            const u32 node=code2::load_le32(state.data.data()+0x12624u);
            require((source_break_growth?
                        (node>=0x30000u && node<state.source_break_end):
                        source_dos_fallback?
                        (node>=kHostDosAlias && node<kHostDosAlias+0xA0000u):
                        (node>=rloft_recomp::DpmiLinearMemory::kFirstLinear)) &&
                    !state.arena_seeded &&
                    code2::load_le32(state.data.data()+node+0x18u)==
                        (source_audio_allocates?9u:8u),
                    "actual original source heap allocation count changed");
            const u32 freeptr=code2::load_le32(state.data.data()+node+0x0Cu);
            const u32 sentinel=node+0x20u;
            require(freeptr>=node+0x2Cu &&
                    freeptr<node+code2::load_le32(state.data.data()+node) &&
                    code2::load_le32(state.data.data()+freeptr+4u)==sentinel &&
                    code2::load_le32(state.data.data()+freeptr+8u)==sentinel &&
                    code2::load_le32(state.data.data()+sentinel+4u)==freeptr &&
                    code2::load_le32(state.data.data()+sentinel+8u)==freeptr &&
                    host_heap_owned(state,freeptr,12u),
                    "original 39BBC/39838 DPMI free-list metadata differs");
        } else {
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
    if(source_host_heap) {
        require(!state.arena_seeded && state.host_heap_growths>=1u &&
                (source_break_growth?
                    (state.source_break_allocs>=1u &&
                     state.source_break_resizes>=2u &&
                     state.source_break_end>0x30000u &&
                     state.host_heap_allocs==0u &&
                     state.dpmi_linear_memory->allocation_count()==0u):
                 source_dos_fallback?
                    (state.host_heap_allocs==0u &&
                     state.source_dpmi_denials>=1u &&
                     state.source_dos_requests==2u &&
                     state.source_dos_releases==1u &&
                     state.dos_heap_memory->allocation_count()==1u &&
                     state.host_startup_descriptors.size()==2u &&
                     state.host_startup_descriptors.get_base(0x1001u).carry &&
                     state.dpmi_linear_memory->allocation_count()==0u):
                    (state.host_heap_allocs>=1u &&
                     state.dpmi_linear_memory->allocation_count()>=1u)) &&
                host_heap_owned(state,code2::load_le32(
                       state.data.data()+0x12624u)+0x2Cu,4u) &&
                state.host_heap_releases==0u,
                "initializer source did not establish tracked memory heap");
        if(source_dos_fallback) {
            const u32 node=code2::load_le32(state.data.data()+0x12624u);
            const u16 selector=code2::load_le16(state.data.data()+node-4u);
            const auto descriptor=state.host_startup_descriptors.get_base(selector);
            require(selector==0x1003u && !descriptor.carry &&
                    descriptor.base()==node-8u &&
                    state.host_startup_descriptors.check(selector,8u,0x34u) &&
                    !state.host_startup_descriptors.check(selector,0x1000u,1u) &&
                    state.host_startup_descriptors.get_base(0x1001u).carry,
                    "original DOS heap selector/base/limit/retirement mismatch");
        }
        if(source_break_growth)
            std::cout<<"ORIGINAL_STARTUP_SOURCE_39E1C_EXTENDER_BREAK=PASS\\n";
        else if(source_dos_fallback)
            std::cout<<"ORIGINAL_STARTUP_SOURCE_DOS48_FALLBACK=PASS\\n"
                        "ORIGINAL_STARTUP_DPMI_0501_DENIED=YES\\n";
        else std::cout<<"ORIGINAL_STARTUP_SOURCE_DPMI_HEAP=PASS\\n";
        std::cout<<"ORIGINAL_STARTUP_HEAP_ARENA_PRESEEDED=NO\\n";
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

    if(retained_owner) {
        require(state.machine.data==state.data.data() &&
                source_host_heap && execute_application &&
                advance_to_mouse && advance_to_sound_file &&
                state.stop_reason==std::string(
                    "STOP_BEFORE_DOS_INT33_MOUSE_RESET"),
                "Cannot retain incomplete original startup/heap guest");
        *retained_owner=std::move(owner);
    }
}
}
namespace rloft { namespace canonical_code2_runtime {
struct Opaque {
    std::filesystem::path installation_root{};
    std::unique_ptr<Session> session;
    std::unique_ptr<rloft::guest_continuation::Worker> live_worker;
    Session* live_session=nullptr; // owned by the worker while suspended
    rloft::guest_continuation::Request pending{};
    ~Opaque() {
        // Shutdown first, before any memory owner is destroyed.
        if(live_worker)live_worker->stop();
    }
};
static Session& held(Opaque* owner) {
    if(!owner)throw std::invalid_argument("Missing canonical CODE.2 owner");
    if(owner->live_worker) {
        // The authenticated guest's OWN source callback may borrow the
        // Watcom heap while running, e.g. 2ECB4 -> 2EB60 -> 3104C ->
        // original 37B42. Host-origin calls require a suspended guest.
        // Requiring Paused unconditionally deadlocked/aborted the worker
        // as soon as genuine 2F468 materialization requested a heap block.
        if(!owner->live_session ||
           (!owner->live_worker->paused() &&
            !owner->live_worker->calling_from_guest_thread()))
            throw std::logic_error(
                "Concurrent host access to a running original guest");
        return *owner->live_session;
    }
    if(!owner->session)
        throw std::invalid_argument("Missing canonical CODE.2 guest");
    return *owner->session;
}
Opaque* start(const std::filesystem::path& original_code2) {
    const auto filename=original_code2.string();
    const auto bytes=read_file(filename.c_str());
    if(!rloft_recomp::is_original_code2(bytes))
        throw std::runtime_error("Original CODE.2 identity mismatch");
    const auto image=rloft_recomp::OriginalLeImage::parse(bytes);
    auto result=std::make_unique<Opaque>();
    result->installation_root=original_code2.parent_path();
    const auto sound_path=original_code2.parent_path()/"SOUND.CFG";
    const auto sound_name=sound_path.string();
    const auto sound=read_file(sound_name.c_str());
    if(sound.size()!=59u)
        throw std::runtime_error("Original SOUND.CFG is not 59 bytes");
    const auto root=original_code2.parent_path();
    // Execute real canonical CODE.2 startup and application main across
    // source 375FE vector install, 30BB4 keyboard IVT, and 59-byte
    // SOUND.CFG host read, STOPPING at the actual unimplemented INT33
    // mouse reset. No made-up driver-present response is supplied.
    run(image,true,true,true,GameConfig::Unset,
        true,true,true,&sound,MouseResetBranch::Stop,
        &root,false,false,true,false,false,&result->session);
    if(!result->session)
        throw std::runtime_error("Canonical CODE.2 startup was not retained");
    return result.release();
}
// This variant really executes original 353A0 -> 20608 on its own
// persistent native thread and suspends at the first genuine unprovided
// DOS INT33 request. The C++ call stack is retained by the waiting thread.
Opaque* start_stackful(const std::filesystem::path& original_code2) {
    auto result=std::make_unique<Opaque>();
    result->installation_root=original_code2.parent_path();
    result->live_worker=std::make_unique<rloft::guest_continuation::Worker>();
    Opaque* const out=result.get();
    result->live_worker->start([out,original_code2]() {
        const auto filename=original_code2.string();
        const auto bytes=read_file(filename.c_str());
        if(!rloft_recomp::is_original_code2(bytes))
            throw std::runtime_error("Original CODE.2 identity mismatch");
        const auto image=rloft_recomp::OriginalLeImage::parse(bytes);
        const auto root=original_code2.parent_path();
        const auto sound=read_file((root/"SOUND.CFG").string().c_str());
        if(sound.size()!=59u)
            throw std::runtime_error("Original SOUND.CFG size changed");
        // Original source heap + original IRQ and mouse startup, one
        // uninterrupted native C++ call stack until a real DOS service.
        run(image,true,true,true,GameConfig::Unset,
            true,true,true,&sound,MouseResetBranch::Stop,
            &root,true,false,true,false,false,
            nullptr,out->live_worker.get(),&out->live_session);
    });
    if(!result->live_worker->await_pause(result->pending) ||
       result->pending.service!=
            rloft::guest_continuation::Service::DosMouseReset ||
       !result->live_session) {
        throw std::runtime_error(
            "Original CODE.2 did not suspend at DOS INT33 reset");
    }
    return result.release();
}
std::uint32_t pending_host_service(const Opaque* owner) {
    if(!owner || !owner->live_worker ||
       !owner->live_worker->paused())
        throw std::logic_error("No pending live DOS callback");
    return static_cast<std::uint32_t>(owner->pending.service);
}
std::uint32_t answer_mouse_reset(Opaque* owner,bool present,
                                  std::uint32_t buttons) {
    if(!owner || !owner->live_worker ||
       !owner->live_worker->paused() ||
       owner->pending.service!=
         rloft::guest_continuation::Service::DosMouseReset)
        throw std::logic_error("Guest is not waiting for DOS INT33 reset");
    if(buttons>3u || (!present && buttons!=0u))
        throw std::invalid_argument("Invalid original DOS mouse reset reply");
    const auto request=owner->pending;
    owner->live_worker->resume(request,
        {present?0xFFFFu:0u,buttons,0u,0u});
    if(!owner->live_worker->await_pause(owner->pending))
        return 0u;
    return static_cast<std::uint32_t>(owner->pending.service);
}
std::uint32_t answer_real_mode_mouse(Opaque* owner,std::uint32_t eax,
                                     std::uint32_t ebx,
                                     std::uint32_t ecx,
                                     std::uint32_t edx) {
    if(!owner || !owner->live_worker || !owner->live_worker->paused() ||
       owner->pending.service!=
           rloft::guest_continuation::Service::DpmiRealModeInt33 ||
       owner->pending.ax!=0x0300u || owner->pending.bx!=0x33u ||
       owner->pending.cx!=0x2B770u || owner->pending.dx!=0x53C1u)
        throw std::logic_error("Original CODE.2 not waiting at 33AA0 DPMI 0300");
    // This is a caller-selected DOS virtual-mouse result, never
    // incorrectly labeled a detected physical input device.
    const auto q=owner->pending;
    owner->live_worker->resume(q,{eax,ebx,ecx,edx});
    if(!owner->live_worker->await_pause(owner->pending))return 0u;
    return static_cast<std::uint32_t>(owner->pending.service);
}
std::uint32_t answer_mouse_range(Opaque* owner,std::uint32_t command) {
    const bool after_startup=owner && owner->live_session &&
        held(owner).original_20440_mouse_started;
    if(!owner || !owner->live_worker || !owner->live_worker->paused() ||
       owner->pending.service!=rloft::guest_continuation::Service::DosMouseRange ||
       (command!=7u && command!=8u) || owner->pending.ax!=command ||
       owner->pending.bx!=0u || owner->pending.cx!=0u ||
       owner->pending.dx!=(command==7u?0x27Eu:(after_startup?0x18Fu:0xC7u)))
        throw std::logic_error("Original CODE.2 INT33 mouse range mismatch");
    const auto q=owner->pending;
    owner->live_worker->resume(q,{0xFFFFu,0u,0u,0u});
    if(!owner->live_worker->await_pause(owner->pending))return 0u;
    return static_cast<std::uint32_t>(owner->pending.service);
}
std::uint32_t answer_original_mouse_pointer(Opaque* owner,std::uint32_t command,
                                            std::uint32_t buttons,
                                            std::uint32_t x,std::uint32_t y) {
    if(!owner || !owner->live_worker || !owner->live_worker->paused() ||
       owner->pending.service!=
           rloft::guest_continuation::Service::Original20440MousePointer ||
       !held(owner).original_20440_mouse_started ||
       (command!=3u && command!=4u) || owner->pending.ax!=command ||
       owner->pending.bx!=0u ||
       owner->pending.cx!=(command==4u?0x140u:0u) ||
       owner->pending.dx!=(command==4u?0xC8u:0u) ||
       buttons>3u || x>0x27Eu || y>0x18Fu ||
       (command==4u && (x!=0x140u || y!=0xC8u)))
        throw std::logic_error("Original CODE2 not waiting for a valid INT33 mouse pointer command");
    const auto request=owner->pending;
    // Explicit virtual-mouse test response. No physical input device or
    // unobserved user movement is inferred.
    owner->live_worker->resume(request,{0xFFFFu,buttons,x,y});
    if(!owner->live_worker->await_pause(owner->pending))return 0u;
    return static_cast<std::uint32_t>(owner->pending.service);
}
std::uint32_t answer_original_mouse_callback(Opaque* owner) {
    if(!owner || !owner->live_worker || !owner->live_worker->paused() ||
       owner->pending.service!=
           rloft::guest_continuation::Service::Original20440MouseCallback ||
       owner->pending.ax!=0x33u || owner->pending.bx!=0xA7ACu ||
       owner->pending.cx!=0xA7C8u || owner->pending.dx!=0xA79Eu ||
       !held(owner).original_20440_mouse_started)
        throw std::logic_error("Original CODE2 not waiting for INT33 fn0C callback registration");
    const auto request=owner->pending;
    owner->live_worker->resume(request,{0u,0u,0u,0u});
    if(!owner->live_worker->await_pause(owner->pending))return 0u;
    return static_cast<std::uint32_t>(owner->pending.service);
}
std::uint32_t answer_host_disk_free(Opaque* owner) {
    if(!owner || !owner->live_worker || !owner->live_worker->paused() ||
       owner->pending.service!=rloft::guest_continuation::Service::DosDiskFree ||
       owner->pending.ax!=0u || !std::filesystem::is_directory(owner->installation_root))
        throw std::logic_error("Original CODE.2 is not waiting for default-drive AH36");
    const auto host=rloft::host_dos_disk::from_host_path(owner->installation_root);
    const auto request=owner->pending;
    owner->live_worker->resume(request,{
        host.sectors_per_cluster,host.free_clusters,
        host.bytes_per_sector,host.total_clusters});
    if(!owner->live_worker->await_pause(owner->pending))return 0u;
    return static_cast<std::uint32_t>(owner->pending.service);
}
void attach_original_vga(
    Opaque* owner,rloft::graphics::OriginalVgaHost* original_video) {
    if(!owner || !original_video || !owner->live_worker ||
       !owner->live_worker->paused())
        throw std::invalid_argument("CODE2 original source VGA owner not paused");
    auto& source=held(owner);
    if(source.original_vga || source.game.app45)
        throw std::logic_error("CODE2 source VGA device already attached");
    if(code2::load_le32(source.data.data()+0xF0B4u)!=11u ||
       code2::load_le32(source.data.data()+0xF0B8u)==0u)
        throw std::logic_error("CODE2 VGA requires original RES cache");
    source.original_vga=original_video;
    source.presentation.call_2e590=&original_2e590_presentation_reset;
    source.presentation.call_291b0=&original_291b0_source_page;
    source.presentation.call_29b40=&original_29b40_source_rectangle;
    source.presentation.call_2e5d4=&original_2e5d4_source_page_pop;
    source.presentation.call_30b40=&original_30b40_keyboard_poll;
    source.game.call_5e38=&original_5e38_boot_options;
    source.game.call_28f58=&original_28f58_volume;
    source.game.call_31218=&original_31218_chars_probe;
    source.game.call_312a0=&original_312a0_next_dos_open;
    source.game.call_16d04=&original_16d04_after_dos_open;
    source.game.call_3838=&original_03838_sintab_load;
    source.game.call_16cd0=&original_16cd0_chars_load;
    source.game.call_325b0=&original_325b0_live_palette;
    source.game.call_291b0=&original_291b0_boot_page;
    source.game.call_291c8=&original_291c8_boot_scanout;
    source.game.call_2ecb4=&original_20440_res0_image_load;
    source.game.call_2f184=&original_20440_res0_image_retain;
    source.game.call_1a0b0=&original_20440_mouse_init;
    source.game.call_1a288=&original_20440_mouse_render;
    source.original_mouse_resources.call_2f29c=&original_20440_mouse_image;
    source.original_mouse_resources.call_30cd4=&original_20440_mouse_copy;
    source.original_mouse_init_callbacks.app33=&source.original_mouse_resources;
    source.original_mouse_init_callbacks.call_2e340=&original_20440_mouse_probe;
    source.original_mouse_init_callbacks.call_33d38=&original_20440_mouse_set_y;
    source.original_mouse_init_callbacks.call_33d18=&original_20440_mouse_set_x;
    source.original_mouse_init_callbacks.call_33c90=&original_20440_mouse_set_position;
    source.original_mouse_init_callbacks.call_33cb0=&original_20440_mouse_get_position;
    source.original_mouse_init_callbacks.call_296c0=&original_20440_mouse_clip;
    source.original_mouse_init_callbacks.call_33a7c=&original_20440_mouse_irq;
    source.original_mouse_draw_callbacks.app34=&source.original_mouse_init_callbacks;
    source.original_mouse_draw_callbacks.call_2e590=&original_20440_mouse_page_push;
    source.original_mouse_draw_callbacks.call_2e5d4=&original_20440_mouse_page_pop;
    source.original_mouse_draw_callbacks.call_292bc=&original_20440_mouse_capture;
    source.original_mouse_draw_callbacks.call_2ae30=&original_20440_mouse_hit;
    source.original_mouse_draw_callbacks.call_29804=&original_20440_mouse_draw;
    source.game.call_2f844=&original_2f844_source_enable;
    source.game.call_181b4=&original_181b4_level_load;
    source.game.app45=&source.presentation;
}
std::uint32_t answer_source_video_mode(Opaque* owner) {
    if(!owner || !owner->live_worker || !owner->live_worker->paused() ||
       owner->pending.service!=
         rloft::guest_continuation::Service::Original290F4VideoMode ||
       owner->pending.ax!=3u)
        throw std::logic_error("Original CODE2 not waiting at VGA 290F4");
    const auto& s=held(owner);
    if(!s.original_vga || s.game.app45!=&s.presentation ||
       !s.game.call_30d30 || !s.game.call_29234 ||
       code2::load_le32(s.data.data()+0xF0B4u)!=11u)
        throw std::logic_error("Original CODE2 VGA callbacks not attached");
    const auto q=owner->pending;
    owner->live_worker->resume(q,{1u,0u,0u,0u});
    if(!owner->live_worker->await_pause(owner->pending))
        throw std::runtime_error("Original CODE2 startup returned before 20294");
    if(owner->pending.service!=
           rloft::guest_continuation::Service::Original16D04SecondInit)
        throw std::runtime_error(
            "Original CODE2 failed source 312A0 read-only DOS open before 16D04");
    const auto& after=held(owner);
    if(after.original_source_video_modes!=1u ||
       after.original_source_dac_streams!=1u ||
       after.original_source_page_pushes!=1u ||
       after.original_source_page_pops!=1u ||
       after.original_source_page_selects!=4u ||
       after.original_source_planar_fills!=2u ||
       after.original_5e38_calls!=1u ||
       after.original_5e38_absent!=1u ||
       after.original_28f58_calls!=1u ||
       after.original_31218_checks!=1u ||
       after.original_312a0_calls!=1u ||
       after.original_16d04_entries!=1u ||
       after.data[0xF0D0u]!=0u ||
       after.original_vga->bios_mode()!=0x13u ||
       after.data[0xEF3Cu]!=3u ||
       code2::load_le16(after.data.data()+0xEF3Fu)!=400u ||
       code2::load_le16(after.data.data()+0xEF41u)!=320u)
        throw std::runtime_error(
            "Original CODE2 VGA 290F4/29234 source did not establish 320x400");
    return static_cast<std::uint32_t>(owner->pending.service);
}
void attach_original_init_callbacks(Opaque* owner,void* original_app30) {
    if(!owner || !original_app30 || !owner->live_worker ||
       !owner->live_worker->paused())
        throw std::invalid_argument("Original CODE2 16D04 INIT owner unavailable");
    auto& source=held(owner);
    if(source.original_16d04_callbacks)
        throw std::logic_error("Original CODE2 INIT callbacks already attached");
    source.original_16d04_callbacks=
        static_cast<code2::Application30Boundary*>(original_app30);
}
std::uint32_t advance_original_second_init(Opaque* owner) {
    if(!owner || !owner->live_worker || !owner->live_worker->paused() ||
       owner->pending.service!=
           rloft::guest_continuation::Service::Original16D04SecondInit ||
       owner->pending.ax!=0u || owner->pending.bx!=0u)
        throw std::logic_error("Original CODE2 is not waiting at source 16D04");
    const auto& before=held(owner);
    if(!before.original_16d04_callbacks ||
       before.original_16d04_entries!=1u ||
       before.original_16d04_completed!=0u)
        throw std::logic_error("Original 16D04 source callback state invalid");
    const auto guest_base=before.machine.data;
    const auto guest_extent=before.data.size();
    const auto request=owner->pending;
    owner->live_worker->resume(request,{1u,0u,0u,0u});
    if(!owner->live_worker->await_pause(owner->pending))return 0u;
    const auto& after=held(owner);
    if(after.machine.data!=guest_base || after.data.size()!=guest_extent ||
       after.original_16d04_completed!=1u)
        throw std::runtime_error("Original CODE2 second INIT did not complete in original guest");
    return static_cast<std::uint32_t>(owner->pending.service);
}
std::uint32_t answer_original_keyboard_event(
    Opaque* owner,std::uint32_t key_code) {
    if(!owner || !owner->live_worker || !owner->live_worker->paused() ||
       owner->pending.service!=
           rloft::guest_continuation::Service::Original1FC14KeyboardEntry ||
       owner->pending.ax!=0u || owner->pending.bx!=0u ||
       key_code>0xFFFFu)
        throw std::logic_error("Original CODE2 not at live 1FC14 keyboard poll");
    const auto& before=held(owner);
    if(before.original_16d04_completed!=1u || !before.original_vga ||
       before.original_source_page_pops!=2u)
        throw std::logic_error("Original CODE2 game entry state not retained");
    const auto* const guest_base=before.machine.data;
    const auto request=owner->pending;
    owner->live_worker->resume(request,{key_code,0u,0u,0u});
    if(!owner->live_worker->await_pause(owner->pending))return 0u;
    if(held(owner).machine.data!=guest_base)
        throw std::runtime_error("CODE2 1FC14 guest memory moved between keyboard polls");
    return static_cast<std::uint32_t>(owner->pending.service);
}
void attach_original_resource_callbacks(Opaque* owner,void* original_app68) {
    if(!owner || !original_app68 || !owner->live_worker ||
       !owner->live_worker->paused())
        throw std::invalid_argument("Original CODE2 resource callback owner unavailable");
    auto& source=held(owner);
    if(source.game_audio.app68!=nullptr)
        throw std::logic_error("Original CODE2 resource callbacks already installed");
    if(code2::load_le32(source.data.data()+0xF0B4u)!=11u ||
       code2::load_le32(source.data.data()+0xF0B8u)==0u)
        throw std::logic_error("Original CODE2 archive cache is not initialized");
    source.game_audio.app68=
        static_cast<code2::Application68Boundary*>(original_app68);
}
std::uint32_t answer_source_resource_selection(Opaque* owner) {
    if(!owner || !owner->live_worker || !owner->live_worker->paused() ||
       owner->pending.service!=
           rloft::guest_continuation::Service::Original2F468ResourceSelection ||
       owner->pending.ax!=0x1134u || owner->pending.bx!=1u ||
       owner->pending.cx!=0x3Cu)
        throw std::logic_error("Original CODE2 not waiting at source 2F468");
    const auto& source=held(owner);
    if(!source.game_audio.app68 ||
       code2::load_le32(source.data.data()+0xF0B4u)!=11u)
        throw std::logic_error("Source 2F468 cannot run without original archives");
    const auto q=owner->pending;
    owner->live_worker->resume(q,{1u,0u,0u,0u});
    if(!owner->live_worker->await_pause(owner->pending))return 0u;
    return static_cast<std::uint32_t>(owner->pending.service);
}
Evidence inspect(const Opaque* owner) {
    const auto& s=held(const_cast<Opaque*>(owner));
    if(s.data.data()!=s.machine.data || !s.dpmi_linear_memory ||
       (owner->live_worker?(!owner->live_worker->paused()):
        s.stop_reason!=std::string("STOP_BEFORE_DOS_INT33_MOUSE_RESET")))
        throw std::runtime_error("Canonical source heap/guest identity changed");
    Evidence out{};
    out.memory_address=s.data.data();
    out.memory_bytes=s.data.size();
    out.heap_arena_header=code2::load_le32(s.data.data()+0x12624u);
    out.source_353a0_executed=s.completed_initializers==4u;
    out.source_20608_executed=s.audio_init_calls==1u;
    out.source_keyboard_vectors_installed=
        s.irq_vector_sets==3u && s.irq_vector_reads==3u;
    out.source_sound_config_read=
        s.dos_file_reads==1u && s.dos_file_closes==1u &&
        s.mouse_reset_calls==1u;
    out.original_getenv_calls=s.getenv_calls;
    out.source_heap_provider_retained=true;
    out.source_cpu_stack_resumable=bool(owner->live_worker);
    out.paused_at_original_mouse_reset=bool(owner->live_worker) &&
        owner->pending.service==
          rloft::guest_continuation::Service::DosMouseReset;
    out.pending_DOS_service_sequence=owner->live_worker?
        owner->pending.sequence:0u;
    out.host_dpmi_dos_blocks=s.dpmi_dos_memory?
        static_cast<std::uint32_t>(s.dpmi_dos_memory->allocation_count()):0u;
    out.original_dpmi_0101_releases=s.dpmi_mouse_free_calls;
    out.original_mouse_range_calls=s.mouse_range_calls;
    out.original_source_page_pushes=s.original_source_page_pushes;
    out.original_source_page_pops=s.original_source_page_pops;
    out.original_source_page_selects=s.original_source_page_selects;
    out.original_source_planar_fills=s.original_source_planar_fills;
    out.original_source_video_mode_calls=s.original_source_video_modes;
    out.original_source_palette_streams=s.original_source_dac_streams;
    out.original_source_325b0_loads=s.original_325b0_completed;
    out.original_boot_render_page_selects=s.original_boot_output_page_selects;
    out.original_boot_scanout_page_selects=s.original_boot_scanout_page_selects;
    out.source_290f4_320x400_video_initialized=
       s.original_source_video_modes==1u && s.original_vga &&
       s.original_vga->bios_mode()==0x13u &&
       s.data[0xEF3Cu]==3u &&
       code2::load_le16(s.data.data()+0xEF3Fu)==400u &&
       code2::load_le16(s.data.data()+0xEF41u)==320u;
    return out;
}
std::vector<std::uint8_t>& guest_memory(Opaque* owner) {
    return held(owner).data;
}
void* canonical_machine(Opaque* owner) {
    return &held(owner).machine;
}
std::uint32_t allocate_source_heap(Opaque* owner,std::uint32_t bytes) {
    if(!bytes)throw std::invalid_argument("Zero byte CODE.2 heap request");
    auto& s=held(owner);
    if(s.machine.data!=s.data.data() || !s.dpmi_linear_memory)
        throw std::runtime_error("Canonical source heap lost its guest aperture");
    Session* const previous=current;
    current=&s;
    const bool previous_heap_trace=tracing_original_136_byte_heap;
    tracing_original_136_byte_heap=(bytes==136u);
    std::jmp_buf host_escape;
    auto* const previous_escape=s.active_escape;
    s.active_escape=&host_escape;
    std::uint32_t value=0;
    // The initial DOS startup probe has returned; its setjmp frame
    // no longer exists. Establish a NEW valid escape frame for the
    // retained canonical heap before invoking any original allocator
    // callback that might hit an unsupported DOS/4GW host service.
    const int boundary=setjmp(host_escape);
    if(boundary!=0) {
        s.active_escape=previous_escape;
        tracing_original_136_byte_heap=previous_heap_trace;
        current=previous;
        throw std::runtime_error(std::string(
            "Original CODE.2 37B42 host heap stopped: ")+s.stop_reason);
    }
    try {
        if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=37B42_ENTER bytes="<<bytes
                                <<" heap_root="<<code2::load_le32(s.data.data()+0x12624u)<<"\n"<<std::flush;
        value=code2::function_37b42(s.heap,bytes);
        if(tracing_original_136_byte_heap)std::cerr<<"CODE2_ORIGINAL_HEAP_TRACE=37B42_RETURN ptr="<<value<<"\n"<<std::flush;
    }catch(...) {
        s.active_escape=previous_escape;
        tracing_original_136_byte_heap=previous_heap_trace;
        current=previous;
        throw;
    }
    s.active_escape=previous_escape;
    tracing_original_136_byte_heap=previous_heap_trace;
    current=previous;
    if(!value || std::uint64_t(value)+bytes>s.data.size())
        throw std::runtime_error("Canonical CODE.2 37B42 heap exhausted");
    return value;
}
void release_source_heap(Opaque* owner,std::uint32_t payload) {
    if(!payload)return;
    auto& s=held(owner);
    if(s.machine.data!=s.data.data() || !s.dpmi_linear_memory)
        throw std::runtime_error("Original CODE.2 heap release lost guest aperture");
    Session* const previous=current;
    current=&s;
    std::jmp_buf host_escape;
    auto* const previous_escape=s.active_escape;
    s.active_escape=&host_escape;
    const int boundary=setjmp(host_escape);
    if(boundary!=0) {
        s.active_escape=previous_escape;
        current=previous;
        throw std::runtime_error(std::string(
            "Original CODE.2 37C37 heap free stopped: ")+s.stop_reason);
    }
    try {
        code2::function_37c37(s.heap,payload);
    }catch(...) {
        s.active_escape=previous_escape;
        current=previous;
        throw;
    }
    s.active_escape=previous_escape;
    current=previous;
}
void destroy(Opaque* owner) noexcept {delete owner;}
}} // namespace rloft::canonical_code2_runtime
#if !defined(RLOFT_BUNDLE_NO_MAIN) && !defined(RLOFT_STARTUP_GATE_NO_MAIN)
int main(int argc,char**argv) {
    try {
        bool extended=false,termination=false,arena_probe=false,failure_probe=false,vector_matrix=false;
        bool execute_application=false,advance_to_sound_file=false,
             sound_no_arguments=false,advance_to_mouse=false,
             host_sound_file=false,host_dpmi=false,multi_host_resource=false,
             host_source_heap=false,host_dos_fallback=false,
             host_break_growth=false;
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
               mode=="--through-initializers-host-dpmi" ||
               mode=="--enter-game-host-dpmi" ||
               mode=="--through-initializers-host-break" ||
               mode=="--enter-game-host-break" ||
               mode=="--to-sound-config-no-args-host-break" ||
               mode=="--to-mouse-reset-host-break" ||
               mode=="--through-initializers-host-dos-fallback" ||
               mode=="--enter-game-host-dos-fallback" ||
               mode=="--to-sound-config-no-args-host-dos-fallback" ||
               mode=="--to-mouse-reset-host-dos-fallback" ||
               mode=="--to-mouse-reset-host-dpmi-heap" ||
               mode=="--to-sound-config-no-args-host-dpmi-heap" ||
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
                advance_to_mouse=(mode=="--to-mouse-reset-host-break" ||
                    mode=="--to-mouse-reset-host-dos-fallback" ||
                    mode=="--to-mouse-reset-host-dpmi-heap" ||
                    mode=="--to-mouse-reset-host-file" ||
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
                advance_to_sound_file=(mode=="--to-sound-config-no-args-host-break" ||
                    mode=="--to-sound-config-no-args-host-dos-fallback" ||
                    mode=="--to-sound-config-no-args-host-dpmi-heap" ||
                    mode=="--to-sound-config" ||
                    mode=="--to-sound-config-configured" ||
                    mode=="--to-sound-config-no-args" ||
                    mode=="--to-sound-config-no-args-configured" ||
                    advance_to_mouse);
                sound_no_arguments=(mode=="--to-sound-config-no-args-host-break" ||
                    mode=="--to-sound-config-no-args-host-dos-fallback" ||
                    mode=="--to-sound-config-no-args-host-dpmi-heap" ||
                    mode=="--to-sound-config-no-args" ||
                    mode=="--to-sound-config-no-args-configured" ||
                    mode=="--to-mouse-reset-no-args" ||
                    mode=="--to-mouse-reset-no-args-configured");
                host_dos_fallback=(mode=="--through-initializers-host-dos-fallback" ||
                                   mode=="--enter-game-host-dos-fallback" ||
                                   mode=="--to-sound-config-no-args-host-dos-fallback" ||
                                   mode=="--to-mouse-reset-host-dos-fallback");
                host_break_growth=(mode=="--through-initializers-host-break" ||
                                   mode=="--enter-game-host-break" ||
                                   mode=="--to-sound-config-no-args-host-break" ||
                                   mode=="--to-mouse-reset-host-break");
                host_source_heap=(host_break_growth || host_dos_fallback ||
                                  mode=="--to-mouse-reset-host-dpmi-heap" ||
                                  mode=="--to-sound-config-no-args-host-dpmi-heap" ||
                                  mode=="--through-initializers-host-dpmi" ||
                                  mode=="--enter-game-host-dpmi");
                execute_application=(mode=="--enter-game-host-break" ||
                    mode=="--enter-game-host-dos-fallback" ||
                    mode=="--enter-game-host-dpmi" ||
                    mode=="--enter-game" ||
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
                extended=(mode=="--through-initializers" ||
                          host_source_heap || execute_application);
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
                     nullptr,mouse_branch,nullptr,host_dpmi,false,
                     host_source_heap,host_dos_fallback,host_break_growth);
        }else{
            const auto b=read_file(argv[original_index]);
            require(rloft_recomp::is_original_code2(b),
                    "CODE.2 supplied media differs from pinned original SHA-256");
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
                     advance_to_mouse,sound_ptr,mouse_branch,nullptr,
                     host_dpmi,false,host_source_heap,host_dos_fallback,
                     host_break_growth);
        }
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_STARTUP_GATE_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}

#endif // RLOFT_BUNDLE_NO_MAIN
