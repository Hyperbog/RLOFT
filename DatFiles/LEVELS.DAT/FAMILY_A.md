# LEVELS.DAT family A — live NPC/creature world records

Status: **the complete 22-byte runtime/direct-save record is structurally mapped; its NPC type, fixed-point position, runtime flags, cached cell, action countdown, current HP, and facing field are executable-backed.** The supplied LEV022.DAT provides a direct mutable-state example for an ASSASSIN instance.

## Definition/instance split

Family A is counted by level-header word `0x258A2` and indexes `NPCS.DAT` directly by its type word.

Archive `LEVELS.DAT` stores an 11-byte compact placement record:

```text
+0x00  uint16 npc_type_id
+0x02  int16  x_cell
+0x04  uint16 x_fraction
+0x06  int16  y_cell
+0x08  uint16 y_fraction
+0x0A  uint8  compact_orientation
```

The archive loader expands that into the mutable 22-byte live record at runtime array `0x14D80`. Standalone `LEVnnn.DAT` serializes those 22-byte live records directly.

## Certified live-record layout

```text
+0x00  uint32 y_position_16_16
+0x04  uint32 x_position_16_16
+0x08  uint32 runtime_flags
+0x0C  uint16 cached_cell_index
+0x0E  uint16 npc_type_id
+0x10  uint16 action_state_countdown
+0x12  int16  current_hp
+0x14  uint16 packed_runtime_state
```

The record is exactly `0x16` bytes.

### `+0x00/+0x04`: fixed-point position

Archive expansion preserves the compact integer/fraction pairs:

```text
y_position = (y_cell << 16) | y_fraction
x_position = (x_cell << 16) | x_fraction
```

Movement/AI paths mutate these live dwords. The standalone writer copies the full live family-A array verbatim, so LEV files preserve sub-cell position rather than only archive placement.

### `+0x08`: mutable runtime flags

Loader physical `0x535DD..0x535EB` indexes `NPCS.DAT` by `npc_type_id * 0x45`, reads definition dword `+0x35`, and stores it into live record `+0x08`.

`NPCS.DAT +0x35` is independently certified as `initial_runtime_flags`. Gameplay and wordcode subsequently mutate/test the live copy rather than altering the definition table. For example, wordcode suppress/hide-NPC behavior sets a bit in the live record.

Thus the definition field is a template and family-A `+0x08` is persistent mutable actor state.

### `+0x0C`: cached map-cell index

The loader computes:

```text
cached_cell_index = y_cell * map_width + x_cell
```

and writes it to `+0x0C`. Movement updates it when the actor crosses cells.

### `+0x0E`: NPCS.DAT type ID

This word indexes the 83-entry `NPCS.DAT` definition table. All supplied archive family-A records use IDs `0..82`.

### `+0x10`: action/state countdown

Archive expansion initializes this word to zero. Independent NPC reset/action paths copy the definition's `NPCS.DAT +0x25 action_delay` into live word `+0x10`, and active behavior decrements the live value toward zero.

The safe live name is therefore `action_state_countdown`. Do not confuse it with the immutable definition-level delay value.

### `+0x12`: current HP

This identity is direct.

During archive expansion, when the copied runtime flags request ordinary HP initialization, physical `0x53642..0x53663` reads `NPCS.DAT +0x1D hit_dice_count`, calls the dice/random helper, and stores the result in live word `+0x12`. NPC type 8 has a special initializer that writes `0x37`.

Later NPC damage/death routine logical `E284..E553` tests live word `+0x12`; zero or negative HP enters death handling. Therefore:

```text
+0x12  int16 current_hp
```

### `+0x14`: packed runtime state

Archive compact byte `+0x0A` contributes its low three bits to live bits `3..5`:

```text
facing = compact_orientation & 7
packed_runtime_state bits 3..5 = facing
```

Loader physical `0x535F1..0x53605` performs the shift/write. Runtime orientation logic later extracts the same three bits and can rewrite them as actors turn.

Other bits in the word have additional runtime roles and must be preserved raw until individually named; do not reduce the field to only a facing enum.

## LEV022.DAT mutable-state proof

Archive level 22 contains one family-A compact record:

```text
09 00 0D 00 00 7C 07 00 00 78 02
```

Interpreted through the certified compact layout:

```text
npc_type       9 = ASSASSIN
x              13 + 0x7C00/65536
y               7 + 0x7800/65536
initial facing  2
```

The current `NPCS.DAT` definition for type 9 is `ASSASSIN`, with `hit_dice_count = 4` and `initial_runtime_flags = 0x10000000`.

The supplied standalone `LEV022.DAT` stores this 22-byte live record:

```text
00 78 07 00 00 7C 12 00 00 00 00 10 E4 00 09 00
04 00 0F 00 30 00
```

Decoded:

```text
y_position_16_16    = 0x00077800   // unchanged
x_position_16_16    = 0x00127C00   // x cell 18, moved from archive x=13
runtime_flags       = 0x10000000
cached_cell_index   = 228          // 7*30 + 18
npc_type_id         = 9            // ASSASSIN
action_state_countdown = 4
current_hp          = 15
facing              = 6            // packed bits 3..5 = 0x30
```

This is direct proof that standalone LEV state is a mutable world snapshot, not merely a decompressed form of the archive record.

## Persistence rule

A faithful implementation should keep the layers distinct:

```text
NPCS.DAT                    immutable NPC definition/template
LEVELS.DAT family A         compact baseline placement/type/orientation
live family-A record        mutable actor state
LEVnnn.DAT family A         serialized live 22-byte actor state
```

When loading from the archive, reproduce the initialization path. When loading a standalone LEV file, restore its live record directly rather than regenerating HP, position, countdown, flags, or facing from the baseline definition.
