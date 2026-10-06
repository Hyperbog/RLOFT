# SAVE.DAT

Status: **source captured; certified CODE.2 serializer/deserializer is available and the persistent global event-flag vector is now directly identified.**

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

## Reconstruction goal

Map the complete serializer/deserializer field-by-field against CODE.2, preserve legacy/deferred record transformations exactly, identify embedded party/world state, and implement byte-exact parse/write verification against this sample. The 32-byte global event flag vector is now one certified anchor for that work.
