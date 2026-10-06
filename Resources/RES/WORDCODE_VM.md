# RES9 interaction/display wordcode VM

Status: **the active `RES9:517..554` wordcode bank is now structurally decoded as a 42-opcode (`0x00..0x29`) 16-bit-word virtual machine. Instruction boundaries, operand consumption, direct control-flow targets, timer targets, choice targets, and the variable-length dialog instruction are validated across all 38 shipped programs. Major gameplay/UI/resource semantics are tied directly to CODE.2; several rendering operations retain conservative behavioral names where the exact original source-level label is unknown.**

The immutable `RES9`, `INIT`, `OBJECTS.DAT`, `LEVELS.DAT`, and original CODE.2 bytes are controlling evidence.

## Container and active bank

The raw non-`EH` container is documented in `NON_EH.md`:

```text
uint16 word_count
uint16 program_words[word_count]
uint16 zero_bank[word_count]
```

CODE.2 `13F70` loads only `program_words`. `INIT +0x1E = 103` and `INIT +0x22 = 414`, giving:

```text
active_RES9_entry = 103 + 414 + program_id
                  = 517 + program_id
```

Therefore program IDs `0..37` are exactly `RES9:517..554`.

## VM core

CODE.2 `15534` fetches a little-endian 16-bit opcode from the program-word buffer at runtime `0x13020`, advances word instruction pointer `0x7FEC`, and dispatches values `0x00..0x29` through the 42-entry table at logical `1548C`. Values above `0x29` take the encoded no-op/default return path.

All instruction pointers and branch destinations are **word indexes**, not byte offsets.

The 42-entry dispatch map is:

```text
00 155D0   01 1562C   02 15689   03 15693   04 1574D   05 156C7
06 156D1   07 15681   08 157C0   09 1583C   0A 15920   0B 15941
0C 15962   0D 15873   0E 157B8   0F 159F8   10 15A82   11 15C78
12 15CF1   13 15786   14 157A4   15 15D31   16 157AE   17 15593
18 15825   19 15575   1A 15795   1B 1553F   1C 15D3D   1D 15D7A
1E 15983   1F 159AA   20 159D1   21 158AE   22 158E8   23 15C06
24 15DCA   25 15B75   26 15ADF   27 155B1   28 1572C   29 156F2
```

## Complete instruction grammar

Except for `0x00`, `0x01`, and `0x02`, all operands are fixed-count 16-bit words.

| Op | Operand words | Safe semantic name |
|---:|---|---|
| `00` | `count`, then `count` words | load choice text IDs |
| `01` | `count`, then `count` words | load choice target IPs |
| `02` | voice selector, then `(condition,text_id)` pairs through condition `0x21` | conditional modal dialog |
| `03` | 2 | set one choice target |
| `04` | 2 | set one choice text ID |
| `05` | 1 | apply/add party-character template |
| `06` | 1 | grant OBJECTS item |
| `07` | 0 | nested-wordcode return |
| `08` | 2 | branch if party possesses item / special coin predicate |
| `09` | 2 | branch if persistent global flag set |
| `0A` | 1 | set persistent global flag |
| `0B` | 1 | clear persistent global flag |
| `0C` | 1 | toggle persistent global flag |
| `0D` | 2 | branch if persistent global flag clear |
| `0E` | 0 | return to four-choice UI |
| `0F` | 3 | configure recurring wordcode timer |
| `10` | 3 | draw archive-9 resource |
| `11` | 0 | display/UI refresh operation |
| `12` | 1 | execute nested wordcode at target IP |
| `13` | 2 | append choice `(text_id,target_ip)` |
| `14` | 1 | remove choice by text ID |
| `15` | 0 | request interaction-loop exit |
| `16` | 1 | suppress matching family-A NPC type |
| `17` | 1 | set archive-9 resource sub-base |
| `18` | 1 | absolute jump |
| `19` | 1 | set indexed-text base |
| `1A` | 2 | prepend choice `(text_id,target_ip)` |
| `1B` | 0 | terminate program |
| `1C` | 1 | play direct sound effect |
| `1D` | 1 | music-sequence transition/start |
| `1E` | 1 | set wordcode-local state bit |
| `1F` | 1 | clear wordcode-local state bit |
| `20` | 1 | toggle wordcode-local state bit |
| `21` | 2 | branch if wordcode-local bit set |
| `22` | 2 | branch if wordcode-local bit clear |
| `23` | 4 | randomized archive-9 resource draw |
| `24` | 1 | play cinematic and terminate active run |
| `25` | 3 | managed resource load/draw/unload variant |
| `26` | 3 | alternate resource draw/blit variant |
| `27` | 1 | set voice/sound index sub-base |
| `28` | 1 | recursively remove OBJECTS item |
| `29` | 1 | spawn loose family-C item at party position |

`0x05` is an important parser trap: its immediate dispatcher handler does not visibly fetch the operand, but the called helper does. It is therefore a one-operand instruction.

