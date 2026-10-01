# CODE.1 staging ledger

This file is operational state for continuation in a fresh chat. Staged bytes are **not verified coverage** and must never be added to `kVerifiedTranslatedBytes` or `PROGRESS.md` until re-disassembled from immutable CODE.1, reconciled, physically folded into `Code1.cpp`, and syntax checked.

Authority: CODE.1 SHA-256 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`. Object-1 denominator: 70,402 (`0x11302`) bytes. Always regenerate `/tmp/obj1.bin` from file offset `0x2C724` before working a staged range.

## Active staging

| Staging source | Active ranges | Functional bytes | Why staged / next proof required |
|---|---:|---:|---|
| `StartupNext.staging.cpp` + `StartupExit.reconciled.cpp` | `CD97..D08B` excluding proven padding/data | 747 | PSP/environment scanning and much of startup are understood, but integration still depends on exact selector/stack provenance around `CE21`, the `CEC8 -> FEFC` register-image contract, and preserving the nonreturning/shared exit paths. Re-disassemble each subrange before folding. |
| `DosCrtNext.staging.cpp` | `D280..D312` | 147 | `D280..D312` performs DOS DTA get/set and 0x2B-byte copies whose DS/ES direction must be represented exactly. `D332..D4F1` is the open/create/truncate descriptor-state machine; its DOS carry/error encoding and `10397/103C3/10415` interactions are mapped but the placeholder staged C++ is not integration quality. `D313..D331`, `D4F2..D55D`, `D55E..D572`, and `D573..D5AC` are already integrated and must not be counted here. |
| `RuntimeGapE0D0.staging.cpp` | `E2C3..E35F` | 157 | Full interrupt trampoline: saves ES/DS and the complete general-register image, calls CEC0, switches SS:ESP to the runtime interrupt stack, far-calls the callback saved at 5102:5106, restores the prior stack with LSS, restores runtime stack metadata and registers, then IRET. It must remain a machine/interrupt boundary, never an ordinary host callback. `E280..E2C2` is now directly reconciled and integrated as the vector installer. |

**Active unique staged bytes: 974.**

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
