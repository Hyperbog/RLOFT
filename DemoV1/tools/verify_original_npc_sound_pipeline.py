#!/usr/bin/env python3
"""Original NPCS+41/+43 sound selection and exact INIT bank bridge.

No claim of native equivalence to DOS 28110 channel/priority scheduling;
the adapter plays the correct authored RES6 VOC immediately.
"""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]

def check(resolver:str,host:str,tests:str,cmake:str,workflow:str)->None:
    for marker in (
        'result.sound_kind==NpcSoundKind::None',
        'NpcSoundKind::DamageReaction?0x14U:0x16U',
        'base+result.sound_id;',
        'std::numeric_limits<std::uint16_t>::max()',
        'return static_cast<std::uint16_t>(res6_id);',
    ):
        if marker not in resolver:
            raise ValueError('original NPC sound source base changed: '+marker)
    for marker in (
        '#include "rloft/original_npc_sound.hpp"',
        'if(outcome.injury)',
        'OriginalNpcSoundModel::res6_id(',
        'assets.source().read("INIT")',
        'assets.sampled_audio(',
        'audio_player.play(',
        '"combat.level22.audio"',
    ):
        if marker not in host:
            raise ValueError('live creature hit/death audio unwired: '+marker)
    # SFX must happen ONLY after the original attack has committed.
    main=host[host.index('runtime.attack_level22_assassin_at('):
              host.index('}catch(const std::exception& error) {',
                         host.index('runtime.attack_level22_assassin_at('))]
    if not(main.index('if(outcome.found_assassin)') <
           main.index('if(outcome.injury)') <
           main.index('OriginalNpcSoundModel::res6_id(')):
        raise ValueError('audio requested before injury or successful world commit')
    if 'catch(const std::exception& audio_error)' not in main:
        raise ValueError('failed audio must not masquerade as failed combat')
    for marker in (
        'event.sound_kind=rloft::NpcSoundKind::DamageReaction;',
        'event.sound_kind=rloft::NpcSoundKind::Death;',
        'event.sound_id=0U;',
        'res6_id(init,event)==69U',
        'res6_id(init,event)==165U',
        'res6_id(init,event)==166U',
        'res6_id(init,event)==256U',
        'res6_id(init,event)==99U',
    ):
        if marker not in tests:
            raise ValueError('NPC sound bank/source fixture missing: '+marker)
    for marker in (
        'src/original_npc_sound.cpp',
        'add_executable(original_npc_sound_tests',
        'add_test(NAME original_npc_sound_tests',
        'NAME enhanced_npc_sound_source_contract_tests',
    ):
        if marker not in cmake:
            raise ValueError('native player missing original NPC sound: '+marker)
    for marker in (
        'combat_level22_strike_tests original_npc_sound_tests combat_live_hand_request_tests',
        '|combat_level22_strike_tests|original_npc_sound_tests|combat_live_hand_request_tests|',
        'python DemoV1/tools/verify_original_npc_sound_pipeline.py',
        '|enhanced_npc_sound_source_contract_tests|',
    ):
        if marker not in workflow:
            raise ValueError('Windows quick build excludes NPC audio: '+marker)

def main()->None:
    paths=(
        'DemoV1/src/original_npc_sound.cpp',
        'DemoV1/src/win32_main.cpp',
        'DemoV1/tests/original_npc_sound_tests.cpp',
        'DemoV1/CMakeLists.txt',
        '.github/workflows/demo-v1-build.yml',
    )
    originals=[(ROOT/p).read_text(encoding='utf-8') for p in paths]
    check(*originals)
    mutations=(
        (0,'NpcSoundKind::DamageReaction?0x14U:0x16U',
           'NpcSoundKind::DamageReaction?0x16U:0x14U'),
        (0,'base+result.sound_id;','base;'),
        (0,'result.sound_kind==NpcSoundKind::None','false'),
        (1,'OriginalNpcSoundModel::res6_id(',
           '/* NPC sound disabled */('),
        (1,'catch(const std::exception& audio_error)',
           'if(false)'),
        (2,'res6_id(init,event)==166U',
           'res6_id(init,event)==0U'),
        (3,'src/original_npc_sound.cpp',
           'src/missing_sound.cpp'),
        (4,'combat_level22_strike_tests original_npc_sound_tests combat_live_hand_request_tests',
           'combat_level22_strike_tests combat_live_hand_request_tests'),
    )
    for index,old,new in mutations:
        if old not in originals[index]:
            raise ValueError('negative mutation anchor missing: '+old)
        altered=list(originals)
        altered[index]=altered[index].replace(old,new,1)
        try:check(*altered)
        except ValueError:continue
        raise ValueError('unsafe original NPC sound mutation escaped: '+old)
    print('ORIGINAL_NPC_SOUND_PIPELINE_PASS (8 mutations rejected)')

if __name__=='__main__':
    main()
