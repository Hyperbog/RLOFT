#!/usr/bin/env python3
"""Win32 native input phase-isolation regression, NOT DOS input parity.

CODE.2 19464 uses a pointer-modal title; CODE.1 043C/2DFC consumes a
separate queue, and exploration owns its own controller. This only
prevents Win32 stale Escape/click/key events crossing those host
presentation boundaries. Full Windows gameplay/DOS IRQ still unproven.
"""
from __future__ import annotations
import argparse
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]

def demand(ok:bool,label:str)->None:
    if not ok:
        raise ValueError("ALPHA_INPUT_PHASE_REJECTED: "+label)

def check(host:str,cmake:str)->None:
    start=host.index('    void set_menu_mode(bool enabled) {')
    char=host.index('    void set_character_mode(bool enabled) {',start)
    explore=host.index('    void set_exploration_input(',char)
    consume=host.index('    int selection() const noexcept',explore)
    menu=host[start:char]
    creation=host[char:explore]
    game=host[explore:consume]
    demand('if (menu_mode_ != enabled)' in menu and
           'escape_pressed_ = false;' in menu and
           'clicked_.reset();' in menu,
           "menu must flush stale Escape/click at ownership changes")
    demand('if (character_mode_ != enabled)' in creation and
           'escape_pressed_ = false;' in creation and
           'key_events_.clear();' in creation and
           'character_click_.reset();' in creation,
           "CODE.1 stage must reset orphaned Win32 key and click state")
    demand('if (exploration_input_ != controller)' in game and
           'escape_pressed_ = false;' in game and
           'key_events_.clear();' in game,
           "first Level22 entry must not inherit character Escape")
    focus=host.index('            case WM_KILLFOCUS:')
    regain=host.index('            case WM_SETFOCUS:',focus)
    move=host.index('            case WM_MOUSEMOVE:',regain)
    loss=host[focus:regain]
    gain=host[regain:move]
    for key in ("exploration_input_->clear()","escape_pressed_ = false;",
                "key_events_.clear();","clicked_.reset();",
                "character_click_.reset();","exploration_click_.reset();",
                "exploration_release_.reset();",
                "exploration_secondary_click_.reset();",
                "selection_ = -1;","character_pointer_.reset();",
                "refresh_pointer_overlay();"):
        demand(key in loss,"WM_KILLFOCUS does not flush "+key)
    demand("update_pointer_from_system_cursor();" in gain and
           "refresh_pointer_overlay();" in gain,
           "WM_SETFOCUS must re-sample title/character hover")
    demand('NAME alpha_input_phase_isolation_contract_tests' in cmake,
           "missing CI test registration")
    # Ownership resets must happen BEFORE storing the new controller
    # or enabling the new mode.
    demand(menu.index('escape_pressed_ = false;')<
           menu.index('menu_mode_ = enabled;'),
           "title ownership transition order")
    demand(creation.index('key_events_.clear();')<
           creation.index('character_mode_ = enabled;'),
           "CODE.1 ownership transition order")
    demand(game.index('key_events_.clear();')<
           game.index('exploration_input_ = controller;'),
           "gameplay ownership transition order")

def negative_tests(host:str,cmake:str)->None:
    mutations=[
        host.replace('if (menu_mode_ != enabled)', 'if (false)',1),
        host.replace('if (character_mode_ != enabled)', 'if (false)',1),
        host.replace('if (exploration_input_ != controller)',
                     'if (false)',1),
        host.replace('            case WM_SETFOCUS:',
                     '            case WM_NOT_SETFOCUS:',1),
        host.replace('                selection_ = -1;\n'
                     '                character_pointer_.reset();\n'
                     '                refresh_pointer_overlay();\n'
                     '                return 0;\n\n'
                     '            case WM_SETFOCUS:',
                     '                character_pointer_.reset();\n'
                     '                refresh_pointer_overlay();\n'
                     '                return 0;\n\n'
                     '            case WM_SETFOCUS:',1),
    ]
    for i,bad in enumerate(mutations,1):
        demand(bad!=host,f"mutation {i} was a no-op")
        try:
            check(bad,cmake)
        except (ValueError,AssertionError):
            continue
        raise ValueError(f"negative mutation {i} was not detected")

def main()->int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--root",type=Path,default=ROOT)
    a=p.parse_args()
    try:
        host=(a.root/"DemoV1/src/win32_main.cpp").read_text(encoding="utf-8")
        cmake=(a.root/"DemoV1/CMakeLists.txt").read_text(encoding="utf-8")
        check(host,cmake)
        negative_tests(host,cmake)
    except (ValueError,AssertionError,OSError) as err:
        p.exit(1,f"ALPHA_INPUT_PHASE_SOURCE_FAIL: {err}\n")
    print("ALPHA_INPUT_PHASE_SOURCE_PASS: 5 unsafe mutations rejected")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
