#!/usr/bin/env python3
"""Enhanced Level22 UI regressions: in-game options and clean automap.

Ensures working player-facing navigation without claiming unfinished
Rest/Pray/Memorize/Save/Load or separate title Continue is implemented.
"""
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]


def check(host:str,auto:str,options:str,cmake:str,workflow:str)->None:
    source=host[host.index("int run_exploration("):
                host.index("bool code1_palette_reveal(")]
    required=(
        '#include "rloft/enhanced_options_presenter.hpp"',
        'rloft::EnhancedOptionsPresenter options_presenter(options_font);',
        'if (!gameplay_music_enabled) return;',
        'rloft::ExplorationUiScreen::Options',
        'rloft::EnhancedOptionsPresenter::action_at(',
        'rloft::EnhancedOptionsSelection::ToggleMusic',
        'rloft::EnhancedOptionsSelection::ToggleSound',
        'gameplay_sound_enabled=!gameplay_sound_enabled;',
        'audio_player.set_enabled(gameplay_sound_enabled);',
        'rloft::EnhancedOptionsSelection::ReturnToGame',
        'gameplay_music_enabled=!gameplay_music_enabled;',
        'midi_player.stop();',
        'world_music_state.invalidate();',
        'options_presenter.draw(',
        'rloft::TopEdgeMenuAction::Options',
    )
    for marker in required:
        if marker not in host if marker.startswith('#include') else marker not in source:
            raise ValueError("missing in-game native options integration: "+marker)

    # Keyboard automap navigation is modal; arrows must not remain
    # pressed in exploration movement after closing the map.
    for marker in (
        '#include "rloft/enhanced_automap_keyboard.hpp"',
        'void set_automap_mode(bool enabled)',
        'if (automap_mode_ && exploration_input_) {',
        'exploration_input_->clear();',
        'EnhancedAutomapKeyboard::action_for_key(',
        'if (automap_mode_ && exploration_input_) return 0;',
        'if(inventory_mode_) {',
        'if(inventory_mode_)return 0;',
    ):
        if marker not in host:
            raise ValueError("automap leaks keyboard input into movement: "+marker)
    if source.count('host.set_automap_mode(true);')!=2 or (
            source.count('host.set_automap_mode(false);')!=3):
        raise ValueError("automap input capture is not balanced at entry/exit")
    if 'if(host.consume_map_toggle()) {' not in source or (
            'ui.open_automap();' not in source):
        raise ValueError("native Tab automap hotkey not integrated")
    if 'if (const auto action=host.consume_key_event()) {' not in source:
        raise ValueError("keyboard map navigation events are ignored")
    if source.count('automap->apply_action(')!=2:
        raise ValueError("map keyboard and mouse must reuse the same modal action handler")

    # Only the original well-tested menu adapter may change screens.
    if source.count("rloft::TopEdgeMenuAction::Options")!=1:
        raise ValueError("in-game options entry has multiple handlers")
    # The already-existing cinematic code also invalidates the music
    # cursor: validate the live Options toggle's *own* side effect rather
    # than accepting unrelated invalidation elsewhere in the host.
    begin=source.index("gameplay_music_enabled=!gameplay_music_enabled;")
    end=source.index("options_presenter.draw(",begin)
    if "world_music_state.invalidate();" not in source[begin:end]:
        raise ValueError("Options music toggle failed to resynchronize XMIDI")

    if source.index("options_presenter.draw(") > source.index(
            'if (ui.state().screen ==\n            rloft::ExplorationUiScreen::Automap) {'):
        raise ValueError("Options overlay must be handled before map modal")

    start=auto.index("void AutomapScreenPresenter::draw(")
    section=auto[start:]
    clear=section.index("framebuffer.clear(0U);")
    chrome=section.index("draw_frame(",clear)
    tiles=section.index("viewport_.draw_tiles(",chrome)
    marker=section.index("viewport_.draw_player_marker(",tiles)
    if not 0<=clear<chrome<tiles<marker:
        raise ValueError("automap must erase previous world before painting")
    if auto.count("framebuffer.clear(0U);")!=1:
        raise ValueError("automap clear must occur once per frame")

    # CODE.2 018A8..018E6 repaints 250 RES0+35 wallpaper
    # tiles before the original KEY icons/text. The missing loop
    # left the player's right-hand legend blank/unreadable.
    for marker in (
        'sidebar_filler_frames(original_irq_seed);',
        'for(int x=0xF0;x<0x140;x+=0x08)',
        'for(int y=0;y<0x190;y+=0x10)',
        'draw_frame(framebuffer,0x35,sidebar_frames[tile++],y,x);',
        'draw_original_legend(framebuffer,session.level.header[0x290U]);',
        'border_filler_frames(original_irq_seed);',
        'for(int x=0x08;x<0x138;x+=0x08)',
        'for(int y=0x10;y<0x180;y+=0x10)',
        'draw_frame(framebuffer,0x35,edges[edge_index++],y,0x138);',
    ):
        if marker not in auto:
            raise ValueError('authentic map legend wallpaper missing: '+marker)
    order=(
        'draw_frame(framebuffer,0x35,sidebar_frames[tile++],y,x);',
        'draw_original_legend(framebuffer,session.level.header[0x290U]);',
        'draw_frame(framebuffer,0x35,edges[edge_index++],y,0x138);',
        'draw_frame(framebuffer, 0x35, 8,    0,     0);',
    )
    if [auto.index(item) for item in order] != sorted(
            auto.index(item) for item in order):
        raise ValueError('CODE.2 map wallpaper/legend/edges/chrome layering changed')
    if source.count('automap_background_seed=code1_tick_count(exploration_epoch);')!=2 or (
            'automap_background_seed);' not in source):
        raise ValueError('map wallpaper was not seeded once per modal entry')

    for marker in (
        'class EnhancedOptionsPresenter final',
        'EnhancedOptionsSelection action_at(',
        'EnhancedOptionsSelection::ToggleMusic',
        'EnhancedOptionsSelection::ToggleSound',
        'EnhancedOptionsSelection::ReturnToGame',
        'fb.clear(0U);',
        'IndexedFontPresenter::draw_centered(',
    ):
        if marker not in options:
            raise ValueError("missing indexed in-game options UI: "+marker)
    for marker in ('add_executable(enhanced_options_presenter_tests',
                   'NAME enhanced_level22_ui_source_tests'):
        if marker not in cmake:
            raise ValueError("native option/modal regressions not registered: "+marker)
    for marker in ('enhanced_options_presenter_tests',
                   'enhanced_automap_keyboard_tests',
                   'enhanced_level22_ui_source_tests',
                   'automap_screen_presenter_tests',
                   'automap_viewport_presenter_tests'):
        if marker not in workflow:
            raise ValueError("Windows-focused UI CTest missing: "+marker)


