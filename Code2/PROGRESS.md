# CODE.2 faithful decompilation progress

![Progress](https://img.shields.io/badge/object%20accounting-88.48%25-red)
![Semantic audit](https://img.shields.io/badge/semantic%20audit-not%20started-lightgrey)

**Executable object-1 denominator:** **239,698 bytes (`0x3A852`)**
**Verified translated instruction bytes:** **207,836**
**Proven non-code/data bytes:** **4,240**
**Reconciled object bytes:** **212,076 / 239,698 = 88.4763% (88.48%)**
**Unresolved object bytes:** **27,622**
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


## Batch 03 — map/search/render application block

Source identity and object-1 SHA-256 were reverified before this batch. Added **2,773 translated instruction bytes** and **19 directly proven alignment bytes**, bringing cumulative accounting to **11,405 translated + 77 non-code = 11,482 / 239,698 = 4.7902% (4.79%)**.

### Newly translated ranges

- `006C0..006FC` — **61 bytes**: signed camera-relative coordinate derivation; stored values clamp at `-40`, while EAX retains the unclamped second coordinate.
- `00700..00791` — **146 bytes**: dual primary/alternate 22-byte-record search selected by `5FCB`, with signed comparisons and exact `6048` snapshot/writeback.
- `00794..007E1` — **78 bytes**: gated search over the `1761A/1761E` record set.
- `007E4..00838` — **85 bytes**: type-`54h` primary-record search.
- `0083C..00930` — **245 bytes**: filtered search through `2ACDC`, `F03C/F03E`, table-bit `40h`, and the encoded +/-10 coordinate test.
- `00934..009AF` — **124 bytes**: alternate `2AD18`/`2ADE0` predicate search.
- `009B0..00B6B` — **444 bytes**: four-neighbor tile-shape classifier. Direct audit corrected a tempting normalization: kinds 2/3/4 fall back to the raw neighbor mask; only kind 1 and the ordinary kind-5 path consult `5FF0`.
- `00B6C..00BF9` — **142 bytes**: tile draw helper with partial-BX palette index semantics and temporary `764D/7651` palette-byte installation/restoration.
- `00BFC..00CAF` — **180 bytes**: fixed-point eight-step marker/line helper feeding the already-integrated `29CEC` line rasterizer.
- `00CB0..010C2` — **1,043 bytes**: multi-family map-record renderer. It preserves live `5FCB` layout selection, address-distinct search families, repeated `258DC` reloads across opaque draws, and the asymmetric alternate-record type tests encoded by the binary.
- `010C4..011A4` — **225 bytes**: eight-call fixed UI-frame decoration routine with `258DC` reloaded before every draw.

### Newly proven non-code/data

The explicit no-op separators `006FD..006FF`, `00792..00793`, `007E2..007E3`, `00839..0083B`, `00931..00933`, `00BFA..00BFB`, `010C3`, and `011A5..011A7` total **19 bytes**. Their instruction forms were decoded directly and a full-object direct branch/call target scan found no incoming control-flow references. They are recorded in `Code2/RESIDUAL_LEDGER.md`.

`006B9..006BF` remains unresolved: it is still only a zero-filled island, not proven alignment/data.

### Next high-value targets

1. Continue the application path at `011A8`.
2. Reconcile startup at `353A0` as a CODE.2-specific path.
3. Continue independently proving relocated CODE.1 runtime/graphics families where complete CODE.2 boundaries are available.


## Batch 04 — text/map UI and packed editor subsystem

Source identity and object-1 SHA-256 were reverified before this batch. Added **4,127 translated instruction bytes** and **17 directly proven alignment bytes**, bringing cumulative accounting to **15,532 translated + 94 non-code = 15,626 / 239,698 = 6.5190% (6.52%)**.

### Newly translated ranges

- `011A8..01226` — **127 bytes**: fixed viewport clear/draw helper with `258DC` reloaded across opaque draw calls.
- `01228..01398` — **369 bytes**: packed 5760h text-record clipping/drawing, signed lengths, CR line breaks, and font-byte advances.
- `0139C..01556` — **443 bytes**: two-pass rectangular map redraw plus optional packed-text/frame overlay.
- `01558..01830` — **729 bytes**: per-cell map classification table generator, including player markers, terrain 93h, object occupancy, and all eight neighbor probes.
- `01834..01C07` — **980 bytes**: map/UI initialization and palette-driven strip/icon rendering.
- `01C08..01C27` — **32 bytes**: packed text-list terminator pointer.
- `01C28..01C4C` — **37 bytes**: packed text-list used extent.
- `01C50..01CA7` — **88 bytes**: bounded text emission helper.
- `01CA8..01CE1` — **58 bytes**: final-line text width helper.
- `01CE4..01F6A` — **647 bytes**: in-place packed text editor with exact accepted-character tests, backspace/CR handling, capacity checks, and redraw gating.
- `01F6C..02078` — **269 bytes**: lower-panel/map redraw and page flip.
- `0207C..021D7` — **348 bytes**: pointer-driven modal text-insertion loop with signed hit tests and 1..4 line clamping.

### Newly proven non-code/data

`01227`, `01399..0139B`, `01557`, `01831..01833`, `01C4D..01C4F`, `01CE2..01CE3`, `01F6B`, and `02079..0207B` total **17 bytes** of decoded no-op alignment. None is a direct branch/call target in a full object-1 target scan.

### Fidelity notes

This batch preserves global reloads of `258DC` and map dimensions across opaque calls instead of caching them. The editor redraw flag changes only after an actual mutation; invalid input and backspace on an empty record do not force a redraw. The packed-list extent routine remains address-distinct and duplicates the binary traversal rather than being normalized into a call to `01C08`.

### Next high-value targets

1. Continue the application/UI path at `021D8`.
2. Reconcile startup at `353A0` as a CODE.2-specific path.
3. Continue harvesting independently proved relocated CODE.1 runtime families.

## Batch 05 — modal text deletion, map export/load, and control panel

Source identity and object-1 SHA-256 were reverified before this batch. Added **5,134 translated instruction bytes** and **78 directly proven non-code/data bytes**, bringing cumulative accounting to **20,666 translated + 172 non-code = 20,838 / 239,698 = 8.6934% (8.69%)**.

### Newly translated ranges

- `021D8..02352` — **379 bytes**: mouse-driven packed-text hit testing and deletion; preserves strict rectangle tests and the encoded forward-copy/clear arithmetic.
- `02354..023B5` — **98 bytes**: 17h status-frame predicate with exact bit acceptance.
- `023B8..02472` — **187 bytes**: packed-record coordinate lookup and two-byte A..Z/A.. label generation.
- `02474..026E5` — **626 bytes**: fixed-width packed-text serializer in character and block-write modes.
- `026E8..02A33` — **844 bytes**: three-plane ASCII map exporter plus packed-text append/flush behavior.
- `02A34..02AE4` — **177 bytes**: numbered export-name preparation/open/export/close path.
- `02AE8..02D6E` — **647 bytes**: numbered map/resource loader, live-level packed-text preservation, record-table allocation, map/text reads, and cleanup.
- `02DB0..034AE` — **1,791 bytes**: sixteen-way control-panel dispatcher driven by the CS table at `02D70`; includes text add/delete actions, level cycling, export, toggle, and exit paths.
- `034B0..03630` — **385 bytes**: priority-ordered viewport navigation bits, signed clamps, redraw, and page flip.

### Newly proven non-code/data

`02353`, `023B6..023B7`, `02473`, `026E6..026E7`, `02AE5..02AE7`, `02D6F`, `034AF`, and `03631..03633` are decoded compiler alignment forms with no incoming direct branch/call target. `02D70..02DAF` is a **64-byte sixteen-entry CS jump table** used by `02DB0`; every target was reconciled. These ranges total **78 bytes**.

`006B9..006BF` remains unresolved.

### Fidelity notes

The control-panel rectangle sentinel is entry 12, while action 11 deliberately uses entry 11 coordinates. Actions 12..15 are not duplicate cases: they synthesize navigation-mask bits 10h..80h from the first four rectangles. The map loader preserves the current packed-text buffer only for the live level, and its signed `< 7FFh` final-size test is retained.

### Next high-value targets

1. Continue the application path at `03634`.
2. Reconcile startup at `353A0` as a CODE.2-specific path.
3. Continue harvesting independently proved relocated CODE.1 runtime families.

## Batch 06 — map-mode setup, resource tables, nibble grids, and proximity helpers

Source identity and object-1 SHA-256 were reverified before this batch. Added **4,031 translated instruction bytes** and **18 directly proven alignment bytes**, bringing cumulative accounting to **24,697 translated + 190 non-code = 24,887 / 239,698 = 10.3826% (10.38%)**.

### Newly translated ranges

- `03634..0380C` — **473 bytes**: map-mode setup/teardown loop, including the exact `759C[i]=i` 256-byte initialization, viewport clamps, map rebuild/redraw, page toggles, and owned-buffer cleanup.
- `03810..0381E` — **15 bytes**: release the `608C` allocation.
- `03820..03835` — **22 bytes**: linear bump allocator over `608C + 6094`.
- `03838..03A63` — **556 bytes**: resource/table loader, allocation layout, scalar/table reads, variable sub-block reads, and final handle close.
- `03A78..03C02` — **395 bytes**: running low/high-nibble clamp over the `2194A` grid.
- `03C04..03DE2` — **479 bytes**: companion clamp over `2199A`, including type-dependent `219A1/219A2` initialization.
- `03DE4..03EA3` — **192 bytes**: `62B0`-controlled nibble enable/disable sweep.
- `03EA4..03F74` — **209 bytes**: `6350`-controlled companion sweep.
- `03F78..0403F` — **200 bytes**: modal two-line prompt/input-release cycle and page restore.
- `04040..040FE` — **191 bytes**: 360-direction fixed-point probe using full coordinates for `2C8E4`.
- `04100..04224` — **293 bytes**: special-level/proximity gate over active records and tile flags.
- `04228..043F0` — **457 bytes**: three-type distant-record relocation search with exact four candidate coordinate mutations/restoration.
- `043F4..044CE` — **219 bytes**: nibble deficit accounting across two 8x10 grids.
- `044D0..04619` — **330 bytes**: capacity-driven nibble consumption across four slots, including repeated same-slot consumption and output publication.

### Newly proven non-code/data

`0381F`, `03836..03837`, `03C03`, `03DE3`, `03F75..03F77`, `040FF`, `04225..04227`, `043F1..043F3`, `044CF`, and `0461A..0461B` total **18 bytes** of decoded compiler no-op alignment. A full object-1 direct branch/call target scan found no incoming target to these bytes.

The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved; they are not counted from appearance alone.

### Fidelity notes

`0363E..0364C` writes each byte's own index into `759C`; it is not a zero fill. `04040` passes the evolving full 16.16 coordinate dwords to `2C8E4`. `044D0` loops within a slot and may consume multiple high-nibble units before advancing; reducing it to one unit per slot changes behavior.

### Next high-value targets

1. Continue the sequential application path at `0461C`.
2. Reconcile startup at `353A0` as a CODE.2-specific path.
3. Continue harvesting independently proved relocated CODE.1 runtime families.

## Batch 07 — four-party simulation/update loop and slot classifier

Source identity and object-1 SHA-256 were reverified before this batch. Added **2,925 translated instruction bytes** and **3 directly proven alignment bytes**, bringing cumulative accounting to **27,622 translated + 193 non-code = 27,815 / 239,698 = 11.6042% (11.60%)**.

### Newly translated ranges

- `0461C..050D8` — **2,749 bytes**: four-party state reconciliation/update loop, including nibble normalization, pending/deficit accounting, timed progression, special-resource checks, escape polling, message construction, and exact normal-vs-hard cleanup paths.
- `050DC..0518B` — **176 bytes**: slot classifier with exact `F240h` exclusion mask, mode-dependent signed byte tests, and the signed `>= 9` word gate.

### Newly proven non-code/data

`050D9..050DB` is the explicit three-byte `LEA EAX,[EAX]` compiler separator between the two routines. A full object-1 direct branch/call target scan found no incoming target to these bytes.

### Fidelity notes

The `04100` early-message path returns directly after output and does **not** execute the `1E74C` cleanup. By contrast, the random `04228` abort and the two party-zero `7D44` aborts jump directly to `1E74C` and skip the normal key-drain, elapsed-time formatting, and D681/D682 publication epilogue. The odd initial special-resource predicate intentionally reuses the `2199B` low nibble while testing the `219B8/219C2/219CC` alternatives.

The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.

### Next high-value targets

1. Continue the sequential application path at `0518C`.
2. Reconcile startup at `353A0` as a CODE.2-specific path.
3. Continue harvesting independently proved relocated CODE.1 runtime families.

## Batch 08 — party/status UI buffers, interaction loops, settings, and status icons

Source identity and object-1 SHA-256 were reverified before this batch. Added **3,656 translated instruction bytes** and **53 directly proven non-code/data bytes**, bringing cumulative accounting to **31,278 translated + 246 non-code = 31,524 / 239,698 = 13.1515% (13.15%)**.

### Newly translated ranges

- `0518C..05577` — **1,004 bytes**: six-row party/status buffer construction, palette remap setup, cell rendering, and page handoff.
- `05578..05694` — **285 bytes**: status-panel palette/identity restoration.
- `05698..05982` — **747 bytes**: mouse hit-test and quantity mutation over six rows, including signed row-byte accumulation and immediate return on decrementing an already-zero quantity.
- `05984..05A10` — **141 bytes**: panel-open path and page flip.
- `05A14..05AA2` — **143 bytes**: panel-close path with three-byte local state forwarding.
- `05AA4..05BDB` — **312 bytes**: six-buffer allocation, pointer-driven editor loop, row-selection quotient/remainder gate, cleanup, and selected-row return.
- `05BDC..05C80` — **165 bytes**: eight-byte capacity preparation and mode-dependent augmentation/zeroing.
- `05C84..05D33` — **176 bytes**: mode-0 selection loop; continuation preserves the signed record-type gate held in ESI.
- `05D34..05DCF` — **156 bytes**: mode-1 selection loop with its distinct continuation rule and optional `620A` shift.
- `05DD0..05E37` — **104 bytes**: settings/resource replace-write path.
- `05E38..05EC2` — **139 bytes**: settings/resource read path plus redraw and two propagated byte settings.
- `05EEC..06007` — **284 bytes**: ten-way status-icon renderer driven by the CS jump table at `05EC4..05EEB`.

### Newly proven non-code/data

`05695..05697`, `05983`, `05A11..05A13`, `05AA3`, `05C81..05C83`, `05DCF`, and `05EC3` are explicit compiler no-op separators with no incoming direct branch/call target. `05EC4..05EEB` is a **40-byte, ten-entry CS jump table** consumed by the indirect jump at `05F34`; its targets are `05F3B`, `05F54`, `05F65`, `05F76`, `05F84`, `05F92`, `05FA5`, `05FB3`, `05FC1`, and `05FD7`. These ranges total **53 bytes**.

The known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.

### Fidelity notes

`05698` sums quantity bytes with **signed byte promotion** before comparing against the unsigned row limit. `05C84` does not simply rerun `050DC` as a loop predicate: after the first pass, ESI contains the signed record-type byte and that value controls the next continuation test. `05D34` has a different continuation shape and therefore remains separate. The three-byte values passed to `32468` are modeled as local byte arrays, preserving that the binary passes a pointer to three stack-local bytes rather than a packed integer.

### Next high-value targets

1. Continue the sequential application path at `06008`.
2. Reconcile startup at `353A0` as a CODE.2-specific path.
3. Continue harvesting independently proved relocated CODE.1 runtime families.

## Batch 09 — settings panel, interaction dispatcher, save serializer, and tile probe

Source identity and object-1 SHA-256 were reverified before this batch. Added **3,188 translated instruction bytes** and **49 directly proven non-code/data bytes**, bringing cumulative accounting to **34,466 translated + 295 non-code = 34,761 / 239,698 = 14.5020% (14.50%)**.

### Newly translated ranges

- `06008..062AA` — **675 bytes**: full settings-panel background/border construction, static labels, ten retained subimage buffers, initial render/page flip.
- `062AC..06351` — **166 bytes**: 0..9 mouse-driven selector with the original shared epilogue behavior represented semantically.
- `06354..06440` — **237 bytes**: 0..14 selector, record byte publication through `F050+6C`, and dependent redraw calls.
- `06444..0651F` — **220 bytes**: 0..128 slider plus the encoded signed divide-by-128 truncation used to derive `C02D`.
- `06548..066E5` — **414 bytes**: ten-way settings interaction dispatcher and nine-handle teardown.
- `066E8..06C66` — **1,407 bytes**: ordered save serializer with exact short-write aborts, dynamic record blocks, 62 variable descriptor payloads, and final close/result path.
- `06C68..06CAC` — **69 bytes**: position record probe publishing the `CF6C` boolean.

### Newly proven non-code/data

`062AB`, `06352..06353`, `06441..06443`, `066E6..066E7`, and `06C67` are explicit compiler no-op separators with no incoming direct relative branch/call target. `06520..06547` is the **40-byte ten-entry CS jump table** consumed by the indirect dispatcher at `065B1`; its targets are `065B9`, `065C3`, `065DB`, `065F3`, `0660B`, `06623`, `0662D`, `06652`, `06693`, and `0669A`.

The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.

### Fidelity notes

`062AC` and `06354` branch into shared epilogue bytes belonging to adjacent address ranges; their C++ entries preserve the resulting return semantics without double-counting those bytes. In `06548`, rows above `78h` and the five-pixel inter-column gutter return directly to input polling, while an out-of-range signed quotient still takes the redraw/page-flip path. The exit teardown releases only handles `12B30..12B50`; the tenth handle at `12B54` is deliberately not folded into that loop. The save serializer preserves every short-write failure as close-and-return-1 and models the `30700` value as a stack-local write through a distinct boundary form.

### Next high-value targets

1. Continue the sequential application path at `06CAD`.
2. Reconcile startup at `353A0` as a CODE.2-specific path.
3. Continue harvesting independently proved relocated CODE.1 runtime families.

## Batch 10 — save-load shared tail, resource panel, background renderer, and wildcard file copy

Added **2,493 translated instruction bytes** and **10 directly proven alignment bytes**, bringing cumulative accounting to **36,959 translated + 305 non-code = 37,264 / 239,698 = 15.5462% (15.55%)**.

### Newly translated ranges

- `06CAD..0706D` — **961 bytes**: backward-jump shared load tail owned by `07938`; seeks past the 32h-byte header, restores core state, party records, 62 variable descriptor payloads, ten 248h-byte blocks, runtime flags, and post-load derived state.
- `07070..0719B` — **300 bytes**: ten numbered 32h-byte label/resource slots with fallback strings, resource creation, draw placement, and text placement.
- `0719C..071B7` — **28 bytes**: ten-resource destructor over `12B30..12B54`.
- `071B8..07375` — **446 bytes**: randomized tiled background, edge/corner variants, numbered resource overlay, page publish/toggle, and final page selection.
- `07378..0739C` — **37 bytes**: FCB delete preparation and DOS INT 21h/AH=13 boundary.
- `073A0..075C8` — **553 bytes**: wildcard file-copy loop with 2Ch-byte local find state, full-file allocation/read, destination replacement/create/write, exact error cleanup, and find-next continuation.
- `07938..079DF` — **168 bytes**: save pathname construction/open and backward transfer into the `06CAD` shared tail.

### Newly proven non-code/data

`0706E..0706F`, `07376..07377`, `0739D..0739F`, and `075C9..075CB` total **10 bytes** of decoded compiler no-op alignment. A full object-1 direct relative branch/call-target scan found no incoming target to these bytes.

### Fidelity notes

`06CAD` is not a callable standalone function: `07938` opens the save file and jumps backward into it with EBX and ESI both carrying the handle. The loader restores the saved level only after forcing `C6C8=-1`, and it reconstructs per-party bits 4/2/1 from signed class bytes after the raw 920h-byte party read. The wildcard copier preserves the original asymmetry: source reads are not short-read checked, while destination writes must equal the full file size or the routine returns failure after freeing/closing resources.

The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.

### Next high-value targets

1. Continue the sequential application path at `075CC`.
2. Reconcile startup at `353A0` as a CODE.2-specific path.
3. Continue harvesting independently proved relocated CODE.1 runtime families.

## Batch 11 — save/load slot UI and confirmation workflows

Added **2,578 translated instruction bytes** plus **38 proven non-code/data bytes**. The sequential application frontier now reaches `0808A`; `0808B` is a one-byte compiler separator and `0808C..080AB` is an eight-entry CS dispatch table consumed by the next routine at `080AC`.

Translated ranges:
- `075CC..0779E` — 467 bytes: 49-character filename editor with first-edit semantics.
- `077A0..07870` — 209 bytes: live-game save-to-slot-00 workflow.
- `07874..07937` — 196 bytes: slot-00 load preparation and fall-through into `07938`.
- `079E0..07B93` — 436 bytes: interactive save-slot selector.
- `07B94..07D42` — 431 bytes: interactive load-slot selector.
- `07D44..0803B` — 760 bytes: randomized two-choice confirmation UI.
- `0803C..0808A` — 79 bytes: modal message helper.

Fidelity notes: `077A0` clears `A79D` before `171FC` and sets it to one immediately before serialization; `07874` is modeled as a side-effect wrapper because its early exits do not establish a meaningful return; both slot selectors intentionally leak the ten panel handles on the encoded right-click return-2 path; `07D44` accepts x coordinates inclusively from `C8h` through `E0h`.

## Batch 12 — left-panel dispatcher + timed status/moving-record core

Added **3,075 verified translated instruction bytes**:

- `080AC..0869E` — **1,523 bytes**: eight-way left-panel dispatcher using the already decoded `0808C` CS table; preserves signed row `IDIV`, the two-poll press gate, ESI-selected-row lifetime, case-specific capture refresh, save/load/settings actions, and exit restoration.
- `086C4..0871A` — **87 bytes**: dual per-slot table-word match and `24A9C` notifications.
- `0871C..0893B` — **544 bytes**: 26-bit timed-status expiration/message path with bit-specific counters, global flags, and signed/byte-width side effects.
- `0893C..08A7C` — **321 bytes**: four-slot deadline sweeper for nine event deadlines, the `21ACE` refresh timer, and 26 status deadlines.
- `08A80..08C87` — **520 bytes**: moving-record constructor/collision loop, including high-word packed-coordinate tile arithmetic, copied nine-dword sound-offset table, owner bits, angle/orientation update, and collision recovery.
- `08C88..08CD7` — **80 bytes**: 16h-byte record compaction; invalid signed indices report through `2F580` but deliberately continue.

Added **41 directly proven non-code/data bytes**: `0869F`, the 36-byte nine-dword table `086A0..086C3`, `0871B`, and `08A7D..08A7F`.

Cumulative accounting: **42,612 translated + 384 proven non-code/data = 42,996 / 239,698 = 17.9376% (17.94%)**. The next unresolved table begins at `08CD8`; its owning dispatcher is intentionally deferred to Batch 13.


## Batch 13 — moving-record collision/effect family

Added **1,992 verified translated instruction bytes**:

- `08CF4..0906C` — **889 bytes**: world-record collision/effect scanner, owner-derived multipliers, mode-specific type masks, and the type `58h`/`5Ah` special paths.
- `09070..09349` — **730 bytes**: four-party moving-record hit propagation with the encoded early return for mode-zero types `57h`/`59h`, proximity box, resistance/special-state checks, and signed halving.
- `09368..09417` — **176 bytes**: seven-way sound selection and distance/visibility gating.
- `09418..094DC` — **197 bytes**: modal pointer-selected angle/value input with signed `IDIV` by three and single-step 360-degree wrap.

Added **64 directly proven non-code/data bytes**: the seven-entry table `08CD8..08CF3`, alignment at `0906D..0906F`, alignment at `0934A..0934B`, the seven-entry table `0934C..09367`, and alignment at `094DD..094DF`.

Fidelity notes: `08CF4` clears its return result on any geometric candidate even when the later mode filter suppresses the effect; `09070` clears bit 1 and returns for mode-zero types `57h`/`59h` before the proximity test; signed damage halving truncates toward zero; and `09418` performs only one encoded add/subtract wrap after the signed division.

Cumulative accounting: **44,604 translated + 448 proven non-code/data = 45,052 / 239,698 = 18.7953% (18.80%)**. Next sequential frontier: **`094E0`**.


## Batch 14 — opposite-buffer selection and party effect insertion

Added **1,636 verified translated instruction bytes**:

- `094E0..095BC` — **221 bytes**: reverse-order opposite-buffer rectangle hit-test, including the encoded fall-through to record[count] when the order search misses.
- `095C0..096DF` — **288 bytes**: interactive target picker with two distinct `1A1F4` polling phases, ESC handling, pointer bounds, and `094E0` hit-testing.
- `096E0..0978C` — **173 bytes**: 12-word effect-slot insertion/fallback wrapper, preserving the mode-selected source pair and `2195C` argument order.
- `09790..09B49` — **954 bytes**: party status/equipment application, including `A5h`/`80h` special cases, `F218`/`F238` sequencing, paired UI refresh, optional sound, and timed-event installation.

Added **8 directly proven alignment bytes** at `095BD..095BF`, `0978D..0978F`, and `09B4A..09B4B`. Full-object direct relative branch/call-target scanning found no incoming target to any separator byte.

Cumulative accounting: **46,240 translated + 456 proven non-code/data = 46,696 / 239,698 = 19.4812% (19.48%)**. Next sequential frontier: `09B4C`.


## Batch 15 — party-effect animation, resolver, and modal apply flow

Added **3,136 verified translated instruction bytes**:

- `09B4C..09D8E` — **579 bytes**: descriptor-driven party-panel animation, including the all-party mode and visual-5 second draw.
- `09D90..0A253` — **1,220 bytes**: type-specific effect resolver with signed 16-bit current/max comparisons, message-only failure exits, partial-word status-bit clears, and the encoded `7Bh` recovery path.
- `0A254..0A5BB` — **872 bytes**: modal four-party target/apply loop preserving its positional eight-argument interface, eligibility tests, optional animation, timer installation, resolver fallback, and input-state restoration.
- `0A5BC..0A608` — **77 bytes**: save/suppress `D681`/`D682` while dirtying all four party panels.
- `0A60C..0A666` — **91 bytes**: restore/normalize saved input-state bytes and dirty all four panels.
- `0A668..0A790` — **297 bytes**: global party animation plus `21A14` bit-20 / `21A6A` timer installation for non-`8000h` party slots.

Added **8 directly proven alignment bytes** at `09D8F`, `0A609..0A60B`, `0A667`, and `0A791..0A793`. Full-object direct relative branch/call-target scanning found no incoming target to any separator byte.

Fidelity notes: `09D90` compares the wrapped 16-bit current/max words as signed 16-bit values; the `7Bh` branch has three distinct message-failure exits and a byte-width `21917` decrement; `0A254` preserves the two-phase input poll, effect-specific target eligibility, and the split between direct status/timer installation and `09D90` fallback.

Cumulative accounting: **49,376 translated + 464 proven non-code/data = 49,840 / 239,698 = 20.7928% (20.79%)**. Next sequential frontier: **`0A794`**.


## Batch 16 — timed status wrappers, object interactions, and projectile/world-record effects

Added **3,449 verified translated instruction bytes**:

- `0A794..0A89F` — **268 bytes**: one-slot animation wrapper and four-party bit-20 status propagation.
- `0A8A0..0AA2C` — **397 bytes**: timed bit-10 status installation with class-dependent cap and `1202C` result capture.
- `0AA30..0ABC3` — **404 bytes**: timed bit-20 status installation with class-dependent cap and byte-width stat increment.
- `0ABC4..0AC2B` — **104 bytes**: table-coordinate signed remainder helper and object flag set.
- `0AC2C..0AD23` — **248 bytes**: type-1/type-4 object interaction, including temporary bit-2 mutation and the `002A8` dispatch path.
- `0AD24..0AE2C` — **265 bytes**: selected/all-party status and event cancellation with `0871C`, `086C4`, and final `0893C` sweep.
- `0AE30..0AED6` — **167 bytes**: fixed-point probe loop with restoration of the saved global position.
- `0AED8..0B02B` — **340 bytes**: clipped 2x2 nearby-world-record effect processor with signed budget exhaustion.
- `0B02C..0B40D` — **994 bytes**: projectile/moving-record spawner, map collision scan, world-record effects, sound/redraw sequence, and type-wide cleanup.
- `0B410..0B515` — **262 bytes**: world-record to temporary moving-record conversion and exact redraw/page-flip sequence.

Added **11 directly proven alignment bytes** at `0AA2D..0AA2F`, `0AE2D..0AE2F`, `0AED7`, `0B40E..0B40F`, and `0B516..0B517`.

Fidelity notes: `0B02C` restores `C02F` and returns immediately if the first collision-cleared fixed-point position is already beyond the encoded high-word map bounds, thereby skipping its later sound/redraw cleanup. Its world-record scan is inclusive through `258A2`, the `0Bh` moving type has a distinct type-8 decrement/effect path, and guarded records force only the coordinate low words to `8000h`. `0B410` performs exactly two redraw pairs after sound `+21h`, followed by one final `28168` page flip before compacting the temporary record.

Cumulative accounting: **52,825 translated + 475 proven non-code/data = 53,300 / 239,698 = 22.2363% (22.24%)**. Next sequential frontier: **`0B518`**.


## Batch 17 — nearby-world effects and 132-way effect dispatcher

Source identity and object-1 SHA-256 were reverified before this batch. Added **4,009 translated instruction bytes** and **534 directly proven non-code/data bytes**, bringing cumulative accounting to **56,834 translated + 1,009 non-code = 57,843 / 239,698 = 24.1316% (24.13%)**; unresolved is **181,855 bytes**.

### Newly translated ranges

- `0B518..0B6F0` — **473 bytes**: nearby-world effect scan with the exact rank-indexed stack alias, signed two's-complement distance behavior, descriptor gates, mutable threshold/offset/counter aliases, and `E284` counter argument.
- `0B6F4..0B73C` — **73 bytes**: world-record descriptor-marking pass with signed record count and explicit restoration of `22230`.
- `0B950..0C6D6` — **3,463 bytes**: complete command/effect dispatcher for command values through `83h`, including all 65 explicit handlers, the shared default tail, modal selection exits, timed-status installation, projectile launch variants, and the `58h` call back into `0B518`.

### Newly proven non-code/data

- `0B6F1..0B6F3` — **3 bytes**, `LEA EAX,[EAX+0]` alignment.
- `0B73D..0B73F` — **3 bytes**, `LEA EAX,[EAX+0]` alignment.
- `0B740..0B94F` — **528 bytes**, 132-entry CS dword jump table indexed at `0B988`; every entry was decoded and reconciled.

### Fidelity notes

`0B518` preserves the encoded rank-indexed write aliases: ranks 6, 7, and 8 can overwrite the loop counter, table offset, and threshold respectively; rank zero aliases the stale dword immediately below the local array. The x86 absolute-value sequence is modeled in 32-bit two's-complement form so `INT_MIN` retains its encoded signed-compare behavior. `0B950` preserves the command-`18h/19h/1Ah/29h/5Ch` no-selection result of `FFFFFFFFh`, the distinct command-`7Ah` zero result, and command `58h`'s forced `FFFFFFFEh` result.

Direct audit also corrected an older Batch 16 boundary model: the call to `1C77C` at `0AD15` receives `(token, 0320h)`, not only `token`; Batch 17's `1Eh` handler likewise calls it as `(slot, 031Dh)`. This correction changes no byte accounting.

The sequential accounting frontier is now `0C6D7`. Bytes `0C6D7..0C703` remain deliberately unresolved; the next directly decoded routine begins at `0C704`. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` also remain unresolved.


## Batch 18 — party/stat checks, transition support, damage and level rollback

Batch 18 adds **3,567 translated instruction bytes** plus **138 directly proven non-code/data bytes**. Cumulative accounting is **60,401 translated + 1,147 non-code = 61,548 / 239,698 = 25.6773% (25.68%)**; unresolved is **178,150 bytes**.

The sequential code frontier is now `0D550`. This batch closes `0C704..0D53D` and proves the intervening compiler separators plus the data/jump tables at `0C6E0..0C703`, `0CF70..0CF87`, `0D098..0D0AF`, and `0D540..0D54F`. Preserve `0CD44`'s non-consuming `219F7` countdown before the two consuming damage buffers, `0CF88`'s three-track average and encoded divide-error edge, `0D0B0`'s class-indexed six-way table, and `0D1E0`'s tie-preserving random track selection (only a strict unique maximum overrides the initial random track). The next directly decoded routine begins at `0D550`. Known earlier zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## Batch 19 — record/effect resolution and party progression (`0D550..0E577`)

Batch 19 adds **4,071 translated instruction bytes** plus **65 directly proven non-code/data bytes**. Cumulative accounting is **64,472 translated + 1,212 non-code = 65,684 / 239,698 = 27.4028% (27.40%)**; unresolved is **174,014 bytes**.

The sequential code frontier is now `0E578`. This batch reconstructs `0D550`'s large record/effect dispatcher with its shared late tails, the `0DACC` selection scan and fallback wrapper, per-track progression/stat helpers through `0DDA4`, directional map-cell clearing at `0E028`, random world-record placement at `0E0F8`, and the five-argument record damage/death path at `0E284`. Preserve `0DDA4`'s distinct per-track experience/class/level strides, the encoded `50DC(0,slot)` argument order, `0D550`'s selected-target damage suppression rule, and `0E284`'s type-specific early-return versus common cleanup/sound/XP paths.


## Batch 20 — effect resolver, modal text UI, and nested item helpers (`0E578..0F66B`)

Batch 20 adds **4,194 translated instruction bytes** plus **146 directly proven non-code/data bytes**. Cumulative accounting is **68,666 translated + 1,358 non-code = 70,024 / 239,698 = 29.2134% (29.21%)**; unresolved is **169,674 bytes**.

The sequential code frontier is now `0F66C`. This batch reconstructs the `0E5F0` effect resolver and its nine-way CS table, the 17-buffer modal text subsystem through `0EC1C`, equipment/stat aggregation at `0EFA0`, bitfield helpers, and recursive nested-item membership/removal/value routines through `0F5F8`. Preserve `0E5F0`'s distinct success code `14h` branches, zero-vs-one sound tail arguments, two-stage duration halving, and `7Fh` amount persistence; preserve `0EC1C`'s byte-vs-dword `C050` accesses and `0F5F8`'s final 16-bit truncation. Next code begins at `0F66C`.


## Batch 21 — item descriptions, burden/rank helpers, character panel (`0F66C..10155`)

Batch 21 adds **2,743 translated instruction bytes** plus **53 directly proven non-code/data bytes**. Cumulative accounting is **71,409 translated + 1,411 non-code = 72,820 / 239,698 = 30.3799% (30.38%)**; unresolved is **166,878 bytes**.

The sequential code frontier is now `10158`; `10156..10157` is proven alignment. This batch preserves `0F790`'s ten-way item-type table split, detailed-description gates, recursive container weight and odd half-unit suffix, `0FD84/0FE44` inventory burden and rank threshold behavior, the character-panel status/HP presentation in `0FF3C`, and the `100C8/10108` wrappers' `A7..C9` BF44 override plus `C050` page toggle.


## Batch 22 — character-sheet presentation and equipment rendering (`10158..10FA8`)

Batch 22 adds **3,662 translated instruction bytes** plus **6 directly proven alignment bytes**. Cumulative accounting is **75,071 translated + 1,417 non-code = 76,488 / 239,698 = 31.9102% (31.91%)**; unresolved is **163,210 bytes**.

The sequential code frontier is now `10FAC`. This batch reconstructs the large `10158` character-sheet presentation routine, `10A70`'s mode-dependent equipment grid, and `10BBC`'s side-aware equipped-item renderer. Preserve the paired six-row state lists, packed three-track class/XP/level layout, signed HP ratio and divide-error edge, mode-3 table-derived grid geometry, and the mirrored item sprite flip/unflip sequence.


## Batch 23 — panel lifecycle, inventory redraw, and item placement (`10FAC..11C2B`)

Batch 23 adds **3,174 translated instruction bytes** plus **42 directly proven alignment/table bytes**. Cumulative accounting is **78,245 translated + 1,459 non-code = 79,704 / 239,698 = 33.2518% (33.25%)**; unresolved is **159,994 bytes**.

The sequential code frontier is now `11C3C`; `11C2C..11C3B` is the proven four-entry CS table owned by that next routine. Preserve the four-page `C050` draw/toggle sequences in `10FAC/110B4`, `11404`'s raw shifted variant-byte write, the `EEB0` range copy and stale-offset boundary in `11630`, and `117A0`'s store-before-`0F238` ordering, nested-placement early returns, repeated `50DC` call, and forced zero variant when the old item is `-1`. `11AD8` deliberately toggles only `DI` on the all-panel path and only the low byte on the single-panel path.

## Batch 37 — small checkpoint: moving-record step/retry wrapper

Added **449 translated instruction bytes** plus **3 directly proven alignment bytes**. The sequential frontier now reaches `1BD9B`; the next ownership unit begins with the four-entry table at `1BD9C` and its routine at `1BDAC`.

Fidelity notes: `1BBD8` preserves the type-9 low-word tile arithmetic, the encoded impossible duplicate `JE` as its effective fall-through semantics, descriptor-driven displacement, low-word fixed-point tile indexing, divide-by-zero fault boundary, type-8 failure state, and exactly three movement retries through `1B760`.

## Batch 38 — small checkpoint: world/record animation update

Added **781 translated instruction bytes** plus **23 directly proven non-code/data bytes**. The sequential frontier now reaches `1C0BF`; the next routine begins at `1C0C0`.

Fidelity notes: `1BDAC` owns the exact four-entry animation-state table at `1BD9C`; state 1 calls the `0D550` effect path before reloading descriptor timer `+25h`, state 2 uses timer 7 (tripled under record bit 80h), and state 3 clears the state bits before falling into the state-0 movement path. The three record-array passes preserve signed count reloads, in-place compaction with `DEC ESI` retry, the encoded always-equal `CMP EDI,EDI` artifact, and dword divide faults when metadata divisor byte `+5` is zero.

## Batch 39 — small checkpoint: bitset/text/dice/file helpers

Added **829 translated instruction bytes** plus **15 directly proven alignment bytes**. The sequential frontier now reaches `1C40B`; the next routine begins at `1C40C`.

Fidelity notes: `1C0C0` keeps both signed IDIV stages before the 90-degree scale; `1C124/1C160/1C1A0/1C1DC` preserve signed divide-by-32 remainder shift counts, including `1C1DC` returning `FFFFFFFFh` when selected bit 31 is arithmetically shifted back; `1C2C4` uses the encoded unsigned deadline comparison; `1C328` keeps low-32 IMUL/logical-shift dice scaling and wrapped additions; `1C368` reloads handle `22224` for each seek/read and XOR-decodes the payload backward with a 16-bit decrement loop.

## Batch 40 — small checkpoint: framed/tiled panel renderer

Added **879 translated instruction bytes** plus **1 directly proven alignment byte**. The sequential frontier now reaches `1C77B`; the next routine begins at `1C77C`.

Fidelity notes: `1C40C` preserves the encoded signed minimum-size tests, the low-byte-only `AND AL,E0h/F0h` rounding after addition, signed truncation-toward-zero half-offset arithmetic, random edge/corner tile selection, signed loop comparisons, and the final `2E5D4` page-release call.

## Batch 41 — enlarged checkpoint: actor modal + selector family

Added **1,628 translated instruction bytes** plus **144 directly proven non-code/data bytes**. The sequential frontier now reaches `1CE67`; the next routine begins at `1CE68`.

Fidelity notes: `1C77C` preserves byte-only `C050` page toggles while passing the full dword to page/render callbacks, the actor graphic index combines signed byte `+12`, unsigned byte `+13 * 17h`, global `258E4`, and optional `+2Eh` from flag `0100h`, and the input wait remains release/press/release. `1C924` performs a random-start four-slot search that rejects `0080h` and `F240h`, then falls back to an unbounded `F240h`-only search. `1CA2C` owns the exact 35-entry criterion table and retains signed tests for bytes `+15..+17` only where the binary uses MOVSX. `1CD48` preserves the criterion-21h pushed-text stack reuse when it calls `1C924` and then `1C77C`, and always latches `arg0+80h` when arg0 is not `FFFFFFFFh`.

## Batch 42 — command/event dispatcher

Added **5,688 translated instruction bytes** and no new non-code/data bytes. The complete `1CE68..1E49F` owner is reconciled; the next routine begins directly at `1E4A0`.

Fidelity notes: the original large unsigned compare tree over `BF44` is expressed as a switch without changing command behavior. All `1CD48` selector argument tuples and ordering are retained, including the live `BF44` reread on commands `11h/12h`, event-bit gates and mutations, command `2Ch`'s zero-valued live-ESI fallback criterion, command `33h`'s `14A60(111Ch)` modal actor selection, item predicates in `2Fh/4Fh/50h`, and the low-32 `ADD EAX,EAX` random text choice used by `4Ch/60h/61h/62h`. The special command `270Fh` is also preserved.

## Batch 44 — terrain cycling, encounter selection, and party-panel refresh

Added **2,499 translated instruction bytes** plus **137 directly proven table/alignment bytes**, reconciling `1EDAC..1F7F7`. The `1EEFC..1EF7B` region is the exact 32-entry CS table owned by `1EF7C`; the next routine begins at `1F7F8`.

Fidelity notes: `1EDAC` performs at most one mapping replacement per lane before cycling the two `242h..244h` / `245h..247h` triplets. `1EF7C` retains unsigned selector bounds and low-32 random scaling. `1F1C8` keeps the truncation-toward-zero signed divisions used to derive `FF50` and the asymmetric `C026/C027/C028` update paths. `1F494` preserves the high-bit actor path's direct jump to `1F745` (skipping `22F40` and both `1EA90` calls), signed HP-bar `IDIV`, and the pre-pushed zero that becomes `30508`'s fifth argument. The same stack pattern proves the Batch-43 `2E6F0` boundary takes one argument; that signature is corrected here without changing byte accounting.

## Batch 45 — movement state, modal input, and main game loop

Added **2,776 translated instruction bytes** plus **32 directly proven table/alignment bytes**, reconciling `1F7F8..202EF`. The five-entry CS table at `1F858..1F86B` belongs to `1F86C`; the next routine begins at `202F0`.

Fidelity notes: `1F86C` preserves signed tick expiry and state-4's `2Dh` call / `C018=-1` / fall-through `2Eh..31h` ordering. `1F8E0` uses the prior routine's shared `RET` when the mode is unchanged and otherwise falls directly into `1F8FC`. `1FC14` leaves `258FE` live at entry, dispatches on AX only, keeps the partial-EDX/low-word movement arithmetic, preserves the selected-axis decay exemption when direction*value is nonnegative, and models the `IMUL; SHR 3; CWDE` fixed-point conversions exactly. The loop has no encoded return.


## CODE.2 Batch 46 checkpoint

Batch 46 added **2,807 translated bytes** plus **33 proven non-code/data bytes**. Cumulative accounting is **138,937 translated + 2,667 non-code = 141,604 / 239,698 = 59.0760% (59.08%)**; unresolved is **98,094 bytes**.

The sequential frontier now reaches `20E07`; the next routine begins at `20E08`. Preserve `20850`'s two-pass actor update, including the reused stack argument slots that become the pointer pair for `207E8`; `20994`'s signed `IDIV`/remainder path before the 90-degree scaling; `20B40`'s byte-wrap effect timers and second cleanup pass only when the active count reaches zero; and `20C0C`'s packed `8000h:word` fixed-point arguments plus type-1/type-2 activation differences. `207E0..207E7` is an eight-byte table referenced later at `2188C`; the preceding fifteen zero bytes are proven padding. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.


## CODE.2 Batch 47 checkpoint

Batch 47 added **2,297 translated instruction bytes** plus **3 proven alignment bytes**. Cumulative accounting is **141,234 translated + 2,670 non-code = 143,904 / 239,698 = 60.0355% (60.04%)**; unresolved is **95,794 bytes**.

The sequential application frontier now reaches `21703`; the next routine begins at `21704`. Preserve `20E08`'s two-stage `1C328` decision and saved `20D7C` outputs, the deliberately non-terminating `210B0` `MOVZX word`/`CMP -1` table scan, `211D8`'s type-0Dh/type-0Fh list insertion fallbacks, and `214B8`'s saved `8600/8602` values across opaque calls while reloading `255D8` before the post-callback type/subtype read. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.

## CODE.2 Batch 48 checkpoint

Batch 48 added **3,266 translated bytes** plus **46 proven non-code/data bytes**. Cumulative accounting is **144,500 translated + 2,716 non-code = 147,216 / 239,698 = 61.4173% (61.42%)**; unresolved is **92,482 bytes**.

The sequential frontier now reaches `223F3`; the next routine begins at `223F4`. `21B24..21B43` is the exact eight-entry CS jump table owned by `21B44`. Preserve `2195C`'s low-word-only `8000h` fixed-point overwrite, `21BF0`'s raw arithmetic `SAR` before later truncation-toward-zero division idioms, and `21F70`'s constant-nonzero `280E8h` test that makes the encoded `28110` branch dead. The long `21F70` orientation loops also retain their encoded unreachable inner arms instead of being normalized into a cleaner angle helper. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.

## CODE.2 Batch 49 checkpoint

Batch 49 added **2,503 translated instruction bytes** plus **133 proven non-code/data bytes**. Cumulative accounting is **147,003 translated + 2,849 non-code = 149,852 / 239,698 = 62.5170% (62.52%)**; unresolved is **89,846 bytes**.

The sequential frontier now reaches `22E3F`; the next routine begins at `22E40`. `22950..229A3` and `229A4..229CB` are the exact CS tables owned by `229CC`. Preserve `223F4`'s AL/CL/DL-only page toggle and its one-past-count record pointer when an ordinal is not found, `229CC`'s persistent `EBX -= 20h` mutation after a failed `223F4` probe, `22894`'s signed `IDIV` quotient/remainder and truncation-toward-zero scale arithmetic, and the queued-input ring's head advance before dispatch. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.

## CODE.2 Batch 50 checkpoint

Batch 50 added **4,444 translated instruction bytes** plus **40 proven non-code/data bytes**. Cumulative accounting is **151,447 translated + 2,889 non-code = 154,336 / 239,698 = 64.3877% (64.39%)**; unresolved is **85,362 bytes**.

The sequential frontier now reaches `23FC3`; the next routine begins at `23FC4`. Preserve `23024`'s equality-only six-row termination (not an `>=` precheck), `231C0`'s two packed-nibble list passes and DEC/JNE ownership, `23438`'s signed panel arithmetic plus high-nibble wrap decrement, `2388C`'s signed quotient/remainder angle path and record rollback, and `23B38`'s stale-record read/modify/write flag behavior plus collision retry loop. `23DE5..23DEF` is proven padding; the older zero islands remain unresolved. Semantic audit remains not started.

## CODE.2 Batch 51 checkpoint

Batch 51 added **2,771 translated instruction bytes** plus **5 proven alignment bytes**. Cumulative accounting at that checkpoint is **154,218 translated + 2,894 non-code = 157,112 / 239,698 = 65.5458% (65.55%)**; unresolved was **82,586 bytes**.

The sequential frontier reaches `24A9B`; the next routine begins at `24A9C`. Preserve `240A8`'s role-dependent admissibility and default-one behavior, `243E0`'s empty-slot global gate, and `24478`'s asymmetric class/rank arbitration, repeated actor-role predicates, low-two-bit item class tests, per-side cooldown dword gates, and selector-0/1 return ownership. Known zero islands remain unresolved. Semantic audit remains not started.

## CODE.2 Batch 52 integrity repair

The previously landed Batch 52 is invalid and has been fully removed: it disassembled raw object offsets beginning at `14A60` but labeled them with an erroneous `+10000h` shift beginning at `24A60`. Fresh immutable CODE.2 disassembly confirms `24478` continues through its RET at `24A9B`; `24A9C` is the true continuation. Batch 51's original boundary and accounting are restored before the corrected Batch 52 is counted.

## CODE.2 Batch 52 checkpoint

Corrected Batch 52 adds **4,937 translated instruction bytes** plus **23 proven alignment bytes**. Cumulative accounting is **159,155 translated + 2,917 non-code = 162,072 / 239,698 = 67.6151% (67.62%)**; unresolved is **77,626 bytes**.

The sequential frontier now reaches `25DFB`; the next object begins at `25DFC` with a CS dword dispatch table retained with its owning dispatcher. Preserve `24A9C`/`24B48` panel-slot arithmetic and cooldown dword ownership, `24BF4`'s signed clipping/alignment sequence, `24CC8`'s byte-only `C050` page toggle and capture lifecycle, `24F60`/`251A0`'s resource/display restoration ordering, `253F8`/`254D4` inventory-group ownership, `256C4`'s 22-byte record compaction, `25798`/`258F8`/`25A10` resource gates, and `25D24`'s low-word add-and-clamp behavior. Known zero islands remain unresolved. Semantic audit remains not started.

## CODE.2 Batch 53 checkpoint

Batch 53 adds **5,071 translated instruction bytes** plus **540 proven table bytes**, bringing cumulative accounting to **164,226 translated + 3,457 non-code = 167,683 / 239,698 = 69.9559% (69.96%)**; unresolved is **72,015 bytes**.

The sequential frontier now reaches `273E6`. `25DFC..26017` is the exact 135-entry CS dword table owned by `26018`; `273E7..273EF` remains unresolved and the next independently decoded routine begins at `273F0`. Preserve `26018`'s cooldown early-return that skips `19CF4`, the load-before-`-1` item-kind override, category-`0Eh` table ownership, AL-only peer-slot XORs, live global reloads after opaque group-effect calls, low-word fixed-field extraction used for `9*x` delays, the partial-word status test at item `42h`, and item `73h`'s ten-argument pre-pushed `B02C` frame. Semantic audit remains not started.


## CODE.2 Batch 54 checkpoint

Batch 54 added **2,797 translated instruction bytes** plus **80 directly proven alignment/padding bytes**, reconciling the prior nine-byte frontier gap and the complete low-level block `273F0..27F22`. Cumulative accounting is **167,023 translated + 3,537 non-code = 170,560 / 239,698 = 71.1562% (71.16%)**; unresolved is **69,138 bytes**.

The sequential frontier now reaches `27F23`; `27F23` is proven `NOP` alignment and the next instruction-owned routine begins at `27F24` with its nonstandard register-save prologue. Preserve `273F0`'s plane-mask rotation plus ADC-driven VGA-byte advance, `274A4`'s unsigned multiply/divide followed by signed-16 wrap, `27580/275E4/27638/27694`'s exact read-map/write-mask widths, `2773C`'s sparse mode overlays, the `27894` interrupt frame's SS/ESP swap and reentry latch, `279D0`'s DOS error-50h filename retry, `27AEC/27BD8`'s encoded stream-cursor ownership, `27C70`'s dword-at-a-time zero detection, and `27D20/27ECC`'s one-time multimedia init/teardown state. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.


## CODE.2 Batch 55 checkpoint

Batch 55 added **2,032 translated instruction bytes** plus **40 directly proven alignment/padding bytes**, covering the complete multimedia queue/resource family `27F24..28738` and its inter-routine separators through `2873B`. Cumulative accounting is **169,055 translated + 3,577 non-code = 172,632 / 239,698 = 72.0206% (72.02%)**; unresolved is **67,066 bytes**.

The sequential frontier is now `2873C`. Preserve `27F24`'s count-preserving REP copy and DS selector arguments, `28110`'s signed existing-vs-incoming priority comparison and `28168`'s tie-to-second-slot selection, `28210`'s exact ten-byte stack filename copy plus fall-through after the error formatter, and `282BC`'s device-type-3 setup. In particular, do not normalize its live ESI behavior: when `EEF8` is zero, ESI still holds the freed file buffer; when the later `EE80` allocation fails, `28564` doubles that live ESI once and closes the same word at `[EEDC+ESI]` eight times. `28670` reloads the `EE8C` base only on a match before adding the record dword. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.


## CODE.2 Batch 56 checkpoint

Batch 56 added **3,214 translated instruction bytes** plus **74 directly proven alignment/padding bytes**, covering the complete ownership fold `2873C..29413`. Cumulative accounting is **172,269 translated + 3,651 non-code = 175,920 / 239,698 = 73.3924% (73.39%)**; unresolved is **63,778 bytes**.

The sequential frontier is now **`29414`**. Preserve the mode-3 wrapped signed scaling in `2873C`, `2878C`'s zero-mapping early exit, current-slot suppression in `28AE0`, state-value behavior in `29058`, BX-only/high-half semantics in `290D0`, `290F4`'s byte/word side effects, the clipping asymmetry of `292BC` versus `2932C`, and the shared-RET/tail-transfer ownership of `2939C/293C0/293EC`. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.


## CODE.2 Batch 57 checkpoint

Batch 57 added **1,281 translated instruction bytes** plus **27 directly proven alignment/padding bytes**, reconciling `29414..2992F`. Cumulative accounting is **173,550 translated + 3,678 non-code = 177,228 / 239,698 = 73.9380% (73.94%)**; unresolved is **62,470 bytes**.

The sequential frontier is now `29930`, the independent VGA mode/setup prologue. Preserve `29414/29564` clipping globals `EF69..EF6F`, their full-vs-clipped return codes 2/1/0, `296C0`'s signed truncation-to-zero quartering and BL-only alignment effect, `29738`'s exact 16-byte copy, and the EFD4 nested scratch-record ownership in `2978C/29804/2986C/298B4`.


## CODE.2 Batch 58 checkpoint

Batch 58 added **939 translated instruction bytes** plus **17 directly proven alignment/table bytes**, reconciling through `29CEB`. Cumulative accounting is **174,489 translated + 3,695 non-code = 178,184 / 239,698 = 74.3369% (74.34%)**; unresolved is **61,514 bytes**.

The sequential frontier is now `29CEC`, the next independent raster primitive. Preserve `29930`'s BIOS/VGA register ordering and descriptor writes, the direct `29AA0 -> 29B1C` wrapper, the unbounded CS-byte lookup at `29B33`, `29B40`'s shared `29B37` epilogue plus partial-register mask/counter behavior, and `29CA0`'s retrace wait before every <=192-byte DAC chunk.


## CODE.2 Batch 59 checkpoint

Batch 59 added **1,048 translated instruction bytes** plus **16 directly proven alignment bytes**, reconciling through `2A113`. Cumulative accounting is **175,537 translated + 3,711 non-code = 179,248 / 239,698 = 74.7808% (74.78%)**; unresolved is **60,450 bytes**.

The sequential frontier is now `2A114`. Preserve `29CEC`'s line-drawing fast paths and plane-mask/error rotations, `29EAC/29F40` low-word sign tests with full-register arithmetic, `2A020`'s CX-only count shift, and `2A078`'s page save/select/restore before mode-1 planar copies.


## CODE.2 Batch 60 checkpoint

Batch 60 added **1,427 translated instruction bytes** plus **9 directly proven alignment bytes**, reconciling through `2A6AF`. Cumulative accounting is **176,964 translated + 3,720 non-code = 180,684 / 239,698 = 75.3799% (75.38%)**; unresolved is **59,014 bytes**.

The sequential frontier is now `2A6B0`, the next independent clipped blitter. Preserve plane-major transfer ownership, partial-register loop counts, and the writable-code immediates/displacements in `2A478` and `2A584`.


## CODE.2 Batch 61 checkpoint

Batch 61 added **1,453 translated instruction bytes** plus **35 directly proven alignment/padding/table bytes**, reconciling through `2AC7F`. Cumulative accounting is **178,417 translated + 3,755 non-code = 182,172 / 239,698 = 76.0006% (76.00%)**; unresolved is **57,526 bytes**.

The sequential frontier is now `2AC80`, the next independent application predicate. Preserve `2A6B0/2A8A0` clipped-plane asymmetries and writable-code bytes, `2AAEC`'s register ABI, the direct `2AC70` table ownership, and low-level VGA/retrace/CRTC ordering.

## CODE.2 Batch 62 checkpoint

Batch 62 added **1,022 translated instruction bytes** plus **10 directly proven alignment bytes**, reconciling through `2B087`. Cumulative accounting is **179,439 translated + 3,765 non-code = 183,204 / 239,698 = 76.4312% (76.43%)**; unresolved is **56,494 bytes**.

Instruction ownership spans `2AC80..2ACD9`, `2ACDC..2AD14`, `2AD18..2ADDC`, `2ADE0..2AE2F`, and `2AE30..2B085`. The four compiler separators total ten bytes and have no direct incoming control-flow target. `2AE30` retains the entry-time opposite-page record base but reloads the `C050`-selected 16-bit count on every loop test, including after the opaque `1C1DC` probes reached through `2AD18`. The next independent routine begins at `2B088`.

## CODE.2 Batch 63 checkpoint

Batch 63 added **1,600 translated instruction bytes** plus **8 directly proven alignment bytes**, reconciling through `2B6CF`. Cumulative accounting is **181,039 translated + 3,773 non-code = 184,812 / 239,698 = 77.1020% (77.10%)**; unresolved is **54,886 bytes**.

The fold owns `2B088..2B185`, `2B188..2B1B9`, `2B1BC..2B2F2`, and `2B2F4..2B6CC`. Preserve grouped-key value propagation in `2B088`, signed range tests in `2B188`, the signed max-distance rounding in `2B1BC`, and `2B2F4`'s signed IDIV quotient/remainder behavior, mutable `2B608` word cache, partial-register heading extraction, and mode-indexed `27100` tail addition. The next independent routine begins at `2B6D0`.

## CODE.2 Batch 64 checkpoint

Batch 64 added **3,005 translated instruction bytes** plus **7 directly proven alignment bytes**, reconciling through `2C293`. Cumulative accounting is **184,044 translated + 3,780 non-code = 187,824 / 239,698 = 78.3586% (78.36%)**; unresolved is **51,874 bytes**.

Instruction ownership spans `2B6D0..2BB9E`, `2BBA0..2BDF0`, `2BDF4..2BFE9`, `2BFEC..2C22E`, and `2C230..2C293`. Preserve the per-iteration signed record-count reloads, the `190h` target-cursor cap, descriptor/metadata reloads after the encoded opaque/random calls, the temporary `7D00h` screen-space shifts around `21BF0`, and the compiler's signed truncation-toward-zero division sequence in `2C230`. The next independent routine begins at `2C294`.

## CODE.2 Batch 65 checkpoint

Batch 65 added **3,418 translated instruction bytes** plus **22 directly proven alignment bytes**, reconciling through `2D003`. Cumulative accounting is **187,462 translated + 3,802 non-code = 191,264 / 239,698 = 79.7937% (79.79%)**; unresolved is **48,434 bytes**.

Instruction ownership spans `2C294..2C7F6`, `2C800..2C8E1`, `2C8E4..2C9D8`, `2C9DC..2CA5B`, `2CA5C..2CAD2`, `2CAD4..2CC84`, `2CC88..2CEB5`, and `2CEB8..2D001`. Preserve `2C294`'s dynamic `F03C/F03E` clip tests and compiler-order rectangle restore, callback-sensitive target/source reloads, `2CAD4`'s independent party-slot branches, and `2CEB8`'s low-word-only `255D4 -> 255D0` transition copy plus post-callback `258EC` reload. The next independent routine begins at `2D004`.

## CODE.2 Batch 66 checkpoint

Batch 66 added **3,862 translated instruction bytes** plus **18 directly proven alignment bytes**, reconciling through `2DF2B`. Cumulative accounting is **191,324 translated + 3,820 non-code = 195,144 / 239,698 = 81.4124% (81.41%)**; unresolved is **44,554 bytes**.

Instruction ownership spans `2D004..2D232`, `2D234..2D2F0`, `2D2F4..2D3C2`, `2D3C4..2D3D4`, `2D3D8..2D5CC`, `2D5D0..2D9F8`, `2D9FC..2DA7A`, `2DA7C..2DBAA`, and `2DBAC..2DF29`. Preserve `2D004`'s signed 25622 range checks and IDIV remainder update, `2D5D0`'s high-word fixed-point window extraction plus AX-only return consumption, `2DA7C`'s doubled-offset tile probe and early outer exit, and `2DBAC`'s low-word tile-code edits and fallback move ordering. The next independent routine begins at `2DF2C`.

## CODE.2 Batch 67 checkpoint

Batch 67 added **2,161 translated instruction bytes** plus **83 directly proven non-code/data bytes**, reconciling through `2E7EF`. Cumulative accounting is **193,485 translated + 3,903 non-code = 197,388 / 239,698 = 82.3486% (82.35%)**; unresolved is **42,310 bytes**.

Instruction ownership spans `2DF2C..2DFE8`, `2E00C..2E0E2`, `2E0E4..2E11C`, `2E120..2E338`, `2E340..2E348`, `2E34C..2E35E`, `2E360..2E372`, `2E374..2E434`, `2E438..2E4A0`, `2E4A4..2E50C`, `2E510..2E535`, `2E538..2E560`, `2E564..2E58C`, `2E590..2E5D2`, `2E5D4..2E612`, `2E614..2E6ED`, `2E6F0..2E708`, `2E70C..2E724`, `2E728..2E749`, `2E74C..2E755`, `2E758..2E7EE`. Preserve `2DF2C`'s signed-high-word clamp condition and exact eight-entry CS table, `2E120`'s 16-bit NEG/MOVSX movement path, nested page-state semantics in `2E590/2E5D4`, and `2E614`'s encoded dead `0x1F0` branch after BL bit 2 is cleared. The next independent routine begins at `2E7F0`.

## CODE.2 Batch 68 checkpoint

Batch 68 added **2,173 translated instruction bytes** plus **71 directly proven non-code/data bytes**, reconciling through `2F0B3`. Cumulative accounting is **195,658 translated + 3,974 non-code = 199,632 / 239,698 = 83.2848% (83.28%)**; unresolved is **40,066 bytes**.

Instruction ownership spans `2E7F0..2E9CA`, `2E9CC..2EA26`, `2EA28..2EA99`, `2EA9C..2EB25`, `2EB28..2EB5D`, `2EB60..2EC72`, `2EC74..2EC83`, `2EC84..2EC94`, `2ECB4..2EFB2`, and `2EFD0..2F0B1`. The exact CS tables `2EC98..2ECB3` and `2EFB4..2EFCF` are owned by their adjacent dispatchers. Preserve uninitialized high bits when `2E7F0` initializes item byte 0, `2ECB4`'s out-of-range error path continuing into the indexed access, low-byte selector arithmetic, callback/global reloads, and `2EFD0`'s type-dependent release calls. This batch also repairs the proven ABI at `28020` and `286C4`: both ignore their first stack argument and consume the stream from the second. The next independent routine begins at `2F0B4`.


## Batch 69 — resource-item locking and environment lifecycle

Source identity and the Batch-68 overlap were reverified before this batch. Added **1,209 translated instruction bytes** and **19 directly proven alignment bytes**, bringing cumulative accounting to **196,867 translated + 3,993 non-code = 200,860 / 239,698 = 83.7971% (83.80%)**.

### Newly translated ranges

- `2F0B4..2F182` — 207 bytes: lowest-age matching item scan across all resource slots, followed by the address-distinct `2EFD0` release path.
- `2F184..2F1B0` / `2F1B4..2F1E0` — 45 bytes each: set/clear item lock bit `10h` only for populated item records.
- `2F1E4..2F25D` — 122 bytes: clear lock bit from every populated item except the globally selected record index, preserving the compiler's final writes back to `F0B8/F0C0`.
- `2F260..2F29A` — 59 bytes: rebuild current slot-0 item then lock it.
- `2F29C..2F2DF` — 68 bytes: load an item/slot pair and return payload only for selector/type byte 1.
- `2F2E0..2F2EB` — 12 bytes: far callback body that stores the encoded stack `+0Ch` value into `F6F8` and returns zero via RETF.
- `2F2EC..2F464` — 377 bytes: environment/bootstrap routine, including far callback installation, backend setup, configuration loading, direct port writes for mode `79h`, graphics/audio backend initialization, and optional resource-slot initialization.
- `2F468..2F4DF` — 120 bytes: switch the current slot-0 item, clear/set lock bits, and publish `F0BC/F0C4`.
- `2F4E0..2F53E` — 95 bytes: ordered backend/resource teardown.
- `2F540..2F551` / `2F554..2F57C` — 18/41 bytes: backend restore wrapper and full shutdown wrapper.

### Fidelity notes

`2F0B4` chooses the minimum unsigned 16-bit age among matching unlocked records and reloads the signed slot/item counts exactly as encoded. `2F2EC` preserves signed 16-bit device fields and low-16-bit I/O port behavior, and treats the `2F2E0` callback as a far-return boundary rather than a normal near callback. `2F1E4` intentionally exempts the same global item index in every slot, rather than interpreting `F0C0` as a slot-qualified pair.

The next independent routine begins at **`2F580`**.

## Batch 70 — expanded presentation, buffered resource decode, image materialization, and text helpers

Added **4,070 translated instruction bytes** and **66 proven non-code/data bytes**, bringing cumulative accounting to **200,937 translated + 4,059 non-code = 204,996 / 239,698 = 85.5226% (85.52%)**.

- `2F580..2F5F0` — presentation/error wrapper around the interactive prompt path and backend restore.
- `2F5F4..2F838` — centered prompt/input presenter, including the encoded incoming-ESI return on delay-only mode, page restoration, and mode-5 auxiliary state.
- `2F83C..2F95F` — presentation flags plus deferred prompt construction; `2F860` deliberately falls through into the independently callable `2F868`.
- `2F960..2FD89` — 1 KiB buffered stream reader, byte/RLE reader, callback-driven row assembly, and raw/RLE planar loaders.
- `2FD8C..2FD9B` — exact four-entry CS jump table owned by `2FD9C`: `2FEBC,2FF32,2FFA4,2FFAB`.
- `2FD9C..30248` — image loader/materializer and capture/descriptor wrappers, preserving signed `IDIV`, low-word allocation truncation in `30070`, and `30050`'s upper-EAX preservation across AX-only multiply.
- `3024C..305A5` — text mutation, metrics, character/string rendering, cursor state, centering, and optional masked draw path. `303C8` and `30458` retain the binary's shared-return behavior semantically without double-counting the earlier tail bytes.

Proven separators/table bytes total **66**; a full object-1 disassembly reference scan found no incoming reference into any separator. The only reference into the non-code ranges is the expected indirect `CS:[2FD8C + index*4]` jump-table use by `2FD9C`.

Sequential frontier: **`305A8`**. Semantic audit remains not started.
## Batch 71 — text layout, keyboard/IRQ state, vector lifecycle, RNG, and memmove

Added **1,843 translated instruction bytes** and **85 directly proven non-code/data bytes**, bringing cumulative accounting to **202,780 translated + 4,144 non-code = 206,924 / 239,698 = 86.3270% (86.33%)**. Unresolved is **32,774 bytes**.

Instruction ownership spans `305A8..306F7`, `30700..30705`, `30708..30735`, `30738..30744`, `30748..307C7`, `307F0..308B8`, `308BC..308F4`, `308F8..30AB9`, `30ABC..30AD6`, `30AD8..30AEA`, `30AEC..30B18`, `30B1C..30B2A`, `30B2C..30B3D`, `30B40..30C36`, `30C38..30C90`, and `30C94..30D25`. The exact ten-entry CS direction table is `307C8..307EF`; all other newly proven non-code bytes are direct compiler alignment or zero padding with no incoming direct control-flow reference.

Fidelity notes: `305A8` intentionally indexes glyph widths with signed characters while `30630` uses unsigned character indices; `308F8` keeps the encoded signed `JLE` split at scan code `80h` and the CODE.2-specific `19h` path that stores word `1` at `1457C`; `307F0` preserves the exact ten-way direction dispatch; vector install/uninstall retains saved offset/selector pairs; and `30CD4` preserves the backward-copy overlap condition and forward-copy behavior. The next independent routine begins at **`30D30`**.
## Batch 72 — DPMI/runtime memory wrappers, allocator recovery, buffered output, and handle helpers

Added **1,852 translated instruction bytes** and **68 directly proven non-code/data bytes**, bringing cumulative accounting to **204,632 translated + 4,212 non-code = 208,844 / 239,698 = 87.1280% (87.13%)**. Unresolved is **30,854 bytes**.

Instruction ownership spans `30D30..30DE6`, `30DE8..30E2E`, `30E30..30E62`, `30E64..30F88`, `30F8C..30FC8`, `30FCC..31012`, `31014..31048`, `3104C..311D4`, `311E0..31232`, `31234..31271`, `31274..31279`, `3127C..3129E`, `312A0..312B1`, `312B4..312C5`, `312C8..312D9`, `312DC..312E8`, `312EC..31300`, `31304..31318`, `3131C..31330`, `31334..3137C`, `31380..3138C`, `31390..313A8`, `313AC..313C0`, `313C4..313EA`, `313EC..31482`, and `31484..314AA`. All 68 intervening bytes are direct NOP/MOV-EAX-EAX/LEA alignment or zero padding; a full object-1 direct CALL/JMP/Jcc scan found no incoming target into any of them.

Fidelity notes: `30D80` and `30E64` retain signed truncation-toward-zero paragraph arithmetic after 32-bit wrap; `31274` preserves the incoming upper 24 EAX bits while replacing AL; `312EC`, `31304`, and `3131C` retain the binary's stack-induced `(third, first, second)` forwarding order; `3104C` keeps its one-byte recovery stage and repeats stage 2 indefinitely after it is reached; `31107` keeps byte-sized stream flag updates while testing the full dword at +0C; and `313FC` restores the saved drive only when pathname open fails. The next independent application routine begins at **`314B0`**.
## Batch 73 — perspective-row renderer

Added **1,025 translated instruction bytes** and **3 directly proven alignment bytes**, bringing cumulative accounting to **205,657 translated + 4,215 non-code = 209,872 / 239,698 = 87.5568% (87.56%)**. Unresolved is **29,826 bytes**.

Instruction ownership is `314B0..318B0`; `318B1..318B3` is the compiler LEA alignment before the next independent routine. A full object-1 direct CALL/JMP/Jcc target scan found no incoming target into those three bytes.

Fidelity notes: record zero is processed unconditionally while later records terminate on word0==0; the random decoration prepass reuses the two arguments left on the cdecl stack by `30CA2`, yielding the exact effective `29838(0,4,sprite,random_set,8)` call; the initial and post-prepass RNG seeds are `C658` and the live tick from `30700`; signed projection uses the encoded `(delta<<7)+12h` then signed divide by 24h; and failed visibility tests try +168h wrap, then -168h wrap without a final visibility rejection. The next independent routine begins at **`318B4`**.
## Batch 74 — packed shading lookup generators

Added **929 translated instruction bytes** and **3 directly proven alignment bytes**, bringing cumulative accounting to **206,586 translated + 4,218 non-code = 210,804 / 239,698 = 87.9457% (87.95%)**. Unresolved is **28,894 bytes**.

Instruction ownership is `318B4..31A11` and `31A14..31C56`; `31A12..31A13` is the compiler `MOV EAX,EAX` alignment and `31C57` is a NOP. A full object-1 direct CALL/JMP/Jcc target scan found no incoming target into either separator.

Fidelity notes: both generators retain signed `IDIV` behavior and arithmetic-right-shift rounding, write only the low byte of intermediate quotients into signed-byte temporaries, apply the parity decrement before the low-intensity and 8/9 collapse rules, and pack the two signed bytes into the published word table. `318B4` mirrors the capped low byte into `FE14/FE94`; `31A14` preserves byte-width subtraction of the fade adjustment, its `F954` per-argument scale divisor, and the two bounded lookup-band writes. The next independent routine begins at **`31C58`**.
## Batch 75 — two-phase packed shading/lookup generator

Added **538 translated instruction bytes** and **2 directly proven alignment bytes**, bringing cumulative accounting to **207,124 translated + 4,220 non-code = 211,344 / 239,698 = 88.1709% (88.17%)**. Unresolved is **28,354 bytes**.

Instruction ownership is `31C58..31E71`; `31E72..31E73` is the compiler `MOV EAX,EAX` alignment. A full object-1 direct CALL/JMP/Jcc target scan found no incoming target into those two bytes.

Fidelity notes: the first phase uses fixed shade/period values 3/4 with argument-dependent left/right widths, captures the signed `IDIV` quotient through AL before clamping, applies the same parity/high-byte rules as the adjacent generators, and publishes its final packed word to `FCFA`. The second phase rebuilds the 128-byte mirrored `FE94/FE14` bands with fixed 15/45 edge widths and the encoded rounded 7/69 interpolation. The next independent routine begins at **`31E74`**.
## Batch 76 — active-party shade helper and paired clamp/setters

Added **282 translated instruction bytes** and **6 directly proven alignment bytes**, bringing cumulative accounting to **207,406 translated + 4,226 non-code = 211,632 / 239,698 = 88.2911% (88.29%)**. Unresolved is **28,066 bytes**.

Instruction ownership is `31E74..31F34` and `31F38..31F90`; the two three-byte LEA separators have no incoming direct branch target. The helper returns 8 when any of four qualifying actor slots has mask 0402h set, otherwise 0. The paired setters clamp `F920` to 0..10 or `F91C` to 0..8, add the helper result to `F91C`, then regenerate shading through `31A14`. Next frontier: **`31F94`**.
## Batch 77 — phase-change view record/shading updater

Added **356 translated instruction bytes** and no new non-code bytes, bringing cumulative accounting to **207,762 translated + 4,226 non-code = 211,988 / 239,698 = 88.4396% (88.44%)**. Unresolved is **27,710 bytes**.

The complete `31F94..320F7` routine normalizes the high word of `CF4C` to phase 0..23, skips work when the phase matches `F9CC`, updates the active view record when `258A8 != FFh`, rotates the RNG-backed selection only on the sentinel state, copies phase-indexed words from `F99A/F96A`, and regenerates shading. Next frontier: **`320F8`**.
## Batch 78 — formatter byte-emitter and sprintf wrapper

Added **74 translated instruction bytes** and **14 proven zero-padding bytes**, bringing accounting to **207,836 translated + 4,240 non-code = 212,076 / 239,698 = 88.4763% (88.48%)**. Unresolved is **27,622 bytes**. `320F8` is the `385B0` byte-emitter callback (cursor at state+0, count at +10h); `3210F` builds the formatter call and terminates destination[count]. `32142..3214F` is zero padding with no incoming direct target. Next frontier: **`32150`**.
