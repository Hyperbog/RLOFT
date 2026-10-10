# Original CODE.2 FF50 upper/lower textured-span arithmetic — 2026-10-10

**Branch:** `demo-v1`. **Workstream:** faithful native engine recreation. **Status:** source-verified isolated C++ projection and deterministic regression; **not wired to live nearest-wall 22220 or claimed original-DOS parity**.

## Source authority

- Original user-supplied **CODE.2**: 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Original executable object-1 file base **0x3AF24**. Logical source offsets below add this base for physical file positions.
- Original user-supplied **SINTAB**: 333,757 bytes, SHA-256 `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87`. Its unsigned 32-bit `SINTAB+0x1B38` value is **127**, copied by original `03838` to mutable `C738`; `SINTAB+0x1B3C` is **508**, the *separate* runtime-lookup maximum. Never substitute 508 for C738.
- Canonical reconstructed source `Code2/Code2.cpp`, `function_03838` and `function_1f1c8`, independently reconciled against actual original object-1 instruction bytes.

## Verified exact source pipeline

1. `039D0` zeros `DWORD [15618]`. `03A20` writes `EBX` to `DWORD [EBX + 15618]`, with `EBX=4,8,...,4*C738` and `C738` loop iterations. Thus the *generated* dynamic table has indices **[0..127]**, and `DWORD[15618+4*i] = 4*i` for this SINTAB. This is not the SINTAB serialized `runtime_lookup` table.
2. `1F255` compares nearest-wall `22220` against source sentinel **0x800**. When equal, `1F2A5` sets initial upper `FF50=128` and `EBX=0`. Otherwise `1F261..1F273` computes **signed, truncation-to-zero `index=22220/8`** and reads `EBX=DWORD[15618+4*index]`.
3. `1F27A..1F29D` computes signed half and eighth of this table value, storing `upper FF50 = 128 - EBX/2 + EBX/8`. No additional rounding, interpolation or GPU pixel scaling is allowed.
4. `1F372..1F384` computes **lower `FF50 = 256 - (EBX + upper FF50)`**, overwriting the upper count. Only when this signed value is **strictly positive** does `1F388..1F3C3` load original bank-4 ground selector and call `34A74` if `C026` permits it. The independent upper branch additionally depends on `C027` and `258A8/258B0/C028` event state.
5. Original `34AB9..34AC9` emits the first `min(FF50,80)` original 4-plane rows, followed at `34B47..34B53` by `max(FF50-80,0)` paired-plane rows, using the original `FF58` perspective and `FE15` shading. Lower origin is `0x50F8` and row step is -80 VGA bytes.

All **19 independently verified instruction windows** and the two source E8 call targets (`1F398→2F29C`, `1F3C3→34A74`) can be reproduced by:

```sh
python DemoV1/tools/verify_original_ground_ff50_source_span.py \
  --original-code2 /path/to/CODE.2 \
  --original-sintab /path/to/SINTAB
```

## Numerical boundary evidence

| Source nearest `22220` | Table entry `EBX` | Upper FF50 | Lower FF50 | 34A74 lower rows, full/paired |
| ---: | ---: | ---: | ---: | ---: |
| 0x800, open-field | 0 | 128 | 128 | 80 / 48 |
| 8 | 4 | 126 | 126 | 80 / 46 |
| 16 | 8 | 125 | 123 | 80 / 43 |
| 400 | 200 | 53 | 3 | 3 / 0 |
| 408 | 204 | 51 | 1 | 1 / 0 |
| 416 | 208 | 50 | -2 | none (original `JLE`) |
| 1016 | 508 | -63 | -189 | none |

The isolated `OriginalGroundFF50SourceModel::from_22220` reproduces the source rows exactly for generated indices **0..C738**, and original no-wall sentinel `0x800`. Out-of-bounds entries, negative or greater-than-sentinel wall-depth states are reported **unverified** rather than reading unrelated native heap memory as if it were the original DOS DGROUP table. The original executable itself does not perform an index bounds check at `1F273`; source execution for unreachable/unverified corrupt states is not represented by this safe bounded adapter.

The new `original_ground_ff50_source_span_tests` CTest is a standalone C++17 regression verifying all **128 generated table entries** plus clipping boundaries, original sentinel, paired-plane split and rejection of unsupported depth. The separate `original_ground_ff50_binary_contract_tests` CTest checks native source and deliberate host isolation; optional original files enable exact machine-byte/SINTAB audits. Registry now **216 CTests when Python is installed**. New tests are authored but **not yet built by Windows CI**; last authorized Windows Full #658 passed **214/214** on its earlier checkout.

### Additional nearest-depth producer control flow (verified after initial FF50 audit)

Original `335C8` initializes nearest `22220=0x800` (open-field sentinel). The direct record materializers in reconstructed `Code2.cpp` are not an unconditional `min(all_ray_depths)`: at original `330E7→330F3` and `33487→33493`, an **inner source resource exactly `0x93` sets `22220=0`** before the routine compares the new record depth as a **signed DWORD** and stores it only if `depth < 22220` (`330FD..3310B` and `3349D..334AB`). This may leave `22220=0` even after later nonnegative records, and subsequent scene records have related writers. Original `Code2.cpp` `function_32f8c`, `function_333bc` and surrounding constructors confirm the semantics; the immutable binary verifier now independently checks **seven additional actual instruction windows** around the 0x93 reset, signed min and 0x800 initialization, for **19 windows total**.

**Crucial:** A native `ViewSweepRun` with `resource_id==0x93` does not establish that one of those constructor-side reset writes occurred in the exact original order. The already-verified Full #658 open-field case remains 22220=0x800. Do not implement the new dynamic row plan by taking the host minimum of sweep depths or by blindly mapping all 0x93 rays to a zero reset; source-equivalent record production/order and `BF34` state still require a separate proof.

## Critical live integration / original DOS acceptance blockers

The Level22 Full #658 field series **already establishes authentic native RES4[655] open-field texture activation** in 64/64 renderer frames with 32768 actual indexed writes. That is a narrower and more accurate checkpoint than claiming all terrain modes are complete. Do not regress that path.

To use this bounded clipping span *in gameplay*, source-verify and implement the full live `22220` producer from `335C8/330F5` wall/occlusion scan, including when the nearest value changes, all valid material/prepass conditions and original numeric sign/width. Also preserve mutable original `C026/C027` and the `258A8/258B0/C028` upper-branch effects, SINTAB-generated `15618` allocation lifecycle, and post-`34A74` `34DE8` ordering. The native `ExplorationPresenter::original_full_ground_eligible` remains intentionally restricted to the already-proved all-source-active-sentinel full 128-row case. **Do not wire the new projection to viewport pixels, inject a guessed wall depth or relax that eligibility guard** yet.

Remaining acceptance: Windows compilation/all 216 CTests after explicit build authorization, native partial-depth field tests, *independent* original DOS 320x400 indexed/palette/IRQ/RNG/camera-matched framebuffer comparison, Level22 mist exit and full-game continuity. No scoring change is justified by this isolated adapter.
