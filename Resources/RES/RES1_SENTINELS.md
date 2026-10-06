# RES1 raw four-byte portrait-slot markers

Status: **runtime placement resolved; literal producer-side marker meaning still unknown.** `RES1:381` and `RES1:427` occupy the final reserved/invalid character-face slot in each of the two RES1 portrait banks. They are not consumable typed graphics in the supplied CODE.2 runtime.

## Physical records

```text
RES1:380  zero-length
RES1:381  60 00 00 00
...
RES1:426  zero-length
RES1:427  60 00 00 00
```

`RES1:427` is the final archive entry.

The remaining non-empty entries in the surrounding portrait ranges are ordinary type-1 graphics.

## Portrait-bank placement

INIT `+0x04` supplies character-face base 336. CODE.2 computes:

```text
local = face_index + sex * 23
id    = 336 + local

if (status_word & 0x0100)
    id += 46
```

`sex` is directly proved by the indexed T reader (`0 -> MALE`, `1 -> FEMALE`). The archive therefore divides as:

```text
RES1 336..381   normal 46-slot face bank
RES1 382..427   status-0x0100 46-slot face bank
```

Within both banks:

```text
sex 0 / MALE    face 0..22 -> all 23 slots contain valid type-1 graphics
sex 1 / FEMALE  face 0..20 -> 21 valid type-1 graphics
sex 1 / FEMALE  face 21    -> zero-length slot
sex 1 / FEMALE  face 22    -> raw 60 00 00 00 slot
```

Thus entries 381 and 427 are the same terminal local face slot in the two parallel banks. Their immediately preceding empty entries are the preceding local face slot.

See `RES1_PORTRAITS.md` for the complete executable/resource proof.

## Why the normal loader cannot consume the raw slots

CODE.2 `2EB60` is the common typed-entry reader. It requires the first word to equal `"EH"` before dispatching types 1..7.

The raw marker entries are only four bytes and begin `60 00`, not `45 48`, so attempting to load either through the normal path enters the resource-error path rather than returning a graphic.

The preceding zero-length entries do not create valid aliases: because their duplicate offsets point directly at the following raw bytes, a typed read from those slots encounters the same invalid marker rather than an `EH` envelope.

## Raw archive access audit

Helper `2EC74(archive)` exposes raw archive handles. A direct call-target scan of the complete certified CODE.2 object finds exactly one call:

```text
13F76: call 2EC74
```

That caller is the active interaction-wordcode loader and hardcodes archive 9. There is no direct archive-1 raw-handle request.

Therefore established CODE.2 paths provide no alternate interpretation of the marker bytes.

## Preservation boundary

The following is now proved:

```text
serialized location = terminal invalid/reserved female face slot in each portrait bank
normal typed consumer = none; invalid EH envelope
raw archive-1 consumer = none in certified CODE.2
serialized value = 0x00000060
```

The following remains unknown:

```text
why the producer chose literal 0x00000060
whether another DreamForge editor/build gave that value a special meaning
```

Do not delete, normalize, convert, or replace these records. Round-trip tools must preserve the preceding duplicate offsets and the four raw bytes exactly.
