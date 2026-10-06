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
| `+0x0C2` | `0x50` | packed 4-bit-value array, `8 x 10` bytes | Runtime `0x2199A`; CODE.2 uses eight banks, ten-byte bank stride, and independently manipulates both nibbles. Gameplay label pending. |
| `+0x13A` | 4 | 32-bit character state/flags | Runtime `0x21A12`; CODE.2 performs byte, word, and dword mask operations here. |

Address arithmetic:

```text
record base                 0x218D8
status word                 0x218F0 = +0x18
class-related byte          0x218F5 = +0x1D
Strength base               0x21912 = +0x3A
Strength current            0x21913 = +0x3B
Dexterity-related current   0x21915 = +0x3D
exceptional STR base        0x2191E = +0x46
exceptional STR current     0x2191F = +0x47
item array                  0x21920 = +0x48
packed-nibble array         0x2199A..0x219E9 = +0xC2..+0x111
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

### `+0x0C2..+0x111`: packed nibble array

CODE.2 proves the exact layout:

```text
byte = record[0x0C2 + bank * 10 + entry]
bank  = 0..7
entry = 0..9
```

The outer executable loops terminate after eight banks and advance the array cursor by ten bytes per bank. Direct element access multiplies the bank index by ten. Each byte contains independently mutable high/low 4-bit values.

This also resolves the former candidate `+0x10A`: it is bank 7, entry 2 (`0x0C2 + 7*10 + 2`), not a separate field. The fixture has `0x0F` there for both populated active characters. The exact gameplay meaning of the banks and nibbles remains under audit.

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
+0x0C2 .. +0x111   packed-nibble array: 8 banks x 10 bytes
+0x112 .. +0x139   unresolved character state
+0x13A .. +0x13D   uint32_le character state/flags
+0x13E .. +0x247   unresolved spell/effect/auxiliary state
```

A native implementation should continue to use an explicit legacy decoder rather than memory-mapping this image into a modern packed struct.

## Next target

Trace the tables/callers that assign meaning to the eight packed banks, identify runtime `0x21A09` (`+0x131`), and map the individual `+0x13A` flag bits to gameplay effects. Then continue decoding the remaining post-inventory regions.