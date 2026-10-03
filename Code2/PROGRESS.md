# CODE.2 faithful decompilation progress

![Progress](https://img.shields.io/badge/object%20accounting-3.63%25-red)
![Semantic audit](https://img.shields.io/badge/semantic%20audit-not%20started-lightgrey)

**Executable object-1 denominator:** **239,698 bytes (`0x3A852`)**  
**Verified translated instruction bytes:** **8,632**
**Proven non-code/data bytes:** **58**
**Reconciled object bytes:** **8,690 / 239,698 = 3.6254% (3.63%)**
**Unresolved object bytes:** **231,008**
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


## Batch 02 — 13-way application dispatcher + relocated graphics families

Source identity and object-1 SHA-256 were reverified before this batch. Added **4,615 translated instruction bytes** and **58 directly proven non-code/data bytes**.

### Newly translated ranges

- `002A8..006B8` — **1,041 bytes**: thirteen-way five-byte command dispatcher. The table at `00274..002A7` was decoded target-by-target; preserved behavior includes temporary bit-2 masking/restoration around `20C0C`, the opcode-2 type-dependent toggles, signed `IDIV` remainder use for opcode 3, packed `8000h` coordinate values, the long page/input sequence for opcode 7, live-EBX input to `1C160`, and the opcode-11 stop path.
- `29930..29A83` — **340 bytes**: mode-13/VGA initialization.
- `29A84..29A9E` — **27 bytes**: mode restore.
- `29AA4..29AEF` — **76 bytes**: planar pixel write.
- `29AF0..29B18` — **41 bytes**: planar pixel read.
- `29B40..29C9D` — **350 bytes**: planar rectangle fill.
- `29CA0..29CE8` — **73 bytes**: DAC palette stream.
- `29CEC..29EA9` — **446 bytes**: line rasterizer.
- `2A1E8..2A20B` — **36 bytes**: image storage-size helper.
- `2A20C..2A255` — **74 bytes**: four-plane transpose.
- `2A258..2A304` — **173 bytes**: in-place plane reversal helper.
- `2A308..2A37F` — **120 bytes**: vertical reversal in four planes.
- `2A380..2A477` — **248 bytes**: four-plane sprite copy.
- `2A478..2A583` — **268 bytes**: subtracting sprite variant.
- `2A584..2A6AF` — **300 bytes**: XLAT/table-translating sprite renderer.
- `2A6B0..2A89E` — **495 bytes**: clipped sprite renderer.
- `2A8A0..2AA9A` — **507 bytes**: clipped subtracting sprite renderer.

### Newly proven non-code/data

- `00073` — **1 byte**, NOP alignment.
- `001FA..001FB` — **2 bytes**, `MOV EAX,EAX` alignment.
- `00271..00273` — **3 bytes**, `LEA EAX,[EAX]` alignment.
- `00274..002A7` — **52 bytes**, thirteen-entry CS jump table for `002A8`.

These 58 bytes are recorded in `Code2/RESIDUAL_LEDGER.md`.

### Cross-binary proof

The graphics/image/sprite families were compared against the completed CODE.1 object instruction-by-instruction. CODE.2 uses a fixed code-address shift of `+0x1FC90` and a graphics DGROUP-state shift of `+0xC4B8`. For every integrated corresponding range, instruction counts and instruction boundaries match; all byte differences are relocated operands/self-patch addresses. No opcode/control-flow divergence was found. The C++ nevertheless uses CODE.2 addresses and was syntax-checked independently.

### Deliberately unresolved

`006B9..006BF` remains unresolved. It is a seven-byte zero island before the next apparent routine, but adjacency/appearance alone is not accepted as classification evidence.

## Next high-value targets

1. Continue application analysis at `006C0` while keeping `006B9..006BF` unresolved until independently classified.
2. Reconcile startup at `353A0` against CODE.1 startup only where CODE.2 bytes prove the relationship.
3. Continue harvesting exact/relocated CODE.1 runtime anchors to accelerate coverage without weakening the evidence standard.
