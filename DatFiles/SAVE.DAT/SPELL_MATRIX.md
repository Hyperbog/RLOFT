# SAVE.DAT spell-preparation matrix

Status: **executable-certified structure and nibble semantics; bank numbering and the shipped memorization UI's visible bank range are now mapped.**

The persistent character spell state occupies record `+0x0C2..+0x111` (`DGROUP 0x2199A..0x219E9`):

```text
cell = record[0x0C2 + bank * 10 + entry]
bank = 0..7
entry storage capacity = 0..9

bits 7..4 = currently memorized / ready copies
bits 3..0 = selected / planned memorization count
low nibble 0xF = unavailable / non-selectable sentinel
```

## Bank identity: one-based spell-level tiers

The memorization UI provides the missing bank-label proof. In the rendering loop at physical CODE.2 `0x401DB..0x40353`, the zero-based bank counter at `[ebp-0x14]` is:

1. multiplied by `10` at `0x401ED` to select the corresponding ten-byte matrix bank;
2. incremented by one at `0x401F4..0x401FA` before being passed to the UI text-formatting path;
3. advanced once per rendered bank at `0x40348`;
4. terminated by the shipped UI after six visible banks at `0x4034F` (`bank < 6`).

This establishes the matrix bank convention as **zero-based storage for one-based spell-level tiers**:

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

The executable allocates and maintains eight banks, while this memorization UI path renders only the first six. Therefore the last two banks are real legacy/runtime capacity even though they are outside this shipped screen's visible range. Do not truncate the serialized matrix to six banks.

## Entry-count table behavior

The same UI does not assume all ten entries in a bank are valid. At physical `0x40330`, its entry loop compares the current entry index against a per-bank dword table addressed from DGROUP `0x6420`, with a four-byte stride per bank. Matrix-management routines at `0x3ED4D..0x3EDB1` and `0x3EE2B..0x3EE81` use the same per-bank counts while enabling/disabling cells.

Consequently:

```text
10 bytes per bank = serialized storage capacity
per-bank table     = number of meaningful spell entries in that bank
```

The exact numeric table contents should not be copied from raw CODE.2 file offsets without reconstructing the LE object's initialized data and relocation state. `Runtime/CODE_OBJECT_MAP.md` establishes that CODE.2 is a bound DOS/4G LE image rather than a flat binary. Instruction references remain valid evidence for table identity and stride; raw-file reads at the apparent DGROUP offset are not canonical table extraction.

## Slot-limit calculation

Routine physical `0x40B00` constructs an eight-byte working vector at DGROUP `0x6208`. Its caller supplies character index, a class-level-derived index, and one of two class-dependent table families (`0x62B0` or `0x6350`). The routine clears eight bytes, copies an eight-byte row selected by level, and applies additional character-dependent adjustments.

The normalization routine at `0x3EB28..0x3ECF9` then compares both selected and ready totals for each matrix bank against `0x6208[bank]`, clamping counts when necessary.

Thus `0x6208[0..7]` is an executable working vector of **per-spell-level preparation-slot limits** for the current character/context. Its values are derived from class/level tables rather than stored directly in SAVE.DAT.

## Native-port rule

Preserve all eight banks and ten bytes per bank exactly in the legacy SAVE decoder. Treat the visible spell level as `bank + 1`, but keep UI visibility separate from storage capacity:

```cpp
constexpr std::size_t kSpellLevelBanks = 8;
constexpr std::size_t kSpellEntriesPerBank = 10;
constexpr std::size_t kShippedMemorizeUiBanks = 6;

uint8_t ready_count(uint8_t cell)    { return cell >> 4; }
uint8_t selected_count(uint8_t cell) { return cell & 0x0F; }
bool unavailable(uint8_t cell)       { return selected_count(cell) == 0x0F; }
```

Do not model the last two banks as padding merely because the audited UI loop stops after spell level 6.

## Next target

Reconstruct the initialized LE data tables behind DGROUP `0x6420`, `0x62B0`, and `0x6350` with relocation-aware tooling, then identify the per-level spell-entry counts and class/level slot-progression rows exactly. In parallel, continue semantic tracing of the `+0x13A` character-state flags.
