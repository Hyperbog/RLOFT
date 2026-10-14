#!/usr/bin/env python3
"""Enhanced PC world-music runtime wiring and frontend isolation guard.

Music ID comes from the authored LevelState header +0x29C (0 is valid).
This test uses public source and synthetic inputs, not original RES5 bytes.
"""
from __future__ import annotations

from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]


def verify(host:str, model:str, cmake:str, workflow:str) -> None:
    start=host.index('int run_exploration(')
    end=host.index('bool code1_palette_reveal(',start)
    world=host[start:end]
    frontend=host[end:]
    required=(
        'rloft::EnhancedWorldMusicState world_music_state;',
        'runtime.session().level_id',
        'runtime.session().level.music_sequence',
        'world_music_state.should_start(level_id,sequence_id)',
        'midi_player.play(',
        'rloft::parse_xmidi_sequence(',
        'assets.music_sequence(',
        'static_cast<int>(sequence_id)',
        '0,100,0x9C4',
        'last_requested_world_music',
        'start_authored_world_music();',
        'world_music_state.invalidate();',
        'OutputDebugStringA(error.what());',
    )
    for r in required:
        if r=='last_requested_world_music':
            if r in world:
                raise ValueError('duplicate legacy non-testable world music state')
        elif r not in world:
            raise ValueError('missing original level music integration: '+r)
    if world.count('start_authored_world_music();')<2:
        raise ValueError('music must start at gameplay entry and check level transitions')
    # The enhanced Options menu adds a third invocation when music is
    # switched back on; no fixed call count should reject new live UI.
    if 'if(gameplay_music_enabled)start_authored_world_music();' not in world:
        raise ValueError('music cannot be resumed from in-game Options')
    if world.count('world_music_state.invalidate();')<2:
        raise ValueError('original CINE08 branches must restore soundtrack')
    if 'gameplay_music_enabled=!gameplay_music_enabled;' not in world:
        raise ValueError('in-game Options music enable toggle is missing')
    if world.index('start_authored_world_music();') > world.index('while (host.pump())'):
        raise ValueError('music must begin before first world frame')
    if world.index('midi_player.play(')>world.index('while (host.pump())'):
        raise ValueError('MIDI parse/player scheduling must not run every 60Hz frame')
    if ('assets.music_sequence(0x18)' not in frontend or
            'midi_player.fade_out_and_stop(0x9C4)' not in frontend):
        raise ValueError('established CODE.1 intro music changed')
    for marker in (
        'class EnhancedWorldMusicState final',
        'should_start(std::uint32_t level_id,',
        'if(last_.has_value() && *last_==next) return false;',
        'void invalidate() noexcept',
    ):
        if marker not in model:
            raise ValueError('deduplicating native music cursor changed: '+marker)
    for marker in (
        'add_executable(enhanced_world_music_tests',
        'add_test(NAME enhanced_world_music_tests',
        'NAME enhanced_world_music_pipeline_contract_tests',
    ):
        if marker not in cmake:
            raise ValueError('native music test unavailable: '+marker)
    if 'enhanced_world_music_tests' not in workflow:
        raise ValueError('CI fails to run native soundtrack unit tests')


def main() -> int:
    host=(ROOT/'DemoV1/src/win32_main.cpp').read_text()
    model=(ROOT/'DemoV1/include/rloft/enhanced_world_music.hpp').read_text()
    cmake=(ROOT/'DemoV1/CMakeLists.txt').read_text()
    workflow=(ROOT/'.github/workflows/demo-v1-build.yml').read_text()
    try:
        verify(host,model,cmake,workflow)
        mutations=(
            ('host','runtime.session().level.music_sequence','0x18'),
            ('host','0,100,0x9C4','0,100,0'),
            ('host','start_authored_world_music();','/* disabled music */'),
            ('host','world_music_state.invalidate();','/* no resume */'),
            ('model','if(last_.has_value() && *last_==next) return false;',
             'if(false) return false;'),
            ('cmake','add_executable(enhanced_world_music_tests',
             'add_executable(ignored_music_tests'),
        )
        for field,before,after in mutations:
            value={'host':host,'model':model,'cmake':cmake}[field]
            if before not in value:
                raise ValueError('missing mutation fixture: '+before)
            changed={'host':host,'model':model,'cmake':cmake}
            changed[field]=value.replace(before,after,1)
            try:
                verify(changed['host'],changed['model'],changed['cmake'],workflow)
            except ValueError:
                continue
            raise ValueError('music regression escaped: '+before)
    except (OSError,ValueError) as exc:
        raise SystemExit('ENHANCED_WORLD_MUSIC_REJECTED: '+str(exc))
    print('ENHANCED_WORLD_MUSIC_SOURCE_PASS 6 negative mutations rejected')
    return 0


if __name__=='__main__':
    raise SystemExit(main())
