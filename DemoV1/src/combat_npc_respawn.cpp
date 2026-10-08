#include "rloft/combat_npc_respawn.hpp"
#include "rloft/original_random.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rloft {

std::optional<CombatNpcRespawnResult> CombatNpcRespawnModel::inspect(
    const CombatNpcRespawnContext& source) noexcept {
    // Original reads unguarded game memory; a missing/invalid snapshot
    // is UNSUPPORTED, not equivalent to a legitimate E0F8 return zero.
    if (source.map_width<3 || source.map_height<3 ||
        source.map_cell_words.size()!=
            static_cast<std::size_t>(source.map_width)*
            static_cast<std::size_t>(source.map_height) ||
        source.respawn_slot>=source.npc_slots.size() ||
        (source.active_npc_count>0 &&
         static_cast<std::size_t>(source.active_npc_count)>
             source.npc_slots.size())) {
        return std::nullopt;
    }

    // E0FE..E10E: 30700 reads IRQ tick; 290D0 falls back to the
    // 0040:006C BIOS word only for zero low16 and seeds 30CC4.
    const auto seed=source.irq_tick_low16!=0U
        ? source.irq_tick_low16 : source.bios_timer_word_46c;
    OriginalRandom rng(seed);
    CombatNpcRespawnResult result{};

    for (std::uint8_t attempt=0;attempt<3U;++attempt) {
        // E11B/E134: two and only two 30CA2 draws for each attempt.
        const auto x=1U+(
            static_cast<std::uint32_t>(rng.next15())*
            static_cast<std::uint32_t>(source.map_width-2) >> 15U);
        const auto y=1U+(
            static_cast<std::uint32_t>(rng.next15())*
            static_cast<std::uint32_t>(source.map_height-2) >> 15U);
        result.attempts=static_cast<std::uint8_t>(attempt+1U);
        const auto cell=y*static_cast<std::uint32_t>(source.map_width)+x;
        // E158/E165: LOW BYTE mask 40h then 3Fh, not word 4000h.
        if ((source.map_cell_words[cell]&0x007FU)!=0U) {
            continue;
        }
        // E185/E19C: each signed cell-axis distance must be >=15.
        const auto dx=static_cast<std::int32_t>(x)-source.party_x_cell;
        const auto dy=static_cast<std::int32_t>(y)-source.party_y_cell;
        if ((dx<0 ? -dx : dx)<15 || (dy<0 ? -dy : dy)<15) {
            continue;
        }
        // E1A7..E1D3: only NON-dead NPC records block a candidate.
        // Original record byte +0B bit02 = runtime DWORD bit25.
        bool occupied=false;
        for (std::int32_t i=0;i<source.active_npc_count;++i) {
            const auto& npc=source.npc_slots[static_cast<std::size_t>(i)];
            if ((npc.runtime_flags&0x02000000U)==0U &&
                npc.cached_cell_index==cell) {
                occupied=true;
                break;
            }
        }
        if (occupied) {
            continue;
        }

        // E1E8..E26D: a successful destination updates the specified
        // existing 22-byte slot. Its type (+0E) is NOT overwritten.
        auto npc=source.npc_slots[source.respawn_slot];
        npc.y_position_16_16=(y<<16U)|0x8000U;
        npc.x_position_16_16=(x<<16U)|0x8000U;
        npc.cached_cell_index=static_cast<std::uint16_t>(cell);
        npc.runtime_flags=source.definition_initial_flags;
        npc.current_hp=static_cast<std::int16_t>(rng.dice(
            source.definition_hit_dice,6U,0,0));
        npc.action_state_countdown=0;
        npc.packed_runtime_state &= 0xFE39U;
        result.placed=true;
        result.respawned_npc=npc;
        result.rng_state_after=rng.state();
        return result;
    }
    // E279: exhausted 3 candidates; the RNG has still been reseeded
    // and advanced by SIX random calls, but no HP-dice roll occurs.
    result.rng_state_after=rng.state();
    return result;
}

} // namespace rloft
