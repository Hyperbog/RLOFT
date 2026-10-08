#include "rloft/combat_npc_death_followon.hpp"

#include <array>
#include <cassert>
#include <cstdint>

int main() {
    using rloft::CombatNpcDeathFollowOnModel;
    using rloft::CombatNpcDeathSpecialBranch;
    const auto plan=[&](unsigned type,int hp) {
        return CombatNpcDeathFollowOnModel::inspect(
            static_cast<std::uint16_t>(type),
            static_cast<std::int16_t>(hp),
            5U,22U,200U,0x1000U,0x2000U);
    };

    // Original E553 damage reaction runs for HP>0 regardless
    // of creature type; do not accidentally perform death drops.
    for(const int hp:{1,2,32767}) {
        const auto p=plan(9U,hp);
        assert(!p.is_death);
        assert(p.special==CombatNpcDeathSpecialBranch::None);
        assert(p.presentation_resource_argument==0x2005U);
        assert(!p.calls_1dda4 && !p.set_story_dword_bf44);
        assert(p.drop_count==0U);
    }

    // E3C9..E443 exact type9 four-drop call order to 3195C.
    for(const int hp:{0,-1,-32768}) {
        const auto p=plan(9U,hp);
        assert(p.is_death);
        assert(p.special==
            CombatNpcDeathSpecialBranch::Type9FourQuestDrops);
        assert(p.calls_1dda4 && p.definition_word_23==200U);
        assert(p.presentation_resource_argument==0x1016U);
        assert(p.set_story_dword_bf44 && p.drop_count==4U);
        constexpr std::uint16_t ids[4]={0x88U,0x87U,0xE9U,0x74U};
        constexpr std::uint16_t seeds[4]={0x6EU,0x78U,0x82U,0x8CU};
        for(unsigned i=0;i<4U;++i) {
            assert(p.drops[i].object_id==ids[i]);
            assert(p.drops[i].screen_seed==seeds[i]);
        }
    }

    // Separate type branches trigger different original calls.
    const struct {
        unsigned type;
        CombatNpcDeathSpecialBranch special;
        unsigned count;
    } cases[]={
        {0x18U,CombatNpcDeathSpecialBranch::Type18ConditionalRespawn,0U},
        {0x21U,CombatNpcDeathSpecialBranch::Type21Special,0U},
        {0x12U,CombatNpcDeathSpecialBranch::Type12Script20,0U},
        {0x50U,CombatNpcDeathSpecialBranch::Type50Script43,0U},
        {0x08U,CombatNpcDeathSpecialBranch::Type8ExtendedSequence,1U},
        {0x07U,CombatNpcDeathSpecialBranch::None,0U},
        {0x51U,CombatNpcDeathSpecialBranch::None,0U},
    };
    for(const auto& c:cases) {
        const auto p=plan(c.type,0);
        assert(p.is_death && p.special==c.special);
        assert(p.drop_count==c.count);
        assert(!p.set_story_dword_bf44);
        if(c.type==0x08U) {
            assert(p.drops[0].object_id==0x74U);
            assert(p.drops[0].screen_seed==0x80U);
        }
        assert(p.type18_requires_external_branch_evidence==
               (c.type==0x18U));
    }

    // Zero WORD+41 / WORD+43 passes the respective BASE, not
    // the previous presentation resource value.
    const auto a=CombatNpcDeathFollowOnModel::inspect(
        9U,3,0U,0U,42U,7000U,8000U);
    assert(a.presentation_resource_argument==8000U);
    const auto d=CombatNpcDeathFollowOnModel::inspect(
        9U,0,0U,0U,42U,7000U,8000U);
    assert(d.presentation_resource_argument==7000U);

    // E50F MOVSX tracker type: -1 must not match unsigned
    // NPC type FFFFh; only actual matching types call E0F8.
    const std::array<std::int16_t,3> tracked{{-1,9,80}};
    const std::array<bool,3> success{{true,false,true}};
    const auto flags=0xD0000042U;
    assert(CombatNpcDeathFollowOnModel::death_runtime_flags(
        9U,flags,tracked,success)==(flags|0x02000000U));
    assert(CombatNpcDeathFollowOnModel::death_runtime_flags(
        80U,flags,tracked,success)==flags);
    assert(CombatNpcDeathFollowOnModel::death_runtime_flags(
        0xFFFFU,flags,tracked,success)==(flags|0x02000000U));
    const std::array<bool,3> succeed_second{{false,true,false}};
    assert(CombatNpcDeathFollowOnModel::death_runtime_flags(
        9U,flags,tracked,succeed_second)==flags);
    // Duplicate tracked entries: original scan stops at the
    // FIRST true E0F8 result, not the first matching type.
    const std::array<std::int16_t,3> duplicates{{9,9,9}};
    const std::array<bool,3> last_true{{false,false,true}};
    assert(CombatNpcDeathFollowOnModel::death_runtime_flags(
        9U,flags,duplicates,last_true)==flags);
    const std::array<bool,3> none{{false,false,false}};
    assert(CombatNpcDeathFollowOnModel::death_runtime_flags(
        9U,flags,duplicates,none)==(flags|0x02000000U));
}
