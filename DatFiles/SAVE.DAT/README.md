# SAVE.DAT

Status: **high-effort audited active reconstruction.** The top-level serializer/deserializer structure is certified; the shared `0x248` character image is now substantially reconstructed through identity enums, abilities, inventory, spell state, combat/effect state, and its terminal reserved region. Remaining work is concentrated in global/non-character SAVE state and unresolved effect/resource catalogues.

- Size: 9,288 bytes (`0x2448`)
- SHA-256: `f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d`
- Begins with the NUL-terminated description `QUICK START PARTY; ELVEN F/M - DWARVEN F/C` followed by binary state.
- Primary CODE.2 path: save `0000:067FE`, load `0000:06CBA`.

## Confirmed top-level structural anchors

| Offset range | Size | Interpretation | Confidence |
|---|---:|---|---|
| `0x0000–0x0076` | `0x77` | Description + prefix/engine-state fields | Boundary confirmed; field semantics incomplete |
| `0x0077–0x0996` | `0x920` | Active party: `4 × 0x248` character records | Confirmed |
| `0x0997–0x0D58` | `0x3C2` | Mixed mutable/global state | Boundary confirmed; semantics incomplete |
| `0x0D59–0x0D67` | `0x0F` | 15-byte charged-item counter vector | Confirmed |
| `0x0D68–0x2437` | `0x16D0` | Saved roster: `10 × 0x248` character records | Confirmed |
| `0x2438–0x2447` | `0x10` | Trailing state | Boundary confirmed; semantics incomplete |

These regions exactly account for all `0x2448` bytes.

The four active-party slots begin at `0x0077`, `0x02BF`, `0x0507`, and `0x074F`. The supplied quick-start fixture contains `EHLIANNA`, `ANVIL DAWN`, then two unused slots. The ten saved-roster records begin at `0x0D68` and contain, in order, `GLORIANNA`, `VELIKA`, `MISCHA`, `VUKO`, `TATIAN`, `IRMGARDE`, `ANSHELM`, `RYSZARD`, `VLADISLAV`, and `FHALKEN`.

## Direct character-image serializer proof

The corrected runtime character base is directly visible in both serializer directions:

```text
physical 0x41949  push 0x920
physical 0x4194E  push 0x218E0
                 write active-party image

physical 0x41CF5  push 0x920
physical 0x41CFA  push 0x218E0
                 read active-party image
```

`0x920 = 4 × 0x248`, so character `n` begins at runtime:

```text
0x218E0 + n*0x248
```

Earlier SAVE notes that used runtime base `0x218D8` translated runtime-derived serialized offsets eight bytes too high. The current focused documents use `0x218E0`.

## Character record reconstruction status

The shared record is documented in `CHARACTER_RECORD.md` and focused notes. Major certified regions include:

```text
+0x000..+0x00F   name
+0x010..+0x011   status word
+0x012           portrait/appearance index
+0x013           sex code
+0x014           race code
+0x015..+0x017   class_code[3]
+0x018..+0x019   age
+0x01A           alignment code
+0x01B           base Armor Class
+0x01C..+0x01F   current/max HP
+0x020..+0x031   class levels + progression
+0x032..+0x03F   six AD&D ability base/current pairs + exceptional Strength
+0x040..+0x069   item[21]
+0x06A..+0x0B9   Mage spellbook matrix
+0x0BA..+0x109   Cleric/Paladin priest-style spell matrix
+0x10A..+0x1F5   combat, spell UI, timed effects, temporary hand effects, recovery/protection state
+0x1F6..+0x247   82-byte serialized reserved/unreferenced terminal region
```

### Certified identity enums

CODE.2's character information screen maps the serialized codes directly into contiguous `T` text records. Cross-checks against the supplied fixture and published game documentation close the legacy numeric values:

```text
sex_code
0 Male
1 Female

race_code
0 Human
1 Half-Elf
2 Halfling
3 Gnome
4 Dwarf
5 Elf

class_code
0 Fighter
1 Paladin
2 Ranger
3 Cleric
4 Mage
5 Thief
-1 absent later class slot

alignment_code
0 Lawful Good
1 Lawful Neutral
2 Neutral Good
3 True Neutral
4 Chaotic Good
5 Chaotic Neutral
```

STRAHD'S POSSESSION exposes six non-evil player alignments; the serialized field is not a nine-value full AD&D alignment enum.

### Ability layout

