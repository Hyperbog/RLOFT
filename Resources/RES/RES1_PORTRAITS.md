# RES1 character portrait banks

Status: **CONFIRMED for the supplied Ravenloft build.** RES1 entries `336..427` are two 46-slot character-face graphic banks selected from the serialized character identity fields and status word. The first bank contains normal portraits; character status bit `0x0100` selects the second, vengeful-spirit-possessed portrait bank. The two formerly opaque raw `0x00000060` entries occupy the terminal invalid/reserved face slots in those banks.

## Executable index formula

INIT supplies:

```text
INIT +0x04 = 336 -> DGROUP 0x258E4
```

CODE.2 portrait paths at `FF80..FFD0`, `16437..16469`, `1C7FA..1C829`, and `1F581..1F5EE` all use the same calculation:

```text
local_face = int8(character[+0x12])
           + uint8(character[+0x13]) * 23

res1_id = 336 + local_face

if (character.status_word & 0x0100)
    res1_id += 46
```

The resource is then drawn through archive 1.

## Character identity fields

The adjacent serialized character bytes are now cross-proved:

```text
+0x12  face_index
+0x13  sex
+0x14  race
```

`+0x13` is directly linked to game text by CODE.2 `104B2..104D3`:

```text
T[0x1B + sex]
```

with the supplied T records:

```text
T[0x1B] = MALE
T[0x1C] = FEMALE
```

`+0x14` is likewise read through:

```text
T[0x0F + race]
```

where records `0x0F..0x14` are the six supplied race names. `+0x12` is the face selector used in the portrait index formula and another bit-indexed character-UI path.

This document records the resource-side proof without changing the concurrently maintained SAVE.DAT schema files.

## Exact bank geometry

The archive has exactly 92 slots from the face base through EOF:

```text
336..381  normal portrait bank
382..427  vengeful-spirit-possessed portrait bank
```

Each bank reserves 23 face indexes for each of two sex values:

```text
local  0..22 = sex 0 / MALE,   face 0..22
local 23..45 = sex 1 / FEMALE, face 0..22
```

The supplied resources populate:

```text
MALE   face 0..22  -> 23 typed type-1 resources
FEMALE face 0..20  -> 21 typed type-1 resources
FEMALE face 21     -> zero-length archive entry
FEMALE face 22     -> raw 4-byte 0x00000060 record
```

Thus:

```text
normal bank
  336..358  male faces 0..22
  359..379  female faces 0..20
  380       female face slot 21: zero-length
  381       female face slot 22: 60 00 00 00

possessed bank
  382..404  male faces 0..22
  405..425  female faces 0..20
  426       female face slot 21: zero-length
  427       female face slot 22: 60 00 00 00
```

No supplied SAVE character uses female face index 21 or 22. The active typed resource loader could not turn the corresponding slots into valid graphics: the zero-length slot aliases the following raw marker bytes, and the raw marker itself lacks the required `EH` envelope.

A malformed/custom character record could still mathematically calculate those indexes. Therefore the preservation claim is **invalid/reserved slots in the supplied content/runtime contract**, not a claim that arbitrary memory corruption could never request them.

## Player-selectable subset and predefined-NPC slots

CODE.1 closes the distinction between the 23-slot runtime bank capacity and the portrait choices offered during character creation.

The CODE.1 party image begins at runtime `0x67F8`, with the same `0x248` record geometry later transferred through `CHARS`. Therefore:

```text
CODE.1 character +0x12 = runtime 0x680A + character_index*0x248 = face_index
CODE.1 character +0x13 = runtime 0x680B + character_index*0x248 = sex_code
```

The portrait-selection routine at physical `0x2F188..0x2F515` operates as follows:

