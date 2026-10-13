#!/usr/bin/env python3
"""Pre-Alpha boot-stage trace contract; no original files required.

A diagnostic session must precede CINE00 and survive into the field-test
package even if CODE.1, title, or the first gameplay load fails.
The trace is OPT-IN Win32 instrumentation, never original DOS parity.
"""
from __future__ import annotations
import argparse
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]

def expect(ok:bool,reason:str)->None:
    if not ok:
        raise ValueError(reason)

def contract(host:str,ps:str,cmake:str,readme:str)->None:
    expect("NAME alpha_boot_stage_trace_contract_tests" in cmake,
           "source contract absent from CTest")
    expect("if(options.dev_trace) {\n        const auto root=" in host,
           "boot tracing not guarded by opt-in")
    expect('std::ofstream latest(root/"latest-boot-session.txt"' in host,
           "no separate startup pointer")
    expect('startup_trace=std::make_unique<rloft::DevSessionTrace>(' in host,
           "no exact-source boot manifest")
    expect("~BootTraceDetach() {host.set_session_trace(nullptr);}" in host,
           "host can retain freed boot trace pointer")
    expect('const auto startup_handoff=[&]() {' in host and
           'startup_event("boot.exploration.handoff");' in host and
           'startup_trace->finish();' in host,
           "stage trace not closed before gameplay")
    expect(host.count('startup_handoff();')==3,
           "preview, existing-party and new-party startup missing handoff")
    # The boot pointer closes before run_exploration. Its own session
    # must exist BEFORE load_fresh/initialize_gameplay_state so a failure
    # there leaves an identifiable, source-pinned ZIP, not a false
    # completed-gameplay inference. This observes stages, not DOS fidelity.
    first=host.index('int run_exploration(')
    entry=host.index('int run_exploration(',first+1)
    boundary=host.index('int run(HINSTANCE instance)',entry)
    exploration=host[entry:boundary]
    start=exploration.index('trace=std::make_unique<rloft::DevSessionTrace>(')
    load=exploration.index('rloft::ExplorationRuntime::load_fresh(')
    world=exploration.index('runtime.initialize_gameplay_state(')
    audit=exploration.index('presenter.framebuffer().set_draw_audit(')
    expect(start<load<world<audit,
           "exploration trace still starts after first-level initialization")
    expect('if(dev_trace) {' in exploration[:start] and
           'host.set_session_trace(trace.get());' in exploration[:load] and
           'exploration_stage("exploration.load.begin");' in exploration[:load],
           "initial load failure cannot leave a source-pinned trace")
    for stage in ('exploration.load.failed','exploration.load.ready',
                  'exploration.world.failed','exploration.world.ready',
                  'exploration.presenters.ready'):
        expect('exploration_stage("'+stage+'")' in exploration,
               "missing source-safe exploration checkpoint "+stage)
    expect('~ExplorationTraceEarlyDetach() {host.set_session_trace(nullptr);}' in exploration,
           "early exploration exception can leave stale host trace pointer")
    expect('if(trace) {\n        presenter.framebuffer().set_draw_audit(' in exploration,
           "optional framebuffer instrumentation lost its opt-in guard")
    for stage in (
        'boot.host.ready','boot.cine00.begin','boot.cine00.result',
        'boot.cine12.begin','boot.cine12.result',
        'boot.splash.first','boot.splash.second','boot.splash.third',
        'boot.title_background','boot.title.ready','boot.title.action',
        'boot.title.cine_replay','boot.code1.creation.begin',
        'boot.code1.controller.ready','boot.code1.chars.build',
        'boot.code1.chars.disk_written','boot.code2.chars.readback_ok',
        'boot.exploration.handoff'
    ):
        expect('startup_event("'+stage+'"' in host,
               "missing required startup stage "+stage)
    expect("if(!startup_trace)return;\n        try {startup_trace->event(stage,value);}" in host,
           "untraced launch can perform instrumentation I/O")
    expect("catch(...) {\n            host.set_session_trace(nullptr);\n"
           "            startup_trace.reset();" in host,
           "failed optional log must not crash normal gameplay")
    ready=host.index('startup_event("boot.host.ready"')
    cine=host.index('assets.source().read("CINE00.ANM")',ready)
    title=host.index('startup_event("boot.title.ready")',cine)
    char=host.index('startup_event("boot.code1.creation.begin")',title)
    reread=host.index('startup_event("boot.code2.chars.readback_ok")',char)
    expect(ready<cine<title<char<reread,"wrong original startup order")
    start=host.index('    if (!host.create(instance))')
    expect(start<ready,"boot trace cannot run before window creates")
    expect("latest-boot-session.txt" in ps and
           "previousBoot" in ps and
           "bootSession" in ps and
           "Startup trace pointer escapes" in ps and
           "Compress-Archive -LiteralPath $bootPath" in ps,
           "early diagnostic may be missing from isolated field package")
    expect("latest-session.txt" in ps and
           "Compress-Archive -LiteralPath $sessionPath" in ps,
           "existing exploration diagnostic package was lost")
    expect("separate startup" in readme.lower(),
           "field trace nature not disclosed")

def main()->int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--root",type=Path,default=ROOT)
    a=p.parse_args()
    try:
        host=(a.root/"DemoV1/src/win32_main.cpp").read_text()
        ps=(a.root/"DemoV1/Windows/Test-Alpha-Continuous-Game.ps1").read_text()
        cm=(a.root/"DemoV1/CMakeLists.txt").read_text()
        readme=(a.root/"DemoV1/README.md").read_text()
        contract(host,ps,cm,readme)
        bads=[
            host.replace('startup_event("boot.cine00.begin");',
                         '/*missing boot event*/',1),
            host.replace('startup_event("boot.code2.chars.readback_ok");',
                         '/*missing CHARS readback*/',1),
            host.replace('startup_handoff();','/* no trace transfer */',1),
            host.replace('std::ofstream latest(root/"latest-boot-session.txt"',
                         'std::ofstream latest(root/"latest-session.txt"',1),
            host.replace('exploration_stage("exploration.load.begin");',
                         '/*no early load checkpoint*/',1),
            host.replace('exploration_stage("exploration.load.failed");',
                         '/*no load exception boundary*/',1),
            host.replace('exploration_stage("exploration.world.ready");',
                         '/*no world-ready boundary*/',1),
        ]
        for i,bad in enumerate(bads,1):
            expect(bad!=host,f"negative mutation {i} missing")
            try:
                contract(bad,ps,cm,readme)
            except ValueError:
                continue
            raise ValueError(f"negative mutation {i} undetected")
    except (OSError,ValueError) as e:
        p.exit(1,"ALPHA_BOOT_TRACE_REJECTED: "+str(e)+"\n")
    print("ALPHA_BOOT_TRACE_SOURCE_PASS: seven absent-stage variants rejected")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
