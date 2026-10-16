#!/usr/bin/env python3
"""Original DDA4 class-track XP applied to the native Assassin kill.

This gate does NOT claim that 50DC/3EA4/3DE4/DC7C level-up
callbacks have been implemented; those remain explicitly pending.
"""
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]

def check(service:str,runtime:str,host:str,unit:str,cmake:str,workflow:str,
          header:str,saves:str)->None:
    for marker in (
        'CombatXpDistributionModel::inspect(',
        'CombatXpLevelModel::inspect(',
        'if(!shares)return std::nullopt;',
        'if(!projection)return std::nullopt;',
        'projection->has_unexecuted_level_change_services;',
        'base+0x26U+lane*4U',
        'award.projected_track_progress[lane]',
        '++result.updated_tracks;',
    ):
        if marker not in service:
            raise ValueError('authentic DDA4 XP arithmetic/write lost: '+marker)
    if not (service.index('CombatXpDistributionModel::inspect(') <
            service.index('CombatXpLevelModel::inspect(') <
            service.index('write_dword(party,base+0x26U+lane*4U')):
        raise ValueError('DDA4 wrote XP before all actor/class preflights')
    for marker in (
        'combat_progression_source_=',
        'CombatProgressionTables::from_code2(code2);',
        'if(injury.died) {',
        'if(!staged.combat_progression_source_)',
        'auto awarded=CombatXpAwardService::apply(',
        'staged.session_.party,injury.experience_award,',
        'result.experience=*awarded;',
        '*this=std::move(staged);',
    ):
        if marker not in runtime:
            raise ValueError('live original Assassin XP integration lost: '+marker)
    begin=runtime.index('ExplorationRuntime::attack_level22_assassin_at(')
    end=runtime.index('InventoryPartyOrderResult ExplorationRuntime::exchange_party_order(',begin)
    section=runtime[begin:end]
    if not (section.index('ExplorationRuntime staged=*this;') <
            section.index('staged.damage_level22_assassin(') <
            section.index('if(injury.died) {') <
            section.index('CombatXpAwardService::apply(') <
            section.index('*this=std::move(staged);')):
        raise ValueError('XP/death/relics are no longer one transaction')
    if 'level_change_services_pending' not in host or (
            'L"XP earned; level-up pending"' not in host):
        raise ValueError('level-change side effects falsely reported complete')
    if 'std::optional<CombatXpAwardResult> experience;' not in header:
        raise ValueError('native combat result no longer reports actual XP')
    for marker in (
        'assert(read32(p,0U,0x26U)==200U);',
        'assert(read32(p,1U,0x26U)==151U);',
        'assert(read32(p,1U,0x2AU)==351U);',
        'assert(reward->updated_tracks==3U);',
        'assert(!reward->level_change_services_pending);',
        'assert(reward && reward->level_change_services_pending);',
        'assert(ascending.bytes[0x20U]==1U);',
        'assert(unsupported.bytes==invalid_before);',
        'assert(empty.bytes==empty_bytes);',
        'assert(read32(wrapping,0U,0x26U)==0U);',
    ):
        if marker not in unit:
            raise ValueError('XP rounding/rollback/boundary test lost: '+marker)
    for marker in (
        'assert(quest_loaded.session.party.bytes[0x26U]==0xC8U);',
        'assert(warped.session.party.bytes[0x26U]==0xC8U);',
    ):
        if marker not in saves:
            raise ValueError('original class XP lost on native Save/Continue: '+marker)
    for marker in (
        'src/combat_xp_award_service.cpp',
        'add_executable(combat_xp_award_service_tests',
        'add_test(NAME combat_xp_award_service_tests',
        'NAME enhanced_live_assassin_xp_contract_tests',
    ):
        if marker not in cmake:
            raise ValueError('native XP service not linked or tested: '+marker)
    for marker in (
        'combat_player_hit_inputs_tests combat_xp_award_service_tests combat_level22_strike_tests',
        '|combat_player_hit_inputs_tests|combat_xp_award_service_tests|combat_level22_strike_tests|',
        'python DemoV1/tools/verify_live_assassin_xp.py',
        '|enhanced_live_assassin_xp_contract_tests)$"',
    ):
        if marker not in workflow:
            raise ValueError('XP source service omitted from Windows CI: '+marker)

def main()->None:
    files=(
        'DemoV1/src/combat_xp_award_service.cpp',
        'DemoV1/src/exploration_runtime.cpp',
        'DemoV1/src/win32_main.cpp',
        'DemoV1/tests/combat_xp_award_service_tests.cpp',
        'DemoV1/CMakeLists.txt',
        '.github/workflows/demo-v1-build.yml',
        'DemoV1/include/rloft/exploration_runtime.hpp',
        'DemoV1/tests/enhanced_game_save_tests.cpp',
    )
    src=[(ROOT/file).read_text(encoding='utf-8') for file in files]
    check(*src)
    mutations=(
        (0,'CombatXpLevelModel::inspect(', '/* skip class threshold preflight */('),
        (0,'base+0x26U+lane*4U','base+0x26U'),
        (0,'if(!projection)return std::nullopt;','if(false)return std::nullopt;'),
        (0,'++result.updated_tracks;','/* no XP track count */'),
        (1,'staged.session_.party,injury.experience_award,',
           'staged.session_.party,0U,'),
        (1,'if(injury.died) {','if(false) {'),
        (1,'CombatXpAwardService::apply(', '/* XP only diagnostic */('),
        (2,'L"XP earned; level-up pending"','L"Level up complete"'),
        (3,'assert(unsupported.bytes==invalid_before);',
           'assert(unsupported.bytes!=invalid_before);'),
        (4,'src/combat_xp_award_service.cpp',
           'src/missing_xp_award_service.cpp'),
        (5,'combat_player_hit_inputs_tests combat_xp_award_service_tests combat_level22_strike_tests',
           'combat_player_hit_inputs_tests combat_level22_strike_tests'),
        (6,'std::optional<CombatXpAwardResult> experience;',
           'std::optional<int> experience;'),
        (7,'assert(warped.session.party.bytes[0x26U]==0xC8U);',
           'assert(warped.session.party.bytes[0x26U]==0U);'),
    )
    for index,old,replacement in mutations:
        if old not in src[index]:
            raise ValueError('mutation fixture missing: '+old)
        changed=list(src)
        changed[index]=changed[index].replace(old,replacement,1)
        try:check(*changed)
        except ValueError:continue
        raise ValueError('unsafe XP mutation escaped: '+old)
    print('LIVE_ASSASSIN_XP_PASS (13 negative mutations rejected)')

if __name__=='__main__':
    main()