- records the current character's sex;
- displays six portrait choices per page;
- uses page bases `0`, `6`, and `12`;
- wraps the page base by `(base + 6) mod 18`;
- computes each displayed resource as `RES1_face_base + sex*23 + page_base + slot`;
- explicitly disables `page_base + slot == 17`;
- also disables a face already used by the other original player-created character when both characters have the same sex;
- when a choice is accepted, stores `face_index = page_base + selected_slot`.

Consequently the shipped character-creation UI exposes exactly:

```text
face_index 0..16  player-selectable
face_index 17     deliberately displayed as disabled on the final page
face_index 18..22 never reached by the creation-page cycle
```

The 17-choice boundary is executable behavior, not inferred from resource counts.

### Roster/NPC portrait assignment

The supplied SAVE roster independently explains the remaining valid bank slots:

```text
MALE predefined roster slots
face 17  FHALKEN
face 18  RYSZARD
face 19  ANSHELM
face 20  VUKO
face 21  MISCHA
face 22  VLADISLAV

FEMALE predefined roster slots
face 17  GLORIANNA
face 18  TATIAN
face 19  VELIKA
face 20  IRMGARDE
face 21  invalid/reserved
face 22  invalid/reserved
```

This yields the exact normal-bank partition:

```text
336..352  male player-selectable faces 0..16
353..358  male predefined-roster faces 17..22

359..375  female player-selectable faces 0..16
376..379  female predefined-roster faces 17..20
380       female reserved slot 21: zero length
381       female reserved slot 22: raw 0x00000060
```

and the possessed bank is the same partition shifted by 46:

```text
382..398  possessed male player faces 0..16
399..404  possessed male predefined-roster faces 17..22
405..421  possessed female player faces 0..16
422..425  possessed female predefined-roster faces 17..20
426       possessed female reserved slot 21
427       possessed female reserved slot 22
```

The saved-roster records themselves do not carry the recruited/template provenance bit while stored in the roster pool; active-party installation is the separate path that sets `status 0x0080`. The portrait-slot classification above is therefore based on the roster record identities and CODE.1 creation exclusion boundary, not on that active-party status bit.

## Portrait resource homogeneity

Every populated portrait slot is a type-1 resource with the same 8-byte descriptor:

```text
count        80
height        1
width_units   6
field_5      FF
compression   3
field_7       1
```

The common decoded size is:

```text
80 * 1 * 6 * 8 = 3,840 bytes
```

The existing type-1 LZSS validator exhausts each stream exactly.

## Status bit `0x0100`: vengeful-spirit possession

The gameplay meaning is now independently certified from CODE.2 state flow plus the reconstructed interaction/text system.

The fixed story routine `000B8..001F9` selects an eligible party member, runs wordcode program 9 (`GHOST`), and sets:

```text
character.status_word |= 0x0100
```

The same story path later clears `0x0100` from the party after the GHOUL LORD completion flag is set. NPC death handling sets that flag when NPC type `0x12` / decimal 18 (`GHOUL LORD`) is defeated; program-9 text independently identifies Victor Grymig as the ghoul lord whose destruction frees the possessed body.

The second 46-slot bank is therefore safely the **vengeful-spirit-possessed portrait bank**, not merely a generic alternate-state bank.

See `DatFiles/SAVE.DAT/STATUS_POSSESSION.md` for the complete setter/completion/clearer evidence chain.

## Relationship to the two raw RES1 markers

The two raw records are therefore no longer positionally mysterious:

```text
RES1:381 = terminal female-face slot in the normal bank
RES1:427 = terminal female-face slot in the possessed bank
```

Their placement is resolved. Their literal producer-side value `0x00000060` remains unexplained and must be preserved exactly.

See:

- `RES1_SENTINELS.md` for loader reachability and preservation evidence;
- `Resources/MISC/INIT/UI_RESOURCE_BASES.md` for the INIT base fields;
- `DatFiles/SAVE.DAT/STATUS_POSSESSION.md` for status bit `0x0100`;
- `DatFiles/SAVE.DAT/CHARACTER_RECORD.md` for the serialized 0x248 character record.