def main()->int:
    paths=[
        "DemoV1/src/win32_main.cpp",
        "DemoV1/src/automap_screen_presenter.cpp",
        "DemoV1/include/rloft/enhanced_options_presenter.hpp",
        "DemoV1/CMakeLists.txt",
        ".github/workflows/demo-v1-build.yml",
    ]
    try:
        originals=[(ROOT/p).read_text(encoding="utf-8") for p in paths]
        check(*originals)
        mutants=(
            (0,"gameplay_music_enabled=!gameplay_music_enabled;",
               "gameplay_music_enabled=true;"),
            (0,"rloft::TopEdgeMenuAction::Options",
               "rloft::TopEdgeMenuAction::Rest"),
            (0,"world_music_state.invalidate();",
               "/* no music state invalidation */"),
            (0,"host.set_automap_mode(true);",
               "/* map captured no keys */"),
            (0,"if (automap_mode_ && exploration_input_) {",
               "if (false && exploration_input_) {"),
            (0,"if(inventory_mode_) {",
               "if(false) {"),
            (1,"framebuffer.clear(0U);",
               "/* stale framebuffer */"),
            (1,"draw_frame(framebuffer,0x35,sidebar_frames[tile++],y,x);",
               "/* no original map sidebar tiles */;"),
            (1,"draw_frame(framebuffer,0x35,edges[edge_index++],y,0x138);",
               "/* no original automap edges */;"),
            (2,"EnhancedOptionsSelection::ReturnToGame",
               "EnhancedOptionsSelection::None"),
            (3,"add_executable(enhanced_options_presenter_tests",
               "add_executable(disabled_options_tests"),
        )
        for target,old,new in mutants:
            if old not in originals[target]:
                raise ValueError("missing regression mutation fixture: "+old)
            altered=originals.copy()
            altered[target]=altered[target].replace(old,new,1)
            try:check(*altered)
            except ValueError:pass
            else:raise ValueError("Level22 UI regression escaped: "+old)
    except (OSError,ValueError) as exc:
        raise SystemExit("ENHANCED_LEVEL22_UI_REJECTED: "+str(exc))
    print("ENHANCED_LEVEL22_UI_SOURCE_PASS (11 negative mutations rejected)")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
