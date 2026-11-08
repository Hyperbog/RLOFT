#!/usr/bin/env python3
"""CODE.1 character-name privacy in opt-in early startup trace.

Host-only observer contract: trace must omit Win32 OEM scan, VK down
and VK up values while original CODE.1 may accept actor names.
This does not change original input, key queues or normal game timing.
"""
from __future__ import annotations
import argparse
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]

def check(host:str,cmake:str)->None:
    def require(ok:bool,reason:str)->None:
        if not ok: raise ValueError(reason)
    window=host[host.index('    LRESULT handle(UINT msg, WPARAM wparam, LPARAM lparam) {'):]
    scan=window[:window.index('        switch (msg) {')]
    down=window[window.index('            case WM_KEYDOWN:'):
                window.index('            case WM_KEYUP:')]
    up=window[window.index('            case WM_KEYUP:'):
              window.index('            case WM_KILLFOCUS:')]
    require('if(session_trace_ && !character_mode_ &&' in scan,
            "scan metadata can leak CODE.1 name input")
    require('session_trace_->event("key.scan_observation"' in scan,
            "normal exploration scan tracing removed")
    require('if (!character_mode_) {\n'
            '                            session_trace_->event("key.down",' in down,
            "raw Win32 down VK leaks CODE.1 name")
    require('if(session_trace_ && !character_mode_) {' in up,
            "raw Win32 key-up VK leaks CODE.1 name")
    require('session_trace_->event("key.up",' in up,
            "normal exploration key-up audit removed")
    require("if (character_mode_) {\n"
            "                    if (wparam >= 'A' && wparam <= 'Z')" in down,
            "actual original character-name keyboard dispatch changed")
    require('if (wparam == VK_ESCAPE)' in down,
            "original Escape handling changed")
    require('NAME alpha_code1_trace_input_privacy_tests' in cmake,
            "Win32 full CI privacy contract not registered")

def main()->int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--root',type=Path,default=ROOT)
    a=p.parse_args()
    try:
        host=(a.root/'DemoV1/src/win32_main.cpp').read_text()
        cmake=(a.root/'DemoV1/CMakeLists.txt').read_text()
        check(host,cmake)
        mutants=[
            host.replace('if(session_trace_ && !character_mode_ &&',
                         'if(session_trace_ &&',1),
            host.replace('if (!character_mode_) {\n'
                         '                            session_trace_->event("key.down",',
                         'if (true) {\n'
                         '                            session_trace_->event("key.down",',1),
            host.replace('if(session_trace_ && !character_mode_) {',
                         'if(session_trace_) {',1),
        ]
        for i,bad in enumerate(mutants,1):
            if bad==host:raise ValueError(f'mutation {i} did not change source')
            try:
                check(bad,cmake)
            except ValueError:
                continue
            raise ValueError(f'privacy mutation {i} escaped')
    except (OSError,ValueError) as e:
        p.exit(1,'ALPHA_CODE1_TRACE_PRIVACY_REJECTED: '+str(e)+'\n')
    print('ALPHA_CODE1_TRACE_PRIVACY_SOURCE_PASS: three leaks rejected')
    return 0

if __name__=='__main__':
    raise SystemExit(main())
