#!/usr/bin/env python3
"""Regression fence for the previously working original-game frontend.

Pins the original cinematic/title/character presenter blobs to the
established ff263c3 checkpoint. The two WinMM output adapters are
separately reviewed to allow fallback to installed device IDs when
Windows has no default multimedia mapper. This SOURCE gate does not
replace an original audio/video listening or field acceptance test.
"""
from __future__ import annotations
import hashlib
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
KNOWN_GOOD_SOURCE_REF='ff263c3821978c2973c75c692c5089cc2799fcb3'
BASELINE={
    'DemoV1/src/anm_cinematic.cpp':'50eaa7ba0cccd8264c340b133028ee95250e97e4',
    # Reviewed native sampled-SFX mute update (October 16):
    # preserves the original VOC decode/PCM WinMM mapper and fallback,
    # merely prevents NEW voices while disabled and stops active voices.
    # Original CODE.1 cinematic frames/sounds, XMIDI and character
    # creation remain governed by their unchanged baseline/source gates.
    'DemoV1/src/win32_audio.cpp':'eeb6e8acfaacf533d10ec343b64929731520c3c6',
    'DemoV1/src/win32_midi.cpp':'4c65f75ab1a045d382c80bb0a5fd71fc87b50e8c',
    'DemoV1/src/character_creation_presenter.cpp':'560d83d90e6da218e8da677bb2afcf27a2f14e2b',
    'DemoV1/src/startup_presenter.cpp':'0dfb6692145b39cd6067c3d6c020f804ea8b966e',
    'DemoV1/src/character_creation_flow.cpp':'f395419c839a0c6809366cdc6d3239c3370ef7a0',
}
HOST=ROOT/'DemoV1/src/win32_main.cpp'


def git_blob_sha(content:bytes)->str:
    # Git checkout on Windows can use CRLF; Git blob identity is based
    # on the committed canonical LF representation for these .cpp files.
    canonical=content.replace(b'\r\n',b'\n')
    raw=b'blob '+str(len(canonical)).encode('ascii')+b'\x00'+canonical
    return hashlib.sha1(raw).hexdigest()


def host_contract(src:str)->None:
    # Same full frontend path, source-driven audio event timing, and
    # original.CODE.1->CHARS->CODE.2 handoff as the approved checkpoint.
    for value in (
        'rloft::AnmCinematic cine00(',
        'rloft::AnmCinematic cine12(',
        'startup_event("boot.splash.first");',
        'startup_event("boot.splash.second");',
        'startup_event("boot.splash.third");',
        'presenter.first_logo();',
        'presenter.second_logo();',
        'presenter.third_logo();',
        'constexpr double kVgaRefreshHz = 70.0863;',
        'std::chrono::microseconds(54925LL * ticks)',
        'frame.wait_ticks > 0',
        'const int sequence_index = event.second + 1;',
        'frame == 1 || frame == 36',
        'frame == 25',
        'assets.sampled_audio(6, sound_id)',
        'frame < rloft::CharacterCreationPresenter::kOpeningFrameCount;',
        'rloft::Win32AudioPlayer audio_player;',
        'rloft::Win32MidiPlayer midi_player;',
        'audio_player.play(rloft::decode_creative_voice(voc));',
        'midi_player.play(',
        'host.present(cinematic.framebuffer(), cinematic.palette());',
        'Clock::now() + pit_ticks(frame.wait_ticks)',
        'Clock::now() + code1_ticks(2)',
        'character_presenter.opening_frame(frame);',
        'character_presenter.palette()',
        'rloft::CharacterCreationPresenter character_presenter(assets);',
        'midi_player.fade_out_and_stop(0x9C4);',
        'const auto image =',
        'rloft::build_character_transfer(',
        'assets.source().read_runtime("CHARS");',
        '*code2_chars,',
    ):
        if value not in src:
            raise ValueError('Established frontend/handoff signature missing: '+value)
    sequence=(
        'rloft::AnmCinematic cine00(',
        'rloft::AnmCinematic cine12(',
        'startup_event("boot.splash.first");',
        'startup_event("boot.splash.second");',
        'startup_event("boot.splash.third");',
        'startup_event("boot.code1.creation.begin");',
        'character_presenter.opening_frame(frame);',
        'if (character_flow.completed()) {',
        'assets.source().read_runtime("CHARS");',
    )
    offsets=[src.index(t) for t in sequence]
    if offsets!=sorted(offsets):
        raise ValueError('Original intro/title/creation and CHARS order changed')


