# SAVE.DAT `0x248` character record

Status: **partial field reconstruction from the certified CODE.2 runtime layout, cross-checked against the supplied SAVE.DAT fixture. Unknown bytes remain unnamed.**

The same `0x248`-byte image is serialized for the four active-party slots and the ten saved-roster slots. The runtime party-record base can now be anchored at DGROUP `0x218D8`: CODE.2 addresses the item array at `0x21920 + character_index*0x248`, which is record offset `+0x48`, and the independently decoded status word at `0x218F0 + character_index*0x248`, which is record offset `+0x18`.

## Certified fields

| Record offset | Size | Meaning | Evidence |
|---|---:|---|---|
| `+0x000` | at least 18 bytes reserved | NUL-terminated character name | Names occur at identical offsets in every populated party/roster record; next proved runtime fields begin at `+0x18`. Exact source array length inside the reserved prefix remains to be proved. |
| `+0x018` | 2 | character status word | Runtime `0x218F0 + actor`; high-byte bits are directly manipulated by NPC attacks and cure handlers. |
| `+0x019` | byte within status word | condition flags | high-byte `0x04=disease`, `0x08=poison`, `0x10=paralysis` are independently proved by CODE.2 attack/cure paths. Other bits remain raw until decoded. |
| `+0x01D` | 1 | class-related discriminator | Runtime `actor + 0x218F5`; CODE.2 tests value `1` as an exemption in the disease-attack path. Do not assign an AD&D class name yet. |
| `+0x048` | `0x2A` | `int16_le item[21]` | Runtime item array `0x21920 + character*0x248 + slot*2`; slots `0..11` inventory, `12..20` equipment. `0xFFFF` is the empty-item sentinel. |

The address arithmetic is exact:

```text
record runtime base       0x218D8
status word               0x218F0 = base + 0x18
class-related byte        0x218F5 = base + 0x1D
item array                0x21920 = base + 0x48
next character            base + 0x248
```

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

All entries are little-endian signed 16-bit object IDs at the legacy interface; `-1` / `0xFFFF` is empty. This preserves the signed-word behavior documented by the CODE.2 semantic audit.

### Reference-fixture check

For active party slot 0 (`EHLIANNA`, file record base `0x0077`), the 21 words beginning at record `+0x48` decode as:

```text
321, 322, 81, 70,
-1, -1, -1, -1, -1, -1,
252, -1, 10, -1, -1, -1,
-1, -1, 0xFF00, -1, 0
```

Some non-`-1` values in nominal item positions may represent runtime/container encodings rather than plain static OBJECTS.DAT IDs. They are therefore preserved as raw signed words until each consumer is audited. The structural placement of the 21-word array itself is proved independently by CODE.2.

## Current record partition

```text
+0x000 .. +0x017   identity/core fields (name known; remainder under audit)
+0x018 .. +0x047   status/stats/class fields (partial semantics known)
+0x048 .. +0x071   item[21] (structure confirmed)
+0x072 .. +0x247   spell/effect/auxiliary character state (under audit)
```

This is deliberately not yet represented as a packed C struct: doing so would falsely imply semantics for the unresolved bytes. A native implementation should use an explicit legacy decoder until every field boundary is certified.

## Next target

Trace CODE.2 references in the `0x218D8..0x21B1F` first-record address window to name the remaining core-stat fields, then identify the arrays after `+0x72` by their loop bounds and element widths. Every proposed field must agree with the `0x248` stride and both SAVE serializer directions.