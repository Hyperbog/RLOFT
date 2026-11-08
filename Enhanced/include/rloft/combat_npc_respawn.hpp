#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/level_world_state.hpp"

namespace rloft {

// Original CODE.2 E0F8..E281, *only* for a known, fully captured
// invocation from the E284 death path. This does not mutate live world
// state and does not claim to model the caller's 258AA tracker words.
struct CombatNpcRespawnContext final {
    std::int16_t map_width{};       // signed WORD at DGROUP 25619
    std::int16_t map_height{};      // signed WORD at DGROUP 2561B
    std::vector<std::uint16_t> map_cell_words; // DGROUP 198E0, low-byte flags
    std::int16_t party_x_cell{};    // DGROUP 255D2
    std::int16_t party_y_cell{};    // DGROUP 255D6
    std::int16_t active_npc_count{}; // signed WORD DGROUP 258A2
    std::vector<LiveNpcRecord> npc_slots; // original 22-byte records
    std::size_t respawn_slot{};     // original E0F8 stack argument
    std::uint16_t irq_tick_low16{}; // 30700 -> MOVZX AX, time observation
    std::uint16_t bios_timer_word_46c{}; // used by 290D0 only when tick=0
    std::uint8_t definition_hit_dice{}; // NPC definition byte +1D
    std::uint32_t definition_initial_flags{}; // NPC definition DWORD +35
};

struct CombatNpcRespawnResult final {
    bool placed{};
    std::uint8_t attempts{};
    std::uint32_t rng_state_after{};
    // Meaningful only when placed=true. Patch applies to one existing slot.
    LiveNpcRecord respawned_npc{};
};

class CombatNpcRespawnModel final {
public:
    // nullopt means unsupported/incomplete source snapshot, never a
    // fabricated original-game failure. For valid snapshots, the result
    // includes the final RNG state even on three rejected attempts.
    [[nodiscard]] static std::optional<CombatNpcRespawnResult> inspect(
        const CombatNpcRespawnContext& source) noexcept;
};

} // namespace rloft
