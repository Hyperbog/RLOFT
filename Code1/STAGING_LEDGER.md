# CODE.1 staging ledger

This file is operational state for continuation in a fresh chat. Staged bytes are **not verified coverage** and must never be added to `kVerifiedTranslatedBytes` or `PROGRESS.md` until re-disassembled from immutable CODE.1, reconciled, physically folded into `Code1.cpp`, and syntax checked.

Authority: CODE.1 SHA-256 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`. Object-1 denominator: 70,402 (`0x11302`) bytes. Always regenerate `/tmp/obj1.bin` from file offset `0x2C724` before working a staged range.

## Active staging

| Staging source | Active ranges | Functional bytes | Why staged / next proof required |
|---|---:|---:|---|
| `StartupNext.staging.cpp` + `StartupExit.reconciled.cpp` | `CE8B..CEBF` only | 53 | The rest of startup staging was re-disassembled and integrated in batch 12. The common exit/fatal continuation remains excluded because its alternate DOS-write entry and shared nonreturning tail still require exact fold. |
| `RuntimeGapE0D0.staging.cpp` | none | 0 | `E2C3..E35F` was re-disassembled and folded in batch 12 as an explicit interrupt-frame/SS:ESP boundary. |

**Active unique staged bytes: 0.**

## Resolved / rejected staging facts that must not regress

- `DE6F..DEA0` is **not** a proven routine boundary. Linear disassembly at `DE6F` conflicts with the known call to `DE70`; do not revive the old fill-front-end staging without resolving the entry semantics from raw bytes/callers.
- `DEA7..E0C7` was already physically present in `Code1.cpp`; it was duplicate staging, not new coverage.
- `DAD9..DDD7` was never missing; it was present later in `Code1.cpp` out of address order. Do not restore or recount it.
- `104B1..104DD` captures ES/CS/SS/DS/FS/GS selector words into 12 bytes; it is not memset/clear.
- `D179..D17B`, `E1F1..E1F3`, `E269..E26B` are alignment instructions/padding and are excluded from functional staged-byte counts where documented.
- Staging C++ is a hypothesis/quarantine representation. CODE.1 bytes always outrank it. Correct, split, or discard staging whenever direct disassembly disproves it.

## Mandatory fold procedure

1. Verify CODE.1 SHA-256 and regenerate object 1.
2. Directly disassemble the exact candidate range and its call/branch edges.
3. Reconcile staging against the bytes; do not copy staging blindly.
4. Preserve selector-qualified memory, overlapping registers, carry/error encodings, stack consumption, shared tails, and interrupt/far-call semantics.
5. Fold only independently proven instruction-bounded code into `Code1.cpp`.
6. Run `g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code1/Code1.cpp`.
7. Update `PROGRESS.md`, this ledger, and `DECOMPILATION_CONTINUITY.md` when state materially changes.
8. Commit before reporting progress. Keep the progress badge red until 100%.

- `D332..D4F1 integrated`: 448-byte DOS open/create/truncate descriptor-state machine directly re-disassembled and folded with AX+CF DOS boundaries; no host fopen substitution.


## D280/D2AD fold update
`D280..D2CC` (77 bytes) is now integrated. `D2CD..D312` (70 bytes) remains staged because AH=2F returns a DTA address whose DS/ES selector use in the two REP MOVSB directions must be represented exactly. The integrated wrappers retain pre/post shuttle calls as explicit machine-boundary callbacks; no flat-memory assumption was introduced.

- `F750..F7B2 integrated`: 99-byte source reader directly disassembled; bit 0 selects the guest-memory copy path, otherwise the encoded D85F/D5AD/D85F seek-read-position sequence is retained.

- `F7B8..F8BC integrated`: 261-byte resource image-size parser directly disassembled; preserves header offset 3Ch, two-byte signature check against DGROUP+038Ch, 0ACh header read, 18h-byte record walk, and encoded close behavior.

- `FCE9..FD6D integrated`: 133 bytes of directly disassembled six-byte runtime-record dispatchers; lower range selects minimum priority, upper range maximum priority, both prefer later equal-priority records and mark each executed record state 2.
- `FE21..FEFB integrated`: 219-byte directly disassembled command-line scanner, preserving whitespace, quote, backslash, in-place compaction, optional argv-output, and stopping-cursor behavior.
- `FFD4..102D3 integrated`: complete 768-byte interrupt veneer table; all 256 entries are mechanically `INT imm8; RET` at `FFD4+3*vector`.

- `F8C4..FC96 integrated`: 979-byte resource image loader/relocator directly disassembled; allocated guest addresses remain behind explicit F750-destination and relocation-write boundaries.

- `102D4..104DD integrated`: 520 functional bytes directly disassembled; four zero bytes at 10436..10437 and 104DE..104DF excluded. Error mapping, descriptor-table state, stack guard, bounded copy/pad, and selector capture are explicit.

- `104E0..1083E`, `10974..109C0`, `10C46..10C6C` integrated: 979 verified heap/runtime bytes. Guest heap addresses and DPMI/DOS release operations remain explicit boundaries; no host allocator substitution.

- `1083F..10973`, `109C1..10C45` integrated: 954 verified heap-growth bytes. DPMI 0501/0503, DOS AH=48/49, segment-base conversion, mode-9 allocation, and break-extension remain explicit boundaries.

- `10C6F..11301` integrated: 1,683 verified runtime-tail bytes. `10C6D..10C6E` are zero alignment and excluded. Program-break OS calls remain explicit boundaries; stream-list/buffer state is guest-address based.


## Increased batch 02
- `D2CD..D312` (70 bytes) moved from staging to integrated after preserving AH=2F and selector-qualified DS:ESI -> ES:EDI / reverse shuttle semantics.
- Active staging is now 904 bytes: startup 747 + E2C3..E35F 157.
- `04B0..0648` and `064C..07E1` renderer push-order contracts were reconciled and their 815 bytes are integrated/counted.
- FCE8 boundary correction adds the previously omitted PUSH ESI byte; FCE7 is zero alignment.

- Integrated directly from authoritative disassembly: `1464..15AD`, `1A04..1EE0`, `1EE4..1EFE` = **1,597 bytes**. Alignment at `15AE..15AF`, `1BD1..1BD3`, `1E62..1E63`, `1EE1..1EE3`, `1EFF` excluded; `1F00..1F23` classified as jump-table data.

- Integrated directly from authoritative disassembly: `1F24..2196`, `24D4..266B`, `28E4..2990`, `2994..2A60` = **1,413 bytes**. `2197`, `2991..2993`, and `2A61..2A63` are alignment and excluded. The 1F00 jump table remains data.

- Integrated directly from authoritative disassembly: actor generation/editor/stat chain `2A64..38A3` excluding alignment `2DF9..2DFB`, `3135..3137`, `3205..3207`, `334A..334B`, `37BE..37BF`, `3882..3883` = **3,633 verified bytes**.

- Integrated directly from authoritative disassembly: `38A4..3C54`, `3C58..40FA`, `40FC..44E0`, `44E4..451E`, `4520..45EE`, `45FC..4976` = **4,286 bytes**. `3C55..3C57`, `40FB`, `44E1..44E3`, `451F`, and `45F9..45FB` are alignment; `45EF..45F8` is a ten-byte lookup table and is data.

- Integrated directly from authoritative disassembly: post-generation/palette/resource/cursor/tree span `4978..59AB` with verified code subranges totaling **4,111 bytes**. Alignment/data excluded: `4A0A..4A0B`, `4B17`, `4C7F`, `4D51..4D53`, `4EEE..4EEF`, `508A..508B`, `522F`, `527A..527B`, `52AA..52AB`, `53B1..53B3`, `53E2..53E3`, `5451..5453`, `550B`, `55DB`, `577D..577F`, `57DB`, and seven zero bytes `5879..587F`. Active staging unchanged.

- Batch 08 integrated **3,671 bytes**: `59AC..6053`, `6184..6653`, `6654..6851`, excluding documented alignment. `6060..6183` remains unresolved hardware/interrupt machinery and is not counted.

## Batch 09 fold
`6854..71C7` selected verified graphics/DPMI wrappers and `72A0..7669` selected byte-copy/buffered-stream helpers are integrated/countable. `71C8..729F` remains unresolved/unintegrated; alignment/data islands remain excluded.

## Batch 10 integration note
`71C8..729F` is no longer unresolved/staged: the guarded allocation ownership contract and magic values are integrated. Interrupt-handler bodies at `7F5C..7F89`, `80BC..826D`, and `8338..8397` remain uncounted; only their install/restore plumbing is integrated. Existing startup/E2C3 staging remains unchanged.

## Batch 11 integration

Integrated and counted 4,116 bytes from selected `87C4..8E1E`, `91D8..95A7`, and `95BC..9BD6`. Excluded `8E20..8E3B` and `91BC..91D7` address tables, `8E3C..91BB` dispatch body pending separate reconciliation, and alignment bytes.

## Batch 12 integration
Re-disassembled and integrated **3,767 bytes**: `6060..6183`, `8E3C..91BB`, selected `9BE0..A058`, `A558..A6EF`, startup `CD97..CE8A`, `CEC0..CEE8`, `CEF0..D08B`, `E2C3..E35F`, and `FEFC..FFD3`. `CE8B..CEBF` remains the only active startup staging range (53 bytes). Address table `8E20..8E3B` remains data.

## Batch 13 integration
Integrated **4,129 bytes** across instruction-bounded low graphics and resource/audio state routines `AE0C..BE60`. Excluded zero/data/alignment islands: `AE5A..AE5B`, `AEA6..AEA7`, `AF05..AF07`, `AF4B`, `AF63`, `B055..B05F`, `B0F6..B0F7`, `B4D2..B4D3`, `B51B`, `B596..B597`, `B5E1..B5E3`, `B6BE..B6BF`, `B6EB`, `B75B`, `B76B`, `B821..B823`, `B8F3`, `BA01..BA03`, `BA97`, `BBE9..BBEB`, `BC1A..BC1B`. Active staging remains the 53-byte `CE8B..CEBF` exit path only.


## Batch 14 integration checkpoint
Integrated **3,921 verified instruction bytes** from `00EC..035B`, `10A4..1461`, selected `BE70..C79E`, and `CBD4..CBEE`. Alignment/data islands are excluded. `C79F..CBD0` is explicitly retained on the unresolved queue because it is position-dependent self-modifying VGA code; it is not staging and is not counted. No previously staged startup/interrupt bytes are double-counted.

## Batch 15 integration
Integrated **3,886 verified bytes** from `15B0..1A00`, `2198..24D0`, `266C..28E1`, functional `A21C..A554`, and `A6F0..A8F3`. Alignment `1A01..1A03`, `24D1..24D3`, `28E2..28E3`, `A2AE..A2AF`, `A359..A35B`, `A36A..A36B`, `A38D..A38F`, `A3E5..A3E7`, `A473`, `A4E7`, `A555..A557` is excluded. The self-modifying subtract-byte behavior of `A7E8` is represented as an explicit per-call byte because both encoded immediates are overwritten on every entry. Active staging remains only `CE8B..CEBF` (53 bytes).


## Batch 16 integration checkpoint
Integrated **2,867 verified bytes**: the full `07E4..10A1` staggered transition renderer, interrupt handlers `7F5C..7F89`, `80BC..826D`, `8338..8397`, and shared startup exit tail `CE8B..CEBF`. The startup staging pool is now empty. Hardware frame entry/IRET and DOS termination remain explicit machine-boundary callbacks rather than host substitutions. Verified total: **66,622 / 70,402 = 94.63%**.


## Batch 17 integration checkpoint
Integrated **1,748 verified bytes** from `A05C..A219`, `A8F4..AA1F`, `AA20..AC0E`, and `AC10..AE0A`. The line rasterizer preserves the encoded endpoint normalization, major-axis decision, error-step tie behavior, VGA map-mask rotation, and graphics-controller mode setup/restore. The sprite family preserves frame offsets, four-plane rotation, explicit/default transparency keys, translation-table mapping, clipping scratch globals `2B55..2B79`, and the self-patched subtraction immediate as an explicit byte parameter. Verified total: **68,370 / 70,402 = 97.11%**. The next high-priority unresolved code is the call-targeted self-modifying blitter family `C7A0..CBD0`; its patched immediates are to be modeled as explicit fixed-point/runtime parameters before counting.


## Batch 18 integration checkpoint
Integrated **1,066 verified bytes** from the four call-targeted self-modifying VGA blitters `C7A0..C8C0`, `C8C4..C9D1`, `C9D4..CAC9`, and `CACC..CBD0`; alignment islands remain excluded. Runtime writes into instruction immediates are represented as explicit variables: destination plane masks, 8.8-style scale increments retaining the literal low byte `0x78`, source-plane strides, key bytes, and source wrap corrections. The translation-table variants preserve the binary distinction that `C7A0` compares the translated byte against the key while `CACC` writes every translated byte. Verified total: **69,436 / 70,402 = 98.63%**.


## Batch 19 integration checkpoint
Resolved the old DE6F ambiguity: the real direct-call entry is **DE70**, reached by `84B8`; `DE6F` is the preceding zero byte. Integrated `DE70..DEA0` (**49 bytes**) as the byte-aligning front end to the existing `DEA7` dword-fill core, preserving the initial destination read, 32-bit pattern rotation during alignment, exact dword-count handoff, and 1/2/3-byte tail. `DEA1..DEA6` are alignment NOPs. Verified total: **69,485 / 70,402 = 98.70%**.


## Batch 20 integration checkpoint
Residual-code and historical-accounting audit: added/accounted **186 unique machine-code bytes** at `0000..0002`, `7F50..7F5B`, `95A8..95BA`, `9E10..9E11`, shared `9EA7..9EAD`, `1058B..1058F`, `CBEF`, `CBF0..CC18`, and the previously translated-but-omitted `DF13..DF72`. Removed a **19-byte duplicate historical count** for `10384..10396` by renaming the older DOS-memory result helper so the actual machine address has one canonical representation. Net verified-accounting increase: **167 bytes**, to **69,652 / 70,402 = 98.93%**. The badge remains red pending complete classification of the remaining object bytes.


## 2026-10-02 integrity repair: 95BC
A post-interruption audit re-disassembled `95BC..9723` directly from immutable CODE.1 and found four semantic discrepancies in the older batch-11 lift. The integrated C++ now preserves the encoded `E280(95A8, CS)` callback installation before `8398`, calls `64B0(0)` rather than `64DC(0)`, passes `argv[1]` from the guest argv table at `30A0` to `E360`, and preserves the cdecl argument order of the `D10C(0,27E)` and `D130(0,C7)` calls. This changes no coverage count because all affected bytes were already counted; it is a fidelity correction only.


## Residual-classification checkpoint
`RESIDUAL_LEDGER.md` now preserves 448 explicitly classified non-code bytes across 117 non-overlapping ranges (118 table + 330 alignment/zero/padding). This is classification only, not translated coverage. Verified code remains 69,652 bytes and 302 conservative residual bytes still require classification or translation. The active staging pool remains zero.


## Residual pass 22
Fourteen explicitly commented one-byte alignment islands were added to `RESIDUAL_LEDGER.md`. Classified non-code is 462 bytes; unresolved conservative residual is 288 bytes. Verified translated code remains 69,652 bytes and active staging remains zero.


## Residual pass 23
Preserved batch/staging evidence classified 38 additional non-code bytes. `RESIDUAL_LEDGER.md` now holds 500 classified bytes; 250 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero.


## Residual pass 24
Historical batch-12 evidence classifies `9EA3..9EA6` as lookup-table data after excluding the later-verified `9EA7..9EAD` code. `RESIDUAL_LEDGER.md` now holds 504 classified bytes; 246 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero.


## Residual pass 25
Arithmetic-closed integrator evidence classified 104 additional non-code bytes from batches 11, 13, 14, and 18. `RESIDUAL_LEDGER.md` now holds 608 classified bytes; 142 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero. The stale batch-08 prose total is not accepted as classification evidence.


## Residual pass 26
The batch-09 preserved integrator proves a 66-byte non-code complement. `RESIDUAL_LEDGER.md` now holds 674 classified bytes; 76 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero.


## Residual pass 27
Corrected batch-10 arithmetic classifies `7D67` and `7F8A..7F8B` as non-code. `RESIDUAL_LEDGER.md` now holds 677 classified bytes; 73 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero.


## Residual pass 28
Small batch-08 checkpoint: classified `631F`, `633B`, `6357`, and `63A2..63A3` as alignment from preserved exact function boundaries and the batch's `alignment excluded` note. `RESIDUAL_LEDGER.md` now holds 682 classified bytes; 68 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero.