The executable displays six current scores from runtime `0x21913`, stepping by two bytes, with a parallel base-value pass from `0x21912`. Mechanical consumers and fixture/published NPC stats certify:

```text
+0x032/+0x033  Strength      base/current
+0x034/+0x035  Dexterity     base/current
+0x036/+0x037  Constitution  base/current
+0x038/+0x039  Intelligence  base/current
+0x03A/+0x03B  Wisdom        base/current
+0x03C/+0x03D  Charisma      base/current
+0x03E/+0x03F  exceptional Strength percentile base/current
```

### Spell families

The two persistent preparation matrices are no longer semantically anonymous:

```text
matrix A  +0x06A..+0x0B9  Mage/scribeable spellbook family
matrix B  +0x0BA..+0x109  Cleric/Paladin priest-style family
```

The casting-capability byte at `+0x131` is a mask:

```text
0x01  Cleric capability
0x02  Mage capability
0x04  Paladin priest capability
```

Matrix A maps to `T[0x42..0x91]`; matrix B maps to `T[0x92..0xE1]` before per-bank content limits are applied.

### Timed-effect audit correction

The high-effort re-audit of the 26-entry primary effect dispatcher corrected one earlier transcription:

```text
primary effect 11 -> owns/clears the protection pool at +0x1EA
primary effect 10 -> shares an external-state update path with effect 1
```

Effects 12, 13, and 23 remain independently producer/expiration-certified against their reversible Strength/Dexterity auxiliary state. See `EFFECT_FLAGS.md`.

### Terminal region

The final record interval is:

```text
record  +0x1F6..+0x247
runtime 0x21AD6..0x21B27
```

No direct character-field reference was found there. Character initialization explicitly zeroes through runtime `0x21B27`, and `0x21B28 - 0x218E0 = 0x248`, proving the region is inside the runtime image. Preserve all 82 bytes exactly rather than treating them as disposable file padding.

## Proven global cross-link: event flags

CODE.2 event routines `1C124`, `1C160`, `1C1A0`, and `1C1DC` respectively set, clear, toggle, and test bits in the 32-byte vector beginning at runtime DGROUP `0x22200`.

The SAVE serializer writes exactly those 32 bytes and the loader restores them. This is a persistent **256-bit global game/event-state flag vector**. LEVELS.DAT event opcodes 8, 9, and 10 manipulate the same state. Its exact SAVE file offset inside the remaining global-state regions is still to be assigned from serializer order.

## Proven global cross-link: charged-item counters

OBJECTS.DAT record byte `+0x31` indexes the 15-byte mutable vector at runtime DGROUP `0x67EC` for the proved charged-device family. Item-use paths test and decrement the selected counter and perform depletion behavior at zero.

SAVE.DAT stores this vector at confirmed file offsets:

```text
0x0D59..0x0D67  current charge_counter[15]
```

The supplied OBJECTS definitions address slots 0..11; slots 12..14 remain serialized runtime capacity.

## Tooling

Two analyzers cover different layers:

```bash
python DatFiles/SAVE.DAT/tools/analyze_save.py /path/to/SAVE.DAT
python DatFiles/SAVE.DAT/tools/analyze_character_records.py /path/to/SAVE.DAT
```

`analyze_save.py` validates top-level boundaries, party/roster placement, the charge vector, and final trailing region.

`analyze_character_records.py` decodes the corrected character layout, including certified identity enums, age/base AC, class slots/levels/progression, all six ability pairs, inventory, both spell matrices, selected post-inventory fields, effect bits, and the complete terminal region.

## Native-port compatibility rule

Treat SAVE.DAT as an explicitly decoded little-endian legacy runtime image. Do not memory-map it onto compiler-native C/C++ structures. Preserve unknown/reserved bytes and legacy enum numbers exactly at the import/export boundary.

## Remaining reconstruction targets

1. Reconstruct global/prefix state `0x0000..0x0076` sequentially from the mirrored save/load operations.
2. Reconstruct mixed mutable state `0x0997..0x0D58` and trailing state `0x2438..0x2447`.
3. Decode/catalogue `T[588..613]` to assign exact player-facing names to all 26 primary effects.
4. Recover exact Mage/priest spell lists and per-level entry limits for `T[0x42..0xE1]`.
5. Audit remaining `status_word` bits and the complete portrait/status-dependent presentation catalogue.

The next structural priority is the **global non-character SAVE state**. Character-record reconstruction is sufficiently stable to support a native compatibility decoder without returning to speculative offset discovery.