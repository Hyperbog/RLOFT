# LEVELS.DAT

Status: **archive/record structure proven; cell-state, 10-byte control, compact-family, fixed event-script, active header-tail/music, automap, dynamic-door, loose-item family, event-callee, and annotation-tail semantics substantially reconstructed. Remaining format-level unknowns are narrow and explicitly preserved.**

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
- `HEADER_TAIL.md` — active fixed-header tail fields at `+0x290..+0x29C`, including default level music and loader normalization.
- `FAMILY_A.md` — complete live/direct-save NPC world-record layout and mutable Level-22 actor proof.
- `FAMILY_C.md` — family C as loose/pickable item world records plus runtime dropped/thrown-item behavior.
- `CELL_WORD.md` — packed 16-bit cell code/flag model.
- `CELL_FLAG_LINKS.md` — executable/corpus proof for special-geometry, render-prepass, hidden-passage, record-state-resource, and shade/decrement flags.
- `CELL_RECORDS.md` — 10-byte record kinds, control bits, coordinates, and kind-specific fields.
- `DYNAMIC_CELL_RECORDS.md` — group-2 animation state and open/closed collision behavior.
- `EVENT_SCRIPTS.md` — 50-slot `0x514` event table and opcode dispatcher.
- `EVENT_CALLEES.md` — resolved sound, music, indexed-text, persistent-flag, and numbered interactive-resource opcode callees.
- `AUTOMAP.md` — cache classifier, connected geometry, dynamic doors, and CP437 export.
- `AUTOMAP_OVERLAYS.md` — placed-object and annotation overlays.
- `CACHE_REBUILD_AUDIT.md` — exact `01558` replay and Level 22 discrepancy.
- `inspect_levels.py` — structural/coordinate validator.
- `analyze_cells.py` — cell-flag statistics.
- `analyze_cell_flag_links.py` — cell-flag/10-byte-record and center/frame adjacency audit.
- `analyze_cell_records.py` — 10-byte kind/control and database checks.
- `analyze_dynamic_records.py` — dynamic group-2 statistics.
- `analyze_event_scripts.py` — fixed event-table slot/opcode validator.
- `analyze_event_callees.py` — archive-wide callee encodings, default music IDs, and INFO.DAT path audit.
- `analyze_header_tail.py` — archive-wide distributions for the proved header-tail controls plus direct-save normalization check.
- `analyze_family_c.py` — family-B/C corpus comparison and family-C OBJECTS.DAT validation.
- `audit_cache_rebuild.py` — deterministic cache-replay validator.

## Current semantic frontier

### Fixed-header tail and default music

The active header tail at `+0x290..+0x29C` is materially decoded from direct CODE.2 references.

```text
+0x290  uint8 environment/render profile selector; 0xFF selects alternate/default path
+0x292  int16 NPC type filter 0
+0x294  int16 NPC type filter 1
+0x296  int16 NPC type filter 2
+0x298  uint8 renderer fill-base A
+0x299  uint8 renderer fill-base B
+0x29A  int16 renderer-threshold floor
+0x29C  uint8 default level music-sequence ID
```

The three signed NPC filters are compared directly against family-A/NPCS.DAT type IDs and predominantly use `-1` as the no-match sentinel.

The two bytes at `+0x298/+0x299` must remain separate: CODE.2 passes them to two different VGA fill routines (`33E28` and `33DE0`).

CODE.2 normalizes header `+0x29A` from zero to one immediately after level load. This exactly explains the sole fixed-header difference between archive level 22 (`0`) and supplied `LEV022.DAT` (`1`). The supplied archive distribution is 14 zeros, 12 ones, and six values of 50.

Header `+0x29C` is the default level music-sequence ID. Every direct consumer routes through the music/sequence subsystem: it is compared against current sequence ID `29050()`, restored after temporary interaction sequences, and started by the options path when the separate music toggle is enabled.

Header byte `+0x291` and `+0x29D..+0x2A3` have no direct absolute CODE.2 consumers found and are zero in all supplied archive records. They remain raw preserved bytes rather than format-mandated padding.

### Compact world-record families

Family A is tied directly to `NPCS.DAT` and is the **NPC/creature world-record family**. Its complete direct/live `0x16` record is now mapped as fixed-point position, mutable runtime flags, cached cell, NPC type, action-state countdown, current HP, and packed runtime state with facing in bits 3..5. The supplied LEV022 actor independently demonstrates live movement/facing/HP/countdown persistence.

Family B is tied to `OBJECTS.DAT` and is the **placed object/item map-record family**. Its supplied compact records use IDs only `0..174`; their compact placement/orientation byte spans `0..7`, and family B supplies the placed-object automap overlay.

