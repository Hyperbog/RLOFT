#!/usr/bin/env python3
"""Regression fence for the previously working original-game frontend.

Pins original cinematic/music/SFX/title/character presenter Git blobs to
the established ff263c3 checkpoint. This is intentionally a SOURCE gate,
not DOS audio/visual acceptance. A deliberate verified change may update
the snapshot, but speculative Level22 work must not rewrite these modules.
"""
from __future__ import annotations
import hashlib
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
KNOWN_GOOD_SOURCE_REF='ff263c3821978c2973c75c692c5089cc2799fcb3'
BASELINE={
    'DemoV1/src/anm_cinematic.cpp':'50eaa7ba0cccd8264c340b133028ee95250e97e4',
    'DemoV1/src/win32_audio.cpp':'e69374f44a5908bc41f52d07f7686fb7734de25e',
    'DemoV1/src/win32_midi.cpp':'8100cb8a45fce4ed1dcfce92713acdb5eba5b406',
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


def main()->int:
    try:
        for path,sha in BASELINE.items():
            file=ROOT/path
            computed=git_blob_sha(file.read_bytes())
            if computed!=sha:
                raise ValueError(f'Preserved frontend module changed: {path} '
                                 f'(baseline {sha}, got {computed}). '
                                 'Any change requires new original evidence and review.')
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
        corrupted=source.replace(
            'assets.source().read_runtime("CHARS");','std::nullopt;',1)
        if corrupted==source:raise ValueError('Missing CHARS mutation target')
        try:host_contract(corrupted)
        except ValueError:pass
        else:raise ValueError('CHARS regression mutation passed')
    except (OSError,ValueError) as exc:
        raise SystemExit('ALPHA_FRONTEND_PRESERVATION_REJECTED: '+str(exc))
    print('ALPHA_FRONTEND_SOURCE_PRESERVED '+KNOWN_GOOD_SOURCE_REF[:12]+' 6 blobs')
    return 0

if __name__=='__main__':raise SystemExit(main())
