# LEVELS.DAT fixed 0x514 event-script table

Status: **slot/command structure and CODE.2 interpreter are instruction-proven; the major sound, music, text, persistent-flag, and numbered-event callees are now semantically closed.**

The fixed `0x514` bytes between compact/runtime world records and the common tail are loaded into a heap buffer whose pointer is stored at DGROUP `0xCF44`. CODE.2 `002A8..006B8` is the active interpreter for this buffer.

## Exact physical layout

`0x514 == 50 * 0x1A`, and the interpreter computes:

```text
slot = event_table + script_index * 26
```

Each of the 50 slots is exactly:

```text
+0x00  uint8 active_command_count   // 0..5
+0x01  command[0]                   // 5 bytes
+0x06  command[1]
+0x0B  command[2]
+0x10  command[3]
+0x15  command[4]
```

Each command is physically five bytes:

```text
+0  uint8 opcode
+1  uint8 arg1
+2  uint8 arg2
+3  uint8 arg3
+4  uint8 arg4
```

The interpreter executes only the first `active_command_count` commands. Bytes in inactive command positions are not required to be zero: archive level 20 slot 1 has count 4 but retains a nonzero fifth command. Lossless tools must preserve all 26 bytes rather than zero-filling the inactive tail.

The interpreter returns immediately when `script_index == -1`. Callers pass signed bytes from 10-byte cell records as script indices, and several engine paths invoke fixed slots directly (including 31..33 and 45..49). This establishes the block as a level-local event/script table rather than generic padding.

## Archive-wide validation

Across all 32 supplied `LEVELS.DAT` records:

```text
physical slots               1,600
active slots                   394
active commands                 555
maximum active count              5
count > 5                        0
active opcode > 12               0
```

Observed active opcode counts:

```text
opcode 0   67
opcode 1  111
opcode 2  188
opcode 3   42
opcode 4   44
opcode 5   20
opcode 6    2
opcode 7    0
opcode 8    4
opcode 9    0
opcode 10   0
opcode 11   6
opcode 12  71
```

Opcodes 7, 9 and 10 are implemented by CODE.2 but are absent from the supplied archive.

## Interpreter dispatcher

The 13-entry CS jump table at `0x274` maps opcodes `0..12` to:

```text
0 -> 0x306
1 -> 0x378
2 -> 0x3D2
3 -> 0x48D
4 -> 0x454
5 -> 0x540
6 -> 0x559
7 -> 0x5B3
8 -> 0x57D
9 -> 0x58F
10 -> 0x5A1
11 -> 0x4E6
12 -> 0x68F
```

Opcode values above 12 skip the command body and continue to the next active command.

## Proven opcode behavior

### Opcode 0 — cell-record action A

Uses `arg1=row`, `arg2=column` and calls the 10-byte record lookup. If no record exists it calls the address-distinct routine at logical `0x0010` with the same cell coordinates. If a kind-1 dynamic record exists and its position byte `+6` is nonzero, it temporarily enables interaction bit `0x04`, calls `20C0C`, and restores that bit from prior state. A non-kind-1 record has bit `0x04` enabled.

### Opcode 1 — cell-record action B

Uses the same row/column lookup. For a kind-1 record, the `20C0C` interaction path is taken only when position `+6 == 0`; the interaction-enable bit is again temporarily modified/restored. A non-kind-1 record has bit `0x04` cleared.

### Opcode 2 — toggle cell-record state

Uses `arg1=row`, `arg2=column`. Kind 1 routes through the dynamic interaction routine. Kind 6 derives bit `0x04` from the inverted high control bit. Other record kinds toggle the low-two-bit state with `state ^= 1` while preserving the other control bits.

### Opcode 3 — spawn transient/moving effect record

`arg1=row` and `arg2=column` are in bounds for all 42 supplied instances. `arg3` is always 0 or 1 in the archive. `arg4` is converted to an orientation in 45-degree steps through the encoded modulo-8 arithmetic, then the command calls `08A80`, which creates a 22-byte transient/moving world record and invokes its sound/effect path. Exact higher-level editor terminology remains open.

### Opcode 4 — replace the six-bit cell code

This is exact:

```text
index = arg1 * map_width + arg2
cell[index].low_byte &= 0xC0
cell[index].word |= (arg3 & 0x3F)
```

Thus `arg1=row`, `arg2=column`, `arg3=new_cell_code`. All 44 supplied coordinates are valid. `arg4` is zero in every supplied instance.

### Opcode 5 — queue a sound-effect request

Interpreter `00540..00554` passes the unaligned dword beginning at command byte `+3` to `28110` as the request ID and supplies:

```text
id       = uint32(command bytes starting at +3)
aux      = 0xFF
channel  = 0
priority = 0
```

`28110` indexes a 12-byte pending-request table by channel, applies the encoded priority comparison, and stores the request for later consumption by `28168` / `27F24`; it is not merely an immediate generic resource call. The options UI separately routes its sound toggle to this `0x280xx` subsystem, while music uses `0x290xx`.