Family C is the **loose/pickable item world-record family**. It expands to runtime array `0x17618`, is inserted by the loose-object spawn/drop path, is removed and compacted by the first-person pickup path, and is rendered as a distinct scene source class. The same 22-byte records acquire heading, lifetime and active-motion state when an item is thrown; CODE.2 `2388C` advances and collision-tests those moving records.

Archive-wide family-C evidence:

```text
records                622
levels containing C     26
maximum in one level    90
unique object IDs       200
object ID range      0..335
compact flag byte         0 for all 622 supplied records
```

All family-C IDs are valid in the 336-entry supplied `OBJECTS.DAT`. The family-C-only legacy migration `0x54 -> 0x58` remains exact executable behavior; its historical reason is not yet known.

### Event scripts and resolved callees

The fixed `0x514` block is **50 fixed 26-byte event slots**, each containing a one-byte active command count followed by five physical five-byte command positions. Across all supplied levels there are 394 active slots and 555 active commands.

The high-value formerly address-named callees are resolved:

```text
opcode 5   queue sound-effect request
opcode 6   transition to music sequence
opcode 7   show indexed XOR-0xDC-decoded text/message
opcode 8   set persistent global event flag
opcode 9   clear persistent global event flag
opcode 10  toggle persistent global event flag
opcode 11  reserved special action or numbered interactive event resource
```

Opcodes 8/9/10 operate on the same 32-byte / 256-bit vector at runtime `0x22200`; SAVE.DAT writes and reads exactly those 32 bytes, proving persistence.

Opcode 11's default path uses the resource/install path loaded from INFO.DAT, appends a backslash and two decimal digits, opens that numbered resource, runs its interactive processor, applies the result, then stops the current event slot. The supplied INFO.DAT path is `D:`.

Opcode 6 belongs to the separate music-sequence subsystem. The two supplied uses request sequence IDs `0x2F` and `0x2E` in level 29.

### Packed cell/automap state

The 16-bit cell word is now substantially closed at the behavioral level:

```text
0x003F  six-bit cell code
0x0040  transient current visibility/raycast mark
0x0080  special-geometry center / dynamic-collision gate
0x0100  special-geometry frame / adjacent-code substitution gate
0x0200  extra render-prepass enable
0x0400  persistent automap explored/revealed mark
0x0800  secret/hidden-passage topology marker
0x1000  10-byte-record state resource-offset enable
0x2000  unresolved
0xC000  two-bit render shade/decrement level
```

The center/frame interpretation is independently supported by archive geometry: every one of the 745 `0x0080` cells has two to four orthogonal `0x0100` neighbors, and 1,682 of 1,690 `0x0100` cells touch an `0x0080` center.

`0x1000` is tied directly to first-group 10-byte record state. `1E9A0` returns `record[1] & 3`, and the renderer adds that value to its selected resource IDs. All 1,335 non-kind-1 records in the supplied archive occur on `0x1000` cells.

`0x0200` selects membership in an additional preliminary render pass before the ordinary all-record pass.

`0x0800` is a special pass-through/hidden topology marker in CODE.2 and resolves through the automap's `0x33 -> CP437 0xB2` path; independent Ravenloft automap documentation identifies that exported glyph as a secret-door marker. All 47 supplied `0x0800` cells are record-free.

`0xC000` is consumed as a 0..3 decrement applied to both byte lanes of a packed render value; only two supplied cells use a nonzero selector, both selector value 2.

`0x2000` remains deliberately unresolved. It occurs only three times, at the same coordinate `(44,34)` in levels 12..14, as raw word `0x2400`.

See `CELL_WORD.md`, `CELL_FLAG_LINKS.md`, and `analyze_cell_flag_links.py`.

## Remaining LEVELS work

The reusable archive/direct-save format is now at practical preservation-grade closure. LEV022 is no longer an opaque mutable sample: its exact `0x0440` exploration delta, unchanged 10-byte lists, mutable live Assassin record, deterministic family-B expansion, cache relation, and writer-unowned preamble byte are documented.

Remaining issues are bounded:

1. the historical schema reason for the family-C legacy numerical migration `0x54 -> 0x58`;
2. the exact meaning of cell bit `0x2000`;
3. story-specific opcode-11 branches `0x0B..0x0D` if reusable semantics can be proved;
4. the known 40-cell LEV022 automap-cache provenance discrepancy.

None blocks structural parsing, lossless preservation, direct-save restoration, or a high-level LEVELS/LEV reader-editor model. Linked `OBJECTS.DAT`, `NPCS.DAT`, and `SAVE.DAT` are already reconstructed far enough to support the current cross-file semantics.

Lossless tooling must preserve every bounded unknown byte/bit and every documented legacy migration verbatim.