## Return/control model

The interpreter's nonzero returns have distinct meanings established by the higher-level runner:

```text
0  continue normal execution
1  nested-wordcode return       (opcode 07)
2  transfer to choice UI        (opcode 0E)
3  terminate active program     (opcode 1B, and cinematic termination path)
```

### Nested execution: `0x12`

`0x12 target_ip` saves the caller IP, switches to `target_ip`, repeatedly invokes the interpreter until a nonzero return, then restores the caller IP. `0x07` therefore behaves as a subroutine return in this nested context. Top-level execution does not treat return code 1 as a full program termination.

### Absolute jump: `0x18`

`0x18 target_ip` writes the target directly to the word instruction pointer.

## Choice system

The runner owns four choice slots represented by paired words:

```text
choice_text_id[4]   // runtime base 0x7FD8, 4-byte slot stride
choice_target_ip[4] // runtime base 0x7FDA, interleaved with text IDs
```

`0xFFFF` is the empty sentinel.

Choice-related opcodes are:

- `0x00`: replace/load the text-ID list;
- `0x01`: replace/load the target-IP list;
- `0x03`: set one target slot;
- `0x04`: set one text slot;
- `0x13`: append a `(text_id,target_ip)` pair to the first empty slot;
- `0x14`: remove a choice by text ID and compact the table;
- `0x1A`: prepend a pair by shifting the existing first three slots;
- `0x0E`: stop script execution with return code 2 so the outer interaction runner presents the four-choice interface and continues from the selected target IP.

The supplied programs use `0x13`, `0x14`, and `0x1A` heavily. `0x03` and `0x04` are executable-supported but absent from the active bank.

## Conditional dialog: `0x02`

`0x02` is variable length:

```text
02
voice_or_sound_selector
condition_0, text_id_0
condition_1, text_id_1
...
0x0021, default_text_id
```

Helper `1430C` evaluates condition words against the current party member and selects the first matching text ID. Condition `0x21` is the terminator/default entry and is still paired with a text ID. The chosen ID is combined with the current text base and read through the same indexed XOR-encoded text subsystem used elsewhere in CODE.2. The surrounding helper displays the result modally and can play associated voice/sound data.

The active corpus uses condition values `0`, `1`, `2`, `7`, and terminator `0x21`.

## Two distinct flag namespaces

These must remain separate in a port or editor.

### Persistent global game/event flags

Opcodes `0x09`, `0x0A`, `0x0B`, `0x0C`, and `0x0D` call the `1C124/1C160/1C1A0/1C1DC` family operating on the 256-bit vector beginning at runtime `0x22200`.

SAVE.DAT serializes this vector, so it is persistent game state.

```text
09 flag,target  branch if set
0A flag         set
0B flag         clear
0C flag         toggle
0D flag,target  branch if clear
```

`0x0B` and `0x0C` do not occur in the supplied active programs but are fully executable-supported.

### Wordcode-local state flags

Opcodes `0x1E..0x22` operate the separate wordcode state dword at runtime `0x7FD0`:

```text
1E bit          set
1F bit          clear
20 bit          toggle
21 bit,target   branch if set
22 bit,target   branch if clear
```

These are interaction-VM state and must not be silently merged with persistent global flags.

## Inventory/item operations

### `0x08`: possession predicate

For an ordinary OBJECTS ID, the handler recursively searches party inventory, including container contents. If the item is found, execution jumps to the encoded target; otherwise execution continues after the instruction.

Operand `0xFFFF` is a special encoded predicate. It recursively counts OBJECTS ID `0x75`, which the reconstructed OBJECTS database names `BAROVIAN COIN`, and succeeds only when the total is exactly 15.

### `0x28`: recursive item removal

`0x28 object_id` uses the paired recursive inventory/container removal helper. Its implementation relationship to the `0x08` possession search is direct.

### `0x06`: grant item

`0x06 object_id` attempts to add the requested object to party inventory. The helper contains object-specific handling for some IDs and can fall back to creating a family-C loose item at the party location when normal insertion cannot complete.

### `0x29`: spawn loose item

`0x29 object_id` explicitly invokes the family-C insertion path at the party's current world position.

### `0x05`: party-character template

`0x05 template_id` selects one of the executable's predefined party-character templates and copies the fixed party-character block into a live party slot through the helper chain. The active corpus uses template IDs `0..9`. The safe name is **apply/add party-character template**; a narrower source-level term such as `recruit` is not asserted for every call site.

## Timer system: `0x0F`

`0x0F` encodes:

```text
slot
handler_ip
interval
```

CODE.2 maintains five 12-byte timer records. Each contains the handler word IP, interval, and next absolute deadline. `handler_ip == 0xFFFF` disables/normalizes the slot.

The timer poller compares the current clock to each deadline. When due, it saves the main VM IP, executes the configured handler until a nonzero VM return, restores the main IP, and reschedules the deadline by the interval.

