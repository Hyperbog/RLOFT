#!/usr/bin/env python3
"""Alpha source contract: actual original intro/title/creation/first level.

Fails if CINE00 menu replay is lost, normal Load Game silently no-ops,
CHARS disk handoff is bypassed, or roadmap wrongly gates Alpha on mist.
Source-only CI; original DOS player comparison is independently required.
"""
from __future__ import annotations
import json
import re
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]

def require(condition:bool,where:str)->None:
    if not condition:
        raise ValueError("ALPHA_STARTUP_CONTRACT_REJECTED: "+where)

def check(host:str,ledger:dict,roadmap:str,cmake:str)->None:
    cold=host.index('assets.source().read("CINE00.ANM")')
    intro12=host.index('assets.source().read("CINE12.ANM")',cold)
    logo=host.index('presenter.first_logo()',intro12)
    title=host.index('presenter.menu_frame(-1)',logo)
    menu=host.index('flow.select_startup_menu(*click)',title)
    require(cold<intro12<logo<title<menu,"original cold start sequence")

    replay=host.index('if (*click == 0)',menu)
    cine=host.index('assets.source().read("CINE00.ANM")',replay)
    play=host.index('const auto result = play_cinematic(',cine)
    restore=host.index('presenter.menu_frame(-1)',play)
    resume=host.index('host.set_menu_mode(true)',restore)
    require(menu<replay<cine<play<restore<resume,"original title action-0 replay")
    section=host[replay:resume]
    for required in ('host.set_menu_mode(false)', 'audio_player.stop_all()',
                     'midi_player.stop()', 'CinematicResult::Quit',
                     'host.set_game_cursor(menu_cursor)',
                     'drawn_selection = -1'):
        require(required in section,"title replay missing "+required)

    loading=host.index('if (*click == 1)',resume)
    creation=host.index(
        'if (*click == rloft::BootFlow::kCharacterWorkflowMenuIndex)',
        loading)
    require(loading<creation,"title load action must be handled explicitly")
    require('Load Game is not yet available' in host[loading:creation],
            "unfinished load action silently ignored")
    require('MessageBoxW' in host[loading:creation],
            "unfinished native load must be visible")
    require('return run_exploration(' in host[creation:],
            "normal new-party transfer never reaches first game scene")
    require('assets.source().read_runtime("CHARS")' in host[creation:],
            "CODE.1->CODE.2 CHARS disk handoff missing")
    require('if (options.front_end_only)' in host[creation:],
            "read-only intro showcase protection missing")
    require('if (startup_chars)' in host[:cold],
            "existing-party startup source branch removed")

    stages=ledger["stages"]
    require([s["id"] for s in stages]==["alpha","beta","final"],
            "release stage order")
    alpha=stages[0]
    require(len(alpha["gates"])==10,"ten Alpha opening gates")
    ids={g["id"] for g in alpha["gates"]}
    wanted={"ALPHA-CINEMATIC","ALPHA-SPLASH","ALPHA-TITLE","ALPHA-MENU",
            "ALPHA-CHARACTER","ALPHA-CHARS","ALPHA-LEVEL22",
            "ALPHA-PLAY","ALPHA-BUILD","ALPHA-FIELD"}
    require(ids==wanted,"opening/first-level gates missing or mist still Alpha")
    require("Level 23 handoff" not in alpha["scope"] and
            "mist" not in alpha["scope"].lower(),
            "Alpha mis-scoped to full Level22/Level23")
    require(all(g["status"]=="open" for g in alpha["gates"]),
            "Alpha artificially advanced")
    require("original mist/Level23" in roadmap,
            "Beta full first-level quest milestone missing")
    require("alpha_title_startup_order_contract_tests" in cmake,
            "startup contract not registered")

def main()->int:
    paths={
        "host":ROOT/"DemoV1/src/win32_main.cpp",
        "gates":ROOT/"DemoV1/RELEASE_GATES.json",
        "road":ROOT/"DemoV1/RELEASE_MILESTONES_ALPHA_BETA_FINAL.md",
        "cmake":ROOT/"DemoV1/CMakeLists.txt",
    }
    try:
        host=paths["host"].read_text(encoding="utf-8")
        gates=json.loads(paths["gates"].read_text(encoding="utf-8"))
        road=paths["road"].read_text(encoding="utf-8")
        cmake=paths["cmake"].read_text(encoding="utf-8")
        check(host,gates,road,cmake)
        # Deliberate negative probes must fail:
        mutations=[
            host.replace('if (*click == 0)', 'if (*click == 9)',1),
            host.replace('if (*click == 1)', 'if (*click == 9)',1),
            host.replace('assets.source().read_runtime("CHARS")',
                         'assets.source().read_runtime("NOTCHARS")',1)
        ]
        for i,m in enumerate(mutations):
            try:
                check(m,gates,road,cmake)
            except (ValueError,AssertionError):
                pass
            else:
                raise ValueError(f"Unsafe title/CHARS mutation {i} passed")
        mis_scoped=json.loads(json.dumps(gates))
        mis_scoped["stages"][0]["scope"]+="; required mist/Level23 exit"
        try:
            check(host,mis_scoped,road,cmake)
        except (ValueError,AssertionError):
            pass
        else:
            raise ValueError("Alpha mist scope mutation passed")
    except (OSError,ValueError,AssertionError) as e:
        raise SystemExit("ALPHA_STARTUP_SOURCE_REJECTED: "+str(e))
    print("ALPHA_TITLE_STARTUP_SOURCE_PASS 4 negative variants rejected")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
