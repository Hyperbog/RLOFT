#!/usr/bin/env python3
"""Source contract: native player-hit inputs must come from original CODE.2.
This checks wiring, NOT a completed 214B8/24478/E5F0 live attack action.
"""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]

def check(strength:str,inputs:str,test:str,cmake:str,workflow:str)->None:
    for marker in (
        '0x75F24U+0x6832U',
        'score(party.bytes[at+0x33U],party.bytes[at+0x3FU])',
        'if(strength==18U && exceptional!=0U)',
        'if(exceptional<=50U)return 1;',
        'if(exceptional<=99U)return 2;',
        'raw<128 ? raw : raw-256',
        'throw std::out_of_range(',
    ):
        if marker not in strength:
            raise ValueError('original C7C4 Strength rule missing: '+marker)
    for marker in (
        'if((in.live_npc_runtime_flags&0x08U)!=0U)return in;',
        'in.base_attack_target=rec[0x10AU];',
        'in.npc_armor_class=definition.armor_class;',
        'in.strength_attack_adjustment=strength.inspect(party,actor);',
        'in.burden_rank=burden.actor(party,inventory,objects,actor).burden_rank;',
        'in.weapon_enchantment=objects.at(',
        'const auto primary_effects=rec[0xF3U];',
        'in.bless=(primary_effects&0x01U)!=0U;',
        'in.aid=(primary_effects&0x08U)!=0U;',
        'in.stone_giant_gauntlets=item_word(rec,20U)==0x54;',
        'in.raw_d20=raw_d20;',
    ):
        if marker not in inputs:
            raise ValueError('player hit source input missing: '+marker)
    if 'OriginalRandom::' in inputs or 'damage_assassin(' in inputs:
        raise ValueError('pure C80C input projection consumes RNG or mutates NPC')
    for marker in (
        'CombatPlayerHitInputsModel::build(',
        'CombatHitCheckModel::resolve(in)',
        'str.score(18U,50U)==1',
        'str.score(18U,51U)==2',
        'str.score(18U,100U)==3',
        'str.score(25U,0U)==7',
        'hit.threshold==13',
        'hit.modified_d20==20 && hit.hit',
    ):
        if marker not in test:
            raise ValueError('native combat hit regression missing: '+marker)
    for marker in (
        'src/combat_strength_attack.cpp',
        'src/combat_player_hit_inputs.cpp',
        'add_executable(combat_player_hit_inputs_tests',
        'add_test(NAME combat_player_hit_inputs_tests',
        'NAME enhanced_combat_input_source_contract_tests',
    ):
        if marker not in cmake:
            raise ValueError('player combat model not in native game build: '+marker)
    for marker in (
        'combat_hit_check_tests combat_player_hit_inputs_tests',
        '|combat_hit_check_tests|combat_player_hit_inputs_tests|',
        'python DemoV1/tools/verify_game_combat_inputs.py',
    ):
        if marker not in workflow:
            raise ValueError('combat source not gated by Windows CI: '+marker)

def main():
    paths=(
        'DemoV1/src/combat_strength_attack.cpp',
        'DemoV1/src/combat_player_hit_inputs.cpp',
        'DemoV1/tests/combat_player_hit_inputs_tests.cpp',
        'DemoV1/CMakeLists.txt',
        '.github/workflows/demo-v1-build.yml',
    )
    originals=[(ROOT/p).read_text(encoding='utf-8') for p in paths]
    check(*originals)
    mutated=(
        (0,'0x75F24U+0x6832U','0x75F24U+0x684BU'),
        (0,'if(exceptional<=50U)return 1;','if(exceptional<=75U)return 1;'),
        (1,'in.burden_rank=burden.actor(party,inventory,objects,actor).burden_rank;',
           'in.burden_rank=0;'),
        (1,'in.stone_giant_gauntlets=item_word(rec,20U)==0x54;',
           'in.stone_giant_gauntlets=false;'),
        (1,'if((in.live_npc_runtime_flags&0x08U)!=0U)return in;',
           '/* lost hold auto-hit */'),
        (3,'src/combat_player_hit_inputs.cpp','src/missing_player_hit_inputs.cpp'),
        (4,'combat_hit_check_tests combat_player_hit_inputs_tests',
           'combat_hit_check_tests'),
    )
    for index,old,new in mutated:
        if old not in originals[index]:
            raise ValueError('mutation fixture absent: '+old)
        altered=list(originals)
        altered[index]=altered[index].replace(old,new,1)
        try:check(*altered)
        except ValueError:continue
        raise ValueError('native combat regression escaped: '+old)
    print('ENHANCED_COMBAT_INPUT_SOURCE_PASS (7 negative mutations rejected)')

if __name__=='__main__':
    main()
