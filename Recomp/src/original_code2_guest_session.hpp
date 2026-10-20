#pragma once
// Opaque, persistent original CODE.2 source guest session.
// Retains the SAME 16MiB guest memory, 11-source RES cache and source
// VGA DAC palette across successive native supervisor dispatcher calls.
// At the unmapped gameplay loop boundary it reports Pending, NEVER
// fabricates a DOS/4GW exit status or a visually successful game boot.
#include <cstdint>
#include <filesystem>
namespace rloft { namespace original_code2_session {
struct Opaque;
struct Evidence {
    const void* memory_address=nullptr;
    std::uint64_t source_state_fingerprint=0;
    std::uint32_t resource_cache_base=0;
    std::uint32_t resource_archives=0;
    std::uint32_t selected_palette=0;
    std::uint32_t vga_port_writes=0;
    std::uint64_t dispatch_calls=0;
    bool initialized=false;
    bool original_game_loop_connected=false;
};
Opaque* create(const std::filesystem::path& original_game_directory,
               const std::filesystem::path& original_extracted_cd_directory,
               unsigned scene_level);
Evidence inspect(const Opaque*);
void dispatch_pending(Opaque*);
void destroy(Opaque*) noexcept;
}} // namespace rloft::original_code2_session
