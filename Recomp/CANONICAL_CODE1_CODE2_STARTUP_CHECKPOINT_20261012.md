# Canonical original-code recompilation — CODE.1/CODE.2 startup convergence

Checkpoint: 2026-10-12. Owner branch: `code1-decompilation`, **not**
`native-engine` or `demo-v1`.

## Verification

**Windows Actions [#41](https://github.com/Hyperbog/RLOFT/actions/runs/38207402256)**:
source commit `fbbc7ae61c6793d41264275a2a1115a941962533`,
**31/31 CTests passed**, all four canonical recompilation developer
executables compile and link. Milestone acceptance 3/12 = **25.00%**.
Windows CI uses explicitly synthetic DOS/PSP/interrupt/media fixtures
and never claims gameplay parity.

## New CODE.1 compiled-original DOS startup

`Recomp/src/code1_startup_gate.cpp` includes exact `Code1/Code1.cpp`,
uses the actual source `translate_cc1c_ccdd`, and checks:

- Original entry offset `CC1C`; source stacks, protected-mode STI,
  DGROUP stores at `5304`, `52F0`, `52FC`; DOS major/minor at
  `5326` and `5327`.
- Stop **before** the actual DOS `INT 21h AH=30h` version call,
  rejecting mismatched source register/stack ABI.
- *Test-only* DOS 6.22 reply to original `CC1C -> CD17 -> CD50`,
  stopping at the next **original DOS AX=FF00h** call.
- *Test-only* extender marker to original `CC1C -> CCDE`, stopping
  at the next **original DOS AH=4Ah** resize call.

No CODE.1 graphics, menu, character creation, CHARS state transfer
or real DOS extender is claimed as operational.

Commands (owner-supplied original CODE.1 is optional; CI uses synthetic
DGROUP only):

```powershell
.\build\recomp\Debug\rloft_code1_startup_gate.exe --stop-before-version "C:\RLOFT\CODE.1"
.\build\recomp\Debug\rloft_code1_startup_gate.exe --dos6-to-ff00 "C:\RLOFT\CODE.1"
.\build\recomp\Debug\rloft_code1_startup_gate.exe --extender-to-resize "C:\RLOFT\CODE.1"
```

Original CODE.1 input: 277,787 bytes and SHA-256
`fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`.
The gate structurally checks file size and original LE geometry. It
does not independently authenticate SHA-256 itself.

## New CODE.2 compiled-original source boundaries

Building on the earlier `353A0 -> 20608 -> 20440 -> 2F2EC`
execution, exact-source DPMI and mouse paths now include:

- Original `30D80 -> 30D52 -> 37A73` DPMI allocation request
  (`AX=0100h`) and its test-owned, explicitly synthetic result.
- Original `33AA0` through its `INT31h AX=0300h` real-mode
  mouse simulation frame.
- A no-driver frame result, original `33B44 -> 30DE8 -> 30D52`
  selector release (`AX=0101h`) and explicit DOS-service boundary.
- Test-owned successful free result only in opt-in probes, followed
  by original `33D18` and `33D38` mouse range setup, stopping at
  genuine external `INT33h 7/8` calls.
- An explicitly opt-in double-successful mouse range continuation
  allowing the original `2F2EC` audio prelude to return naturally,
  and then original `20440 -> 202F0 -> 35286` disk free-space
  query, stopping **before DOS `INT21h AH=36h`**.

The 19-mode `Recomp/tools/original_media_matrix.py` asserts
independent terminal boundaries with canonical original CODE.2
SHA-256 validation and optional real SOUND.CFG input. CTest and
media matrix verify fixture results are **never reported as authentic
original DOS services or a playable game**.

## The exact gap to close

CODE.1 and CODE.2 now have independently compiled, source-executed
startup witnesses. They are **not joined into a live game process**.
The accepted gate ledger remains 3/12, 25.00%.

Next required integration, in dependency order:

1. Replace opt-in DOS/DPMI/PSP/interrupt fixtures with a properly
   constrained runtime provider implementing original memory,
   selector and service semantics and rejection behaviors.
2. Carry real CODE.1 source execution through actual character
   creation and the certified CHARS transition into CODE.2, without
   an alternate native engine.
3. Extend CODE.2 into actual resource, input, graphics/audio and
   interactive gameplay startup, with original DOS differential traces.

Do not count new trace frontiers or synthetic milestones as completed
whole-game acceptance gates. The 25.00% is a fixed, coarse milestone
score, not a measured percentage of playable functionality.
