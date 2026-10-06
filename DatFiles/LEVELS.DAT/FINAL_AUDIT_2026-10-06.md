# LEVELS.DAT / LEVnnn.DAT final reconstruction audit — 2026-10-06

Status: **PRESERVATION-GRADE / ARCHIVAL RECONSTRUCTION COMPLETE FOR THE SUPPLIED BUILD, WITH ONE BOUNDED LEV022 CACHE-PROVENANCE DISCREPANCY AND RAW LEGACY STATE PRESERVED.**

This checkpoint re-audits the master LEVELS archive and standalone LEV format against the supplied immutable binaries and cross-linked databases. It certifies the reconstructed geometry and runtime semantics needed to parse, preserve, inspect, and model the supplied build. It does not claim that an independent byte-exact encoder has been implemented unless separately demonstrated by tooling.

## Controlling source identities

```text
LEVELS.DAT   499,878 bytes  50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91
LEV022.DAT     6,028 bytes  f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e
CODE.2       558,689 bytes  853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314
OBJECTS.DAT   21,844 bytes  f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0
NPCS.DAT       5,731 bytes  f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b
```

CODE.2 object-1 is the controlling executable image for the loader/writer/interpreter evidence. Its corrected physical start is `0x3AF24`.

## Archive-wide geometry recheck

All 32 LEVELS.DAT records satisfy the documented boundary equations without exception.

Aggregate supplied corpus:

```text
archive records             32
packed cell words      115,584
10-byte cell records     1,659
family-A compact records 1,258
family-B compact records 4,324
family-C compact records   622
event physical slots      1,600
event active slots          394
event active commands       555
```

For every record:

```text
record preamble                        0x1D bytes
fixed header                           0x2A4 bytes
packed grid                            width * height * 2
legacy column lists                    width * {int16 count + count*10}
family A                               header_count_A * 11
family B                               header_count_B * 11
family C                               header_count_C * 11
event table                            0x514 bytes
common tail                            repeated dimensions + cache + annotations
```

The archive directory, record boundaries, body families, event-table extent, and final tails all reconcile exactly.

## Standalone LEV form

The direct-save writer serializes expanded mutable state rather than the compact archive form:

- group-1 10-byte records;
- active-animation byte;
- group-2 10-byte dynamic records;
- live 22-byte family-A actors;
- counted 22-byte family-B placed-object records;
- live 22-byte family-C loose/moving item records;
- unchanged 0x514 event table;
- regenerated automap/display cache and annotation tail.

The standalone preamble byte at file offset 0 is not written by the certified writer. Existing artifacts must preserve it as raw file residue.

## Level-22 mutable-state fixture

Archive record 22 and supplied LEV022.DAT independently validate the direct-save model.

Certified semantic delta:

```text
header:
  +0x29A normalized 0 -> 1

packed cell layer:
  313 / 450 words changed
  every changed word XOR = 0x0440
  no topology/code/other flag changed

10-byte state:
  group 1 unchanged
  active animation count = 0
  group 2 empty

family A:
  one ASSASSIN
  x-cell 13 -> 18
  facing 2 -> 6
  current HP 15
  action/state countdown 4

family B:
  all 110 records equal deterministic archive expansion

family C:
  empty in both

event table:
  byte-identical, all zero
```

This establishes LEV022.DAT as a real mutable runtime-state fixture rather than a second static level-schema variant.

## Cell-word closure

The supplied-build behavioral model is:

```text
0x003F  six-bit cell code
0x0040  transient current visibility/raycast mark
0x0080  special-geometry center / dynamic-collision gate
0x0100  special-geometry frame / adjacent-code substitution gate
0x0200  extra render-prepass enable
0x0400  persistent automap explored/revealed mark
0x0800  secret/hidden-passage topology marker
0x1000  10-byte-record state resource-offset enable
0x2000  preserved authoring/legacy marker; no direct runtime consumer proved
0xC000  two-bit render shade/decrement level
```

A final object-1 reference audit found 76 direct CODE.2 instructions addressing cell storage at DGROUP `0x198E0`. Direct masks/operations cover every active field above except `0x2000`.

Only three supplied cells carry `0x2000`:

```text
level 12  (44,34)  0x2400
level 13  (44,34)  0x2400
level 14  (44,34)  0x2400
```

No 10-byte record or supplied event command targets that coordinate. Whole-object literal `0x2000` instructions belong to unrelated character/transient/graphics state. The bit therefore remains raw-preserved authoring/legacy state; this audit does not call it padding or assign speculative gameplay semantics.

