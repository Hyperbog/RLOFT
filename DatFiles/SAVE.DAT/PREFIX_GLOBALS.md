# SAVE.DAT fixed-prefix globals `+0x70..+0x76`

Status: **executable-certified semantic closure for the final seven bytes of the fixed `0x77`-byte prefix.** These fields are not anonymous selectors: `CF4C` is the 24-hour fixed-point time-of-day clock, while `8600/8602` preserve the globally held/cursor item and its known/identified state.

## Serialized layout

```text
SAVE +0x70..+0x73  uint32 time_of_day_16_16_hours   // DGROUP 0xCF4C
SAVE +0x74..+0x75  int16  cursor_item_object_id     // DGROUP 0x8600
SAVE +0x76          uint8  cursor_item_known         // DGROUP 0x8602
```

The supplied quick-start fixture stores:

```text
time_of_day_16_16_hours = 0x000E0000  // 14:00 exactly
cursor_item_object_id   = -1           // no globally held item
cursor_item_known       = 0
```

## `CF4C`: 24-hour 16.16 fixed-point time-of-day clock

`CF4C` is a 32-bit fixed-point clock whose upper 16 bits are the hour in a 24-hour day and whose lower 16 bits are the fractional part of the hour.

The executable evidence is direct and mutually reinforcing:

1. Initialization at physical `0x5B497` writes `0x000E0000`, i.e. integer hour 14 with zero fractional part.
2. Routine physical `0x6CEB8` extracts the upper word, normalizes it modulo 24, preserves the lower 16 bits, and writes the recombined dword back to `CF4C`.
3. Environment/world logic at physical `0x529D5` and `0x59EA1` shifts `CF4C` right by 16 and compares the resulting value with hour boundaries. Multiple paths treat hours strictly between 4 and 21 differently from the night interval.
4. Time-display/UI logic at physical `0x5AE34` divides `(hour + 1)` by 24 and passes both the current hour and wrapped next-hour value to the formatting path.
5. A scripted/time-advance path at physical `0x5D1E4` repeatedly increments the word at `CF4C+2` and calls the normalization/update routine until the integer hour reaches 2.
6. A resting/time-advance path at physical `0x3F8E2` adds `0x445` to the full fixed-point value. `0x445 / 65536` hours is approximately one minute, matching a fine-grained time-of-day advance rather than an opaque counter.

Safe native representation:

```cpp
uint32_t time_of_day_16_16_hours;

uint16_t hour = time_of_day_16_16_hours >> 16;
uint16_t fractional_hour = time_of_day_16_16_hours & 0xFFFF;
```

Do not collapse the serialized value to an integer hour: the low word is retained by the executable and participates in incremental advancement.

This clock is distinct from both other saved timing domains:

```text
SAVE +0x6C          interrupt-driven engine tick snapshot (DGROUP 0xF58C)
middle DGROUP A7E4  pausable gameplay/deadline tick counter
SAVE +0x70          24-hour 16.16 world time-of-day clock (DGROUP 0xCF4C)
```

## `8600`: globally held/cursor object ID

DGROUP `0x8600` is a signed 16-bit OBJECTS.DAT object ID for the item currently attached to the global item cursor/held-item UI state. `-1` means no item is being carried by that cursor state.

Evidence:

- UI initialization at physical `0x55044` sets `0x8600 = -1` and clears `0x8602`.
- Setter path physical `0x55268` accepts an object ID, writes it to `0x8600`, indexes the `0x41`-byte OBJECTS.DAT table with that ID, reads object byte `+0x2D` (the certified inventory/UI graphic selector), loads the resulting RES1 graphic, and refreshes the cursor/UI image.
- The richer redraw/swap path at physical `0x554D4..0x55562` performs the same OBJECTS `object_id * 0x41` lookup and writes the new ID to `0x8600`.
- Inventory-transfer code at physical `0x4C6C4..0x4C963` reads a character inventory object and its corresponding known bit, reads the current `0x8600/0x8602` pair, swaps the object into the inventory slot, and then rebuilds the globally held-item display. This is an item-transfer state pair, not a generic UI selection.
- SAVE loading does not merely restore the two bytes to memory. Physical `0x41CC0..0x41CEC` reads them into temporaries and calls the held-item setter `0x55268`, rebuilding the runtime cursor graphic from the restored object ID and flag.

Safe type:

```cpp
int16_t cursor_item_object_id; // -1 = empty
```

## `8602`: cursor-held item known/identified flag

DGROUP `0x8602` travels with `0x8600` as the held item's identification/known state.

The strongest cross-check is the inventory swap path:

- character record `+0x12B` is the certified `item_known_bits` bitset;
- physical `0x4C6C4` extracts the known bit corresponding to the selected character inventory slot;
- the same path reads `0x8602` beside `0x8600`, swaps the object IDs, and propagates the corresponding known state with the moved item.

Item-description routine physical `0x4A6B4` accepts `(object_id, known_flag)`. It always emits the base object name, but only when the object has a nonzero enchantment byte at OBJECTS `+0x37` **and** `known_flag != 0` does it append the enchantment suffix/value. Numerous UI paths call this routine with exactly `(0x8600, 0x8602)`.

Thus the safe serialized identity is:

```cpp
uint8_t cursor_item_known; // zero = not known/identified; nonzero = known
```

The engine commonly treats it as boolean state; preserve the raw byte when round-tripping arbitrary saves.

## SAVE/load behavior

Save routine physical `0x418CB..0x4192E` writes, in order:

```text
4 bytes from CF4C
2 bytes from 8600
1 byte  from 8602
```

Load routine physical `0x41CB0..0x41CEC` restores `CF4C`, reads the object ID and known byte to temporary storage, then calls `0x55268(object_id, known)` so runtime graphics/UI state are reconstructed consistently.

## Compatibility requirements

A faithful native implementation should:

- preserve the complete 32-bit 16.16 time-of-day value;
- normalize/display its upper word in the same 24-hour domain rather than treating the field as an arbitrary dword;
- preserve `-1` as the empty cursor-item sentinel;
- move `cursor_item_known` with `cursor_item_object_id`, just as character `item_known_bits` move with inventory objects;
- rebuild any native cursor/item presentation after loading instead of assuming serialized graphics state exists in SAVE.DAT.
