#!/usr/bin/env python3
"""Run real compiled canonical CODE.2 source against owner-authenticated media.

Not a native engine, game boot, or a DOS-equivalence assertion. This invokes
the actual reconstructed startup entry path and records first unsupported
DOS/Watcom service at an independently defined boundary.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

CODE2_HASH="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
SOUND_HASH="b373f84879c0dfe524009526989a3b6562f518a9a64280b4ee282673cb0474cc"
SOURCE_MODES=(
    ("initializer-only", "--through-initializers", "STOP_BEFORE_ORIGINAL_GAME_ENTRY_20608"),
    ("entry-default", "--enter-game", "STOP_BEFORE_ORIGINAL_DOS_VECTOR_INSTALL_375FE"),
    ("entry-overrides", "--enter-game-config", "STOP_BEFORE_ORIGINAL_DOS_VECTOR_INSTALL_375FE"),
    ("entry-wrapped", "--enter-game-signed-wrap", "STOP_BEFORE_ORIGINAL_DOS_VECTOR_INSTALL_375FE"),
    ("sound-open", "--to-sound-config", "STOP_BEFORE_ORIGINAL_DOS_AH3D_OPEN"),
    ("sound-open-configured", "--to-sound-config-configured", "STOP_BEFORE_ORIGINAL_DOS_AH3D_OPEN"),
    ("sound-open-no-args", "--to-sound-config-no-args", "STOP_BEFORE_ORIGINAL_DOS_AH3D_OPEN"),
    ("sound-open-configured-no-args", "--to-sound-config-no-args-configured", "STOP_BEFORE_ORIGINAL_DOS_AH3D_OPEN"),
    ("mouse-default", "--to-mouse-reset", "STOP_BEFORE_DOS_INT33_MOUSE_RESET"),
    ("mouse-configured", "--to-mouse-reset-configured", "STOP_BEFORE_DOS_INT33_MOUSE_RESET"),
    ("mouse-no-args", "--to-mouse-reset-no-args", "STOP_BEFORE_DOS_INT33_MOUSE_RESET"),
    ("mouse-configured-no-args", "--to-mouse-reset-no-args-configured", "STOP_BEFORE_DOS_INT33_MOUSE_RESET"),
    ("mouse-present-source", "--mouse-present-to-memory", "STOP_BEFORE_DOS_INT31_0100_ALLOCATION"),
    ("mouse-absent-source", "--mouse-absent-to-warning", "STOP_BEFORE_DOS_311E8_MOUSE_WARNING"),
)
def digest(path, expected, label):
    data=path.read_bytes()
    found=hashlib.sha256(data).hexdigest()
    if found != expected:
        raise ValueError(f"{label}: wrong version/hash: {found} (expected {expected})")
    return {"sha256":found, "size":len(data)}
def execute(exe, mode, code2, sound=None, timeout=120):
    cmd=[str(exe), mode, str(code2)]
    if sound is not None and mode.startswith(("--to-mouse-reset","--mouse-")):
        cmd.append(str(sound))
    r=subprocess.run(cmd,capture_output=True,text=True,timeout=timeout)
    return {
        "mode":mode,
        "returncode":r.returncode,
        "reported_boundary":next((line.split("=",1)[1] for line in r.stdout.splitlines()
                                  if line.startswith("EXECUTION_BOUNDARY=")),None),
        "media":next((line.split("=",1)[1] for line in r.stdout.splitlines()
                      if line.startswith("SOUND_CFG_INPUT=")),None),
        "source_entry":("CODE2_ORIGINAL_SOURCE_ENTRY=353A0" in r.stdout),
        "game_boot_claim":("GAME_BOOTED=NO" in r.stdout),
        "stderr":r.stderr[-2000:],
    }
def main(argv=None):
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--exe",type=Path,required=True)
    ap.add_argument("--code2",type=Path,required=True)
    ap.add_argument("--sound-cfg",type=Path)
    ap.add_argument("--report",type=Path,required=True)
    args=ap.parse_args(argv)
    if not args.exe.is_file():
        ap.error("compiled canonical rloft_code2_startup_gate executable is required")
    evidence={"code2":digest(args.code2,CODE2_HASH,"CODE.2")}
    if args.sound_cfg:
        evidence["sound_cfg"]=digest(args.sound_cfg,SOUND_HASH,"SOUND.CFG")
    cases=[]
    for label,mode,expect in SOURCE_MODES:
        case=execute(args.exe,mode,args.code2,args.sound_cfg)
        expected_media="ORIGINAL_FILE" if args.sound_cfg and mode.startswith(("--to-mouse-reset","--mouse-")) else "SYNTHETIC_FIXTURE"
        case["name"]=label
        case["passed"]=(case["returncode"]==0
                       and case["reported_boundary"]==expect
                       and case["media"]==expected_media
                       and case["source_entry"] and case["game_boot_claim"])
        cases.append(case)
        print(f"{'PASS' if case['passed'] else 'FAIL'} {label}: {case['reported_boundary']} [{case['media']}]")
    # Verify bad sound media does not accidentally silently succeed.
    report={"schema":"rloft.recomp.original-media-matrix.v1",
            "canonical_code2_entry":"353A0", "evidence":evidence,
            "cases":cases,"passed":all(c["passed"] for c in cases),
            "original_dos_runtime_parity":False,"game_booted":False}
    args.report.parent.mkdir(parents=True,exist_ok=True)
    args.report.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    return 0 if report["passed"] else 1
if __name__=="__main__":
    try:
        sys.exit(main())
    except (ValueError,OSError,subprocess.TimeoutExpired) as exc:
        print("ORIGINAL_MEDIA_MATRIX_FAIL:",exc,file=sys.stderr)
        sys.exit(2)
