#!/usr/bin/env python3
"""Source-only Alpha normal-launch packaging and original-save protection.

No proprietary game bytes required; no Windows game execution.
Validates that an explicitly chosen original installation is copied
BEFORE any native normal-game launch and only the copy is modified.
"""
from __future__ import annotations
import argparse
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

def check(cmd: str, ps: str, workflow: str, host: str, intro: str) -> None:
    assert '"%~dp0Test-Alpha-Continuous-Game.ps1"' in cmd
    assert 'powershell.exe -NoProfile -STA' in cmd
    assert 'ravenloft_demo_v1.exe' in cmd
    assert '--front-end-only --choose-install' in intro
    assert 'if (options.choose_install)' in host
    assert 'arg == L"--dev-trace"' in host
    assert 'result.source = std::filesystem::path(argv[index]);' in host
    assert 'source = rloft::GameSource::open(*explicit_path);' in host

    required = (
        "$original = [System.IO.Path]::GetFullPath($folder.SelectedPath)",
        "[Guid]::NewGuid().ToString('N')",
        "$workspaceRoot = Join-Path $env:TEMP 'RLOFT-Alpha-Field'",
        "$scratch = Join-Path $workspaceRoot $runId",
        "The selected folder overlaps the Alpha temporary workspace.",
        "ReparsePoint",

        "& robocopy.exe $original $scratch /E /XJ",
        "if ($copyStatus -ge 8)",
        "Get-ChildItem -LiteralPath $scratch -Filter 'CHARS'",
        "Remove-Item -LiteralPath $entry.FullName",
        "$argv = '--dev-trace \"' + $scratch + '\"'",
        "Start-Process -FilePath $engine -ArgumentList $argv",
        "Get-Content -LiteralPath $identity -Raw",
        "$session -eq $previous",
        "StartsWith($rootPrefix, [StringComparison]::OrdinalIgnoreCase)",
        "Compress-Archive -LiteralPath $sessionPath",
        "exit $gameStatus",
    )
    for token in required:
        if token not in ps:
            raise ValueError("missing Alpha isolation/trace behavior: "+token)
    before_launch = ps.index("$process = Start-Process")
    for prior in (
        "if ($copyStatus -ge 8)",
        "Get-ChildItem -LiteralPath $scratch -Filter 'CHARS'",
        "Remove-Item -LiteralPath $entry.FullName",
        '$argv = ',
    ):
        if ps.index(prior) > before_launch:
            raise ValueError("Alpha runner launches BEFORE sandbox protection")
    if re.search(r"(?im)^\s*(Remove-Item|Clear-Content|Set-Content)\b.*\$original", ps):
        raise ValueError("original-owned game installation is mutated")
    if "--choose-install" in ps.split("$argv = ",1)[1].split("\n",1)[0]:
        raise ValueError("Alpha field runner must pass the sandbox, not choose original")
    if "--explore-level22" in ps.split("$argv = ",1)[1].split("\n",1)[0]:
        raise ValueError("Alpha runner still warps directly to level")
    # robocopy is intentionally an executable that READS $original.
    # Only reject a native game launch that names the user's source.
    if re.search(r"(?im)^\s*Start-Process\s+-FilePath\s+\$original(?:\s|$)", ps):
        raise ValueError("native executable executes on original installation")
    for label in (
        "Test-Alpha-Continuous-Game.cmd",
        "Test-Alpha-Continuous-Game.ps1",
        "Ravenloft-Alpha-Field-Test-Windows-x64",
    ):
        if label not in workflow:
            raise ValueError("isolated Alpha artifact not published: "+label)
    if "Ravenloft-Original-Intro-Windows-x64" not in workflow:
        raise ValueError("read-only original intro artifact was removed")
    # Preserve player-facing intro showcase and retired warp restrictions.
    block = workflow.split("- name: Publish original intro and character creation",1)[1]
    block = block.split("- name:",1)[0]
    if "Test-Alpha-Continuous-Game." in block:
        raise ValueError("experimental Alpha launcher mixed into intro artifact")


def mutate(cmd: str, ps: str, workflow: str, host: str, intro: str) -> None:
    mutations = (
        (ps.replace("robocopy.exe $original $scratch /E /XJ",
                    "robocopy.exe $original $original /E /XJ",1),workflow),
        (ps.replace("/E /XJ", "/E /SL",1),workflow),
        (ps.replace("$argv = '--dev-trace \"' + $scratch + '\"'",
                    "$argv = '--dev-trace \"' + $original + '\"'",1),workflow),
        (ps.replace("Remove-Item -LiteralPath $entry.FullName",
                    "Remove-Item -LiteralPath $original",1),workflow),
        (ps,workflow.replace("Ravenloft-Alpha-Field-Test-Windows-x64",
                              "No-Alpha-Field-Test",1)),
    )
    for i,(bad_ps,bad_workflow) in enumerate(mutations,1):
        if bad_ps==ps and bad_workflow==workflow:
            raise ValueError(f"mutation {i} did not change source")
        try:
            check(cmd,bad_ps,bad_workflow,host,intro)
        except (ValueError,AssertionError):
            continue
        raise ValueError(f"unsafe Alpha field launcher mutation {i} survived")


def main()->int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--root",type=Path,default=ROOT)
    args=p.parse_args()
    root=args.root
    try:
        cmd=(root/"DemoV1/Windows/Test-Alpha-Continuous-Game.cmd").read_text()
        ps=(root/"DemoV1/Windows/Test-Alpha-Continuous-Game.ps1").read_text()
        workflow=(root/".github/workflows/demo-v1-build.yml").read_text()
        host=(root/"DemoV1/src/win32_main.cpp").read_text()
        intro=(root/"DemoV1/Windows/Play-Original-Intro.cmd").read_text()
        check(cmd,ps,workflow,host,intro)
        mutate(cmd,ps,workflow,host,intro)
    except (ValueError,AssertionError,OSError) as exc:
        p.exit(1,"ALPHA_SANDBOX_FIELD_REJECTED: "+str(exc)+"\n")
    print("ALPHA_SANDBOX_FIELD_SOURCE_PASS: 5 unsafe variants rejected")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
