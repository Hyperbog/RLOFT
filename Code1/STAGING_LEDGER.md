# CODE.1 staging ledger

This file is operational state for continuation in a fresh chat. Staged bytes are **not verified coverage** and must never be added to `kVerifiedTranslatedBytes` or `PROGRESS.md` until re-disassembled from immutable CODE.1, reconciled, physically folded into `Code1.cpp`, and syntax checked.

Authority: CODE.1 SHA-256 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`. Object-1 denominator: 70,402 (`0x11302`) bytes. Always regenerate `/tmp/obj1.bin` from file offset `0x2C724` before working a staged range.

## Active staging

| Staging source | Active ranges | Functional bytes | Why staged / next proof required |
|---|---:|---:|---|
| `StartupNext.staging.cpp` + `StartupExit.reconciled.cpp` | `CD97..D08B` excluding proven padding/data | 747 | PSP/environment scanning and much of startup are understood, but integration still depends on exact selector/stack provenance around `CE21`, the `CEC8 -> FEFC` register-image contract, and preserving the nonreturning/shared exit paths. Re-disassemble each subrange before folding. |
| `DosCrtNext.staging.cpp` | `D280..D312`, `D332..D4F1` | 595 | `D280..D312` performs DOS DTA get/set and 0x2B-byte copies whose DS/ES direction must be represented exactly. `D332..D4F1` is the open/create/truncate descriptor-state machine; its DOS carry/error encoding and `10397/103C3/10415` interactions are mapped but the placeholder staged C++ is not integration quality. `D313..D331`, `D4F2..D55D`, `D55E..D572`, and `D573..D5AC` are already integrated and must not be counted here. |
| `RuntimeGapE0D0.staging.cpp` | `E280..E35F` excluding padding and integrated ranges | 224 | `E280` installs INT 24h with extender-specific behavior. `E2C3..E35F` is an IRET trampoline and must remain a machine/interrupt boundary, never an ordinary host callback. `E0D0..E173` is now directly reconciled and integrated, along with the constructors, three 7240 wrappers, and `E360..E3B2`. |

**Active unique staged bytes: 1,566.**

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
