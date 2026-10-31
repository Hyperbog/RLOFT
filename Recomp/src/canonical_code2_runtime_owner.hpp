#pragma once
// Canonical CODE.2 353A0 startup OWNER. Keeps the original Watcom heap,
// DPMI selector services and source-initialized guest RAM alive after the
// first bounded host dependency. The new stackful owner can preserve the
// original application call stack at the actual DOS mouse service; the
// legacy bounded owner still unwinds at its explicit stop.
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>
#include "original_vga_host.hpp"

namespace rloft { namespace canonical_code2_runtime {
struct Opaque;
struct Evidence {
    const void* memory_address=nullptr;
    std::size_t memory_bytes=0;
    std::uint32_t heap_arena_header=0;
    bool source_353a0_executed=false;
    bool source_20608_executed=false;
    std::uint32_t original_getenv_calls=0;
    bool source_keyboard_vectors_installed=false;
    bool source_sound_config_read=false;
    bool source_heap_provider_retained=false;
    bool source_cpu_stack_resumable=false;
    bool paused_at_original_mouse_reset=false;
    std::uint64_t pending_DOS_service_sequence=0;
    std::uint32_t host_dpmi_dos_blocks=0;
    std::uint32_t original_dpmi_0101_releases=0;
    std::uint32_t original_mouse_range_calls=0;
    std::uint32_t original_source_page_pushes=0;
    std::uint32_t original_source_page_pops=0;
    std::uint32_t original_source_page_selects=0;
    std::uint32_t original_source_planar_fills=0;
    std::uint32_t original_source_video_mode_calls=0;
    std::uint32_t original_source_palette_streams=0;
    std::uint32_t original_source_325b0_loads=0;
    std::uint32_t original_boot_render_page_selects=0;
    std::uint32_t original_boot_scanout_page_selects=0;
    bool source_290f4_320x400_video_initialized=false;
    // Count only source 1FC14 scene+page+HUD+audio cycles that also
    // passed the native VGA/RGB6 audit after a live 181B4 transition.
    std::uint32_t original_live_portal_full_cycle_frames=0;
    std::uint32_t original_live_pending_portal_level=0xFFFFFFFFu;
    std::uint32_t original_live_level_transitions=0;
    std::uint32_t original_1fc14_source_scene_returns=0;
    std::uint32_t original_1fc14_post_scene_cycles=0;
    std::uint32_t original_1fc14_page_handoffs=0;
    // Last actual low-16 CODE.2 1FC14 key result. Exposes real F590
    // translation only after the retained source worker reaches 30B40.
    std::uint32_t original_1fc14_last_scan_code=0;
};
Opaque* start(const std::filesystem::path& original_code2);
Opaque* start_stackful(const std::filesystem::path& original_code2);
// Returns the exact outstanding host service; no implicit response.
// Service 1 = original DOS INT33 reset, service 3 = DPMI 0300 INT33.
std::uint32_t pending_host_service(const Opaque*);
// Host-confirmed mouse driver availability, not inferred from screenshots.
// Requires the exact pending INT33 reset and waits for the next stop.
std::uint32_t answer_mouse_reset(Opaque*, bool driver_present,
                                 std::uint32_t button_count);
// Explicit host reply to DPMI AX=0300 / original 33AA0 INT33.
// Writes actual real-mode register-frame bytes before resuming 33B44.
std::uint32_t answer_real_mode_mouse(Opaque*,std::uint32_t eax,
                                     std::uint32_t ebx=0u,
                                     std::uint32_t ecx=0u,
                                     std::uint32_t edx=0u);
// Acknowledge the source INT33 function 7 or 8 range request.
std::uint32_t answer_mouse_range(Opaque*,std::uint32_t command);
std::uint32_t answer_original_mouse_pointer(Opaque*,std::uint32_t command,
                                            std::uint32_t buttons,
                                            std::uint32_t x,std::uint32_t y);
std::uint32_t answer_original_mouse_callback(Opaque*);
// Answer the genuine DOS INT21 AH36 query from the actual original
// install volume. 16-bit DOS cluster geometry is bounded/saturated;
// this is a host filesystem adaptation, not retail DOS disk parity.
std::uint32_t answer_host_disk_free(Opaque*);
// Called by the persistent CODE.2 resource owner while the source stack
// is paused. This pointer must refer to a live original Application68
// callback table and is NOT a memory copy.
void attach_original_resource_callbacks(Opaque*,void* original_app68);
 // Borrow the same original Application30 INIT file callback table that
 // bootstrapped the source resource archive. The guest owner and this
 // table must both remain alive until actual CODE.2 child termination.
void attach_original_init_callbacks(Opaque*,void* original_app30);
// R.EXE status-10 CODE.1->CODE.2 handoff: publish an exact CHARS
// image in CODE.2's read-only DOS namespace before its first INT33
// startup response. No original installation files are modified.
void install_original_chars_handoff(Opaque*,const std::uint8_t*,
                                    std::size_t);
// Answer only the actual compiled 20440 -> 16D04 entry request.
// Executes compiled 16D04 on the suspended original CODE.2 worker
// using its retained heap/RAM and the borrowed original INIT callbacks.
// Returns the subsequent service ID or zero on genuine child completion.
std::uint32_t advance_original_second_init(Opaque*);
// Deliver an original BIOS-style 16-bit key (zero means none) to
// 1FC14 -> 30B40. The same retained source worker may request original
// INT33 mouse-position service during the frame; answer it using the
// sampled host device state, then continue until the NEXT source poll.
// The default mouse state preserves existing headless acceptance clients.
std::uint32_t answer_original_keyboard_event(
    Opaque*,std::uint32_t key_code,
    std::uint32_t mouse_buttons=0u,
    std::uint32_t mouse_x=0x140u,
    std::uint32_t mouse_y=0xC8u);
// Attach exactly the original persistent guest's VGA device while the
// CODE2 source stack is paused at 290F4. Do not create another VGA page.
void attach_original_vga(Opaque*,rloft::graphics::OriginalVgaHost*);
// Reply only after the VIDEO device is attached and source-mode contract
// is validated. Calls full original 290F4, 29930, CRTC and 29234 DAC.
// Returns the next original-code dependency, not an invented frame.
std::uint32_t answer_source_video_mode(Opaque*);
// Continue through authenticated source 2F468 only after the 11-RES
// callback context is attached; then expose next original dependency.
std::uint32_t answer_source_resource_selection(Opaque*);
Evidence inspect(const Opaque*);
// Copies the original 181B4 source-selected WORK bytes from private RAM.
// An absent snapshot means the authentic LEVELS.DAT archive path.
bool copy_saved_level_snapshot(const Opaque*,std::uint32_t,
                               std::vector<std::uint8_t>&);
std::vector<std::uint8_t>& guest_memory(Opaque*);
void* canonical_machine(Opaque*);
std::uint32_t allocate_source_heap(Opaque*,std::uint32_t bytes);
void release_source_heap(Opaque*,std::uint32_t guest_payload);
void destroy(Opaque*) noexcept;
}} // namespace rloft::canonical_code2_runtime
