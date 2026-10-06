# LEV022.DAT mutable-state delta against LEVELS.DAT record 22

Status: **exact archive-to-standalone mutation audit.** The supplied standalone Level 22 file is structurally valid and preserves live runtime state on top of the archive baseline. The comparison below separates deterministic archive expansion, actual gameplay mutation, generated cache state, and one bounded provenance discrepancy.

## Source pair

~~~text
LEVELS.DAT record 22
  archive offset  0x5AD83
  record size     0x1288

LEV022.DAT
  file size       0x178C
~~~

Both describe:

~~~text
map                 30 x 15 = 450 cells
family A / NPCs       1
family B / placed   110
family C / loose      0
event table         0x514 bytes, all zero
annotation blob       0 bytes
~~~

## Preamble

Archive record 22:

~~~text
byte 0 = 0x00
boundaries =
  0x001D, 0x0645, 0x06E5, 0x06F0,
  0x0BAA, 0x0BAA, 0x10BE
~~~

Standalone LEV022:

~~~text
byte 0 = 0xDF
boundaries =
  0x001D, 0x0645, 0x0722, 0x0738,
  0x10AE, 0x10AE, 0x15C2
~~~

The larger standalone boundaries are fully explained by the second 10-byte-record group and 22-byte direct runtime records.

### Byte 0 is not writer-owned level state

The standalone writer physical `0x52120..` first creates/reopens the LEV file, then at `0x521CB` seeks directly to file offset `0x1D` before its first payload write.

After all payload/tail output, physical `0x52649` seeks to offset `1` and writes the seven boundary dwords beginning there.

No writer operation writes file byte 0.

Therefore LEV022's `0xDF` is **an unwritten/writer-unowned byte**, not a semantic level marker produced by the game serializer. Its exact filesystem/runtime provenance is not asserted. Existing files should preserve it byte-for-byte; a native serializer should not invent gameplay meaning for it.

## Fixed 0x2A4 header

There is exactly one differing header byte:

~~~text
header +0x29A
archive = 0
LEV022  = 1
~~~

CODE.2 immediately normalizes zero to one after archive load, so this is the already-certified `render_threshold_floor` loader normalization. No other fixed-header byte changed.

## 16-bit cell layer

Exactly **313 of 450 cell words** differ.

For every one of those 313 cells:

~~~text
archive_word XOR LEV022_word = 0x0440
~~~

No other bit changed.

Thus the mutable delta is exactly:

~~~text
0x0040  current visibility/raycast mark
0x0400  persistent automap explored/revealed mark
~~~

combined as `0x0440`.

The underlying six-bit cell codes, special-geometry flags, render flags, hidden-passage state, record-state flag, and shade/decrement bits remain identical to the archive baseline in every cell.

This is stronger than the earlier byte-difference count: the supplied gameplay session changed only visibility/exploration state in the topology word array.

## 10-byte cell/control records

Archive level 22 contains ten legacy 10-byte-list records.

Standalone LEV022 contains:

~~~text
group 1 records            10
active_animation_count      0
group 2 records             0
~~~

All ten group-1 records are byte-for-byte identical to the archive records, including their column ownership and coordinates.

Therefore this sample contains **no mutable 10-byte-record change** and no active dynamic-door/special-cell animation.

## Family A — live NPC state

The archive has one compact family-A record: NPC type 9, `ASSASSIN`.

Archive baseline:

~~~text
x cell/fraction = 13 / 0x7C00
y cell/fraction =  7 / 0x7800
facing          =  2
~~~

LEV022's direct 22-byte live record restores:

~~~text
x cell/fraction        = 18 / 0x7C00
y cell/fraction        =  7 / 0x7800
runtime flags          = 0x10000000
cached cell            = 228
type                   = 9 / ASSASSIN
action-state countdown = 4
current HP             = 15
facing                 = 6
~~~

So during the saved session the Assassin moved five cells in X and changed facing from 2 to 6; the save also preserves its current HP and action-state countdown.

See `../LEVELS.DAT/FAMILY_A.md`.

## Family B — placed object state

Archive family B contains 110 compact records.

Applying the certified archive expansion to every record reproduces:

~~~text
110 / 110
~~~

LEV022 family-B 22-byte records byte-for-byte.

There is therefore no family-B mutable delta in this sample.

## Family C

Both forms contain zero family-C records.

## Event scripts

The complete fixed `0x514` event table is byte-identical and all zero in both forms.

## Generated automap/display cache

Archive level 22 stores all 450 cache bytes as `0xFF`.

LEV022 stores:

~~~text
0x2E  313
0xFF  130
0x01    7
~~~

All 313 cells whose cell word gained `0x0440` correspond exactly to the 313 saved `0x2E` cache cells.

The seven `0x01` cells are the generated visible-boundary class adjacent to explored space.

### Bounded 40-cell discrepancy

The separate instruction-level cache audit remains in force. Certified CODE.2 `01558`, replayed from LEV022's own serialized header/cell state, would classify 40 unexplored tile-code-1 cells differently because `0x25622[1] == 0x0093`.

Those 40 cells are the only replay mismatch. The direct-save artifact behaves as if the unexplored `0x0093` shortcut were absent, while the supplied CODE.2 unquestionably contains it.

This discrepancy does **not** affect the structural/mutable-state conclusions above. Preserve imported cache bytes and implement the certified CODE.2 branch for native regeneration until provenance is independently resolved.

## Annotation tail

Both forms have annotation length zero.

## Final Level 22 delta model

The supplied direct save differs from the archive baseline through four understood mechanisms:

1. deterministic archive-to-live expansion of compact structures;
2. loader normalization of header `+0x29A: 0 -> 1`;
3. gameplay mutation:
   - 313 cell words gain `0x0440`;
   - the one live Assassin moves, turns, and carries current HP/action state;
4. generated automap/cache output.

The file also contains:
- writer-unowned byte 0 (`0xDF`);
- the bounded 40-cell cache provenance discrepancy.

No other unexplained mutable difference is present in the supplied Level 22 pair.