## Compact-family closure

### Family A

Family A is the NPC/creature world-record family and indexes NPCS.DAT. The live/direct 0x16 record preserves fixed-point position, runtime flags, cached cell, NPC type, action countdown, current HP, and packed state/facing.

### Family B

Family B is the placed object/item map-record family. It indexes OBJECTS.DAT and participates in map rendering/automap overlay behavior.

### Family C

Family C is the loose/pickable item family and also carries runtime dropped/thrown/moving item state.

The legacy loader rule:

```text
compact type 0x54 -> runtime type 0x58
```

is now semantically resolved. In the current OBJECTS.DAT:

```text
0x54  STONE GIANT GAUNTLETS
0x58  ARROW OF SLAY UNDEAD
```

The conversion happens before runtime `+0x0E` is stored, so all normal item/render/pickup code sees **ARROW OF SLAY UNDEAD**.

The sole compact 0x54 record is level 10 family-C record 28 at cell `(78,113)`, immediately adjacent in the serialized record list to:

```text
PALADIN'S SWORD
PALADIN'S DAGGER
ADAMANTITE CHAIN HELM
CHAIN COIF
legacy 0x54 -> ARROW OF SLAY UNDEAD
```

The semantic alias is therefore closed. Only the exact historical object-table revision/provenance that caused the ID migration remains unknown.

## Event-table closure

The 0x514 block is exactly 50 slots * 26 bytes, each slot storing one active-count byte plus five physical five-byte commands. Inactive command bytes are persistent and must be preserved.

Supported opcode domain is 0..12.

The six supplied opcode-11 uses are fully classified:

```text
level  5 slot 23  arg3=13  Irmgarde-dependent fixed-cell story trigger
level 10 slot  3  arg3=12  Vengeful-Spirit possession lifecycle
level 12 slot 10  arg3= 7  numbered interactive resource
level 13 slot  8  arg3= 9  numbered interactive resource
level 29 slot 28  arg3=11  final-Strahd story activation / reveal-enable
level 31 slot  1  arg3= 2  numbered interactive resource
```

Reserved branch details:

- `0x0B` invokes RES9 program 28, **FINAL STRAHD**, sets event flag `0x3A`, and unsuppresses live NPC type 8, **STRAHD**.
- `0x0C` invokes the GHOST/Vengeful-Spirit possession onset/resolution routine independently tied to SAVE character status.
- `0x0D` searches the recruited party for **IRMGARDE**, rejects dead/paralyzed state, clears the six-bit code at fixed cell row 18 / column 54, sets flag `0x46`, and returns a success result used by the interpreter.

These are hard-coded story hooks, not reusable generic editor opcodes.

## Automap/cache boundary

CODE.2 routine `01558` is instruction-accounted and the standalone writer invokes it immediately before writing the final byte cache.

The one remaining artifact discrepancy is sharply bounded:

- Level 22 header contains `primary_resource_A[1] == 0x0093`;
- certified `01558` applies the `0x0093` shortcut in both explored and unexplored paths;
- supplied LEV022 cache behaves as though the shortcut were absent for exactly 40 unexplored code-1 cells;
- all explored code-1 cells and all other deterministic ordinary cells agree;
- the known lookup animator cannot transform `0x0093`;
- writer ordering does not provide an intervening table mutation.

Therefore this remains a **save provenance / executable-revision / unproved production-path discrepancy**, not permission to alter reconstructed CODE.2.

Compatibility rule:

- reproduce certified CODE.2 cache generation for native behavior;
- when importing an existing standalone LEV artifact, preserve its serialized cache bytes unless intentionally rebuilding them;
- do not invent a hidden flag or modify the executable branch solely to force this fixture to match.

## Bounded raw state

The following remain deliberately raw where evidence stops:

- preamble byte 0 in standalone LEV files;
- unused header bytes with no direct consumers;
- cell bit `0x2000` source/editor identity;
- compact bytes that the loader does not consume;
- inactive event command positions;
- annotation payload bytes;
- the exact historical provenance of the family-C Arrow alias;
- the 40-cell LEV022 cache-production discrepancy.

These are preservation requirements, not unresolved structural holes.

## Final decision

For the supplied Ravenloft build, LEVELS.DAT and the LEVnnn.DAT direct-save form have no remaining structural unknown that prevents a faithful parser, semantic model, or lossless-preservation editor.

The archive reconstruction is therefore complete to the supplied evidence boundary. Future work may recover historical editor/source names or external-version provenance, but such findings should refine labels rather than change the certified layout or silently normalize legacy bytes.
