# SAVE.DAT spell-ID resource lookup

Status: **closed for the shipped spell catalogue.** Matrix A is the Mage/scribeable spellbook family; matrix B is the shared Cleric/Paladin priest-style family. The raw `T` resource has now been decoded directly and the executable per-bank entry-count tables have been recovered.

See `SPELL_CATALOGUE.md` for the complete level-by-level labels and the level-loss compatibility quirk.

## Common spell/action record expansion

CODE.2 physical `0x5E0E4..0x5E2C8` expands ready spell copies from both persistent character matrices into runtime records.

For one selected spell-level bank it constructs:

```text
Mage / matrix A ID base   = 0x42 + bank * 10
Priest / matrix B ID base = 0x92 + bank * 10
```

It walks stored cells, emitting one runtime record per ready copy. Runtime records retain spell level/bank, entry index, character index, and family discriminator (`0` = Mage, `1` = priest).

## Backing resource: `T`

CODE.2 opens the one-character filename `T`/`t` and stores its handle at DGROUP `0x22224`. The supplied raw file matches the certified resource identity:

```text
bytes    133,584
SHA-256 a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315
records  1,073
```

The indexed reader performs:

```text
seek(T, 4 + index * 4)
record_offset = read_u32(T)
seek(T, record_offset)
payload_length = read_u16(T)
read payload_length bytes
append runtime NUL
for each payload byte: byte ^= 0xDC
```

Thus spell IDs are direct `T` indexes; there is no unidentified intermediate name archive.

## Spell ID geometry

```text
Mage family:   T_index = 0x42 + bank*10 + entry
Priest family: T_index = 0x92 + bank*10 + entry
bank           = 0..7
entry          = 0..9
```

Storage reserves 80 indexes per family:

```text
Mage / matrix A:   T[0x42..0x91]  = T[66..145]
Priest / matrix B: T[0x92..0xE1]  = T[146..225]
```

## Executable active-entry counts

CODE.2 stores two eight-dword count tables:

```text
DGROUP 0x6420 / physical 0x7C344
priest = [7, 6, 5, 4, 4, 2, 0, 0]

DGROUP 0x6440 / physical 0x7C364
Mage   = [7, 6, 10, 4, 2, 6, 0, 0]
```

Normal Cleric/Paladin matrix-B setup uses `0x6420`; Mage matrix-A normalization uses `0x6440`. These tables match the directly decoded active spell labels in `T`.

The shipped prepared catalogues therefore contain:

```text
Mage prepared spells:   35
priest prepared spells: 28
```

Banks 7 and 8 remain serialized but have zero active entries.

## Priest-family action labels outside the prepared count

The first priest resource bank contains two additional named records after its seven prepared spells:

```text
T[153] / dispatch 87  LAY HANDS
T[154] / dispatch 88  TURN UNDEAD
```

Because the priest bank-0 active-entry count is seven, these records are not matrix-B memorization cells even though they share the same priest-family text/dispatch namespace.

## Family identity closure

Certified class codes and initialization paths remain:

```text
class 4 Mage
    -> capability 0x02
    -> matrix A / runtime 0x2194A

class 3 Cleric
    -> capability 0x01
    -> matrix B / runtime 0x2199A

class 1 Paladin
    -> capability 0x04
    -> matrix B / runtime 0x2199A
```

The Mage-only scribing path independently operates on matrix A.

## Level-loss compatibility quirk

Routine `0x3EB28`, called after level loss for Cleric and Paladin class slots, operates on priest matrix B but bounds its entry loops with `0x6440`, the Mage-shaped count table, rather than normal priest table `0x6420`.

This does not reverse the family identities; it is an original executable inconsistency/likely legacy bug. A byte/behavior-faithful implementation should reproduce that path unless an explicit compatibility fix is enabled.

## Native-port requirement

Preserve `(family, bank, entry)` and the original T index/dispatch geometry at the compatibility boundary. Do not compact the serialized matrices to only the shipped 35/28 active spells: both remain fixed `8 x 10` byte arrays, including reserved/unused cells.