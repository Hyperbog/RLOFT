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
    bool original_20608_executed_to_host_dependency=false;
    std::uint32_t original_352ee_getenv_calls=0;
    bool original_game_loop_connected=false;
};
Opaque* create(const std::filesystem::path& original_game_directory,
               const std::filesystem::path& original_extracted_cd_directory,
               unsigned scene_level);
Evidence inspect(const Opaque*);
void dispatch_pending(Opaque*);
// Original-media CI driver only: deliberately provides test virtual
// mouse replies and a real host disk query to execute the ACTUAL source
// from DOS startup through source 2F468 resource selection, then stop at
// the next 290F4 graphics/VGA dependency. No production device is claimed.
std::uint32_t test_advance_to_original_video_mode(Opaque*);
/* Original-media only: execute certified 290F4 video bootstrap on the
   SAME VGA device, then stop at the first original 20294/2E590 call. */
std::uint32_t test_advance_through_original_video_mode(Opaque*);
// Owner-original source experiment: resume the authentic second 16D04
// call with the SAME borrowed original Application30 INIT callback
// table, then report the next observed service or real child completion.
// Zero indicates no next resumable host-service request was observed.
std::uint32_t test_advance_past_original_second_init(Opaque*);
// Explicit experimental terminal input into the retained original 30B40
// keyboard-host boundary. Host-translated WASD is NOT DOS IRQ9 parity.
// Run only after test_advance_past_original_second_init returns service 11.
std::uint32_t drive_console_keyboard(Opaque*);
void destroy(Opaque*) noexcept;
}} // namespace rloft::original_code2_session
