# SAVE.DAT `0x248` character record

Status: **active field reconstruction from the certified CODE.2 runtime layout, cross-checked byte-for-byte against the supplied SAVE.DAT fixture. Unknown bytes remain unnamed.**

The same `0x248`-byte image is serialized for the four active-party slots and the ten saved-roster slots. The runtime party-record base is DGROUP `0x218D8`: CODE.2 addresses the item array at `0x21920 + character_index*0x248`, record offset `+0x48`, and the independently decoded status word at `0x218F0 + character_index*0x248`, record offset `+0x18`.

## Certified executable-linked fields

| Record offset | Size | Meaning | Evidence |
|---|---:|---|---|
| `+0x000` | `0x10` fixture field | character name storage | Every populated fixture record contains its NUL-terminated name wholly inside `+0x00..+0x0F`; `+0x10` begins a distinct binary field in both populated and empty records. The exact original C declaration is not claimed yet. |
| `+0x018` | 2 | character status word | Runtime `0x218F0 + actor`; high-byte bits are directly manipulated by NPC attacks and cure handlers. |
| `+0x019` | byte within status word | condition flags | high-byte `0x04=disease`, `0x08=poison`, `0x10=paralysis` are independently proved by CODE.2 attack/cure paths. Other bits remain raw. |
| `+0x01D` | 1 | class-related discriminator | Runtime `actor + 0x218F5`; CODE.2 tests value `1` as an exemption in the disease-attack path. Do not assign an AD&D class name yet. |
| `+0x048` | `0x2A` | `int16_le item[21]` | Runtime item array `0x21920 + character*0x248 + slot*2`; slots `0..11` inventory, `12..20` equipment. `0xFFFF` is the ordinary empty-item sentinel. |
| `+0x13A` | at least 1 byte | character state/flags field | Runtime `0x21A12 = base + 0x13A`; the final CODE.2 semantic audit records exact bit tests and byte clears at this address. Individual bit meanings remain to be named from their consumers. |

The address arithmetic is exact:

```text
record runtime base       0x218D8
status word               0x218F0 = base + 0x18
class-related byte        0x218F5 = base + 0x1D
item array                0x21920 = base + 0x48
state/flags byte          0x21A12 = base + 0x13A
next character            base + 0x248
```

## Core-stat region: fixture-proved field boundaries

The reference fixture exposes a highly regular core region before the item array. These boundaries are safe to parse even where the gameplay name is not yet executable-certified:

```text
+0x00..+0x0F  fixed 16-byte name storage in this format
+0x10..+0x17  eight bytes of identity/class metadata
+0x18..+0x19  status word
+0x1A..+0x1B  two one-byte core fields
+0x1C..+0x1D  16-bit value / two-byte field pair (semantic split under audit)
+0x1E..+0x1F  16-bit value
+0x20..+0x25  three 16-bit core values
+0x26..+0x31  three 32-bit little-endian value slots
+0x32..+0x3D  six adjacent byte pairs
+0x3E..+0x3F  seventh byte pair
+0x40..+0x47  four 16-bit words immediately preceding item[21]
```

The six pairs at `+0x32..+0x3D` have the numerical domain and duplicated current/base pattern expected of the six AD&D ability scores in the supplied characters (for example EHLIANNA: `17/17, 15/15, 15/15, 17/17, 12/12, 16/16`; ANVIL DAWN: `18/18, 14/14, 19/19, 11/11, 16/16, 13/13`). The following pair is `0/0` for EHLIANNA and `100/100` for ANVIL DAWN, strongly matching the exceptional-strength percentile representation. This is now a **high-confidence format inference**, but the exact STR/INT/WIS/DEX/CON/CHA ordering will remain provisional until each byte is tied to an executable consumer.

The three 32-bit slots at `+0x26`, `+0x2A`, and `+0x2E` show `0xFFFFFFFF` sentinels for absent entries and ordinary nonnegative values for present entries. Their width/sentinel structure is fixture-proved; experience/class association is not promoted to canonical semantics until CODE.2 consumers are traced.

## Item slots inside the serialized record

Because the item array begins at record `+0x48`, the executable-proved slot routing maps directly into SAVE.DAT:

```text
+0x48 .. +0x5F  item[0..11]   ordinary inventory grid
+0x60           item[12]      ring_A
+0x62           item[13]      ring_B
+0x64           item[14]      hand_A
+0x66           item[15]      hand_B
+0x68           item[16]      torso/body garment
+0x6A           item[17]      head
+0x6C           item[18]      dedicated accessory
+0x6E           item[19]      quiver
+0x70           item[20]      bracer/gauntlet
```

All entries are little-endian signed 16-bit legacy words. `-1 / 0xFFFF` is the ordinary empty sentinel, but other negative values occur in the saved image and must not be normalized away.

### Corrected reference-fixture check

For active party slot 0 (`EHLIANNA`, file record base `0x0077`), the 21 words beginning at record `+0x48` are exactly:

```text
321, 322, 81, 70,
-1, -1, -1, -1, -1, -1,
252, -1, 10, -1, -1, -1,
-1, -256, -1, 0, -256
```

This corrects the earlier hand-transcribed tail of the list. The non-`-1` negative values are part of the fixture and demonstrate why the legacy signed words must be preserved exactly rather than treated as a simple array of OBJECTS.DAT IDs.

## Empty-record representation

The two unused active-party records are not zero-filled. In the reference fixture they contain `0xFF` throughout most of the record, while bytes `+0x10..+0x11` are `00 80`. Therefore record occupancy must not be inferred from an all-zero test. A parser should preserve the entire raw `0x248` image and use only an executable-proved occupancy predicate once identified.

## Current record partition

```text
+0x000 .. +0x017   identity/core metadata (name boundary now mapped)
+0x018 .. +0x047   status/stats/class region (several boundaries mapped)
+0x048 .. +0x071   item[21] (structure confirmed)
+0x072 .. +0x139   unresolved character arrays/state
+0x13A             executable-referenced state/flags byte
+0x13B .. +0x247   unresolved spell/effect/auxiliary state
```

This is deliberately not yet represented as a packed C struct: doing so would falsely imply semantics for unresolved bytes. A native implementation should use an explicit legacy decoder until every field boundary is certified.

## Next target

Resolve the six paired ability bytes against their individual CODE.2 consumers, identify the `+0x26/+0x2A/+0x2E` dwords and the `+0x40..+0x47` words, then audit the array shapes between `+0x72` and `+0x139`. The `+0x13A` state byte provides the next hard executable anchor for that post-inventory region.