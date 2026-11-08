#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace rloft {

// CODE.2 logical E284:E31F..E577, reached after E284 HP/reaction
// mutation. A pure branching/presentation plan; does not invoke
// 38110/3195C/1DDA4, award XP, consume RNG, or spawn objects.
enum class CombatNpcDeathSpecialBranch : std::uint8_t {
    None,
    Type18ConditionalRespawn,
    Type21Special,
    Type12Script20,
    Type9FourQuestDrops,
    Type50Script43,
    Type8ExtendedSequence,
};

struct CombatNpcDeathDrop final {
    std::uint16_t object_id{};
    std::uint16_t screen_seed{};
};

struct CombatNpcDeathFollowOnPlan final {
    bool is_death{};
    CombatNpcDeathSpecialBranch special{
        CombatNpcDeathSpecialBranch::None};
    // The exact item IDs and screen seeds passed to 3195C,
    // not guessed offsets from a scene coordinate.
    std::array<CombatNpcDeathDrop,4U> drops{};
    std::size_t drop_count{};
    bool set_story_dword_bf44{};
    // E4C9..E508 generic death 38110 path, or E553..E56F
    // living damage-reaction 38110 path. The source WORD
    // can be 0; in that case only the global resource base
    // is used. This is a resource/event argument, not a
    // completed audio-playback assertion.
    std::uint32_t presentation_resource_argument{};
    // E53D..E54B passes NPC definition WORD+23 to 1DDA4.
    // True means this call appears on the generic death path;
    // whether XP is awarded is the downstream routine's job.
    bool calls_1dda4{};
    std::uint16_t definition_word_23{};
    bool type18_requires_external_branch_evidence{};
};

// Original E4C9..E577 common presentation: no implicit HP
// threshold override, no host-side sound delay or RNG.
class CombatNpcDeathFollowOnModel final {
public:
    [[nodiscard]] static CombatNpcDeathFollowOnPlan inspect(
        std::uint16_t npc_type,
        std::int16_t npc_hp_after_injury,
        std::uint16_t npc_definition_word_41,
        std::uint16_t npc_definition_word_43,
        std::uint16_t npc_definition_word_23,
        std::uint32_t global_resource_base_258bc,
        std::uint32_t global_resource_base_258c0) noexcept;

    // E284:E50B..E53D scans THREE signed WORD entries from
    // 258AA, comparing each against the zero-extended NPC type.
    // Only a matching slot invokes E0F8(external context); if
    // that call succeeds, byte npc+0B bit02 is not set.
    // Caller supplies the OBSERVED entries and the three actual
    // E0F8 results, not invented session values.
    [[nodiscard]] static std::uint32_t death_runtime_flags(
        std::uint16_t npc_type,
        std::uint32_t flags_after_3acdc,
        const std::array<std::int16_t,3U>& tracked_types_258aa,
        const std::array<bool,3U>& actual_e0f8_successes) noexcept;
};

} // namespace rloft
