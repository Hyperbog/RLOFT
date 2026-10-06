# LEVELS.DAT event-opcode callee reconstruction

Status: **the remaining high-value event opcodes are now behaviorally resolved from certified CODE.2; persistent flag, sound-effect, music-sequence, indexed-text, and numbered interactive-resource paths are separated.**

The fixed event interpreter is CODE.2 `002A8..006B8`. This document closes the callees previously left address-named in `EVENT_SCRIPTS.md`.

## Opcode 5 — queue sound-effect request

Interpreter `00540..00554` performs:

```text
28110(
    id       = uint32(command bytes starting at +3),
    aux      = 0xFF,
    channel  = 0,
    priority = 0
)
```

The dword load is intentionally unaligned: it begins at physical command byte `+3` and extends two bytes beyond the five-byte command if necessary. The supplied archive is safe because all 20 opcode-5 instances place zero bytes after the encoded low word, producing effective ID `0x000000B8`.

Routine `28110` is not an immediate generic resource loader. It indexes a pending-request table in 12-byte entries by `channel`, compares stored priority, and records `{id, aux, priority}` for later consumption by `28168` and the `27F24` playback path.

The options UI independently distinguishes this `0x280xx` subsystem from the adjacent music subsystem: the sound toggle calls `28098`, whereas the music toggle calls `290A8`. `28110` is therefore safely described as the **sound-effect request queue**.

Archive evidence:

```text
opcode-5 commands             20
levels containing opcode 5     1 (level 5)
effective queued ID        0xB8 in all 20
channel                         0
priority                        0
aux                           0xFF
```

## Opcode 6 — transition to another music sequence

Interpreter `00559..00578` performs:

```text
28DCC(0x9C4)       // begin timed transition/stop on current sequence
28EF0()            // poll/wait and finalize current sequence
28C54(new_id, 1)   // load/select and start requested sequence
```

The requested ID is the same unaligned dword beginning at command byte `+3`.

The `0x28C54..0x290C2` family is the game's music/sequence subsystem:

- current sequence ID is stored at `0xEF28` and returned by `29050`;
- current active sequence slot/state is returned by `29048`;
- `28C54` loads a new sequence resource when the requested ID differs and starts it through `288FC`;
- `28DCC` applies the supplied duration to the active sequence through the low-level sequence driver;
- `28EF0` polls the active sequence until the transition completes, then tears it down;
- UI music enable/disable is controlled by `290A8`.

The supplied archive contains two opcode-6 commands, both in level 29:

```text
slot 23 -> sequence 0x2F
slot 28 -> sequence 0x2E
```

These IDs are 46 and 47, just above the highest default level-music ID observed in the fixed headers (`45`).

## Header `+0x29C` is the default level music-sequence ID

All direct CODE.2 references to header byte `0x258B4` (`header +0x29C`) route through the music/sequence family:

- `1831A` compares it to `29050()` (current sequence ID) and starts a transition if it differs;
- `18C13..18C4C` waits/finalizes and starts `28E58(header_music, 1, 0x9C4)` when needed;
- `0E4B9` restores the header value after a temporary interaction sequence;
- options path `06681` starts the header value when music is enabled.

The safe semantic name is therefore **`level_music_sequence_id`**, replacing the earlier generic `resource_set_id` label.

Across the 32 supplied levels the observed IDs are:

```text
0,1,2,3,4,5,6,7,8,9,10,11,12,13,
34,35,36,37,38,39,40,41,42,45
```

## Opcode 7 — modal indexed-text display

Opcode 7 is executable-supported but absent from the supplied archive.

Interpreter `005B3..0068D` temporarily changes UI/display mode, creates a text rectangle, calls:

```text
1C368(buffer_759C, command_arg3)
```

then renders the decoded string and waits for the encoded press/release input cycle before restoring the UI.

Routine `1C368` proves the indexed-text file format used by this path:

```text
seek(file, 4 + index*4)
read uint32 text_offset
seek(file, text_offset)
read uint16 text_length
read text_length bytes
append NUL
for each byte: byte ^= 0xDC
```

Thus opcode 7 is a **modal indexed-text/message display**, with `arg3` selecting the message. The exact original resource filename/editor label remains unresolved.

## Opcodes 8/9/10 — persistent global event flags

These three opcodes are exact bit operations on the same dword array beginning at DGROUP `0x22200`:

```text
opcode 8  -> 1C124(arg3) -> SET bit arg3
opcode 9  -> 1C160(arg3) -> CLEAR bit arg3
opcode 10 -> 1C1A0(arg3) -> TOGGLE bit arg3
```

Companion routine `1C1DC(arg)` tests the same bit.

The mapping is:

```text
dword_index = signed_trunc(arg / 32)
bit_index   = signed_remainder(arg, 32)
mask        = 1 << bit_index
```

For ordinary event IDs `0..255`, this is a 256-bit flag vector.

Crucially, SAVE serialization proves persistence: CODE.2 writer `067FE..06811` writes exactly `0x20` bytes from `0x22200`, and loader `06CBA..06CC7` reads exactly `0x20` bytes back. These are therefore **persistent global game/event-state flags**, not transient rendering flags.

Only opcode 8 occurs in the supplied LEVELS archive:

```text
set flag 0x52
set flag 0x55
set flag 0x56
set flag 0x57
```

Opcodes 9 and 10 remain valid executable-supported commands even though the supplied archive does not use them.

## Opcode 11 default branch — numbered interactive event resource

Opcode 11 reserves `arg3 = 0x0B`, `0x0C`, and `0x0D` for three address-distinct story/special routines. Any other value executes:

```text
13EF8(arg3)
stop_current_event_slot = true
```

Routine `13EF8` enters the game's numbered interactive-resource processor. Its setup path `13AD8` calls `139AC(number)`, which constructs a pathname from the installation/resource path loaded from INFO.DAT:

```text
<INFO.DAT resource path> + "\\" + two decimal digits
```

For the supplied INFO.DAT, CODE.2 `2E768..2E7EE` reads:

```text
13 configuration bytes : "0000000001111"
uint16 path_length      : 2
path bytes              : "D:"
```

so event 7, for example, resolves to `D:\\07`.

`13EF8` then:

1. initializes the numbered resource through `13AD8`;
2. runs the substantial interactive processor at `13834`;
3. applies/cleans up the resulting transition through `13C88`;
4. returns the interaction result.

The surrounding binary contains encounter/dialog/adventure terminology, but the format documentation deliberately uses the conservative name **numbered interactive event resource** rather than claiming an original source-level function name.

The supplied opcode-11 `arg3` values are:

```text
0x02, 0x07, 0x09, 0x0B, 0x0C, 0x0D
```

Thus three archive commands use the numbered-resource default path (`02`, `07`, `09`) and three use the reserved special branches (`0B`, `0C`, `0D`).

## Cross-DAT consequences

This callee pass creates two direct format links outside LEVELS.DAT:

- `INFO.DAT` contains the installation/resource path prefix used to open numbered interactive resources.
- `SAVE.DAT` contains the persistent 32-byte / 256-bit global event-flag vector at runtime `0x22200`.

Those facts should be preserved when reconstructing those formats in their own workspaces.

## Safe implementation boundary

A semantic event parser may now expose:

```text
5  queue_sound_effect
6  transition_music_sequence
7  show_indexed_message
8  set_global_event_flag
9  clear_global_event_flag
10 toggle_global_event_flag
11 run_special_or_numbered_event
```

The physical command bytes must still be retained exactly, especially the unaligned dword behavior of opcodes 5 and 6 and executable-supported opcodes absent from the supplied archive.
