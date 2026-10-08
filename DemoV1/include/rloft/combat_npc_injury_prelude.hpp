#pragma once

#include <cstdint>

namespace rloft {

// Original CODE.2 E284..E306: initial NPC injury mutation BEFORE
// type-specific death/drop logic and the external 3ACDC classifier.
// This is a pure reconstruction; it does NOT mark NPCs dead,
// update animations, play sounds or spawn loot.
struct CombatNpcInjuryPreludeInputs final {
    std::uint16_t npc_type_id{};
    std::int16_t previous_hp{};
    std::uint16_t previous_packed_state_14{};
    std::uint8_t npc_definition_reaction_byte_1c{};
    std::uint32_t reaction_mode{};
    // Original 32-bit damage argument; E2E7/E2FE read only AX,
    // preserving 16-bit subtraction/wrap in the NPC health word.
    std::int32_t raw_damage{};
    // Original NPC DWORD+08 contains BYTE+0B; E317 updates it
    // according to 3ACDC's original NPC type classification.
    std::uint32_t previous_runtime_flags{};
};

struct CombatNpcInjuryPreludeResult final {
    std::int16_t next_hp{};
    std::uint16_t next_packed_state_14{};
    std::uint16_t next_action_countdown_10{};
    bool hp_subtraction_skipped{};
    bool type8_hp_floor_applied{};
    std::uint32_t next_runtime_flags{};
    bool classification_state_applied{};
    // Further E306..E55C creature death/event behavior is NOT
    // included in this bounded initial mutation.
};

class CombatNpcInjuryPreludeModel final {
public:
    [[nodiscard]] static CombatNpcInjuryPreludeResult inspect(
        const CombatNpcInjuryPreludeInputs& inputs) noexcept;
};

} // namespace rloft
