#!/usr/bin/env python3
"""Bounded original CODE.2 214B8/24478/26018 hand gate in the live game.

It is not the unfinished E5F0 hit/RNG/damage/cooldown dispatcher.
Empty-hand OBJECTS[-1] heap memory is never invented.
"""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]

def check(model:str,host:str,tests:str,cmake:str,workflow:str)->None:
    for required in (
        'CombatHandDispatchModel::inspect(',
        'if(hand_item(0U)==-1 || hand_item(1U)==-1)',
        'CombatLiveHandStatus::MissingOriginalEmptyHandData',
        'CombatHandPairEligibilityModel::evaluate(',
        'CombatSingleHandContext{level.header[0x290U]}',
        'out.pair_checked=true;',
        'out.pair_selected_eligible=pair.selected_eligible;',
        'if(!pair.selected_eligible)',
        'out.status=CombatLiveHandStatus::PairVetoed;',
        'out.status=CombatLiveHandStatus::AttackCandidate;',
    ):
        if required not in model:
            raise ValueError('live original hand legality lost: '+required)
    if not (model.index('CombatHandDispatchModel::inspect(') <
            model.index('if(hand_item(0U)==-1 || hand_item(1U)==-1)') <
            model.index('CombatHandPairEligibilityModel::evaluate(') <
            model.index('out.status=CombatLiveHandStatus::AttackCandidate;')):
        raise ValueError('source hand action gate ordering changed')
    for required in (
        '#include "rloft/combat_live_hand_request.hpp"',
        'gameplay_hand_at(click->row,click->column)',
        'handled_hand_click=true;',
        'CombatLiveHandRequestModel::inspect(',
        'runtime.session().level,',
        'case HandStatus::MissingOriginalEmptyHandData:',
        'case HandStatus::PairVetoed:',
        'case HandStatus::AttackCandidate:',
        'trace->event("combat.hand.gate",',
        '!opened_actor_panel && !handled_hand_click &&',
    ):
        if required not in host:
            raise ValueError('primary hand action not wired in the game: '+required)
    if 'damage_level22_assassin(' in model:
        raise ValueError('nonattack preflight secretly mutates NPC HP')
    for required in (
        'plan.status==S::AttackCandidate',
        'plan.status==S::PairVetoed',
        'plan.status==S::MissingOriginalEmptyHandData',
        'assert(!plan.pair_checked && !plan.may_enter_player_attack());',
        'plan.status==S::ActorBlocked',
        'plan.status==S::ActionBusy',
        'plan.status==S::CategoryEAction',
        'plan.status==S::SpecialAction',
    ):
        if required not in tests:
            raise ValueError('source live hand gate tests missing: '+required)
    for required in (
        'src/combat_live_hand_request.cpp',
        'add_executable(combat_live_hand_request_tests',
        'add_test(NAME combat_live_hand_request_tests',
        'NAME enhanced_live_hand_dispatch_contract_tests',
    ):
        if required not in cmake:
            raise ValueError('native player executable not gated: '+required)
    for required in (
        'combat_player_hit_inputs_tests combat_xp_award_service_tests combat_level22_strike_tests original_npc_sound_tests combat_live_hand_request_tests',
        'python DemoV1/tools/verify_game_live_hand_dispatch.py',
        '|enhanced_live_hand_dispatch_contract_tests|',
    ):
        if required not in workflow:
            raise ValueError('normal Windows tests omit live hand gate: '+required)

def main()->None:
    paths=(
        'DemoV1/src/combat_live_hand_request.cpp',
        'DemoV1/src/win32_main.cpp',
        'DemoV1/tests/combat_live_hand_request_tests.cpp',
        'DemoV1/CMakeLists.txt',
        '.github/workflows/demo-v1-build.yml',
    )
    src=[(ROOT/p).read_text(encoding='utf-8') for p in paths]
    check(*src)
    mutations=(
        (0,'level.header[0x290U]','level.header[0x291U]'),
        (0,'if(hand_item(0U)==-1 || hand_item(1U)==-1)',
           'if(false)'),
        (0,'out.status=CombatLiveHandStatus::PairVetoed;',
           'out.status=CombatLiveHandStatus::AttackCandidate;'),
        (1,'!opened_actor_panel && !handled_hand_click &&',
           '!opened_actor_panel &&'),
        (1,'CombatLiveHandRequestModel::inspect(',
           'CombatHandDispatchModel::inspect('),
        (2,'assert(!plan.pair_checked && !plan.may_enter_player_attack());',
           'assert(plan.pair_checked && plan.may_enter_player_attack());'),
        (3,'src/combat_live_hand_request.cpp',
           'src/missing_live_hand_request.cpp'),
        (4,'combat_player_hit_inputs_tests combat_xp_award_service_tests combat_level22_strike_tests original_npc_sound_tests combat_live_hand_request_tests',
           'combat_player_hit_inputs_tests combat_hand_dispatch_tests'),
    )
    for index,old,new in mutations:
        if old not in src[index]:
            raise ValueError('negative case no longer exists: '+old)
        changed=list(src)
        changed[index]=changed[index].replace(old,new,1)
        try:check(*changed)
        except ValueError:continue
        raise ValueError('live hand mutation escaped guard: '+old)
    print('ENHANCED_LIVE_HAND_DISPATCH_PASS (8 mutations rejected)')

if __name__=='__main__':
    main()
