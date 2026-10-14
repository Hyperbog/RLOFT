#!/usr/bin/env python3
"""Source contract for shipping an enhanced native PC Alpha on push.

Prevents the old mandatory DOS research CTests from silently blocking
player-facing Windows executables; the full research suite remains
available on deliberate workflow_dispatch. No original game assets.
"""
from __future__ import annotations

from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]


def check(workflow:str,cmake:str) -> None:
    required=(
        "name: Ravenloft-Continuous-Alpha-Windows-x64",
        "      - name: Build game and enhanced-port regressions",
        "      - name: Test game and enhanced-port regressions",
        "      - name: Build historical full research suite (manual)",
        "      - name: Test historical full research suite (manual)",
        "      - name: Optional canonical CODE.2 C++17 research syntax audit",
        "if: ${{ github.event_name == 'push' || inputs.build_mode == 'quick' }}",
        "if: ${{ github.event_name == 'workflow_dispatch' && inputs.build_mode == 'full' }}",
        "enhanced_type7_compositor_tests enhanced_scene_mode_tests",
        "enhanced_level22_player_scene_source_tests",
        "alpha_continuous_game_launch_contract_tests",
        "alpha_frontend_preservation_contract_tests",
        "release_readiness_manifest_contract_tests",
        "build/demo-v1/Release/build-source-sha.txt",
        "build/demo-v1/Release/ravenloft_demo_v1.exe.sha256",
        'Set-Content -NoNewline -Path DemoV1/Windows/build-source-sha.txt -Value "${{ github.sha }}"',
        "name: demo-v1",
    )
    for required_marker in required[:-1]:
        if required_marker not in workflow:
            raise ValueError('missing native Alpha build gate: '+required_marker)
    if "      - demo-v1" not in workflow:
        raise ValueError("demo-v1 source pushes no longer start the build")

    windows=workflow[workflow.index('  windows-build:'):]
    positions={}
    for item in (
        "      - name: Configure",
        "      - name: Build game and enhanced-port regressions",
        "      - name: Test game and enhanced-port regressions",
        "      - name: Stage original front-end and character creation",
        "      - name: Stage continuous normal-game Alpha launch (no picker)",
        "      - name: Publish continuous original intro-to-Level22 Alpha",
        "      - name: Keep Windows executable in git",
    ):
        if windows.count(item)!=1:
            raise ValueError("ambiguous or absent step "+item)
        positions[item]=windows.index(item)
    if list(positions.values())!=sorted(positions.values()):
        raise ValueError("Windows steps no longer build-test-stage-publish")

    quick=workflow[workflow.index("      - name: Build game and enhanced-port regressions"):
                   workflow.index("      - name: Build historical full research suite (manual)")]
    if quick.count("github.event_name == 'push' || inputs.build_mode == 'quick'")!=2:
        raise ValueError("source pushes must build AND test playable port")
    if "Verify CODE.2 faithful" in quick:
        raise ValueError("DOS source recompilation incorrectly blocks native game")
    full=workflow[workflow.index("      - name: Build historical full research suite (manual)"):
                  workflow.index("      - name: Stage isolated Alpha normal-game field launcher")]
    if full.count("github.event_name == 'workflow_dispatch' && inputs.build_mode == 'full'")!=2:
        raise ValueError("the full historical suite must be opt-in")
    canonical=windows[windows.index("      - name: Optional canonical CODE.2 C++17 research syntax audit"):
                       windows.index("      - name: Configure")]
    if "github.event_name == 'workflow_dispatch' && inputs.build_mode == 'full'" not in canonical:
        raise ValueError("CODE.2 syntax audit is still a mandatory PC port gate")

    for name in (
        'enhanced_type7_compositor_tests',
        'enhanced_scene_mode_tests',
        'enhanced_level22_player_scene_source_tests',
        'alpha_continuous_game_launch_contract_tests',
        'alpha_frontend_preservation_contract_tests',
    ):
        if 'NAME '+name not in cmake and 'add_test(NAME '+name not in cmake:
            raise ValueError('CMake test missing: '+name)
    if 'NAME enhanced_port_build_pipeline_contract_tests' not in cmake:
        raise ValueError('build-pipeline contract was not registered')


def main() -> int:
    workflow=(ROOT/'.github/workflows/demo-v1-build.yml').read_text()
    cmake=(ROOT/'DemoV1/CMakeLists.txt').read_text()
    try:
        check(workflow,cmake)
        mutants=(
            ("if: ${{ github.event_name == 'push' || inputs.build_mode == 'quick' }}",
             "if: ${{ inputs.build_mode == 'quick' }}"),
            ("      - name: Test game and enhanced-port regressions",
             "      - name: Test DOS-only regressions"),
            ("      - name: Publish continuous original intro-to-Level22 Alpha",
             "      - name: Publish only frontend preview"),
            ('Set-Content -NoNewline -Path DemoV1/Windows/build-source-sha.txt -Value "${{ github.sha }}"',
             'Set-Content -NoNewline -Path DemoV1/Windows/unknown-source.txt -Value "${{ github.sha }}"'),
            ("enhanced_type7_compositor_tests enhanced_scene_mode_tests",
             "exploration_presenter_tests"),
            ("if: ${{ github.event_name == 'workflow_dispatch' && inputs.build_mode == 'full' }}",
             "if: ${{ github.event_name == 'push' || inputs.build_mode == 'full' }}"),
        )
        for old,new in mutants:
            if old not in workflow:
                raise ValueError("missing pipeline mutation source "+old)
            altered=workflow.replace(old,new,1)
            try:
                check(altered,cmake)
            except ValueError:
                continue
            raise ValueError("unblocked unsafe build pipeline mutation "+old)
    except (OSError,ValueError) as exc:
        raise SystemExit('ENHANCED_PORT_BUILD_REJECTED: '+str(exc))
    print('ENHANCED_PORT_BUILD_PIPELINE_PASS 6 negative mutations rejected')
    return 0


if __name__=='__main__':
    raise SystemExit(main())
