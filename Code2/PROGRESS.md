# CODE.2 faithful decompilation progress

![Progress](https://img.shields.io/badge/object%20accounting-1.68%25-red)
![Semantic audit](https://img.shields.io/badge/semantic%20audit-not%20started-lightgrey)

**Executable object-1 denominator:** **239,698 bytes (`0x3A852`)**  
**Verified translated instruction bytes:** **4,017**  
**Proven non-code/data bytes:** **0**  
**Reconciled object bytes:** **4,017 / 239,698 = 1.6759% (1.68%)**  
**Unresolved object bytes:** **235,681**  
**Status color:** RED until the entire executable object is instruction-accounted or directly proven non-code/data.

## Immutable source identity

- `CODE.2` size: 558,689 bytes (`0x88661`)
- SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 file start: `0x3AF24`
- Object-1 virtual base: `0x10000`
- Object-1 virtual size: `0x3A852`
- Object-1 page count: 59
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- Entry logical offset: `0x353A0`
- Object-2 virtual base: `0x50000`
- Object-2 virtual size: `0x2DD10`
- Object-2 physical start: `0x75F24`
- Object-2 stored bytes: `0x1273D`

## Measurement rule

The denominator is the complete executable object-1 virtual extent. Count a byte only after it is either:

1. part of an instruction-bounded range translated into `Code2.cpp` and reconciled directly against fresh CODE.2 disassembly; or
2. directly proven to be non-code/data/padding and recorded in a future CODE.2 residual ledger.

Staging, guessed boundaries, CODE.1 similarity, research, scaffolding, and preliminary function matches do not count.

## Batch 01 — bootstrap application + audited runtime anchors

Source identity and object-1 SHA-256 were reverified before this batch. Added **4,017 translated instruction bytes**:

- `00010..00072` — **99 bytes**: signed-count search over 22-byte records; matching type/key record clears word bits `01C0h` and sets byte bit `40h`.
- `00074..000B7` — **68 bytes**: type-8 record flag cleanup plus the encoded `16524`/`1C124`/`1F1C8` call sequence.
- `000B8..001F9` — **322 bytes**: resource/slot selection state machine, including its four-entry `0x248` stride scans and the encoded `27C70` fallback tests.
- `001FC..00270` — **117 bytes**: resource-46 gate, slot allocation, `5000h` flag rejection, indexed table-bit clear, and `BF44=33h` success state.
- `35F88..369A6` — **2,591 bytes**: timer/PIT/event subsystem corresponding structurally to audited CODE.1 `E3B4..EDD2`, including the reentrant IRQ-8 body, separate saved-vector far-chain trampoline, PIT programming, timer allocation/state transitions, registration, and all 48 address-distinct event thunks.
- `391F2..39525` — **820 bytes**: full-register interrupt adapter plus all 256 three-byte interrupt veneers. CODE.2 adds a CS override to the veneer-address LEA, making this family one byte longer than CODE.1 `FFA1..102D3`; vector 3 remains the special `CC 90 C3` veneer.

### Cross-binary proof used in this batch

For `35F88..369A6`, the corresponding CODE.1 range is `E3B4..EDD2`. Across all 2,591 bytes, every CODE.2/CODE.1 difference is confined to **101 runs of exactly three bytes**. Every run is explained by one of two relocation shifts:

- DGROUP state: CODE.1 address + `0xB164`
- code address: CODE.1 address + `0x27BD4`

No opcode or control-flow byte differs. The C++ was nevertheless rewritten with CODE.2 addresses and directly checked against fresh CODE.2 disassembly rather than copied as assumed proof.

`391F2..39525` was likewise expanded to complete CODE.2 function boundaries; the exact common block begins at `39204`, while `391F2..39203` contains the CODE.2-specific CS-qualified veneer-address setup.

### Deliberately unresolved nearby bytes

The first batch does **not** classify the obvious separators/table after the early routines. In particular, `00073`, `001FA..001FB`, and `00271..002A7` remain unresolved for accounting until their alignment/table roles are recorded under the same evidence standard used for CODE.1. This avoids using easy-looking non-code to inflate the opening percentage.

## Next high-value targets

1. Continue the application path at `002A8`, using its 13-entry table at `00274..002A7` only after explicit target reconciliation.
2. Reconcile the entry/startup family beginning at `353A0`; it shares substantial Watcom structure with CODE.1 but has meaningful layout and extender-path differences, so it must not be copied mechanically.
3. Harvest additional exact/near-exact CODE.1 runtime anchors already identified later in object 1, while preserving CODE.2-specific addresses and function boundaries.

C++17 verification for this checkpoint:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code2/Code2.cpp
```

passed with no diagnostics.
