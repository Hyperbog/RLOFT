# LEVELS.DAT

Status: **archive/record structure proven; cell-state, 10-byte control, compact-family, fixed event-script, automap, dynamic-door, and annotation-tail semantics substantially reconstructed.**

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
- `audit_cache_rebuild.py` — deterministic cache-replay validator.

## Current semantic frontier

### Event scripts

The former unresolved `0x514` block is now proved to be **50 fixed 26-byte event slots**. Each slot contains a one-byte active command count followed by five physical five-byte command positions. CODE.2 selects `slot = table + script_index*26` and dispatches opcodes `0..12` through a 13-entry jump table.

Across all supplied levels:

```text
physical slots   1600
active slots      394
active commands   555
max active count    5
```

One level retains nonzero bytes in an inactive fifth command while its count is four, so inactive bytes are persistent and must be preserved.

Opcode 4 is fully closed as a six-bit cell-code replacement at `(row,column)`. Opcodes 0/1/2 operate on 10-byte cell-record interaction/state, opcode 3 creates a transient/moving effect record, opcode 5 enters the sound/resource path, opcode 11 is a special subdispatcher, and opcode 12 writes its encoded state byte to global `0xBF44`. See `EVENT_SCRIPTS.md` for exact behavior and unresolved call boundaries.

### Cell/automap and record state

The six-bit cell code, visibility bit `0x0040`, explored bit `0x0400`, dynamic group-2 door/barrier state, generated automap cache, connected wall/inset/door resolver, placed-object overlay, and automap annotation list are all executable-backed.

Family A is tied directly to `NPCS.DAT` and is the NPC/creature world-record family. Family B is tied to `OBJECTS.DAT` and is used as the placed object/item family. Family C remains within the object namespace but its exact B/C distinction is unresolved.

## Next major targets

The remaining highest-value LEVELS work is now:

1. close the still-unnamed header tail/control fields around `0x258A8..0x258B4`;
2. resolve the remaining event-opcode callees where useful;
3. identify family C precisely;
4. then move to the next DAT database while preserving the established cross-file links.

Lossless tooling must preserve every unresolved byte and flag verbatim.