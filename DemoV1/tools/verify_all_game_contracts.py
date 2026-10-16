#!/usr/bin/env python3
"""Aggregate the exact source preflight scripts already listed in Windows CI.

Do not modify gameplay semantics. Run each independent contract in a
subprocess, collect ALL failures, and reject the build if ANY fails.
This saves repeated Windows minutes on legacy test-contract drift.
"""
from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
WORKFLOW=ROOT/'.github/workflows/demo-v1-build.yml'
PATTERN=re.compile(
    r'^\s*python\s+DemoV1/tools/([\w-]+\.py)(?=\s|$)',re.MULTILINE)


def main()->int:
    workflow=WORKFLOW.read_text(encoding='utf-8')
    entries=list(dict.fromkeys(PATTERN.findall(workflow)))
    entries=[name for name in entries
             if name!='verify_all_game_contracts.py']
    if len(entries)<12 or 'verify_game_level22_transition.py' not in entries:
        raise SystemExit('PREFLIGHT_UNDERCOVERED: workflow scripts missing')
    failures=[]
    for name in entries:
        script=ROOT/'DemoV1/tools'/name
        if not script.is_file():
            failures.append((name,'missing source contract file'))
            continue
        cmd=[sys.executable,str(script)]
        if name=='check_release_gates.py':
            cmd.append('--self-test')
        outcome=subprocess.run(cmd,cwd=ROOT,capture_output=True,
                               text=True,check=False)
        if outcome.returncode==0:
            print('PASS '+name,flush=True)
        else:
            diagnostic=(outcome.stdout+'\n'+outcome.stderr).strip()
            failures.append((name,diagnostic or
                             ('exited '+str(outcome.returncode))))
            print('FAIL '+name,flush=True)
    for name,why in failures:
        print('\nPRECHECK_FAIL '+name+'\n'+why,file=sys.stderr,
              flush=True)
    print('ALL_GAME_CONTRACTS '+str(len(entries)-len(failures))+
          '/'+str(len(entries))+' PASS',flush=True)
    return 1 if failures else 0


if __name__=='__main__':
    raise SystemExit(main())
