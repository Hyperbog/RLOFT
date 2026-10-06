# SAVE.DAT

Status: **source captured; certified CODE.2 serializer/deserializer is available; persistent global event flags and charged-item counter state are now directly identified.**

- Size: 9,288 bytes
- SHA-256: `f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d`
- Begins with the NUL-terminated description `QUICK START PARTY; ELVEN F/M - DWARVEN F/C` followed by binary state.

## Proven cross-link: persistent event flags

CODE.2 event routines `1C124`, `1C160`, `1C1A0`, and `1C1DC` respectively set, clear, toggle, and test bits in the dword vector beginning at runtime DGROUP `0x22200`.

The SAVE serializer at `067FE..06811` writes exactly:

```text
32 bytes from DGROUP 0x22200
```

and loader `06CBA..06CC7` reads exactly the same 32 bytes back.

This is therefore a persistent **256-bit global game/event-state flag vector**. LEVELS.DAT event opcodes 8, 9, and 10 manipulate this same saved state directly.

The exact file offset of this field within the complete SAVE.DAT layout will be assigned when the preceding serializer fields are mapped in order; the executable-side width and runtime identity are already proved.

## Proven cross-link: persistent charged-item counters

OBJECTS.DAT record byte `+0x31` selects mutable state outside the object-definition table. For the proved charged-device family—ROD OF SMITING and the wand definitions—CODE.2 uses it as an index into the byte vector beginning at runtime DGROUP `0x67EC`.

The item-use paths:

```text
test selected counter for zero
decrement on successful use
invoke depletion/removal behavior when the counter reaches zero
```

The vector is exactly **15 bytes**. In the supplied certified CODE.2 data image all 15 initial values are `12` (`0x0C`).

SAVE serialization writes exactly:

```text
15 bytes from DGROUP 0x67EC
```

and the loader restores exactly the same 15 bytes.

The supplied quick-start `SAVE.DAT` contains a unique run of fifteen `0x0C` bytes at:

```text
file +0x0D59 .. +0x0D67
```

This is therefore a strong sample-file anchor for the persisted charge vector while the complete preceding SAVE layout is reconstructed.

The architectural division is:

```text
OBJECTS.DAT  static item definition -> charge_counter_index
SAVE.DAT     mutable game state      -> current charge_counter[15]
```

The supplied OBJECTS database addresses charge slots 0..11; slots 12..14 remain serialized runtime capacity but are not selected by the supplied object definitions.

## Reconstruction goal

Map the complete serializer/deserializer field-by-field against CODE.2, preserve legacy/deferred record transformations exactly, identify embedded party/world state, and implement byte-exact parse/write verification against this sample.

Current certified SAVE anchors include:

- 32-byte persistent global event-flag vector at runtime `0x22200`;
- 15-byte persistent charged-item vector at runtime `0x67EC`, sample file offset `0x0D59`.

These anchors must be integrated into the eventual sequential file-offset map rather than treated as isolated byte-pattern guesses.