# SAVE.DAT spell-preparation matrix

Status: **executable-certified structure, nibble semantics, family ownership, and shipped per-level entry counts.** Offsets use corrected runtime character base DGROUP `0x218E0`.

Each character has two adjacent 80-byte matrices:

```text
Mage matrix A:   +0x06A..+0x0B9  runtime 0x2194A..0x21999
Priest matrix B: +0x0BA..+0x109  runtime 0x2199A..0x219E9
```

Each matrix is `8 x 10` stored cells:

```text
cell = matrix[bank * 10 + entry]
bank = 0..7
entry storage capacity = 0..9

bits 7..4 = currently memorized / ready copies
bits 3..0 = selected / planned memorization count
low nibble 0xF = unavailable / non-selectable sentinel
```

## Bank identity

The memorization UI renders the zero-based bank counter as `bank + 1` and stops after six visible banks:

```text
bank 0 -> spell level 1
bank 1 -> spell level 2
bank 2 -> spell level 3
bank 3 -> spell level 4
bank 4 -> spell level 5
bank 5 -> spell level 6
bank 6 -> reserved level-7 storage
bank 7 -> reserved level-8 storage
```

Both reserved banks remain part of the serialized image and must not be truncated.

## Shipped active-entry counts

The raw CODE.2 data image contains two eight-dword count tables:

```text
DGROUP 0x6420 / physical 0x7C344
priest matrix B = [7, 6, 5, 4, 4, 2, 0, 0]

DGROUP 0x6440 / physical 0x7C364
Mage matrix A   = [7, 6, 10, 4, 2, 6, 0, 0]
```

Normal Cleric/Paladin setup routines `0x3ED08` / `0x3EDC8` access matrix B and use `0x6420`. Mage normalization routine `0x3E99C` accesses matrix A and uses `0x6440`.

These values agree exactly with the directly decoded `T` spell catalogue:

```text
Mage active entries by level:   7, 6, 10, 4, 2, 6  -> 35 total
priest active entries by level: 7, 6,  5, 4, 4, 2  -> 28 total
```

See `SPELL_CATALOGUE.md` for every label/T index/dispatch ID.

## Priest-family action records outside matrix counts

`T[153] = LAY HANDS` and `T[154] = TURN UNDEAD` occupy the first priest resource bank but lie beyond that bank's preparation count of seven. They share the priest action namespace but are not prepared matrix-B cells.

## Slot-limit calculation

Physical routine `0x40B00` constructs an eight-byte working vector at DGROUP `0x6208`. Its caller supplies character index, a class-level-derived index, and a class-dependent progression table (`0x62B0`, `0x6350`, or the Mage table path). The vector limits total selected/ready copies by spell level.

Normalization compares the per-cell selected/ready totals with `0x6208[bank]` and clamps them when required. This slot-limit vector is distinct from the entry-count tables above:

- entry-count tables say **which named cells exist** in a bank;
- `0x6208` says **how many spell copies may be prepared** for the current character/level.

## Original level-loss inconsistency

Priest level-loss normalization routine `0x3EB28` is called for both Paladin and Cleric class slots and operates on matrix B at runtime `0x2199A`, but its inner loops use count table `0x6440` instead of normal priest table `0x6420`.

This is an executable-level legacy inconsistency/likely bug. It must not be used to reverse the otherwise independently certified Mage/priest family identities. Preserve it when emulating original behavior, or correct it only behind an explicit compatibility deviation.

## Native-port rule

```cpp
constexpr std::size_t kSpellLevelBanks = 8;
constexpr std::size_t kSpellEntriesPerBank = 10;
constexpr std::size_t kShippedMemorizeUiBanks = 6;

uint8_t ready_count(uint8_t cell)    { return cell >> 4; }
uint8_t selected_count(uint8_t cell) { return cell & 0x0F; }
bool unavailable(uint8_t cell)       { return selected_count(cell) == 0x0F; }
```

Preserve both `0x50`-byte matrices byte-for-byte, including inactive/reserved entries.

Earlier revisions documented the matrices at `+0x072` and `+0x0C2`; those offsets came from obsolete runtime base `0x218D8` and are superseded.