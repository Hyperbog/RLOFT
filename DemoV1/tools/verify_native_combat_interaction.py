#!/usr/bin/env python3
"""Native PC interaction safety, independent music/SFX toggles and CTest wiring.

Does NOT claim original DOS DB94 auto-target or 24B48/A7E4 cooldown.
"""
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]

def check(arming:str,host:str,tests:str,options:str,option_tests:str,
          cmake:str,workflow:str,audio:str,audio_header:str)->None:
    for marker in (
        'world_revision==other.world_revision',
        'level_id==other.level_id',
        'coordinate_a==other.coordinate_a',
        'coordinate_b==other.coordinate_b',
        'facing==other.facing',
        'void cancel() noexcept { armed_.reset(); }',
        'const auto previous=*armed_;',
        'armed_.reset();',
        'return {true,frame && previous.frame.matches(*frame),',
    ):
        if marker not in arming:
            raise ValueError('scene-locked combat interaction lost: '+marker)
    take=arming[arming.index('EnhancedCombatTargetAttempt take('):]
    if not(take.index('armed_.reset();') <
           take.index('return {true,frame && previous.frame.matches(*frame),')):
        raise ValueError('the armed action was not reset in take()')
    for marker in (
        '#include "rloft/enhanced_combat_target_arming.hpp"',
        'rloft::EnhancedCombatTargetArming pending_level22_weapon_hand;',
        'combat_scene_stamp(*displayed_scene_frame)',
        'pending_level22_weapon_hand.arm(',
        'pending_level22_weapon_hand.pending()',
        'pending_level22_weapon_hand.take(',
        'pending_level22_weapon_hand.cancel();',
        'attacked_assassin=true;',
        '!attacked_assassin &&',
        'L"Ravenloft - No Assassin at that target"',
        'rloft::EnhancedOptionsSelection::ToggleSound',
        'gameplay_sound_enabled=!gameplay_sound_enabled;',
        'audio_player.set_enabled(gameplay_sound_enabled);',
        'audio_player.set_enabled(true);',
        'if(outcome.injury && gameplay_sound_enabled)',
    ):
        if marker not in host:
            raise ValueError('unsafe host combat or SFX toggle: '+marker)
    start=host.index('bool attacked_assassin=false;')
    finish=host.index('runtime.pickup_and_advance_level22_at(',start)
    branch=host[start:finish]
    if not (branch.index('attacked_assassin=true;') <
            branch.index('pending_level22_weapon_hand.take(') <
            branch.index('runtime.attack_level22_assassin_at(') <
            branch.index('!attacked_assassin &&')):
        raise ValueError('combat click was not consumed before pickup')
    # Original modal screen or menu resets any armed source click.
    if 'if (pressed_menu >= 0) {\n                pending_level22_weapon_hand.cancel();' not in host:
        raise ValueError('top-menu actions retained an armed attack')
    if 'if(ui.state().screen!=rloft::ExplorationUiScreen::Viewport) {\n            pending_level22_weapon_hand.cancel();' not in host:
        raise ValueError('modal screen retained an armed attack')
    for marker in (
        'assert(stale.consumed && !stale.valid);',
        'assert(!state.take(changed).consumed);',
        'assert(no_frame.consumed && !no_frame.valid);',
        'assert(!state.pending());',
        'assert(!state.take(seen).consumed);',
    ):
        if marker not in tests:
            raise ValueError('stale attack/once-only test missing: '+marker)
    for marker in (
        'None, ToggleMusic, ToggleSound, ReturnToGame',
        'EnhancedOptionsSelection::ToggleSound',
        'sound_enabled?"SOUND: ON":"SOUND: OFF"',
        'music_enabled?"MUSIC: ON":"MUSIC: OFF"',
    ):
        if marker not in options:
            raise ValueError('separate music/sound native button removed: '+marker)
    for marker in (
        'assert(EnhancedOptionsPresenter::action_at(211,52)==',
        'assert(EnhancedOptionsPresenter::action_at(249,267)==',
        'EnhancedOptionsSelection::ToggleSound',
    ):
        if marker not in option_tests:
            raise ValueError('SFX hit-region test removed: '+marker)
    for marker in (
        'bool effects_enabled{true};',
        'if(!effects_enabled)return;',
        'effects_enabled=enabled;',
        'if(!enabled)stop_all();',
        'impl_->set_enabled(enabled);',
    ):
        if marker not in audio:
            raise ValueError('global Win32 sampled-audio mute bypassed: '+marker)
    if 'void set_enabled(bool enabled);' not in audio_header:
        raise ValueError('shared Windows audio mute API is unavailable')
    for marker in (
        'add_executable(enhanced_combat_target_arming_tests',
        'add_test(NAME enhanced_combat_target_arming_tests',
        'NAME enhanced_native_combat_interaction_contract_tests',
    ):
        if marker not in cmake:
            raise ValueError('native C++ test/contract not registered: '+marker)
    for marker in (
        'combat_live_hand_request_tests enhanced_combat_target_arming_tests',
        '|combat_live_hand_request_tests|enhanced_combat_target_arming_tests|',
        'python DemoV1/tools/verify_native_combat_interaction.py',
        '|enhanced_native_combat_interaction_contract_tests)$"',
    ):
        if marker not in workflow:
            raise ValueError('Windows quick build missing combat-state test: '+marker)

