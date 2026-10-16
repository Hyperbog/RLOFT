#!/usr/bin/env python3
"""Source contract for first real native weapon->Assassin HP/death path.

The native explicit two-click target is an ENHANCED input adapter to
E5F0's source-supported explicit target, not original DB94 targeting.
The still-missing 24B48 action clock, XP level-up and effect graphics
must never be silently declared complete.
"""
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]

def check(strike:str,runtime:str,host:str,tests:str,cmake:str,workflow:str)->None:
    for marker in (
        'CombatAttackStatusService::apply(',
        'const bool held=(npc.runtime_flags&0x08U)!=0U;',
        'random.dice(1U,20U)',
        'CombatPlayerHitInputsModel::build(',
        'CombatHitCheckModel::resolve(inputs);',
        'if(!result.hit.hit)return result;',
        'const auto initial=random.dice(',
        'CombatWeaponDamageGateModel::inspect(',
        'definition.behavior_flags',
        'result.damage_after_c910=',
        'item_id==0x4A || (item_id>=0x7E && item_id<=0x86)',
        'CombatDamageStatBonusModel::inspect(party,actor)',
        'CombatDamageDiceModel::inspect_for_actor(',
        'weapon_at(party,actor,hand)',
    ):
        if marker not in strike:
            raise ValueError('original E5F0 damage stage lost: '+marker)
    if not (strike.index('CombatAttackStatusService::apply(') <
            strike.index('random.dice(1U,20U)') <
            strike.index('CombatHitCheckModel::resolve(inputs);') <
            strike.index('const auto initial=random.dice(') <
            strike.index('CombatWeaponDamageGateModel::inspect(')):
        raise ValueError('source E578/C80C/1C328/C910 call ordering wrong')
    for marker in (
        'combat_strength_source_=',
        'CombatStrengthAttackModel::from_code2(code2);',
        'combat_burden_source_=',
        'InventoryEncumbranceModel::from_code2(code2);',
        'frame.world_scene_revision!=world_scene_revision_',
        'world_scene_target_at(',
        'Level22ViewportTargetModel::assassin_index(',
        'CombatLiveHandRequestModel::inspect(',
        'if(!hand_plan.may_enter_player_attack())',
        'ExplorationRuntime staged=*this;',
        'CombatLevel22StrikeModel::resolve(',
        'staged.original_random_,actor,hand,',
        'if(strike.hit.hit)',
        'staged.damage_level22_assassin(',
        '*index,strike.damage_after_c910,0U,',
        'placement_passable_2d5d0(',
        '*this=std::move(staged);',
    ):
        if marker not in runtime:
            raise ValueError('real game hit/world bridge lost: '+marker)
    begin=runtime.index('ExplorationRuntime::attack_level22_assassin_at(')
    end=runtime.index('InventoryPartyOrderResult ExplorationRuntime::exchange_party_order(',
                      begin)
    part=runtime[begin:end]
    if not (part.index('world_scene_target_at(') <
            part.index('CombatLiveHandRequestModel::inspect(') <
            part.index('ExplorationRuntime staged=*this;') <
            part.index('CombatLevel22StrikeModel::resolve(') <
            part.index('staged.damage_level22_assassin(') <
            part.index('*this=std::move(staged);')):
        raise ValueError('original world identity or atomic attack order lost')
    for marker in (
        'pending_level22_weapon_hand;',
        'pending_level22_weapon_hand=*hand;',
        'runtime.attack_level22_assassin_at(',
        'outcome.found_assassin',
        'outcome.strike.hit.hit',
        'outcome.injury->died',
        'trace->event(',
        'combat.level22.strike',
        '!attacked_assassin',
    ):
        if marker not in host:
            raise ValueError('live user attack click is not connected: '+marker)
    for marker in (
        'assert(hit.hit.hit);',
        'assert(miss.raw_d20==1U);',
        'assert(miss_rng.state()==single.state());',
        'assert(held_hit.hit.auto_hit_held);',
        'assert(held_rng.state()==one_die.state());',
        'assert(restricted.damage_after_c910==1U);',
        'assert(killed.died);',
        'assert(killed.spawned_loose_items==4U);',
        'assert(world.loose_items[0].object_type_id==0x88U);',
        'assert(world.loose_items[3].object_type_id==0x74U);',
        'assert(failed && invalid_rng.state()==1U);
    ):
        if marker not in tests:
            raise ValueError('essential RNG/HP branch regression absent: '+marker)
    for marker in (
        'src/combat_level22_strike.cpp',
        'add_executable(combat_level22_strike_tests',
        'add_test(NAME combat_level22_strike_tests',
        'NAME enhanced_live_level22_weapon_strike_contract_tests',
    ):
        if marker not in cmake:
            raise ValueError('native C++ build missing source attack: '+marker)
    for marker in (
        'combat_level22_strike_tests',
        'python DemoV1/tools/verify_live_level22_weapon_strike.py',
        '|enhanced_live_level22_weapon_strike_contract_tests)$"',
    ):
        if marker not in workflow:
            raise ValueError('Windows quick run missing player attack: '+marker)

def main()->None:
    paths=(
        'DemoV1/src/combat_level22_strike.cpp',
        'DemoV1/src/exploration_runtime.cpp',
        'DemoV1/src/win32_main.cpp',
        'DemoV1/tests/combat_level22_strike_tests.cpp',
        'DemoV1/CMakeLists.txt',
        '.github/workflows/demo-v1-build.yml',
    )
    sources=[(ROOT/p).read_text(encoding='utf-8') for p in paths]
    check(*sources)
    mutations=(
        (0,'if(!result.hit.hit)return result;','/* roll damage even on miss */'),
        (0,'const bool held=(npc.runtime_flags&0x08U)!=0U;',
           'const bool held=false;'),
        (0,'definition.behavior_flags','0U'),
        (0,'CombatAttackStatusService::apply(','/* skip E578 */('),
        (1,'*index,strike.damage_after_c910,0U,',
           '*index,1U,0U,'),
        (1,'ExplorationRuntime staged=*this;\n    auto& npc=staged.world_.npcs.at(*index);',
           'ExplorationRuntime& staged=*this;\n    auto& npc=staged.world_.npcs.at(*index);'),
        (1,'Level22ViewportTargetModel::assassin_index(',
           '/* fake NPC target */('),
        (2,'runtime.attack_level22_assassin_at(',
           'runtime.damage_level22_assassin_at('),
        (2,'!attacked_assassin','true'),
        (3,'assert(miss_rng.state()==single.state());',
           'assert(miss_rng.state()!=single.state());'),
        (3,'assert(killed.spawned_loose_items==4U);',
           'assert(killed.spawned_loose_items==0U);'),
        (4,'src/combat_level22_strike.cpp',
           'src/missing_combat_level22_strike.cpp'),
        (5,'combat_level22_strike_tests',
           'combat_hit_check_tests'),
    )
    for index,old,new in mutations:
        if old not in sources[index]:
            raise ValueError('source-negative fixture absent: '+old)
        altered=list(sources)
        altered[index]=altered[index].replace(old,new,1)
        try:check(*altered)
        except ValueError:continue
        raise ValueError('unsafe real combat change escaped: '+old)
    print('LIVE_LEVEL22_WEAPON_STRIKE_PASS (13 mutations rejected)')

if __name__=='__main__':
    main()
