#include "rloft/combat_npc_death_followon.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace rloft {

CombatNpcDeathFollowOnPlan CombatNpcDeathFollowOnModel::inspect(
    std::uint16_t npc_type,
    std::int16_t npc_hp_after_injury,
    std::uint16_t npc_definition_word_41,
    std::uint16_t npc_definition_word_43,
    std::uint16_t npc_definition_word_23,
    std::uint32_t global_resource_base_258bc,
    std::uint32_t global_resource_base_258c0) noexcept {
    CombatNpcDeathFollowOnPlan plan{};
    plan.is_death=npc_hp_after_injury<=0;
    plan.definition_word_23=npc_definition_word_23;

    // E553..E577: alive uses definition WORD+41 (or 0) and
    // global 258C0. The death branch's +43 / 258BC must not
    // be used for a damage reaction.
    if (!plan.is_death) {
        plan.presentation_resource_argument=
            global_resource_base_258c0+
            static_cast<std::uint32_t>(npc_definition_word_41);
        return plan;
    }

    // E32A..E4C9: original strict type dispatch in order.
    // Type 18 has an external C1DC(3Bh)/level 19/E0BC/
    // 1C328(1,3,0,0) path that can RETURN EARLY. It is
    // deliberately marked unresolved, not forced generic.
    switch (npc_type) {
        case 0x18U:
            plan.special=
                CombatNpcDeathSpecialBranch::Type18ConditionalRespawn;
            plan.type18_requires_external_branch_evidence=true;
            break;
        case 0x21U:
            plan.special=CombatNpcDeathSpecialBranch::Type21Special;
            break;
        case 0x12U:
            plan.special=CombatNpcDeathSpecialBranch::Type12Script20;
            break;
        case 0x09U:
            plan.special=
                CombatNpcDeathSpecialBranch::Type9FourQuestDrops;
            plan.drops={{
                {0x88U,0x006EU},
                {0x87U,0x0078U},
                {0xE9U,0x0082U},
                {0x74U,0x008CU},
            }};
            plan.drop_count=4U;
            plan.set_story_dword_bf44=true;
            break;
        case 0x50U:
            plan.special=CombatNpcDeathSpecialBranch::Type50Script43;
            break;
        case 0x08U:
            plan.special=
                CombatNpcDeathSpecialBranch::Type8ExtendedSequence;
            plan.drops[0]={0x74U,0x80U};
            plan.drop_count=1U;
            break;
        default:
            break;
    }
    // E4C9..E54B is the common post-death presentation.
    // For type18 the caller must first resolve external
    // early-return conditions; this field is CONDITIONAL.
    plan.presentation_resource_argument=
        global_resource_base_258bc+
        static_cast<std::uint32_t>(npc_definition_word_43);
    plan.calls_1dda4=true;
    return plan;
}

std::uint32_t CombatNpcDeathFollowOnModel::death_runtime_flags(
    std::uint16_t npc_type,
    std::uint32_t flags_after_3acdc,
    const std::array<std::int16_t,3U>& tracked_types_258aa,
    const std::array<bool,3U>& actual_e0f8_successes) noexcept {
    // E50F MOVSX type at 258AA+2*i; E516 MOVZX live NPC type.
    // A matching slot with successful E0F8 bypasses the OR.
    for (std::size_t i=0U;i<3U;++i) {
        if (static_cast<std::int32_t>(tracked_types_258aa[i])==
                static_cast<std::int32_t>(npc_type) &&
            actual_e0f8_successes[i]) {
            return flags_after_3acdc;
        }
    }
    // E539 OR BYTE [NPC+0B],02h = DWORD bit 25.
    return flags_after_3acdc|0x02000000U;
}

} // namespace rloft