def audio_backend_contract(pcm:str,midi:str)->None:
    # The originals still use source-decoded VOC and XMIDI. Only the
    # OS output selection has changed. A missing MIDI mapper is not
    # evidence that no physical / synthesizer MIDI device is installed.
    for required in (
        'waveOutOpen(', 'WAVE_MAPPER', 'waveOutGetNumDevs()',
        'waveOutOpen(\n                            &wave,i,&format,',
        'waveOutReset(wave)', 'waveOutUnprepareHeader(',
        'bool effects_enabled{true};',
        'if(!effects_enabled)return;',
        'effects_enabled=enabled;',
        'if(!enabled)stop_all();',
        'void Win32AudioPlayer::set_enabled(bool enabled)',
    ):
        if required not in pcm:
            raise ValueError('Reviewed PCM backend lost: '+required)
    for required in (
        'midiOutOpen(', 'MIDI_MAPPER', 'midiOutGetNumDevs()',
        'midiOutOpen(\n                            &out,i,0,0,CALLBACK_NULL)',
        'device_open_failed=true;', 'bool sequence_started{};',
        'return stop_requested || sequence_started ||',
        'if (stop_requested || device_open_failed) return;',
        'sequence_started = true;', 'midiOutShortMsg(out,',
    ):
        if required not in midi:
            raise ValueError('Reviewed XMIDI backend lost: '+required)


def main()->int:
    try:
        for path,sha in BASELINE.items():
            file=ROOT/path
            computed=git_blob_sha(file.read_bytes())
            if computed!=sha:
                raise ValueError(f'Preserved frontend module changed: {path} '
                                 f'(baseline {sha}, got {computed}). '
                                 'Any change requires new original evidence and review.')
        pcm=(ROOT/'DemoV1/src/win32_audio.cpp').read_text(encoding='utf-8')
        midi=(ROOT/'DemoV1/src/win32_midi.cpp').read_text(encoding='utf-8')
        audio_backend_contract(pcm,midi)
        audio_mutations=(
            (pcm,midi,'waveOutGetNumDevs()','/* no PCM fallback */'),
            (pcm,midi,'if(!effects_enabled)return;','/* mute ignored */'),
            (pcm,midi,'if(!enabled)stop_all();','/* keep old sounds */'),
            (pcm,midi,'waveOutOpen(\n                            &wave,i,&format,','/* no PCM retry */('),
            (midi,pcm,'midiOutGetNumDevs()','/* no MIDI fallback */'),
            (midi,pcm,'device_open_failed=true;','device_open_failed=false;'),
            (midi,pcm,'bool sequence_started{};','bool sequence_started_removed{};'),
        )
        for index,(original,other,old,new) in enumerate(audio_mutations):
            if old not in original:
                raise ValueError('Missing audio negative-test anchor: '+old)
            candidate=original.replace(old,new,1)
            try:
                if index<2: audio_backend_contract(candidate,other)
                else: audio_backend_contract(other,candidate)
            except ValueError:
                continue
            raise ValueError('WinMM fallback/source-fade mutation escaped: '+old)
        source=HOST.read_text(encoding='utf-8')
        host_contract(source)
        # Mutation test: losing original audio framing must fail.
        wrong=source.replace(
            'host.present(cinematic.framebuffer(), cinematic.palette());',
            '/* skipped cinematic frame */',1)
        if wrong==source: raise ValueError('Missing timing mutation target')
        try:host_contract(wrong)
        except ValueError:pass
        else:raise ValueError('Cinematic regression mutation passed')
        wrong_clock=source.replace(
            'std::chrono::microseconds(54925LL * ticks)',
            'std::chrono::microseconds(1000LL * ticks)',1)
        if wrong_clock==source: raise ValueError('Missing original timer target')
        try:host_contract(wrong_clock)
        except ValueError:pass
        else:raise ValueError('Original BIOS tick pacing mutation passed')
        wrong_card_sfx=source.replace('frame == 1 || frame == 36',
                                      'frame == 2 || frame == 36',1)
        if wrong_card_sfx==source: raise ValueError('Missing CODE.1 sound target')
        try:host_contract(wrong_card_sfx)
        except ValueError:pass
        else:raise ValueError('CODE.1 animation SFX cue shift escaped')
        corrupted=source.replace(
            'assets.source().read_runtime("CHARS");','std::nullopt;',1)
        if corrupted==source:raise ValueError('Missing CHARS mutation target')
        try:host_contract(corrupted)
        except ValueError:pass
        else:raise ValueError('CHARS regression mutation passed')
    except (OSError,ValueError) as exc:
        raise SystemExit('ALPHA_FRONTEND_PRESERVATION_REJECTED: '+str(exc))
    print('ALPHA_FRONTEND_SOURCE_PRESERVED '+KNOWN_GOOD_SOURCE_REF[:12]+
          ' 4 original blobs + 2 reviewed WinMM adapters/PCM mute; '+
          str(len(audio_mutations))+' audio mutations rejected')
    return 0

if __name__=='__main__':raise SystemExit(main())
