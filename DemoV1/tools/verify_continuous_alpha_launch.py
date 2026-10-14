#!/usr/bin/env python3
"""Public contract for one-click continuous original intro -> Level22 Alpha.

Source-only; no proprietary media, Windows runtime, or success claim.
Mutations ensure a later packaging edit cannot restore the chooser,
developer warp, read-only showcase cut-off, or unsafe owned-media writes.
"""
from __future__ import annotations

import argparse
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def check(player: str, cmd: str, workflow: str, host: str) -> None:
    required = (
        ("$original = Join-Path $package 'Game'", player),
        ("$workspaceRoot = Join-Path $env:TEMP 'RLOFT-Continuous-Alpha'", player),
        ("$runId = [Guid]::NewGuid().ToString('N')", player),
        ("$scratch = Join-Path $workspaceRoot $runId", player),
        ("Get-FileHash -LiteralPath $engine -Algorithm SHA256", player),
        ("if ($hashLine -cmatch", player),
        ("$expected = $Matches[1]", player),
        ("$sourceSha = (Get-Content -LiteralPath $sourceFile -Raw).Trim()", player),
        ("-Attributes ReparsePoint", player),
        ("/E /XJ /R:1 /W:1", player),
        ("$charsCandidates = @(", player),
        ("Join-Path $scratch 'CHARS'", player),
        ("Join-Path (Join-Path $scratch 'RLOFT') 'CHARS'", player),
        ("Remove-Item -LiteralPath $candidate -Force", player),
        ("$arguments = '--no-remember-install \"' + $launchMediaRoot + '\"'", player),
        ("$launchMediaRoot = $scratch", player),
        ("if (-not $hasRootMedia -and $hasNestedMedia) {", player),
        ("$launchMediaRoot = Join-Path $scratch 'RLOFT'", player),
        ("$hasGogRoot = Test-Path -LiteralPath (Join-Path $original 'game.gog')", player),
        ("$hasGogNested = Test-Path -LiteralPath (Join-Path $nested 'game.gog')", player),
        ("Start-Process -FilePath $engine -ArgumentList $arguments", player),
        ('Play-Ravenloft-Alpha.ps1', cmd),
        ('powershell.exe -NoProfile -ExecutionPolicy Bypass', cmd),
        ("name: Ravenloft-Continuous-Alpha-Windows-x64", workflow),
        ('build/demo-v1/Release/Play-Ravenloft-Alpha.cmd', workflow),
        ('build/demo-v1/Release/Play-Ravenloft-Alpha.ps1', workflow),
        ('build/demo-v1/Release/Game/ORIGINAL-GAME-FILES-GO-HERE.txt', workflow),
        ('build/demo-v1/Release/build-source-sha.txt', workflow),
        ('build/demo-v1/Release/ravenloft_demo_v1.exe.sha256', workflow),
        ('if (character_flow.completed()) {', host),
        ('assets.source().read_runtime("CHARS");', host),
        ('return run_exploration(', host),
        ('if (!options.dev_trace && !options.no_remember_install) {', host),
        ('result.no_remember_install = true;', host),
        ('*code2_chars,', host),
    )
    for token, source in required:
        if token not in source:
            raise ValueError('missing continuous-game contract: '+token)
    # A malformed source edit once appended a SECOND launcher body after
    # its exit statement. String-presence tests alone did not detect it.
    if player.count("$original = Join-Path $package 'Game'") != 1 or (
            player.count('exit [int]$process.ExitCode') != 1):
        raise ValueError('duplicated or truncated player-facing launcher')

    # Original launch is no-flag, no chooser or developer warp. The PS
    # script's safety comments name the forbidden arguments, so examine
    # only the actual argument construction and Start-Process.
    tail = player[player.index("$arguments = '--no-remember-install \"' + $launchMediaRoot + '\"'"):]
    start = tail.index('Start-Process -FilePath $engine')
    if any(flag in tail[:start] for flag in
           ('--front-end-only', '--explore-level22', '--choose-install',
            '--dev-trace')):
        raise ValueError('continuous launch still has a diagnostic override')
    if '--no-remember-install' not in player or '--dev-trace' in tail[:start]:
        raise ValueError('Alpha must protect registry without enabling trace')
    for prohibited in ('FolderBrowserDialog', 'ShowDialog(', 'MessageBox]::Show',
                       'Read-Host', 'Remove-Item -LiteralPath $original',
                       'Remove-Item -LiteralPath $sourcePrefix'):
        if prohibited in player:
            raise ValueError('continuous launcher prompts or writes original media: '+prohibited)
    if "$hashLine -cnotmatch" in player:
        raise ValueError("negated regex leaves SHA256 capture untrustworthy")
    if player.index('Get-FileHash -LiteralPath $engine') > player.index(
            "$original = Join-Path $package 'Game'"):
        raise ValueError('binary validation must precede original-media access')
    copy = player.index('& robocopy.exe $original $scratch')
    remove = player.index('Remove-Item -LiteralPath $candidate')
    launch = player.index('Start-Process -FilePath $engine')
    # GameSource::open requires nested loose media at the actual RLOFT path.
    if not remove < player.index("$launchMediaRoot = $scratch") < launch:
        raise ValueError('Actual media root must resolve after temporary copy')
    if not copy < remove < launch:
        raise ValueError('isolated copy, CHARS cold start, then real launch required')
    if ('--front-end-only' in cmd or '--explore-level22' in cmd
            or '--choose-install' in cmd):
        raise ValueError('CMD reintroduced developer-only paths')
    if workflow.index('Stage continuous normal-game Alpha launch') > workflow.index(
            'Publish continuous original intro-to-Level22 Alpha'):
        raise ValueError('CI publication must follow staging')
    # Preserve the authentic disk CHARS handoff, never in-memory synthetic party.
    at=host.index('if (character_flow.completed()) {')
    context=host[at:at+12500]
    order=('build_character_transfer(', 'if (options.front_end_only) {',
           'runtime_output_path(', 'out.write(', 'flow.finish_character_creation(',
           'flow.supervisor_dispatch();', 'assets.source().read_runtime("CHARS");',
           'return run_exploration(')
    loc=[context.find(x) for x in order]
    if any(i<0 for i in loc) or loc!=sorted(loc):
        raise ValueError('the normal CODE.1->CHARS->CODE.2 continuous path changed')


