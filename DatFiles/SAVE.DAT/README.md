# SAVE.DAT

Status: **high-effort audited active reconstruction.** The serializer/deserializer sequence is certified; the shared `0x248` character image is substantially reconstructed, and the global serializer order is now mapped. A key format correction is now explicit: SAVE.DAT is **variable-length** because the middle section contains `N * 0x16` live-world-object records. The supplied quick-start fixture is 9,288 bytes only because `N = 0`.

- Supplied fixture size: 9,288 bytes (`0x2448`)
- Supplied fixture SHA-256: `f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d`
- Begins with a fixed 50-byte description field containing `QUICK START PARTY; ELVEN F/M - DWARVEN F/C`.
- Primary CODE.2 path: save `0000:067FE`, load `0000:06CBA`.

## Serializer-certified top-level structure

```text
fixed 0x77-byte global prefix
4 * 0x248 active-party character records
uint32 live_world_object_count
count * 0x16 live-world-object records
fixed/descriptor-sized global + level-local mutable state
15 charged-item counters
10 * 0x248 saved-roster character records
16-byte trailing party-local auxiliary state
```

For the supplied fixture, `live_world_object_count == 0`, producing these absolute offsets:

| Fixture offset range | Size | Interpretation |
|---|---:|---|
| `0x0000–0x0076` | `0x77` | fixed global prefix |
| `0x0077–0x0996` | `0x920` | active party: `4 × 0x248` |
| `0x0997–0x0D58` | `0x3C2` | zero-count world-object header + mutable global/level state |
| `0x0D59–0x0D67` | `0x0F` | charged-item counters |
| `0x0D68–0x2437` | `0x16D0` | saved roster: `10 × 0x248` |
| `0x2438–0x2447` | `0x10` | trailing party-local auxiliary state |

For a save containing `N` live world objects, every supplied-fixture offset from `0x099B` onward shifts by `N * 0x16`. Therefore `0x0D59`, `0x0D68`, `0x2438`, and total size `0x2448` are **fixture anchors, not universal format constants**.

See `GLOBAL_STATE.md` for the field-by-field serializer reconstruction.

## Fixed prefix now mapped

The `0x77` bytes before the active party are serialized as:

```text
0x0000..0x0031  50-byte save description
0x0032..0x0051  256 persistent event flags
0x0052..0x0055  current level/map index
0x0056..0x0059  player/world coordinate A
0x005A..0x005D  player/world coordinate B
0x005E..0x0061  player facing/orientation
0x0062..0x006B  four shared primary-effect counters/state values
0x006C..0x006F  save-time game-clock snapshot
0x0070..0x0076  remaining global selectors/state
```

The supplied fixture stores level/map `22`, coordinates `0x00048000/0x00048000`, facing `270`, and game-clock value `1899`.

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

`0x920 = 4 × 0x248`, so character `n` begins at runtime `0x218E0 + n*0x248`.

The four fixture party slots begin at `0x0077`, `0x02BF`, `0x0507`, and `0x074F`, containing `EHLIANNA`, `ANVIL DAWN`, then two unused slots.

## Character record reconstruction

The shared record is documented in `CHARACTER_RECORD.md` and focused notes. Major certified regions are:

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

```text
sex_code:       0 Male, 1 Female
race_code:      0 Human, 1 Half-Elf, 2 Halfling, 3 Gnome, 4 Dwarf, 5 Elf
class_code:     0 Fighter, 1 Paladin, 2 Ranger, 3 Cleric, 4 Mage, 5 Thief, -1 absent
alignment_code: 0 Lawful Good, 1 Lawful Neutral, 2 Neutral Good,
                3 True Neutral, 4 Chaotic Good, 5 Chaotic Neutral
```

### Ability layout

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

```text
matrix A  +0x06A..+0x0B9  Mage/scribeable spellbook family
matrix B  +0x0BA..+0x109  Cleric/Paladin priest-style family
```

Casting capability byte `+0x131`: `0x01` Cleric, `0x02` Mage, `0x04` Paladin priest capability.

### Timed-effect audit correction

The high-effort dispatcher re-audit established:

```text
primary effect 11 -> owns/clears protection pool +0x1EA
primary effect 10 -> shared external-state update path
```

Effects 12, 13, and 23 remain independently producer/expiration-certified. See `EFFECT_FLAGS.md`.

## Dynamic live-world-object section

Immediately after the active party, CODE.2 writes `DGROUP 0xC038` as a 32-bit count, then writes `count * 0x16` bytes from runtime `0x248EC`.

World-object creation/removal code proves:

```text
structure stride = 0x16
maximum count    = 0x96 (150)
```

The quick-start fixture count is zero. A parser that seeks directly to fixture offset `0x0D59` will therefore fail on saves containing live world objects.

## Level-local mutable state

After the world-object array, CODE.2 serializes additional globals plus 62 descriptor-sized mutable blocks. Descriptor table `0x7330 + slot*10` supplies a word-array length/pointer and one companion byte for each slot. The quick-start fixture's 62-slot payload is 938 bytes (`0x3AA`).

This is the next major semantic catalogue target: correlate the 62 slots with LEVELS.DAT and level initialization/runtime consumers.

## Charged-item counters and roster

The 15-byte charged-device vector at DGROUP `0x67EC` follows the mutable-state bank. In the supplied zero-world-object fixture it is at `0x0D59..0x0D67`.

The serializer then writes exactly `0x16D0 = 10 × 0x248` roster bytes from runtime `0xA808`. In the supplied fixture the roster begins at `0x0D68` and contains `GLORIANNA`, `VELIKA`, `MISCHA`, `VUKO`, `TATIAN`, `IRMGARDE`, `ANSHELM`, `RYSZARD`, `VLADISLAV`, and `FHALKEN`.

## Trailing 16 bytes

The final operation writes 16 bytes from DGROUP `0x6FA0`. Gameplay code indexes this region as four 4-byte party-local entries and directly consumes bytes `+0`, `+1`, and `+3` of each entry. The supplied fixture stores all `0xFF`; exact symbolic field names remain under audit.

## Tooling

```bash
python DatFiles/SAVE.DAT/tools/analyze_save.py /path/to/SAVE.DAT
python DatFiles/SAVE.DAT/tools/analyze_character_records.py /path/to/SAVE.DAT
```

The top-level analyzer must follow serializer order rather than assuming the quick-start fixture's late absolute offsets. `analyze_character_records.py` covers the corrected character layout.

## Native-port compatibility rule

Treat SAVE.DAT as an explicitly decoded little-endian legacy runtime image. Parse the middle section sequentially using the serialized live-world-object count. Preserve unknown/reserved bytes and legacy enum numbers exactly.

## Remaining reconstruction targets

1. Catalogue the 62 level-local mutable-state descriptors against LEVELS.DAT/runtime consumers.
2. Resolve the remaining prefix selectors at `0x70..0x76` and trailing four-entry state at DGROUP `0x6FA0`.
3. Decode/catalogue `T[588..613]` for all 26 primary-effect names.
4. Recover exact Mage/priest spell lists and per-level entry limits.
5. Audit remaining character `status_word` bits and portrait/status presentation variants.

The principal structural uncertainty is no longer SAVE byte ordering; it is the **semantic catalogue of the level-local mutable-state bank and remaining small global selectors**.