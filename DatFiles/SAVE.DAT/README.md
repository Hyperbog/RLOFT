# SAVE.DAT

Status: **active reconstruction; certified CODE.2 serializer/deserializer available; top-level party, charge, and saved-roster boundaries now mapped.**

- Size: 9,288 bytes (`0x2448`)
- SHA-256: `f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d`
- Begins with the NUL-terminated description `QUICK START PARTY; ELVEN F/M - DWARVEN F/C` followed by binary state.
- Primary CODE.2 path: save `0000:067FE`, load `0000:06CBA`.

## Confirmed top-level structural anchors

The audited CODE.2 serializer identifies a `0x920`-byte active-party block. The supplied SAVE fixture proves this is exactly four consecutive `0x248`-byte character records beginning at file offset `0x0077`.

| Offset range | Size | Interpretation | Confidence |
|---|---:|---|---|
| `0x0000–0x0076` | `0x77` | Description + prefix/engine-state fields | Boundary confirmed; field semantics incomplete |
| `0x0077–0x0996` | `0x920` | Active party: `4 × 0x248` character records | Confirmed |
| `0x0997–0x0D58` | `0x3C2` | Mixed mutable state | Boundary confirmed; semantics incomplete |
| `0x0D59–0x0D67` | `0x0F` | 15-byte charged-item counter vector | Confirmed |
| `0x0D68–0x2437` | `0x16D0` | Saved roster: `10 × 0x248` character records | Confirmed |
| `0x2438–0x2447` | `0x10` | Trailing state | Boundary confirmed; semantics incomplete |

These regions exactly account for all `0x2448` bytes of the reference file.

### Character-record evidence

The `0x248` stride is visible directly in the fixture and agrees with the CODE.2 serializer. The four active-party slots begin at `0x0077`, `0x02BF`, `0x0507`, and `0x074F`. The first two contain `EHLIANNA` and `ANVIL DAWN`; the remaining two slots are empty in this quick-start fixture.

The ten-record saved-roster block begins at `0x0D68`. Its populated records contain `GLORIANNA`, `DOOM GUARD`, `RAVEN`, `MELODY`, `RANDAR`, `BOW DYN`, `WYND SONG`, `KRAAL`, and `BARRUK`, with one empty slot.

This establishes that the same fixed-size character image is reused in both the active-party and saved-roster sections.

## Proven cross-link: persistent event flags

CODE.2 event routines `1C124`, `1C160`, `1C1A0`, and `1C1DC` respectively set, clear, toggle, and test bits in the dword vector beginning at runtime DGROUP `0x22200`.

The SAVE serializer at `067FE..06811` writes exactly 32 bytes from DGROUP `0x22200`, and loader `06CBA..06CC7` reads exactly the same 32 bytes back. This is therefore a persistent **256-bit global game/event-state flag vector**. LEVELS.DAT event opcodes 8, 9, and 10 manipulate this same saved state directly.

Its exact file offset within the prefix/middle state is still being assigned from serializer order; the executable-side width and runtime identity are proved.

## Proven cross-link: persistent charged-item counters

OBJECTS.DAT record byte `+0x31` selects mutable state outside the object-definition table. For the proved charged-device family—ROD OF SMITING and the wand definitions—CODE.2 uses it as an index into the byte vector beginning at runtime DGROUP `0x67EC`.

The vector is exactly **15 bytes**. The item-use paths test the selected counter for zero, decrement it on successful use, and invoke depletion/removal behavior when it reaches zero. In the supplied certified CODE.2 data image all 15 initial values are `12` (`0x0C`).

SAVE serialization writes 15 bytes from DGROUP `0x67EC`, and the loader restores exactly the same 15 bytes. The supplied quick-start SAVE contains the matching fifteen-byte run at `0x0D59..0x0D67`, making this a confirmed file-level anchor.

The architectural division is:

```text
OBJECTS.DAT  static item definition -> charge_counter_index
SAVE.DAT     mutable game state      -> current charge_counter[15]
```

The supplied OBJECTS database addresses charge slots 0..11; slots 12..14 remain serialized runtime capacity but are not selected by the supplied object definitions.

## CODE.2 serializer evidence

The final semantic audit records the save/load sequence as player position/direction, level number, world/quest flag blocks, the `0x920` active-party block, inventory and hand-item state, the 15-byte charges array, saved-character/roster records, and trailing counters/state. The loader mirrors the write order and then performs runtime fixups.

For the future native port, SAVE.DAT should therefore be treated as a serialized DOS-era runtime image. It should not be memory-mapped directly into modern C++ structures without explicit little-endian and legacy-layout handling.

## Tooling

`tools/analyze_save.py` validates the confirmed boundaries and prints the names at each `0x248` character-record slot without assigning speculative semantics to unknown bytes.

```bash
python DatFiles/SAVE.DAT/tools/analyze_save.py /path/to/SAVE.DAT
```

The analyzer checks the reference size, four active-party records, charge-vector position, ten saved-roster records, and final `0x10`-byte trailing region.

## Next reconstruction target

Reconstruct the `0x248` character record field-by-field, then map `0x0000–0x0076` and `0x0997–0x0D58` sequentially against the certified save/load operations. A C/C++ layout becomes canonical only when its offsets reproduce the fixture and agree with both serializer directions.