def main() -> int:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--root',type=Path,default=ROOT)
    args=ap.parse_args()
    try:
        base=args.root
        player=(base/'DemoV1/Windows/Play-Ravenloft-Alpha.ps1').read_text()
        cmd=(base/'DemoV1/Windows/Play-Ravenloft-Alpha.cmd').read_text()
        workflow=(base/'.github/workflows/demo-v1-build.yml').read_text()
        host=(base/'DemoV1/src/win32_main.cpp').read_text()
        check(player,cmd,workflow,host)
        # Each mutation must fail independently.
        mutants=(
            ('player',"$original = Join-Path $package 'Game'",
             "$original = Join-Path $package 'RLOFT-INCORRECT'"),
            ('player','/E /XJ /R:1 /W:1','/E /R:1 /W:1'),
            ('player','$hashLine -cmatch','$hashLine -cnotmatch'),
            ('player','exit [int]$process.ExitCode',
             'exit [int]$process.ExitCode\\n$original = Join-Path $package \\'Game\\''),
            ('player','-Attributes ReparsePoint','-Attributes Hidden'),
            ('player',"Get-FileHash -LiteralPath $engine -Algorithm SHA256",
             'Get-FileHash -LiteralPath $engine -Algorithm MD5'),
            ('player',"$scratch = Join-Path $workspaceRoot $runId",
             "$scratch = $original"),
            ('player',"Remove-Item -LiteralPath $candidate -Force",
             "Remove-Item -LiteralPath $original -Force"),
            ('player',"$arguments = '--no-remember-install \"' + $launchMediaRoot + '\"'",
             "$arguments = '--front-end-only \"' + $scratch + '\"'"),
            ('player',"$arguments = '--no-remember-install \"' + $launchMediaRoot + '\"'",
             "$arguments = '--explore-level22 \"' + $scratch + '\"'"),
            ('player',"$arguments = '--no-remember-install \"' + $launchMediaRoot + '\"'",
             "$arguments = '--choose-install'"),
            ('workflow','name: Ravenloft-Continuous-Alpha-Windows-x64',
             'name: Ravenloft-Front-End-Only'),
            ('host','assets.source().read_runtime("CHARS");',
             'std::nullopt;'),
        )
        sources={'player':player,'cmd':cmd,'workflow':workflow,'host':host}
        for field,before,after in mutants:
            if before not in sources[field]:
                raise ValueError('missing deliberate negative mutation target: '+before)
            changed=sources.copy()
            changed[field]=changed[field].replace(before,after,1)
            try:
                check(changed['player'],changed['cmd'],changed['workflow'],changed['host'])
            except ValueError:
                continue
            raise ValueError('unsafe mutation escaped: '+before)
    except (ValueError,OSError) as exc:
        ap.exit(1,'CONTINUOUS_ALPHA_REJECTED: '+str(exc)+'\n')
    print('CONTINUOUS_ALPHA_LAUNCH_SOURCE_PASS (13 negative mutations)')
    return 0

if __name__=='__main__':
    raise SystemExit(main())
