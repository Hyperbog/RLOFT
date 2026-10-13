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
    # A disposable explicit test copy must never replace the normal
    # user's persistent Windows installation preference in HKCU.
    conditional = re.search(
        r'} else if \(const auto explicit_path = options\.source\) \{'
        r'\s*source = rloft::GameSource::open\(\*explicit_path\);'
        r'[\s\S]*?if \(!options\.dev_trace\) \{'
        r'\s*save_preferred_install\(source\.location\(\)\.install_root\);'
        r'\s*\}',
        host)
    if conditional is None:
        raise ValueError("diagnostic temporary game copy overwrites saved install preference")

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
        "$hashFile = Join-Path $package 'ravenloft_demo_v1.exe.sha256'",
        "$hashRecord -cnotmatch '^([0-9a-fA-F]{64})  ravenloft_demo_v1\\.exe$'",
        "Get-FileHash -LiteralPath $engine -Algorithm SHA256",
        "$actualExeHash.Equals($expectedExeHash, [StringComparison]::OrdinalIgnoreCase)",
        "$partyRoot = Join-Path $scratch 'CHARS'",
        "$partyChild = Join-Path (Join-Path $scratch 'RLOFT') 'CHARS'",
        "if ($null -eq $partyFile) {",
        "if ($partySize -ne 0x920) {",
        "$session -eq $previous",
        "StartsWith($rootPrefix, [StringComparison]::OrdinalIgnoreCase)",
        "Compress-Archive -LiteralPath $sessionPath",
        "exit $gameStatus",
    )
    for token in required:
        if token not in ps:
            raise ValueError("missing Alpha isolation/trace behavior: "+token)
    # Field output may identify the earliest exploration-load blocker,
    # but must never print the unfiltered JSONL record (which can hold
    # input and game-state metadata).
    stages = ("exploration.load.begin", "exploration.load.failed",
              "exploration.load.ready", "exploration.world.failed",
              "exploration.world.ready", "exploration.presenters.ready",
              "exploration.frame.build.begin", "exploration.frame.build.failed",
              "exploration.frame.build.ready", "exploration.frame.raster.failed",
              "exploration.frame.raster.ready", "exploration.frame.present.submitted")
    if not all(stage in ps for stage in stages):
        raise ValueError("missing first-level preload diagnostic stage")
    if ("if ($preloadStages -contains [string]$entry.event) {" not in ps or
        'Write-Host "Last exploration initialization checkpoint: $lastPreload"' not in ps):
        raise ValueError("field runner prints unfiltered diagnostic events")
    before_launch = ps.index("$process = Start-Process")
    if ps.index("$hashFile = Join-Path") > ps.index("Add-Type -AssemblyName"):
        raise ValueError("EXE integrity checked after original media picker")
    if ps.index("if ($partySize -ne 0x920)") > before_launch:
        raise ValueError("existing-party route validated only after launch")
    checksum = "build/demo-v1/Release/ravenloft_demo_v1.exe.sha256"
    if workflow.count(checksum) < 3 or "Get-FileHash -LiteralPath $packageExe -Algorithm SHA256" not in workflow:
        raise ValueError("native executable checksum is not staged in both artifacts")
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
    # Reject a disabled checksum verifier or a fabricated existing
    # party before the native executable touches the isolated media.
    checksum_bypass=ps.replace(
        "$actualExeHash.Equals($expectedExeHash, [StringComparison]::OrdinalIgnoreCase)",
        "$true",1)
    size_bypass=ps.replace("if ($partySize -ne 0x920) {",
                           "if ($false) {",1)
    absent_bypass=ps.replace("if ($null -eq $partyFile) {",
                             "if ($false) {",1)
    lost_package=workflow.replace(
        "Get-FileHash -LiteralPath $packageExe -Algorithm SHA256",
        "Get-FileHash -LiteralPath $packageExe -Algorithm MD5",1)
    for label,bad_ps,bad_workflow in (
        ("checksum bypass",checksum_bypass,workflow),
        ("invalid CHARS bypass",size_bypass,workflow),
        ("missing CHARS bypass",absent_bypass,workflow),
        ("unverified package checksum",ps,lost_package),
    ):
        if bad_ps==ps and bad_workflow==workflow:
            raise ValueError(label+" negative mutation failed")
        try:
            check(cmd,bad_ps,bad_workflow,host,intro)
        except (ValueError,AssertionError):
            pass
        else:
            raise ValueError(label+" passed source safety contract")

    # Seventh mutation would print raw JSONL events instead of only
    # whitelisted symbolic checkpoints.
    raw_stage=ps.replace(
        "if ($preloadStages -contains [string]$entry.event) {",
        "if ($true) {",1)
    if raw_stage==ps:
        raise ValueError("raw-stage mutation did not match runner")
    try:
        check(cmd,raw_stage,workflow,host,intro)
    except (ValueError,AssertionError):
        pass
    else:
        raise ValueError("private event stream escaped field summary guard")
    # Eighth mutation checks the new visible-frame submission stage.
    # A source guard cannot itself prove that Windows drew the pixels.
    no_submit=ps.replace("            'exploration.frame.present.submitted'",
                         "            'unsafe-placeholder'",1)
    if no_submit==ps:
        raise ValueError("frame-submission mutation did not match runner")
    try:
        check(cmd,no_submit,workflow,host,intro)
    except (ValueError,AssertionError):
        pass
    else:
        raise ValueError("first-frame submission stage went missing")
    # Sixth mutation attacks only Win32 persistent registry behavior.
    broken_host=host.replace(
        'if (!options.dev_trace) {\n'
        '            save_preferred_install(source.location().install_root);',
        'if (true) {\n'
        '            save_preferred_install(source.location().install_root);',1)
    if broken_host==host:
        raise ValueError("registry mutation did not match host")
    try:
        check(cmd,ps,workflow,broken_host,intro)
    except (ValueError,AssertionError):
        pass
    else:
        raise ValueError("diagnostic install preference corruption escaped")


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
    print("ALPHA_SANDBOX_FIELD_SOURCE_PASS: 12 unsafe variants rejected")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
