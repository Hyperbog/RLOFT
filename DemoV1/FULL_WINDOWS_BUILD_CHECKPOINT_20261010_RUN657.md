# Windows Full #657 accepted — native original RES4 EH6 loader — 2026-10-10

**CI:** [GitHub Actions run #657](https://github.com/Hyperbog/RLOFT/actions/runs/38084050726), run ID `38084050726`, completed **SUCCESS** on 2026-10-10 at 20:35:59 UTC.

| Gate | Verified value |
| --- | --- |
| Exact Windows CMake/CTest source checkout | `68fd2ccbcb8db46f53bc224dabb7a22a996ebadc` |
| Gameplay/test code baseline before temporary CI metadata | `3dc37d0fbc3a2a97461139ad3d3c0fcfc60d9f52` |
| Windows compiler / full native C++ target set | **PASS** |
| Original `CODE.2` strict C++17 syntax preflight | **PASS** |
| Full Windows CTest | **214/214 PASS — 100%** |
| `original_res4_eh6_runtime_binary_contract_tests` | **PASS**, Test #6 |
| `original_res4_selection_probe_tests` | **PASS**, Test #7 |
| `original_ground_res4_audit_tests` | **PASS** (native synthetic EH6 + EH1 fixtures) |
| Native artifact name | `Ravenloft-Demo-v1-Windows-x64` |
| Artifact ID / ZIP size | `11681438913` / **826,561 bytes** |
| Artifact ZIP SHA-256 | `0fa445584090011bb9a3dd2763daa02a2220d1e21de0236af2c10766ff81798d` |
| Built `ravenloft_demo_v1.exe` SHA-256 | `66b36894cb459f603b10a1c93e8f9c210196e107414ea60d97231107a7344f52` |
| Executable committed by workflow | `db5a88664db2b409d5bc13764efa1512caa95945` |
| Manual-only workflow after one-shot trigger | `7c8e5720f38c0690d01457ea3daf556124725ef2` (restored), marker removed |
| Linux job | Skipped by original user-authorized workflow configuration |

## What changed since unsuccessful #656

User-authorized [#656](https://github.com/Hyperbog/RLOFT/actions/runs/38083481440) compiled successfully but **failed 1/214 CTests**. All original EH6 source-decoder assertions completed; the lone failure was Windows Python's CP1252 output refusing a Unicode arrow in the final `PASS` diagnostic of `test_probe_original_res4_selection.py`. Only the string was changed to ASCII at [commit `3dc37d0`](https://github.com/Hyperbog/RLOFT/commit/3dc37d0fbc3a2a97461139ad3d3c0fcfc60d9f52). The user-authorized full retry #657 definitively passed the originally failing test plus all other 213. No original game behavior was modified by this follow-up.

## Independent original-source identity

Original-game supplied `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Independently verified **nine exact original EH6/37590/37700 instruction windows, seven original near E8 call targets, and the original EH6 switch-table DWORD** against the immutable mounted `CODE.2` binary. Original `LEV022.DAT` header+7 gives **55**. The user's exact installed-media probe gave `INIT+0x0E` base **600** and selected **RES4[655]**, an **EH6** disk entry of 2,515 bytes. Independent source-aligned actual-media oracle reports **4,096** decoded palette indices with SHA-256 `2266dc05fa40037116bb7e6914624ee1d1deb0d636d4d404f58e3da3be94a0a0` and FNV1a32 `c1e71881`. The new native code handles EH6->runtime type1 correctly, preserving the genuine 2-byte source length prefix; no fabricated pixel data.

## Remaining field acceptance (not a CI claim)

The Windows runner does **not** include the private installed game RES4/INIT data and did **not** run original DOS. Therefore **214/214 alone does not prove**:

1. The native installed-game loader now reaches `ground_original_res4_verified=1`, `ground_original_res4_absolute_entry=655`, and selects the expected source decoded hash; verify via `Probe-Level22-Ground.cmd` in the #657 artifact.
2. The actual Level22 native renderer enters the source-guarded `34A74` textured lower path where all 256 rays match original open-field sentinel `0x93`, rather than the known flat fallback; verify `Diagnose-Level22.cmd`, field screenshot and capture TSV.
3. The rendered native 320x400 indexed VGA/palette pixels, sprites, input, original game ticks and RNG agree with an independently captured matching original DOS state.

See [formal field/DOS acceptance gate](LEVEL22_EH6_GROUND_NATIVE_DOS_ACCEPTANCE_20261010.md). Do not increase provisional 49.8% weighted engineering readiness or original-DOS accepted score until actual matched DOS evidence passes. Keep the original game assets unmodified, **Level22-first** and CI **manual-only**.
