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
// Answer the genuine DOS INT21 AH36 query from the actual original
// install volume. 16-bit DOS cluster geometry is bounded/saturated;
// this is a host filesystem adaptation, not retail DOS disk parity.
std::uint32_t answer_host_disk_free(Opaque*);
Evidence inspect(const Opaque*);
std::vector<std::uint8_t>& guest_memory(Opaque*);
void* canonical_machine(Opaque*);
std::uint32_t allocate_source_heap(Opaque*,std::uint32_t bytes);
void release_source_heap(Opaque*,std::uint32_t guest_payload);
void destroy(Opaque*) noexcept;
}} // namespace rloft::canonical_code2_runtime
