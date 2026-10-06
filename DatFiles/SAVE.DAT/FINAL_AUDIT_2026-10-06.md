# SAVE.DAT final reconstruction audit — 2026-10-06

Status: **ARCHIVAL-COMPLETE FOR THE SUPPLIED RAVENLOFT BUILD, WITH BOUNDED RAW/LEGACY FIELDS PRESERVED.**

This checkpoint re-audits the reconstructed SAVE format against the supplied immutable `SAVE.DAT`, `CODE.1`, `CODE.2`, `T`, `OBJECTS.DAT`, and `NPCS.DAT`. It certifies format geometry and the semantic mappings required for a faithful importer/exporter or native compatibility layer. It does not invent source-level names for fields whose shipped producer identity is not recoverable.

## Source identities

```text
SAVE.DAT     9,288 bytes  f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d
CODE.1     277,787 bytes  fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
CODE.2     558,689 bytes  853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314
T          133,584 bytes  a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315
OBJECTS.DAT 21,844 bytes  f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0
NPCS.DAT     5,731 bytes  f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b
```

## Structural audit

The supplied quick-start fixture has zero live transient effects. Sequential decoding produces:

```text
0x0000..0x0076  fixed prefix
0x0077..0x0996  active party, 4 * 0x248
0x0997..0x099A  live_effect_count = 0
0x099B..0x09A2  spell UI selected characters
0x09A3..0x09A6  gameplay tick counter
0x09A7..0x0D50  62 persistent container states
0x0D51..0x0D58  renderer/environment controls
0x0D59..0x0D67  charged-device counters
0x0D68..0x2437  saved roster, 10 * 0x248
0x2438..0x2447  party container UI contexts
end = 0x2448
```

For nonzero live-effect count, every later block shifts by `count * 0x16`.

The 62 CODE.2 container descriptors contain 438 slots exactly:

```text
438 * 2-byte object IDs + 62 state bytes = 938 = 0x3AA
```

This independently reproduces the complete mutable-container interval.

## Character-image audit

CODE.2 transfers `0x920 = 4 * 0x248` bytes beginning at runtime `0x218E0`. The prior `0x218D8` reconstruction is retained only in explicit correction-history notes.

The fixed character record is therefore exactly `0x248` bytes. Identity, status, portrait, race/class/alignment, age, HP, class progression, all six ability pairs, inventory, both spell matrices, primary/secondary effects, timers, item-known bits, equipment auxiliaries, and the terminal reserved region are all placed at the normalized offsets documented in `CHARACTER_RECORD.md`.

## Portrait and age closure

CODE.1 proves:

```text
face_index 0..16  player-selectable
face_index 17     deliberately disabled in creation UI
face_index 18..22 not reachable by the creation-page cycle
```

The saved roster occupies the predefined valid upper slots; female face slots 21..22 are invalid/reserved resources.

CODE.1 also directly generates serialized age from the six race rows:

```text
Human      15 + 1d4
Half-Elf   15 + 1d6
Halfling   20 + 3d4
Gnome      60 + 3d12
Dwarf      40 + 5d6
Elf       100 + 5d6
```

and later displays the same stored word in character information UI.

## Text/resource cross-checks

Raw `T` independently decodes:

```text
T[653..657] = POSSESSED, HELD, DISEASED, POISONED, PARALYZED
```

and all 26 primary-effect expiration labels at `T[588..613]`. These agree with the executable producer catalogue.

CODE.2's spell count tables also recheck exactly as:

```text
priest [7, 6, 5, 4, 4, 2, 0, 0]
Mage   [7, 6, 10, 4, 2, 6, 0, 0]
```

## Dynamic transient-effect records

The counted `0x16` array is certified as the class-3 transient/moving-effect family. Principal fields are:

```text
+0x00 coordinate B fixed-point dword
+0x04 coordinate A fixed-point dword
+0x08 heading/orientation
+0x0C cached map-cell index
+0x0E transient-effect type
+0x10 auxiliary word
+0x12 type-specific auxiliary
+0x14 packed movement/direction/frame/source state
```

Shipped producer evidence covers types 0,1,2,3,4,6,8,9,10,11. Types 5 and 7 retain valid INIT/RES3 descriptors but no direct producer was found in the audited supplied paths; preserve them numerically rather than assigning speculative spell names.

## Bounded unknowns

The following are not format blockers and must remain raw/preserved:

- character status bit `0x2000`: engine-recognized blocking tier, no proved player-facing identity or direct lifecycle producer;
- character status low bits `0x0001..0x0020`: no proved gameplay ownership;
- character `+0x1BA` dword: serialized, no direct CODE.2 field reference found;
- exact source-level names for the three paired timed slots and selected auxiliary quantities;
- remaining unnamed packed bits/enum labels inside transient-effect state;
- historical source identifiers where only behavioral identities survive.

These are explicit evidence boundaries, not invitations to infer AD&D semantics.

## Shipped compatibility anomalies retained

A faithful implementation must preserve documented original behavior, including:

- OBJECTS container selector 62 versus serialized descriptor/state range 0..61;
- priest level-loss normalization using the Mage-shaped count table;
- partially initialized transient-effect auxiliary/packed fields;
- variable SAVE size from the live-effect array;
- raw/reserved character bytes and status bits.

## Final decision

For the supplied STRAHD'S POSSESSION build, SAVE.DAT no longer has an unresolved structural region that blocks a byte-faithful reader/writer or faithful native-state import/export layer.

Future work may improve source-level names for bounded opaque fields, but such naming is archival enrichment rather than required format reconstruction. Any new claim must continue to prefer immutable executable/resource evidence over inference.
