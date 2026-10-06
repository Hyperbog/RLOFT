# LEVELS.DAT fixed 0x514 event-script table

Status: **slot/command structure and CODE.2 interpreter are instruction-proven; opcode names remain behavioral where callees are not yet semantically closed.**

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

### Opcode 5 — play resource/sound through `28110`

Calls `28110` with selector `0xFF`, zeroed mode arguments, and an unaligned dword loaded beginning at command byte `+3` (`[esi+4]` relative to the slot cursor). All 20 supplied opcode-5 commands are final active commands and encode the effective value `0x000000B8`; following inactive bytes are zero in those slots. The exact unaligned load must be preserved.

### Opcode 6 — resource/audio transition path

Calls `28DCC(0x9C4)`, `28EF0()`, then `28C54(value,1)` where `value` is the same unaligned dword beginning at command byte `+3`. The two supplied instances encode effective values `0x2F` and `0x2E`. Stronger naming awaits closure of those callees.

### Opcode 7 — UI/interaction sequence

Executes the substantial path beginning at `005B3`, including temporary mode toggles, input polling, drawing, and a call using command byte `+3`. No supplied archive command uses opcode 7, so no data-driven editor name is assigned.

### Opcodes 8, 9, 10 — one-byte dispatches

Each passes command byte `+3` to a distinct routine:

```text
8  -> 1C124(arg3)
9  -> 1C160(arg3)
10 -> 1C1A0(arg3)
```

Only opcode 8 appears in the supplied archive (four commands, values `0x52`, `0x55`, `0x56`, `0x57`).

### Opcode 11 — special subcommand dispatcher

Reads `arg3`:

```text
0x0B -> call 0x0074
0x0C -> call 0x00B8
0x0D -> call 0x01FC; if its result is nonzero and a cell record exists, clear that record's bit 0x04
default -> call 13EF8(arg3), then stop the current script after this command
```

All six supplied opcode-11 commands use arg3 values `2, 7, 9, 11, 12, 13`.

### Opcode 12 — assign global state code

Writes command byte `+3` directly to dword global `0xBF44`:

```text
BF44 = arg3
```

All 71 supplied opcode-12 commands have `arg1=arg2=arg4=0`.

## Relationship to 10-byte cell records

Multiple interaction paths pass signed bytes from 10-byte cell records directly to this interpreter. In particular, proved kind-4 and other interaction paths use record bytes `+2` or `+3` as script selectors. The fixed table therefore supplies the command sequences executed by cell-local interactions and by several engine-driven timed/special events.

## Safe implementation boundary

A faithful parser/repacker may expose the block as `50 event slots`, each with `active_count` plus five raw five-byte commands. It may decode opcode 4 completely and expose the other opcode behaviors above, but should retain raw command bytes and avoid replacing address-distinct calls with guessed game concepts until their callees are independently closed.
