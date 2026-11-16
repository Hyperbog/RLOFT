# Stone original-runtime event and attack tracing

This is an opt-in hook for the **original DOS game** running in DOSBox-X
2026.10.01 **normal CPU core**. It does not patch guest executables,
media or saves. Only a copied game directory is mounted writable.

## Build on Windows (VS 2022 Developer PowerShell)

    & "C:\RLOFT\Recomp\tools\dosboxx_stone_runtime_capture\build_windows_stone_capture.ps1" -RloftRoot "C:\RLOFT" -DosboxSourceRoot "C:\dosbox-x-Stone"

The builder pins upstream DOSBox-X commit and core file Git blob.
The resulting executable is
C:\dosbox-x-Stone\bin\x64\Release SDL2\dosbox-x.exe.

## Run an isolated original Stone guest

    & "C:\RLOFT\Recomp\tools\dosboxx_stone_runtime_capture\Start-Stone-Capture.ps1" -GameDir "C:\Games\Stone\PROPHET" -CdDir "C:\Games\Stone\CD_Extracted" -DosboxExe "C:\dosbox-x-Stone\bin\x64\Release SDL2\dosbox-x.exe"

This verifies the installed Stone CODE.2 SHA, copies PROPHET into
%LOCALAPPDATA%\RLOFT-Stone-Original-Trace, mounts that copy as C:,
mounts the extracted CD folder as D:, and runs SP.EXE.
Original saves remain untouched. CD-path or configuration fixes, if
needed, belong in the disposable copy only.

Add -EveryInstruction for per-instruction event-buffer comparisons.
This slows guest execution. Default sampling sees changes at code
checkpoints but does not attribute writes to a particular instruction.
Even strict sampling misses other guest code segments.

## Private trace and validator

The original-code hook records: verified CODE.2 segment; resolved
0x514-byte event buffer snapshots and deltas; opcode-13 spawn attempts;
constructor entries; imported and restored NPC IDs; and ECX immediately
before original IDIV at CODE.2 0x27CA8.

A zero divisor at that instruction is a **pre-execution condition**,
not independently captured CPU exception or proven crash. No observed
affected NPC does not prove stock-game unreachability.

Trace JSONL stays private. The launcher runs
summarize_stone_runtime_trace.py to produce an evidence summary.
Do not commit raw gameplay telemetry.

Source evidence: Tools/phase1_stone_level_npc_encounters.py.
Synthetic C++11 compile test: test_stone_runtime_capture.py.
Focused CI authenticates the pinned DOSBox-X normal-core source and
tests the observer without executing any original DOS game.
