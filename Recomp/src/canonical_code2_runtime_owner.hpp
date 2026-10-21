#pragma once
// Canonical CODE.2 353A0 startup OWNER. Keeps the original Watcom heap,
// DPMI selector services and source-initialized guest RAM alive after the
// first bounded host dependency. No C++ call stack is resumable after
// the intentional source startup longjmp stop.
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
};
Opaque* start(const std::filesystem::path& original_code2);
Evidence inspect(const Opaque*);
std::vector<std::uint8_t>& guest_memory(Opaque*);
void* canonical_machine(Opaque*);
std::uint32_t allocate_source_heap(Opaque*,std::uint32_t bytes);
void release_source_heap(Opaque*,std::uint32_t guest_payload);
void destroy(Opaque*) noexcept;
}} // namespace rloft::canonical_code2_runtime
