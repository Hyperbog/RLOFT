# SAVE character status `0x0100` — vengeful-spirit possession

Status: **CONFIRMED for the supplied Ravenloft build.** Character `status_word` bit `0x0100` is the persistent possession state applied by the GHOST / Vengeful Spirit story event. It selects the second RES1 character-portrait bank while active and is cleared when the GHOUL LORD / Victor Grymig resolution path completes. The co-cleared `0x0040` bit is now behaviorally tied to the recurring Vengeful-Spirit warning/coercion path rather than left wholly anonymous.

See `STATUS_WORD.md` for the consolidated status map.

## Serialized field

Each `0x248`-byte character record stores:

```text
+0x010  uint16 status_word
```

At runtime character `n` begins at `0x218E0 + n*0x248`, so the status word is at `0x218F0 + n*0x248`.

```text
0x0100  persistent Vengeful-Spirit possession
0x0040  possession-associated warning/coercion presentation/action-blocking state
```

Both are serialized as part of the complete character image.

## Possession setter — CODE.2 `000B8..001F9`

The fixed story routine at logical `000B8` is the only explicit direct setter of status bit `0x0100` found in the certified CODE.2 object.

On the unresolved/not-yet-completed branch it:

1. tests persistent event flag `0x3C` and returns if the possession event is already active;
2. selects an eligible active party character;
3. invokes interaction/display wordcode program **9** through `16524(9)`;
4. marks the selected party slot for UI refresh;
5. executes `or byte [character + 0x11],01h`, i.e. `status_word |= 0x0100`;
6. sets persistent event flag `0x3C`.

Program 9 is independently mapped to NPC type 43 / `GHOST` by the reconstructed RES9 interaction index.

## Story-text cross-check

Program 9 uses T text base 343. The supplied raw T records describe the approaching spirit entering a party member's body, identify it as the vengeful former jeweler Strobel, demand destruction of Victor Grymig / the Ghoul Lord, threaten the occupied body, and promise release after the target is destroyed.

This narrative evidence corroborates the executable setter/clearer lifecycle; it is not being used to invent the bit assignment.

## Recurring warning/coercion companion state `0x0040`

Physical routine `0x5275C` scans all four party records. For a non-empty character it requires `0x0100` possession before entering the warning logic.

The routine first clears low-byte status bit `0x40`:

```text
physical 0x52792
status_word &= ~0x0040
```

After event-flag/current-level gating, it calls the engine LCG `rand()` routine at physical `0x6BBC6`. That routine returns `0..32767`; the warning code computes:

```text
(rand * 4) >> 15
```

which yields exactly `0..3`, then selects:

```text
T index = 0x15F + value = T[351..354]
```

The supplied raw records are all explicit Vengeful-Spirit coercion messages. They warn the party to fulfill the bargain, destroy the Ghoul Lord, return to the cemetery, or risk destruction of the possessed body.

The message is displayed with the affected character portrait through physical `0x576A0`, after which physical `0x528AC` executes:

```text
status_word |= 0x0040
```

`0x0040` is also consumed by character-presentation state selection and by aggregate action/eligibility masks. It is therefore not merely an unused story marker. The safe name is **Vengeful-Spirit warning/coercion presentation/action-blocking state**; the original source identifier and exact lifetime between maintenance calls remain unknown.

Importantly, this is **not** the game's Held status. The character information screen maps `0x0200` directly to raw `T[654] = HELD`.

## Grymig / GHOUL LORD completion flag

The NPC damage/death routine at logical `E284..E553` handles a record whose type is stored at live record word `+0x0E`.

Once HP at record `+0x12` falls to zero or below, it reloads the NPC type and contains:

```text
E3B5  cmp eax,12h
E3B8  jne ...
E3BA  push 20h
E3BC  call 1C124       ; set persistent event flag
```

The reconstructed `NPCS.DAT` table identifies type `0x12` / decimal 18 as `GHOUL LORD`. Thus defeating the Ghoul Lord sets persistent event flag `0x20`.

The GHOST text identifies Victor Grymig as that ghoul lord, independently tying the narrative target to this executable branch.

## Possession clearer

At the start of logical `000B8`, persistent flag `0x20` selects the resolution branch. That path:

1. invokes wordcode program 9 again;
2. sets persistent flag `0x3D`;
3. clears persistent flag `0x3C`;
4. walks all four active-party slots and applies:

```text
status_word &= 0xFEBF
```

`~0xFEBF & 0xFFFF == 0x0140`, so this clears both:

```text
0x0100  persistent Vengeful-Spirit possession
0x0040  its warning/coercion companion state
```

A level-transition path around logical `18CD1..18CF2` also checks persistent flag `0x20`, invokes the possession-resolution routine, then clears flag `0x20`.

## RES1 portrait-bank effect

INIT `+0x04` supplies RES1 character-face base 336. Portrait callers compute:

```text
local_face = face_index + sex_code * 23
res1_id    = 336 + local_face

if status_word & 0x0100:
    res1_id += 46
```

Therefore the two 46-slot banks are behaviorally identified as:

```text
RES1 336..381  normal character portraits
RES1 382..427  Vengeful-Spirit-possessed portraits
```

The second bank contains transformed variants, but the semantic identity comes from executable state flow plus resource/story evidence rather than appearance alone.

The invalid/reserved terminal slots remain:

```text
RES1:380 / 426  zero-length
RES1:381 / 427  raw 60 00 00 00
```

See `Resources/RES/RES1_PORTRAITS.md` for full portrait-bank geometry.

## Safe implementation boundary

A faithful runtime may expose:

```c
bool vengeful_spirit_possessed = (status_word & 0x0100u) != 0;
bool vengeful_spirit_warning_state = (status_word & 0x0040u) != 0;
```

while preserving the raw 16-bit word and original compound-mask behavior. Do not merge either state with Held (`0x0200`), death, poison, disease, paralysis, or empty-slot state.