# LEVELS.DAT

Status: **archive/record structure proven; cell-state, 10-byte control, compact-family, fixed event-script, active header-tail, automap, dynamic-door, and annotation-tail semantics substantially reconstructed.**

- Size: 499,878 bytes
- SHA-256: `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`
- CODE.2 loader: logical `181B4..18D8B`
- CODE.2 standalone writer: logical `171FC..17837`
- CODE.2 level event interpreter: logical `002A8..006B8`

## Proven structure

`LEVELS.DAT` begins with a 32-bit count of **32** followed by 32 little-endian record offsets. The directory ends at `0x84`, exactly the first record offset.

Every archive record validates as:

1. `0x1D`-byte preamble with seven boundary dwords;
2. fixed `0x2A4` header;
3. `width * height` packed 16-bit cell-state words;
4. width-indexed lists of 10-byte cell records;
5. three legacy 11-byte compact record families expanded to 22-byte runtime records;
6. fixed `0x514` level event-script table;
7. repeated dimensions, generated automap cache, and length-prefixed automap annotation blob.

Every boundary equation and final record length validates with no archive exceptions.

## Documentation and tools

- `FORMAT.md` — archive/direct-save structure, event-table layout, and annotation-tail serialization.
- `SEMANTICS.md` — integrated semantic model.
- `HEADER_TAIL.md` — active fixed-header tail fields at `+0x290..+0x29C`, including renderer/profile/resource controls and loader normalization.
- `CELL_WORD.md` — packed 16-bit cell code/flag model.
- `CELL_RECORDS.md` — 10-byte record kinds, control bits, coordinates, and kind-specific fields.
- `DYNAMIC_CELL_RECORDS.md` — group-2 animation state and open/closed collision behavior.
- `EVENT_SCRIPTS.md` — 50-slot `0x514` event table and opcode dispatcher.
- `AUTOMAP.md` — cache classifier, connected geometry, dynamic doors, and CP437 export.
- `AUTOMAP_OVERLAYS.md` — placed-object and annotation overlays.
- `CACHE_REBUILD_AUDIT.md` — exact `01558` replay and Level 22 discrepancy.
- `inspect_levels.py` — structural/coordinate validator.
- `analyze_cells.py` — cell-flag statistics.
- `analyze_cell_records.py` — 10-byte kind/control and database checks.
- `analyze_dynamic_records.py` — dynamic group-2 statistics.
- `analyze_event_scripts.py` — fixed event-table slot/opcode validator.
- `analyze_header_tail.py` — archive-wide distributions for the proved header-tail controls plus direct-save normalization check.
- `audit_cache_rebuild.py` — deterministic cache-replay validator.

## Current semantic frontier

### Fixed-header tail

The active header tail at `+0x290..+0x29C` is now materially decoded from direct CODE.2 references.

```text
+0x290  uint8 environment/render profile selector; 0xFF selects alternate/default path
+0x292  int16 NPC type filter 0
+0x294  int16 NPC type filter 1
+0x296  int16 NPC type filter 2
+0x298  uint8 renderer fill-base A
+0x299  uint8 renderer fill-base B
+0x29A  int16 renderer-threshold floor
+0x29C  uint8 resource-set identifier
```

The three signed NPC filters are compared directly against family-A/NPCS.DAT type IDs and predominantly use `-1` as the no-match sentinel.

The two bytes at `+0x298/+0x299` must remain separate: CODE.2 passes them to two different VGA fill routines (`33E28` and `33DE0`).

Most importantly, CODE.2 normalizes header `+0x29A` from zero to one immediately after level load. This exactly explains the sole fixed-header difference between archive level 22 (`0`) and supplied `LEV022.DAT` (`1`); the direct-save file contains the normalized live header, not an unexplained mutation. The supplied archive distribution for this field is 14 zeros, 12 ones, and six values of 50.

Byte `+0x29C` is repeatedly compared with the active resource identifier and passed into the resource-load family, so `resource_set_id` is now supported terminology.

Header byte `+0x291` and `+0x29D..+0x2A3` have no direct absolute CODE.2 consumers found in the complete object scan and are zero in all supplied archive records. They remain raw preserved bytes rather than format-mandated padding.

### Event scripts

The former unresolved `0x514` block is proved to be **50 fixed 26-byte event slots**. Each slot contains a one-byte active command count followed by five physical five-byte command positions. CODE.2 selects `slot = table + script_index*26` and dispatches opcodes `0..12` through a 13-entry jump table.

Across all supplied levels:

```text
physical slots   1600
active slots      394
active commands   555
max active count    5
```

One level retains nonzero bytes in an inactive fifth command while its count is four, so inactive bytes are persistent and must be preserved.

### Cell/automap and record state

The six-bit cell code, visibility bit `0x0040`, explored bit `0x0400`, dynamic group-2 door/barrier state, generated automap cache, connected wall/inset/door resolver, placed-object overlay, and automap annotation list are all executable-backed.

Family A is tied directly to `NPCS.DAT` and is the NPC/creature world-record family. Family B is tied to `OBJECTS.DAT` and is used as the placed object/item family. Family C remains within the object namespace but its exact B/C distinction is unresolved.

## Next major targets

The highest-value remaining LEVELS work is now:

1. identify family C precisely and close its distinction from family B;
2. resolve remaining event-opcode callees where they materially improve format semantics;
3. tighten the few still-behavioral cell/control-bit names;
4. then move into the other DAT databases while preserving the established cross-file links.

Lossless tooling must preserve every unresolved byte and flag verbatim.
