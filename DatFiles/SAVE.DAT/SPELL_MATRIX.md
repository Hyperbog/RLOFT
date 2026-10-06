# SAVE.DAT spell-preparation matrix

Status: **executable-certified structure and nibble semantics, normalized to corrected runtime character base DGROUP `0x218E0`.**

Each character has two parallel 80-byte matrices. The matrix audited here is the second family at record `+0x0BA..+0x109` / runtime `0x2199A..0x219E9`. The first family is immediately before it at `+0x06A..+0x0B9` / runtime `0x2194A..0x21999`.

```text
cell = matrix[bank * 10 + entry]
bank = 0..7
entry storage capacity = 0..9

bits 7..4 = currently memorized / ready copies
bits 3..0 = selected / planned memorization count
low nibble 0xF = unavailable / non-selectable sentinel
```

## Bank identity: one-based spell-level tiers

The memorization UI provides the bank-label proof. In CODE.2 physical `0x401DB..0x40353`, the zero-based bank counter is multiplied by ten to select the bank, incremented by one for display, advanced once per rendered bank, and stopped after six visible banks.

Therefore storage is zero-based while player-facing spell level is one-based:

```text
bank 0 -> spell level 1
bank 1 -> spell level 2
bank 2 -> spell level 3
bank 3 -> spell level 4
bank 4 -> spell level 5
bank 5 -> spell level 6
bank 6 -> spell level 7 storage
bank 7 -> spell level 8 storage
```

The executable maintains all eight banks even though this shipped UI path displays six. Do not truncate either serialized matrix.

## Entry-count table behavior

At physical `0x40330`, the UI compares the entry index against a per-bank dword table based at DGROUP `0x6420`, four-byte stride per bank. Matrix-management routines `0x3ED4D..0x3EDB1` and `0x3EE2B..0x3EE81` use the same counts while enabling/disabling cells.

Thus ten bytes per bank is storage capacity; the table supplies the number of meaningful spell entries for each bank.

## Slot-limit calculation

Physical routine `0x40B00` constructs an eight-byte working vector at DGROUP `0x6208`. Its caller supplies character index, a class-level-derived index, and one of two class-dependent table families (`0x62B0` or `0x6350`). It clears eight bytes, copies an eight-byte level-selected row, and applies character-dependent adjustments.

Normalization at `0x3EB28..0x3ECF9` compares selected and ready totals for each bank against `0x6208[bank]`, clamping when necessary. `0x6208[0..7]` is therefore the per-spell-level preparation-slot limit vector for the current character/context.

## Native-port rule

```cpp
constexpr std::size_t kSpellLevelBanks = 8;
constexpr std::size_t kSpellEntriesPerBank = 10;
constexpr std::size_t kShippedMemorizeUiBanks = 6;

uint8_t ready_count(uint8_t cell)    { return cell >> 4; }
uint8_t selected_count(uint8_t cell) { return cell & 0x0F; }
bool unavailable(uint8_t cell)       { return selected_count(cell) == 0x0F; }
```

Preserve both matrices byte-for-byte:

```text
family A  +0x06A..+0x0B9
family B  +0x0BA..+0x109
```

Earlier revisions documented these as `+0x072..+0x0C1` and `+0x0C2..+0x111`; those offsets were based on the obsolete `0x218D8` runtime base and are superseded.