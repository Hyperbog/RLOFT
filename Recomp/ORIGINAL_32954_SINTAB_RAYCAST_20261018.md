# Original CODE.2 03838 SINTAB → 334C4 → 32954 raycast checkpoint (2026-10-18)

**Owning workstream:** `recomp`. **Validated source commit:** [`7f3d5773a0e1592196097966bfa5f419eb831c57`](https://github.com/Hyperbog/RLOFT/commit/7f3d5773a0e1592196097966bfa5f419eb831c57). **Windows/MSVC:** [#39100156051](https://github.com/Hyperbog/RLOFT/actions/runs/39100156051), success, **72/72 CTests** plus the authentic-media, read-only Level 22 source-raycast gate.

## Executed compiled source and proven inputs

1. The file-backed original `181B4` source uses bounded, read-only DOS-style open/seek/read/close to load preserved `LEVELS.DAT`. Its guest `0x25618` fixed header and `0x198E0` packed grid are independently checked against the authenticated archive parser. Level 22 is 30×15, and its startup cell (4,4) has no blocking material.
2. Original compiled `03838` opens the preserved `SINTAB` through the same allowlisted file service, loads its 450-entry sine and 900-entry tangent Q16 tables into original guest pointers `C65C`/`C660`, and reads the original renderer constants, the 256-entry angle table `C73C`, 509-word lookup, and 127 encoded scale blocks. The test checks source globals (256 rays, 127 blocks, 508 lookup-max, 324,989 payload bytes), actual loaded sine/tangent values and source angle entries.
3. Camera state is **replayed from original `20608` no-argument defaults**, rather than invented: level `0x16` (22), position (4,4) in 16.16 fixed-point with `0x8000` half-tile fraction, and facing `0x10E` (270 degrees). These assignments are source-derived but **the original `20608` game-entry function is not executed end-to-end in this gate**.
4. Compiled `334C4` uses the actual source SINTAB pointers and original `35680` fixed-point multiplication. Its `32954` callback invokes **compiled original `function_32954`**, with original `356D8` multiplication, `35730` depth calculation, `1E930`/`1E9A0` record lookups, and compiled `32F4C`, `32F8C`, `33124`, `332B4`, `333BC` ray/wall record paths. Original patch-opcode requests are observed by a bounded callback; executable self-modification is not installed.
5. The original raycaster traversed **256 rays**, produced **256 28-byte wall/depth records**, and marked **231 real grid cells** through its original `0x0440` visibility operations. The test checks that nonvisibility packed cell bits remain unchanged. The independent 32-level archive `181B4→334C4` pre-ray regression is still maintained.

The original archives are authenticated in CI before execution: `LEVELS.DAT` SHA-256 `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`; `SINTAB` SHA-256 `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87`. Their bytes are **not** uploaded into the published engineering artifacts.

## Reproduction

```powershell
.\rloft_original_code2_level_loader.exe --raycast "C:\RLOFT\LEVELS.DAT" "C:\RLOFT\SINTAB"
```

[Windows engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39100156051/artifacts/12126355953) · [CTest/JUnit evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39100156051/artifacts/12125792871).

## Remaining boundaries

This is a **real source camera/raycast world-record milestone**, not a screenshot or complete game. The ray-result records have **not yet** been fed to compiled original `335DC → 3263C → 338C8` with genuine RES4 lookups, shading, occlusion and VGA output. Other authenticated camera positions, complete `20608` CODE.1→CODE.2 boot, original DOS runtime trace comparison, near-wall dynamic SINTAB code execution, UI/HUD and R04 are unaccepted. Do not equate 72 passing CTests, file SHA evidence or individual source function execution with player-facing game acceptance.
