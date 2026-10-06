# SAVE.DAT `0x248` character record

Status: **active field reconstruction from the certified CODE.2 runtime layout, cross-checked byte-for-byte against the supplied SAVE.DAT fixture. Unknown bytes remain unnamed.**

The same `0x248`-byte image is serialized for the four active-party slots and ten saved-roster slots. Runtime party-record base is DGROUP `0x218D8`; all addresses below use `character_index * 0x248`.

## Certified executable-linked fields

| Record offset | Size | Meaning | Evidence |
|---|---:|---|---|
| `+0x000` | `0x10` fixture field | character name storage | Populated fixture names terminate inside `+0x00..+0x0F`; `+0x10` begins binary metadata. |
| `+0x018` | 2 | character status word | Runtime `0x218F0`; attack/cure paths manipulate its high-byte condition bits. |
| `+0x019` | byte within status word | condition flags | `0x04=disease`, `0x08=poison`, `0x10=paralysis` are executable-proved. |
| `+0x01D` | 1 | class-related discriminator | Runtime `0x218F5`; value `1` is an explicit disease-effect exemption. |
| `+0x03A` | 1 | Strength base/reference byte | Runtime `0x21912`; Strength tables and exceptional-Strength calculation consume it. |
| `+0x03B` | 1 | current/effective Strength byte | Runtime `0x21913`; melee/damage paths consume it; Stone Giant Gauntlets logic substitutes/restores it. |
| `+0x03D` | 1 | Dexterity-related current/effective byte | Runtime `0x21915`; Gauntlets of Dexterity equipment transitions alter this byte. |
| `+0x046` | 1 | exceptional-Strength base/reference percentile | Runtime `0x2191E`; Strength table logic interprets AD&D percentile bands. |
| `+0x047` | 1 | exceptional-Strength current/effective percentile | Runtime `0x2191F`; current Strength calculations consume it. |
| `+0x048` | `0x2A` | `int16_le item[21]` | Runtime `0x21920`; slots `0..11` inventory and `12..20` equipment. |
| `+0x0C2` | `0x50` | spell-preparation/memorization matrix A, `8 x 10` packed bytes | Runtime `0x2199A`; CODE.2 uses eight banks, ten-byte stride, independently manipulates both nibbles, expands ready copies, and maps cells to spell/resource IDs `0x42 + bank*10 + entry`. |
| `+0x12F` | 4 | auxiliary dword; low byte is reversible Dexterity adjustment | Runtime `0x21A07`; one transition adds its low byte to `+0x03D`, companion path subtracts it. Upper-byte semantics remain unresolved. |
| `+0x133` | 4 | per-character dynamic bitfield | Runtime `0x21A0B`; CODE.2 extracts, clears, and sets dynamically selected bits. Exact gameplay labels remain unresolved. |
| `+0x139` | 1 | class/casting capability mask | Runtime `0x21A11`; initialization derives bits from class codes and invokes the two spell-matrix initializers. Later UI code treats values `1..4` as capability/display selectors. |
| `+0x13A` | 4 | 32-bit character state/flags | Runtime `0x21A12`; CODE.2 performs byte, word, and dword mask operations here. |

Address arithmetic:

```text
record base                 0x218D8
status word                 0x218F0 = +0x18
class bytes                 0x218F5..0x218F7 = +0x1D..+0x1F
Strength base               0x21912 = +0x3A
Strength current            0x21913 = +0x3B
Dexterity-related current   0x21915 = +0x3D
exceptional STR base        0x2191E = +0x46
exceptional STR current     0x2191F = +0x47
item array                  0x21920 = +0x48
spell matrix A              0x2199A..0x219E9 = +0xC2..+0x111
unresolved dword            0x21A03..0x21A06 = +0x12B..+0x12E
Dex-adjustment aux dword    0x21A07..0x21A0A = +0x12F..+0x132
dynamic bitfield            0x21A0B..0x21A0E = +0x133..+0x136
aux bytes                   0x21A0F..0x21A10 = +0x137..+0x138
class/casting capability    0x21A11 = +0x139
state/flags dword           0x21A12 = +0x13A
next character              base + 0x248
```

## Core-stat correction and proof

Fixture visual pairing alone is not sufficient to assign gameplay semantics. Direct executable consumers place Strength at `+0x3A/+0x3B` and exceptional Strength at `+0x46/+0x47`. CODE.2 combines these values and recognizes percentile ranges `1..50`, `51..75`, `76..90`, `91..99`, and `100`.

The helper beginning at physical `0x4CF50` checks equipped object ID `0x54` (`STONE GIANT GAUNTLETS`) and adjusts current/effective Strength. A companion transition helper recognizes object `0x53` (`GAUNTLETS OF DEXTERITY`) and alters `+0x3D`, independently anchoring that byte to the Dexterity effect family.

## Core region: current safe boundary map

```text
+0x00..+0x0F  fixed 16-byte name storage in this fixture format
+0x10..+0x17  identity/class metadata
+0x18..+0x19  status word
+0x1A..+0x25  core fields; partial semantics pending
+0x26..+0x31  three 32-bit little-endian slots; sentinel structure proved
+0x32..+0x39  four byte pairs; semantics unresolved
+0x3A/+0x3B  Strength base/current
+0x3C/+0x3D  paired field; +0x3D is Dexterity-related current/effective byte
+0x3E/+0x3F  paired field; semantics unresolved
+0x40..+0x45  three 16-bit words; semantics unresolved
+0x46/+0x47  exceptional-Strength base/current percentile
+0x48..+0x71  item[21]
```