All 20 supplied opcode-5 commands occur in level 5 and produce effective request ID `0x000000B8`. The exact unaligned dword access must be preserved.

### Opcode 6 — transition to another music sequence

This path is now closed as the music/sequence subsystem:

```text
28DCC(0x9C4)       // timed transition/stop request for current sequence
28EF0()            // poll/wait and finalize current sequence
28C54(new_id, 1)   // load/select and start requested sequence
```

The requested ID is the unaligned dword beginning at command byte `+3`. Current music-sequence ID is stored at `0xEF28` and returned by `29050`; `290A8` is the UI music-enable control.

The two supplied opcode-6 commands are both in level 29 and request sequences `0x2F` and `0x2E` respectively.

### Opcode 7 — modal indexed-text display

Opcode 7 is executable-supported but absent from the supplied archive. The interpreter temporarily changes UI/display mode, creates a text rectangle, calls `1C368(buffer_759C,arg3)`, renders the resulting string, waits for the encoded input press/release sequence, and restores the UI.

`1C368` proves the indexed encoded-text lookup:

```text
seek(file, 4 + index*4)
read uint32 text_offset
seek(file, text_offset)
read uint16 text_length
read text_length bytes
append NUL
XOR every payload byte with 0xDC
```

Thus opcode 7 is safely exposed as **show indexed message/text**, with `arg3` selecting the entry. The original source/editor label for the text resource remains unresolved.

### Opcodes 8, 9, 10 — persistent global event flags

These are exact bit operations on the vector beginning at DGROUP `0x22200`:

```text
8  -> 1C124(arg3) -> SET bit arg3
9  -> 1C160(arg3) -> CLEAR bit arg3
10 -> 1C1A0(arg3) -> TOGGLE bit arg3
```

Companion `1C1DC(arg3)` tests the same bit.

SAVE serialization proves persistence: writer `067FE..06811` writes exactly `0x20` bytes from `0x22200`, and loader `06CBA..06CC7` reads the same `0x20` bytes back. This is therefore a persistent **256-bit global game/event-state flag vector**, not transient renderer state.

Only opcode 8 appears in the supplied archive, setting flags `0x52`, `0x55`, `0x56`, and `0x57`.

### Opcode 11 — special or numbered interactive event

Reads `arg3`:

```text
0x0B -> call 0x0074
0x0C -> call 0x00B8
0x0D -> call 0x01FC; if its result is nonzero and a cell record exists, clear that record's bit 0x04
default -> call 13EF8(arg3), then stop the current script after this command
```

The default callee is now materially identified. `13EF8` enters a numbered interactive-resource processor. Setup routine `13AD8` calls `139AC(number)`, which constructs:

```text
<resource path loaded from INFO.DAT> + "\\" + two decimal digits
```

CODE.2 `2E768..2E7EE` reads the supplied INFO.DAT as 13 configuration bytes, a 16-bit path length of 2, then path bytes `D:`. Thus numbered event 7 resolves to `D:\\07` for this installation image.

After opening the numbered resource, `13EF8` initializes its state, runs the interactive processor at `13834`, applies/cleans up the result through `13C88`, and returns the result. The surrounding binary contains encounter/dialog/adventure terminology; the conservative format name remains **numbered interactive event resource** rather than an asserted original source-level function name.

All six supplied opcode-11 commands use arg3 values `2, 7, 9, 11, 12, 13`. Values `2`, `7`, and `9` take the numbered-resource default branch; `11`, `12`, and `13` take the three reserved special branches.

### Opcode 12 — assign global state code

Writes command byte `+3` directly to dword global `0xBF44`:

```text
BF44 = arg3
```

All 71 supplied opcode-12 commands have `arg1=arg2=arg4=0`.

## Relationship to 10-byte cell records

Multiple interaction paths pass signed bytes from 10-byte cell records directly to this interpreter. In particular, proved kind-4 and other interaction paths use record bytes `+2` or `+3` as script selectors. The fixed table therefore supplies the command sequences executed by cell-local interactions and by several engine-driven timed/special events.

## Cross-DAT links

The callee reconstruction proves two direct relationships beyond LEVELS.DAT:

- INFO.DAT supplies the installation/resource path prefix used by opcode-11 numbered interactive resources.
- SAVE.DAT persists the 32-byte global event-flag vector manipulated by opcodes 8/9/10.

See `EVENT_CALLEES.md` and `analyze_event_callees.py` for the detailed callee trace and corpus verification.

## Safe implementation boundary

A faithful parser/repacker may expose the block as `50 event slots`, each with `active_count` plus five raw five-byte commands. Current safe semantic opcode names include:

```text
5  queue_sound_effect
6  transition_music_sequence
7  show_indexed_message
8  set_global_event_flag
9  clear_global_event_flag
10 toggle_global_event_flag
11 run_special_or_numbered_event
```

Raw command bytes must still be retained exactly. In particular, do not normalize the unaligned dword reads used by opcodes 5 and 6, and preserve executable-supported commands that simply do not occur in the supplied archive.
