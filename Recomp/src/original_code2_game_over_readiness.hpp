#pragma once
// Full original 0CBC8/0CADC callback contract: read-only verification.
// This does NOT replace CODE.2 art/VGA/audio/input behavior or certify a
// retained-guest game-over frame; the host must wire every required service.
#include <cstddef>
#include <cstdint>
// Include this contract after Code2.cpp in its source owner; do not create
// duplicate compiled source definitions in host translation units.

namespace rloft { namespace source_game_over {
struct Readiness {
    bool ready;
    const char* reason;
};
inline Readiness validate(const code2::Application18Boundary& b,
                          const std::uint8_t* memory,
                          std::size_t capacity) noexcept {
    if(memory==nullptr || capacity<0x258E0u)
        return {false,"ORIGINAL_0CBC8_GUEST_MEMORY_UNAVAILABLE"};
    // Order is source call-graph order, including source 0CADC's allocation,
    // exactly 400 owned records, random delays and paired scanout.
    if(!b.call_1f1c8) return {false,"ORIGINAL_0CBC8_SCENE_CALLBACK_PENDING"};
    if(!b.call_1ea78) return {false,"ORIGINAL_0CBC8_PAGE_HANDOFF_PENDING"};
    if(!b.call_1f494) return {false,"ORIGINAL_0CBC8_MODAL_FRAME_PENDING"};
    if(!b.call_28168) return {false,"ORIGINAL_0CBC8_AUDIO_STATUS_PENDING"};
    if(!b.call_1a9f8) return {false,"ORIGINAL_0CBC8_MOUSE_RESET_PENDING"};
    if(!b.call_291b0) return {false,"ORIGINAL_0CBC8_VGA_PAGE_SELECT_PENDING"};
    if(!b.call_2e608) return {false,"ORIGINAL_0CBC8_FRAME_PUMP_PENDING"};
    if(!b.call_2e510) return {false,"ORIGINAL_0CBC8_ARTWORK_RENDER_PENDING"};
    if(!b.call_1ea04) return {false,"ORIGINAL_0CBC8_INDEXED_PAGE_PRESENT_PENDING"};
    if(!b.call_28c54) return {false,"ORIGINAL_0CBC8_MUSIC_REQUEST_PENDING"};
    if(!b.call_2e614) return {false,"ORIGINAL_0CBC8_SOURCE_TIMING_PENDING"};
    if(!b.call_1aacc) return {false,"ORIGINAL_0CBC8_CURSOR_RESTORE_PENDING"};
    if(!b.call_27f24) return {false,"ORIGINAL_0CBC8_SAMPLED_AUDIO_PENDING"};
    if(!b.call_3104c) return {false,"ORIGINAL_0CADC_SOURCE_ALLOCATION_PENDING"};
    if(!b.call_30ca2) return {false,"ORIGINAL_0CADC_SOURCE_RNG_PENDING"};
    if(!b.call_2a078) return {false,"ORIGINAL_0CADC_VGA_COPY_PENDING"};
    if(!b.call_310b0) return {false,"ORIGINAL_0CADC_SOURCE_FREE_PENDING"};
    if(!b.call_1a288) return {false,"ORIGINAL_0CBC8_CURSOR_REDRAW_PENDING"};
    if(!b.call_1a1f4) return {false,"ORIGINAL_0CBC8_MODAL_MOUSE_INPUT_PENDING"};
    if(!b.call_205b4) return {false,"ORIGINAL_0CBC8_ORIGINAL_EXIT_PENDING"};
    const std::uint64_t art=code2::load_le32(memory+0x258DCu);
    const std::uint64_t audio=code2::load_le32(memory+0x258D0u);
    if(art+0x62u>0xFFFFFFFFull || audio+1u>0xFFFFFFFFull)
        return {false,"ORIGINAL_0CBC8_RESOURCE_ID_WRAP"};
    return {true,"ORIGINAL_0CBC8_SOURCE_SERVICES_READY"};
}
}} // namespace rloft::source_game_over