The three dwords at `+0x26`, `+0x2A`, and `+0x2E` contain `0xFFFFFFFF` sentinels for absent entries and nonnegative values for present entries. Width and sentinel behavior are fixture-proved; class/experience semantics remain under executable audit.

## Item slots

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

Entries are legacy signed 16-bit words. `-1 / 0xFFFF` is the ordinary empty sentinel, but other negative values occur and must be preserved exactly.

## Post-inventory certified structures

### `+0x0C2..+0x111`: spell-preparation/memorization matrix A

CODE.2 proves the exact layout:

```text
byte = record[0x0C2 + bank * 10 + entry]
bank  = 0..7
entry = 0..9
spell/resource ID = 0x42 + bank * 10 + entry
```

The outer executable loops terminate after eight banks and advance the array cursor by ten bytes per bank. Direct element access multiplies the bank index by ten. Each byte contains independently mutable high/low 4-bit values. A low nibble of `0xF` is treated as unavailable/sentinel state; a nonzero high nibble represents one or more ready copies, and successful spell use decrements that high nibble by exactly one while preserving the low nibble.

The common ready-spell expansion at physical `0x5E0E4..0x5E2C8` converts qualifying cells into runtime spell/action records and uses the formula above to address the indexed `T` resource archive. This establishes the subsystem as persistent spell preparation/memorization state without guessing the original source names of the two nibble quantities.

The former candidate `+0x10A` is bank 7, entry 2 (`0x0C2 + 7*10 + 2`), not a separate field.

### `+0x12F..+0x132`: auxiliary Dexterity-adjustment dword

CODE.2 accesses the complete dword at runtime `0x21A07`. In one equipment/state transition the low byte is added to the certified Dexterity-related current/effective byte at `+0x03D`; a companion path subtracts the same low byte. This proves a reversible Dexterity-adjustment role for byte `+0x12F` while leaving the upper three bytes conservatively unnamed.

### `+0x133..+0x136`: dynamic bitfield

Runtime `0x21A0B` is a 32-bit per-character bitfield. The helper at physical `0x4A13C` returns an arbitrary selected bit. The companion helper at `0x4A15C` clears that selected bit and conditionally installs a replacement bit value when the corresponding item slot is occupied. Other paths copy and clear the whole dword. This is sufficient to certify width and bitfield behavior, but not individual gameplay meanings.

### `+0x139`: class/casting capability mask

Character initialization zeroes runtime byte `0x21A11` and derives its bits from the three class-code bytes:

```text
class code 1 in first class slot -> set 0x04; call spell initializer 0x3EDC8
class code 3 in any class slot   -> set 0x01; call spell initializer 0x3ED08
class code 4 in any class slot   -> set 0x02; grant object 0xA6 in item[0]
```

The `0x3ED08` and `0x3EDC8` routines initialize/update the persistent spell-preparation matrices from class/level-related values. UI code at physical `0x5DE64` then interprets whole-byte values `1`, `2`, `3`, and `4` as four display/capability selections. Thus `+0x139` is safely a class/casting capability mask/selector; exact original class names remain separate from this structural identification.

### `+0x13A..+0x13D`: state flags

This is a 32-bit flag field. Proved masks include bits `0x1`, `0x2`, `0x10`, `0x20`, `0x40`, `0x200`, `0x400`, and `0x01000000`; dynamic bit masks are also used. See `POST_INVENTORY_STATE.md` for instruction-level evidence.

## Empty-record representation

Unused active-party records are not zero-filled. In the reference fixture most bytes are `0xFF`, while `+0x10..+0x11` are `00 80`. Occupancy must therefore not be inferred from an all-zero test; preserve the complete raw image until the executable occupancy predicate is certified.

## Current record partition

```text
+0x000 .. +0x017   identity/core metadata
+0x018 .. +0x047   status/stats/class region; several fields executable-linked
+0x048 .. +0x071   item[21]
+0x072 .. +0x0C1   unresolved character arrays/state
+0x0C2 .. +0x111   spell-preparation/memorization matrix A: 8 banks x 10 packed bytes
+0x112 .. +0x12A   unresolved character state
+0x12B .. +0x12E   executable-accessed dword; semantics unresolved
+0x12F .. +0x132   auxiliary dword; low byte is reversible Dexterity adjustment
+0x133 .. +0x136   dynamic per-character bitfield; individual meanings unresolved
+0x137 .. +0x138   executable-accessed bytes; semantics unresolved
+0x139             class/casting capability mask/selector
+0x13A .. +0x13D   uint32_le character state/flags
+0x13E .. +0x247   unresolved spell/effect/auxiliary state
```

A native implementation should continue to use an explicit legacy decoder rather than memory-mapping this image into a modern packed struct.

## Next target

Map the exact class identities behind capability bits at `+0x139`, trace `+0x133` dynamic bits back to their item/state owners, then assign gameplay identities to individual `+0x13A` flags. The spell-resource catalogue can be completed independently once the source `T` archive bytes are available.