The supplied programs use timer slots `0..3`; slot 4 exists in the executable but is unused in this corpus.

## NPC suppression: `0x16`

`0x16 npc_type` scans live family-A/NPC records and sets runtime byte `+0x0B` bit `0x02` on matching active type IDs. First-person family-A scene emission tests that bit as a skip/suppression gate, so the operation is safely **suppress/hide matching NPC type** rather than an ordinary database edit.

The binary also contains encoded special handling for operands `0xFFFF` and `0x29`, including a coupled type operation. Preserve those cases exactly; do not normalize them into a generic loop.

## Resource bases and draw operations

Three per-program bases are explicitly mutable:

```text
17 value  -> archive-9 typed-resource sub-base 0x7FF0
19 value  -> indexed-text base                 0x7FF4
27 value  -> voice/sound index sub-base        0x7FF8
```

For ordinary interaction graphics, archive-9 resource selection is based on:

```text
INIT interaction base (103)
+ current resource sub-base
+ encoded resource delta
```

### `0x10`

`0x10 resource_delta,x,y` is the dominant draw instruction. It selects the archive-9 typed resource and renders it at the encoded coordinates.

### `0x23`

`0x23 resource_delta,random_span,x,y` applies the executable's random selection within the encoded resource span before drawing. This is a randomized resource variant draw, not a control-flow random branch.

### `0x25` and `0x26`

Both consume `resource_delta,x,y` and use separate graphics helper paths. `0x25` explicitly performs a managed load/draw/release sequence; `0x26` uses another temporary rectangle/display-state draw path. Their execution is understood, but historical source names and exact compositor-mode labels are not asserted.

### `0x11`

`0x11` has no operands and invokes a display/UI refresh/reset path used extensively between interaction draws. Its higher source-level name remains conservative.

## Audio and cinematics

- `0x1C sound_id`: direct SFX playback after adding the `INIT +0x1C` sound-effect base. The executable includes two encoded ID exclusions. This opcode is unused by the active 38 programs.
- `0x1D music_id`: transition/select/start through the music-sequence subsystem. One program-ID-specific special case is preserved in CODE.2.
- `0x24 cinematic_id`: invokes the ANM cinematic path and sets the VM termination condition. The active bank uses cinematic IDs 1 and 6.

## Interaction-loop exit: `0x15`

`0x15` sets runtime exit-request byte `0x8100`. It is not the same mechanism as VM return code 3. The higher-level interaction runner checks the byte and exits its loop after the relevant execution phase.

## Supplied-corpus validation

`analyze_wordcode.py` reproduces the active bank from `INIT` and validates all 38 programs.

Current supplied-corpus results:

```text
programs                         38
program words                11,396
decoded instructions          4,237
parse errors                      0
invalid direct control targets    0
timer slots used                0..3
executable timer slots           0..4
```

Opcode frequency:

```text
00:39   01:39   02:173  03:0    04:0    05:17   06:11
07:403  08:6    09:35   0A:50   0B:0    0C:0    0D:10
0E:40   0F:327  10:1044 11:476  12:56   13:161  14:61
15:39   16:28   17:38   18:91   19:38   1A:4    1B:40
1C:0    1D:36   1E:78   1F:74   20:316  21:98   22:242
23:50   24:2    25:37   26:22   27:37   28:17   29:2
```

Executable-supported but unused by the supplied active bank:

```text
03  set one choice target
04  set one choice text
0B  clear persistent global flag
0C  toggle persistent global flag
1C  direct sound-effect playback
```

Every encoded direct target checked by the validator—conditional branches, absolute jumps, nested calls, enabled timer handlers, choice target lists, and dynamic choice insertions—lands on the start of a decoded instruction in its owning program.

## Parser/port boundary

A faithful implementation should:

1. retain word-addressed instruction pointers;
2. implement `0x02` as a variable-length condition/text list terminated by condition `0x21`;
3. retain indirect operand consumption for `0x05`;
4. distinguish the three VM return codes;
5. distinguish persistent global flags from local wordcode flags;
6. model timer callbacks as temporary IP transfers with caller-IP restoration;
7. preserve all encoded special-item/NPC cases rather than simplifying them away;
8. preserve the serialized second zero bank even though the proved active loader does not read it;
9. keep conservative names for renderer operations whose behavior is known but whose original source-level labels remain unavailable.

## Remaining semantic frontier

The instruction **grammar and active-bank control flow are closed**. Remaining work is narrower:

- assign exact compositor/source-level names to draw variants `0x10/0x11/0x23/0x25/0x26` only if additional renderer evidence supports them;
- identify the intended lifecycle of the serialized zero companion bank;
- determine whether/how the ID-aligned `RES12` wordcode bank is selected;
- resolve the two four-byte `RES1` non-EH sentinels;
- correlate individual program IDs with named in-game interactions and story events where code/level callers make that identity provable.
