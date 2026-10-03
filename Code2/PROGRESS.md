# CODE.2 faithful decompilation progress

![Progress](https://img.shields.io/badge/object%20accounting-19.48%25-red)
![Semantic audit](https://img.shields.io/badge/semantic%20audit-not%20started-lightgrey)

**Executable object-1 denominator:** **239,698 bytes (`0x3A852`)**  
**Verified translated instruction bytes:** **46,240**
**Proven non-code/data bytes:** **456**
**Reconciled object bytes:** **46,696 / 239,698 = 19.4812% (19.48%)**
**Unresolved object bytes:** **193,002**
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