def main()->None:
    paths=(
        'DemoV1/include/rloft/enhanced_combat_target_arming.hpp',
        'DemoV1/src/win32_main.cpp',
        'DemoV1/tests/enhanced_combat_target_arming_tests.cpp',
        'DemoV1/include/rloft/enhanced_options_presenter.hpp',
        'DemoV1/tests/enhanced_options_presenter_tests.cpp',
        'DemoV1/CMakeLists.txt',
        '.github/workflows/demo-v1-build.yml',
        'DemoV1/src/win32_audio.cpp',
        'DemoV1/include/rloft/win32_audio.hpp',
    )
    originals=[(ROOT/p).read_text(encoding='utf-8') for p in paths]
    check(*originals)
    mutations=(
        (0,'return {true,frame && previous.frame.matches(*frame),',
           'return {true,true,'),
        (0,'armed_.reset();\n        return {true,',
           '/* infinite armed attacks */\n        return {true,'),
        (1,'pending_level22_weapon_hand.take(',
           'pending_level22_weapon_hand.pending('),
        (1,'!attacked_assassin &&','true &&'),
        (1,'if (pressed_menu >= 0) {\n                pending_level22_weapon_hand.cancel();',
           'if (pressed_menu >= 0) {'),
        (1,'gameplay_sound_enabled=!gameplay_sound_enabled;',
           'gameplay_sound_enabled=true;'),
        (1,'if(outcome.injury && gameplay_sound_enabled)',
           'if(outcome.injury)'),
        (2,'assert(stale.consumed && !stale.valid);',
           'assert(stale.valid);'),
        (3,'sound_enabled?"SOUND: ON":"SOUND: OFF"',
           'sound_enabled?"MUSIC: ON":"MUSIC: OFF"'),
        (4,'assert(EnhancedOptionsPresenter::action_at(211,52)==',
           'assert(EnhancedOptionsPresenter::action_at(152,52)=='),
        (5,'add_executable(enhanced_combat_target_arming_tests',
           'add_executable(missing_combat_target_tests'),
        (6,'combat_live_hand_request_tests enhanced_combat_target_arming_tests',
           'combat_live_hand_request_tests'),
        (7,'if(!effects_enabled)return;',
           '/* mute skipped */'),
        (7,'effects_enabled=enabled;',
           'effects_enabled=true;'),
        (7,'if(!enabled)stop_all();',
           '/* left effects playing */'),
        (8,'void set_enabled(bool enabled);',
           'void unused_mute(bool enabled);'),
    )
    for index,old,new in mutations:
        if old not in originals[index]:
            raise ValueError('negative mutation fixture absent: '+old)
        altered=list(originals)
        altered[index]=altered[index].replace(old,new,1)
        try:check(*altered)
        except ValueError:continue
        raise ValueError('combat/SFX regression escaped: '+old)
    print('ENHANCED_NATIVE_COMBAT_INTERACTION_PASS (16 mutations rejected)')

if __name__=='__main__':
    main()
