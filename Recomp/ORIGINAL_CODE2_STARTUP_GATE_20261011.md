# CODE.2 authentic startup execution and initializer source evidence — 2026-10-11

Canonical branch: `code1-decompilation`; owner: `Recomp/`. Latest Windows-green source commit: [`3030843bafa8896932e6fa1bd1663151f6b3cfdb`](https://github.com/Hyperbog/RLOFT/commit/3030843bafa8896932e6fa1bd1663151f6b3cfdb). Its source-only Windows runner [#38174227958](https://github.com/Hyperbog/RLOFT/actions/runs/38174227958) **passed all 3/3 CTests**. No Windows build with actual original game media is claimed.

## Full path exercised

`src/code2_startup_gate.cpp` includes and compiles the exact `Code2/Code2.cpp` and invokes `function_353a0` through its real `tail_35418` boundary. The reconstructed complete `35418..3562E` startup runs with bounded, explicit test-machine DOS 6.22 (`INT 21h/AH=30h`) and `AX=FF00h` results, PSP selector 0x24, environment selector 0x2C, code selector 0x17, and DGROUP/SS selector 0x30. Any unknown DOS call, DPMI request, selector, or segment offset fails closed rather than inventing success.

The test drives original PSP command-tail whitespace trimming; original environment `NO87=1` detection; original DS store via the code selector at `35675`; the startup's real DGROUP clearing and register writes; and an original scheduler call `35623 -> 395E2`. The real scheduler scans exactly four six-byte callbacks at DGROUP offsets `12720..12737`. Original entries, all priority `0x20`, are `3968E,3A1B3,3A2AD,3A16E`. The original condition is `JA` after priority comparison, so equal priorities **replace** the selected entry: the first executed callback is the **last** table entry, `3A16E`. That canonical function stores SS to DGROUP word `126EC` and returns; the subsequent scheduler callback is `3A2AD`. Execution intentionally halts **before** the missing original environment/heap allocator boundary rather than treating no-op callbacks as progress.

The call's ceiling is the original `EAX=0xFF`: CODE.2 `3561E: B8 FF 00 00 00`, `35623: E8 BA 3F 00 00`, and scheduler `395E7: 89 C2` (`MOV EDX,EAX`). The committed gate checks the passed value, not prior EDX.

## Independently authenticated original-byte witness

Using the owner-uploaded original `CODE.2` (558,689 bytes; SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`), `tools/verify_code2_startup_oracle.py` verifies original file identity, five full instruction span hashes, seven exact opcode windows, four raw table entries, and the equal-priority dispatch identity. It passed locally; deliberate byte corruption was rejected. No copyrighted original code or media are stored in the script.

```sh
python3 Recomp/tools/verify_code2_startup_oracle.py /path/to/CODE.2
```

## Explicit limits / next large integration unit

The source-only CTest is a controlled test-environment execution of the actual reconstructed functions, **not** a DOS runtime equivalence result and not a boot to original menu/character creation. The optional original-input executable path loads its authentic CODE.2 LE image from disk but uses deterministic PSP/DOS fixtures. Exact DOS 6.22, extender, segment lifecycle, original command/environment selection and hardware service parity must be independently established.

Next boundary is actual `3A2AD..3A389`, which obtains source environment via `FS:[12669]`, uses `12665`, allocates through `37B42`, copies environment strings, and populates `12688/1268C`. Implement source-backed selector and heap arena adapters; then continue the remaining initializer set (`3A1B3`, `3968E`) and post-initializer `39589`. No fabricated allocator, missing callback defaults, or cross-workstream native engine substitutions.

## Superseding milestone: four source-backed initializers executed (2026-10-11)

The earlier stop before `3A2AD` remains the intentionally bounded default CTest. A new **separate** CTest now executes all four canonical reconstructed priority callbacks with explicit test-only original heap/environment adapters, runs real `39589` post-init, and halts before original game-entry `20608`. [Full updated gate and exact unresolved bounds](INITIALIZER_20608_INTEGRATION_20261011.md). [Windows #38186500373](https://github.com/Hyperbog/RLOFT/actions/runs/38186500373) passed all four CTests. The original file oracle now additionally pins initializer/allocator/argv source spans and original stream records.
