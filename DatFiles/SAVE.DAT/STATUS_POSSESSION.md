# SAVE character status `0x0100` — vengeful-spirit possession

Status: **CONFIRMED for the supplied Ravenloft build.** Character `status_word` bit `0x0100` is the persistent possession state applied by the GHOST / vengeful-spirit story event. It selects the second RES1 character-portrait bank while active and is cleared when the GHOUL LORD / Victor Grymig resolution path completes.

## Serialized field

Each 0x248-byte character record stores:

```text
+0x010  uint16 status_word
```

At runtime character `n` begins at `0x218E0 + n*0x248`, so the status word is at `0x218F0 + n*0x248`.

The bit documented here is:

```text
0x0100  vengeful-spirit possession
```

This state is serialized because the complete 0x248-byte party records are written to and restored from SAVE.DAT.

## Possession setter — CODE.2 `000B8..001F9`

The fixed story routine at `000B8` is the only explicit direct setter of status bit `0x0100` found in the certified CODE.2 object.

On the unresolved/not-yet-completed branch it:

1. tests persistent event flag `0x3C` and returns if the possession event is already active;
2. selects an eligible active party character;
3. invokes interaction/display wordcode program **9** through `16524(9)`;
4. marks the selected party slot for UI refresh;
5. executes:

```asm
001E7  or byte [character + 0x11], 01h
```

which is exactly:

```c
character.status_word |= 0x0100;
```

6. sets persistent event flag `0x3C`.

Program 9 is independently mapped to NPC type 43 / `GHOST` by the reconstructed RES9 interaction index.

## Story-text cross-check

Program 9 uses T text base 343. The corresponding supplied text records describe:

- an approaching spirit entering/using a party member's body;
- the spirit identifying itself as the vengeful former jeweler Strobel;
- its demand that Victor Grymig, the ghoul lord, be destroyed;
- repeated warnings that it controls or can destroy the occupied body;
- release of the party member after the ghoul lord has been destroyed.

This text is not being used to invent the bit assignment: it independently explains the executable-proved setter/clearer lifecycle and the program-9 identity.

## Grymig / GHOUL LORD completion flag

The NPC damage/death routine at `E284..E553` handles a record whose type is stored at live record word `+0x0E`.

Once HP at record `+0x12` falls to zero or below, it reloads the NPC type and contains:

```asm
E3B5  cmp eax, 12h
E3B8  jne ...
E3BA  push 20h
E3BC  call 1C124       ; set persistent event flag
```

The reconstructed `NPCS.DAT` table identifies type `0x12` / decimal 18 as:

```text
GHOUL LORD
```

Thus defeating the GHOUL LORD sets persistent event flag `0x20`.

The GHOST text identifies Victor Grymig as that ghoul lord, so this executable branch and the narrative identity converge on the same story target.

## Possession clearer

At the start of `000B8`, persistent flag `0x20` selects the resolution branch. That path:

1. invokes wordcode program 9 again;
2. sets persistent flag `0x3D`;
3. clears persistent flag `0x3C`;
4. walks all four active-party character slots and applies:

```text
status_word &= 0xFEBF
```

`~0xFEBF & 0xFFFF == 0x0140`, so this clears:

```text
0x0100  vengeful-spirit possession
0x0040  another co-cleared status bit; semantic name remains separate/unresolved here
```

The same story routine therefore both establishes and later removes `0x0100`.

A level-transition path around `18CD1..18CF2` also checks persistent flag `0x20`, calls `000B8`, then clears flag `0x20`; this is the executable handoff that causes the completed GHOUL LORD event to run the possession-release logic.

## RES1 portrait-bank effect

INIT `+0x04` supplies RES1 character-face base 336. All portrait callers compute:

```text
local_face = face_index + sex_code * 23
res1_id   = 336 + local_face

if status_word & 0x0100:
    res1_id += 46
```

Therefore the two 46-slot banks are now behaviorally identified as:

```text
RES1 336..381  normal character portraits
RES1 382..427  vengeful-spirit-possessed portraits
```

The second bank contains visibly transformed variants, but the semantic name above comes from executable state flow plus program/text evidence, not from visual appearance.

The invalid/reserved terminal slots remain:

```text
RES1:380 / 426  zero-length
RES1:381 / 427  raw 60 00 00 00
```

See `Resources/RES/RES1_PORTRAITS.md` for the complete portrait-bank geometry.

## Safe implementation boundary

A faithful SAVE/runtime model may expose:

```c
bool vengeful_spirit_possessed = (status_word & 0x0100u) != 0;
```

while preserving the raw 16-bit status word.

Do not merge this state with death, unconsciousness, poison, disease, paralysis, or the co-cleared `0x0040` bit. Those are separate encoded conditions.
