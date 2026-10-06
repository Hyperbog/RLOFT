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
| `+0x03B` | 1 | current/effective Strength byte | Runtime `0x21913`; melee/damage paths consume it; the Stone Giant Gauntlets path substitutes/restores it and caps adjustment at 20. |
| `+0x03D` | 1 | Dexterity-related current/effective byte | Runtime `0x21915`; Gauntlets of Dexterity equipment transitions alter exactly this byte. |
| `+0x046` | 1 | exceptional-Strength base/reference percentile | Runtime `0x2191E`; Strength table logic interprets ranges `1..50`, `51..75`, `76..90`, `91..99`, `100`. |
| `+0x047` | 1 | exceptional-Strength current/effective percentile | Runtime `0x2191F`; current Strength calculations and UI/combat consumers use it. |
| `+0x048` | `0x2A` | `int16_le item[21]` | Runtime `0x21920`; slots `0..11` inventory and `12..20` equipment. |
| `+0x13A` | at least 1 byte | character state/flags | Runtime `0x21A12`; audited CODE.2 performs direct tests/clears here. |

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
state/flags                 0x21A12 = +0x13A
next character              base + 0x248
```

## Correction to the initial fixture-only inference

The earlier fixture pass treated `+0x32..+0x3F` as seven adjacent ability-score pairs and tentatively treated `+0x3E/+0x3F` as exceptional Strength. Direct executable consumers disprove that interpretation.

Exceptional Strength is actually stored at `+0x46/+0x47`: CODE.2 combines runtime `0x21912` Strength with `0x2191E` percentile and explicitly recognizes percentile bands `1..50`, `51..75`, `76..90`, `91..99`, and `100`. Parallel current-value consumers use `0x21913` with `0x2191F`.

This is an important reconstruction rule: visual pairing in one SAVE fixture is not sufficient to assign gameplay semantics. Executable consumers remain controlling evidence.

## Strength equipment and combat proof

The helper beginning at CODE.2 physical `0x4CF50` indexes the character by `0x248`, checks equipped object ID `0x54`, and otherwise adjusts byte `0x21913` with a cap of 20. OBJECTS/RES cross-links identify object `0x54` as `STONE GIANT GAUNTLETS`. A companion getter returns either the gauntlet-backed value or `0x21913`.

The transition helper at physical `0x4CFE0` moves values between `0x21913` and a per-character auxiliary dword when object `0x54` is equipped/removed. The same helper recognizes object `0x53` (`GAUNTLETS OF DEXTERITY`) and adjusts byte `0x21915`, independently anchoring that byte to the Dexterity effect family.

Combat/weapon paths at physical `0x5E9DD` and `0x5EB69` multiply damage-like words by current/effective `0x21913` and divide by 18. This independently confirms that `+0x3B` is the effective Strength byte rather than an arbitrary paired statistic.

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

For active party slot 0 (`EHLIANNA`), the 21 words are:

```text
321, 322, 81, 70,
-1, -1, -1, -1, -1, -1,
252, -1, 10, -1, -1, -1,
-1, -256, -1, 0, -256
```

## Empty-record representation

Unused active-party records are not zero-filled. In the reference fixture most bytes are `0xFF`, while `+0x10..+0x11` are `00 80`. Occupancy must therefore not be inferred from an all-zero test; preserve the complete raw image until the executable occupancy predicate is certified.

## Current record partition

```text
+0x000 .. +0x017   identity/core metadata
+0x018 .. +0x047   status/stats/class region; several fields now executable-linked
+0x048 .. +0x071   item[21]
+0x072 .. +0x139   unresolved character arrays/state
+0x13A             executable-referenced state/flags byte
+0x13B .. +0x247   unresolved spell/effect/auxiliary state
```

A native implementation should continue to use an explicit legacy decoder rather than memory-mapping this image into a modern packed struct.

## Next target

Resolve the remaining paired bytes at `+0x32..+0x39` and `+0x3E/+0x3F`, identify the three dwords at `+0x26/+0x2A/+0x2E`, then audit the post-inventory array shapes from `+0x72` toward the hard `+0x13A` anchor.