#!/usr/bin/env python3
"""Fail-closed instrumenter for original DOSBox-X 2026.10.01 CPU normal core.

No game data is patched. This is opt-in guest observation, not an emulator.
"""
import argparse
import hashlib
from pathlib import Path

UPSTREAM_COMMIT = "b6abbd5980a885f5f310a4088c59a8688d1b116c"
CORE_GIT_BLOB = "e582cc06f38809f2ee8ee8d6f1922f3eba5b6ab5"
HEADER = "rloft_stone_runtime_capture.hpp"

def git_blob(data):
    return hashlib.sha1(b"blob " + str(len(data)).encode("ascii") +
                        b"\x00" + data).hexdigest()

def apply(root, header, check_only=False):
    path = Path(root) / "src/cpu/core_normal.cpp"
    target = path.with_name(HEADER)
    original = path.read_bytes()
    if git_blob(original) != CORE_GIT_BLOB:
        raise ValueError("Wrong upstream core_normal.cpp blob; refusing patch")
    if target.exists():
        raise ValueError("Capture header already exists; refusing overwrite")
    txt = original.decode("utf-8")
    anchors = {
        '#include "paging.h"\n':
            '#include "paging.h"\n#include "' + HEADER + '"\n',
        '\twhile (CPU_Cycles-->0) {\n\t\tLOADIP;':
            '\twhile (CPU_Cycles-->0) {\n'
            '\t\trloft_stone_capture::tick((uint32_t)SegValue(cs),\n'
            '\t\t    (uint32_t)SegBase(cs),(uint32_t)SegBase(ds),\n'
            '\t\t    (uint32_t)SegBase(ss),(uint32_t)reg_eip,\n'
            '\t\t    (uint32_t)reg_eax,(uint32_t)reg_esi,\n'
            '\t\t    (uint32_t)reg_ebx,(uint32_t)reg_edi,\n'
            '\t\t    (uint32_t)reg_ecx,(uint32_t)reg_edx,\n'
            '\t\t    (uint32_t)reg_esp);\n'
            '\t\tLOADIP;'
    }
    for old, replacement in anchors.items():
        if txt.count(old) != 1:
            raise ValueError(f"Original core anchor count !=1: {old[:30]!r}")
        txt = txt.replace(old, replacement, 1)
    if txt.count("rloft_stone_capture::tick(") != 1:
        raise AssertionError("CPU normal hook insertion incomplete")
    if not check_only:
        target.write_text(header, encoding="utf-8")
        path.write_text(txt, encoding="utf-8", newline="")
    return txt

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--dosbox-root",type=Path,required=True)
    p.add_argument("--header",type=Path,required=True)
    p.add_argument("--check-only",action="store_true")
    a=p.parse_args()
    apply(a.dosbox_root,a.header.read_text(encoding="utf-8"),
          check_only=a.check_only)
    print("STONE_DOSBOXX_NORMAL_CORE_HOOK_" +
          ("CHECKED" if a.check_only else "INSTALLED"))

if __name__=="__main__":
    main()
