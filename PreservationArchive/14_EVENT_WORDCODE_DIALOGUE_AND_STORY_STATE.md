# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 14 — Event Scripts, Interaction Wordcode, Dialogue, and Story-State Dispatch

**Status:** PRIMARY SCRIPT/DIALOGUE RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation preservation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

Ravenloft does not have one universal script format.

This chapter documents the distinct data-driven/story-control layers now reconstructed:

- the fixed 50-slot per-level event VM embedded in `LEVELS.DAT`;
- the active 38-program interaction/display wordcode bank in `RES9`;
- NPC type → active wordcode program mapping;
- four-choice interaction state and conditional dialogue;
- persistent global flags versus wordcode-local flags;
- nested wordcode execution and recurring timers;
- item possession, grant, removal, and world-spawn operations;
- party-character template application;
- live NPC suppression;
- resource/text/sound sub-base selection;
- interaction graphics, music, sound, and cinematics;
- the separate numbered interactive-resource processor reached from LEVELS opcode 11;
- hard-coded story hooks including Strahd, Vengeful Spirit, and Irmgarde;
- indexed text access through the XOR-encoded `T` archive;
- legacy `RES12` wordcode lineage;
- the transition from inline 141-word character templates to centralized CODE.2 template IDs;
- persistence boundaries into SAVE/global event state;
- faithful-port VM, parser, and modding requirements.

Primary sources: [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md), [WORDCODE_PROGRAM_INDEX.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_PROGRAM_INDEX.md), and [LEVELS event scripts](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md).

## 2. Two active script systems must remain separate

The major active script layers are:

```text
LEVELS.DAT event table
    level-local fixed command slots
    13 opcodes
    world/cell/audio/global-state hooks

RES9 interaction wordcode
    NPC/story interaction programs
    42 opcodes
    choices/dialogue/timers/items/graphics/audio/cinematics
```

They communicate through shared engine services such as persistent global flags, family-A NPC state, family-C loose items, party inventory, audio, T text, and cinematics.

But they have separate file formats, interpreters, instruction pointers, and control models.

## 3. LEVELS event VM recap

Every level contains:

```text
50 slots * 26 bytes = 0x514 bytes
```

Each slot is an active-count byte plus five fixed five-byte commands.

Each command is:

```text
uint8 opcode
uint8 arg1
uint8 arg2
uint8 arg3
uint8 arg4
```

[LEVELS event scripts](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md)

## 4. LEVELS event opcode ownership

The behavioral map is:

```text
0   cell-record action A
1   cell-record action B
2   toggle cell-record state
3   spawn class-3 transient/moving effect
4   replace six-bit cell code
5   queue sound-effect request
6   transition music sequence
7   show modal indexed message
8   set persistent global flag
9   clear persistent global flag
10  toggle persistent global flag
11  run hard-coded story hook or numbered interactive resource
12  assign global state code
```

[LEVELS event scripts](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md)

The LEVELS VM is principally a compact world/event command layer. It is not the full conversation/interaction language.

## 5. RES9 wordcode is the active interaction/display VM

The active wordcode programs are raw non-`EH` resources:

```text
RES9:517..554
38 programs
program IDs 0..37
```

Each resource has:

```text
uint16 word_count
uint16 program_words[word_count]
uint16 companion_bank[word_count]
```

Every supplied companion-bank word is zero. [Non-EH resources](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/NON_EH.md)

The certified CODE.2 loader consumes only the first word bank.

## 6. Active wordcode archive addressing is INIT-derived

CODE.2 raw loader `13F70` computes:

```text
INIT +0x1E = 103
INIT +0x22 = 414

entry =
    103
  + 414
  + program_id

entry =
    517 + program_id
```

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

Thus the 38 program IDs are runtime-proved rather than inferred from archive adjacency.

## 7. The serialized companion bank is ignored by the supplied executable

The active raw loader reads `word_count` and exactly `word_count * 2` bytes into the VM program buffer, then stops.

A whole-object raw-access audit found no second consumer for the trailing bank. [Non-EH resources](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/NON_EH.md)

Therefore:

```text
program_words
    runtime-active

companion_bank
    serialized but runtime-unconsumed
    historical producer purpose UNKNOWN
```

A repacker must retain the second bank despite its inactive status.

## 8. Wordcode uses 16-bit word-addressed instructions

Interpreter `15534` fetches a little-endian 16-bit opcode, advances word IP `0x7FEC`, and dispatches opcodes `0x00..0x29` through a 42-entry table.

All branch, call, choice, and timer-handler targets are **word indexes**, not byte offsets. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

## 9. Complete active opcode domain

```text
00  load choice text IDs
01  load choice target IPs
02  conditional modal dialog
03  set one choice target
04  set one choice text ID
05  apply/add party-character template
06  grant OBJECTS item
07  nested-wordcode return
08  branch if party possesses item / special coin predicate
09  branch if persistent global flag set
0A  set persistent global flag
0B  clear persistent global flag
0C  toggle persistent global flag
0D  branch if persistent global flag clear
0E  return to four-choice UI
0F  configure recurring wordcode timer
10  draw archive-9 resource
11  display/UI refresh operation
12  execute nested wordcode at target IP
13  append choice pair
14  remove choice by text ID
15  request interaction-loop exit
16  suppress matching family-A NPC type
17  set archive-9 resource sub-base
18  absolute jump
19  set indexed-text base
1A  prepend choice pair
1B  terminate program
1C  play direct sound effect
1D  transition/start music sequence
1E  set wordcode-local state bit
1F  clear wordcode-local state bit
20  toggle wordcode-local state bit
21  branch if wordcode-local bit set
22  branch if wordcode-local bit clear
23  randomized archive-9 resource draw
24  play cinematic and terminate active run
25  managed resource load/draw/unload variant
26  alternate resource draw/blit variant
27  set voice/sound sub-base
28  recursively remove OBJECTS item
29  spawn loose family-C item at party position
```

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

## 10. Grammar is almost entirely fixed-arity

Except for opcodes `00`, `01`, and `02`, active operands are fixed-count 16-bit words.

One parser trap is opcode `05`: its immediate dispatcher handler does not visibly fetch the operand, but the called helper consumes the one-word template ID.

A disassembler that infers arity only from direct handler reads will lose instruction synchronization.

## 11. Interpreter return codes have distinct meanings

```text
0  continue normal execution
1  nested-wordcode return
2  transfer to choice UI
3  terminate active program
```

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

These are not equivalent generic stop results.

## 12. Nested execution behaves like a wordcode subroutine

Opcode `12 target_ip` saves the caller IP, switches to the target, and executes until a nonzero return.

Opcode `07` returns code 1, which restores the caller IP and resumes the caller in nested context.

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

## 13. Opcode 18 is a true absolute jump

`18 target_ip` writes the target directly to the word instruction pointer.

Every direct target in all 38 supplied programs has been validated to land on a decoded instruction boundary. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

## 14. The interaction choice table has exactly four slots

Runtime choice state is:

```text
choice_text_id[4]
choice_target_ip[4]

0xFFFF = empty
```

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

This is the script-side origin of the four-choice interaction interface.

## 15. Choice instructions mutate that four-slot table

```text
00  replace/load text-ID list
01  replace/load target-IP list
03  set one target slot
04  set one text slot
13  append pair to first empty slot
14  remove by text ID and compact
1A  prepend pair, shifting existing first three
0E  return to/present choice UI
```

Opcodes 3 and 4 are executable-supported but absent from the active supplied bank.

## 16. Opcode 0E hands control to the outer interaction UI

Opcode `0E` returns VM code 2.

The outer runner then presents the four choices and resumes execution from the selected choice's target IP. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

## 17. Conditional dialog opcode 02 is variable length

Grammar:

```text
02
voice_or_sound_selector
condition_0, text_id_0
condition_1, text_id_1
...
0x0021, default_text_id
```

Condition `0x21` terminates the list and still carries a default text ID.

The active corpus uses condition values `0`, `1`, `2`, `7`, and default `0x21`. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

## 18. Conditional dialog is party-member-sensitive

Helper `1430C` evaluates the condition words against the current party member and selects the first matching text ID.

The selected text is displayed modally and may have associated voice/sound data. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

The original source-level labels for condition values 0/1/2/7 remain unresolved.

## 19. Wordcode maintains a mutable indexed-text base

Opcode `19 value` sets runtime text base `0x7FF4`.

Subsequent interaction text operands are local IDs relative to that base, not necessarily absolute T indexes.

## 20. T is the principal indexed text archive

The active `T` resource contains:

```text
1,073 indexed records
payload bytes XORed with 0xDC
25 legitimate zero-length records
```

[Resource archive chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/03_RESOURCE_ARCHIVES_CACHE_AND_RESOLUTION.md)

The direct chain is:

```text
NPC type
    -> wordcode program
    -> opcode-19 text base
    -> local text ID
    -> T indexed record
```

The preservation archive records indexes and behavior rather than reproducing copyrighted dialogue text.


## 21. Wordcode has two independent flag namespaces

### Persistent global flags

Opcodes:

```text
09  branch if set
0A  set
0B  clear
0C  toggle
0D  branch if clear
```

operate the 256-bit vector at runtime `0x22200`, serialized by SAVE.DAT.

### Wordcode-local flags

Opcodes:

```text
1E  set
1F  clear
20  toggle
21  branch if set
22  branch if clear
```

operate a separate local dword at runtime `0x7FD0`.

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

A port must not merge these namespaces.

## 22. Persistent global flags connect multiple scripting systems

The same SAVE-backed flag vector is consumed by:

- LEVELS event opcodes 8/9/10;
- RES9 wordcode opcodes 09/0A/0B/0C/0D;
- hard-coded story routines;
- helper `1C1DC`.

[LEVELS event callees](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_CALLEES.md)

This makes the vector the principal cross-level/cross-script persistent story-state store.

## 23. Wordcode-local flags are interaction-lifetime state

The local dword at `0x7FD0` is manipulated only by the interaction VM family.

These bits belong to the currently running interaction and must not be serialized into the 256-bit SAVE vector merely because both use bit operations.

## 24. Recurring wordcode timers are VM callbacks

Opcode:

```text
0F slot, handler_ip, interval
```

configures one of five 12-byte timer records.

Each stores:

- handler word IP;
- interval;
- next absolute deadline.

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

The active corpus uses slots `0..3`; slot 4 exists in CODE.2 but is unused.

## 25. Timer execution temporarily replaces the main IP

When due, the timer poller:

1. saves the main VM instruction pointer;
2. changes IP to the configured handler;
3. executes until a nonzero VM result;
4. restores the saved main IP;
5. advances/reschedules the deadline by the interval.

This is callback/subroutine behavior inside one interaction state, not a separately serialized script thread.

## 26. Timer disable sentinel is part of the VM ABI

```text
handler_ip == 0xFFFF
```

disables/normalizes the timer slot.

A host implementation may use an optional internally, but debugger/export state should retain the original sentinel semantics.

## 27. Inventory predicates recurse through containers

Opcode:

```text
08 object_id,target_ip
```

searches party inventory recursively, including container contents.

When found:

```text
IP = target_ip
```

otherwise execution falls through. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

This reuses the same nested container model documented in Component 10.

## 28. Operand 0xFFFF is a story-specific Barovian Coin predicate

For opcode 08:

```text
object_id = 0xFFFF
```

does not mean an empty or wildcard item.

It invokes a special recursive count of:

```text
OBJECTS 0x75
BAROVIAN COIN
```

and branches only when the count is exactly:

```text
15
```

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

This hard-coded sentinel must survive semantic lifting.

## 29. Item grant can fall back to the world

Opcode:

```text
06 object_id
```

attempts to add the object to party inventory.

Its helper contains object-specific handling and can fall back to creating a loose family-C item at the party position if ordinary insertion cannot complete. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

A script-level `grant_item` operation is therefore not guaranteed to end with the item inside a character slot.

## 30. Item removal is recursive

Opcode:

```text
28 object_id
```

uses the paired recursive inventory/container removal helper.

Thus scripts can remove items stored inside nested containers rather than only top-level character slots.

## 31. Opcode 29 creates ordinary family-C world state

```text
29 object_id
```

spawns a loose family-C item at the current party position.

The resulting item is a normal mutable world item and is later persisted through LEV state. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

## 32. Active opcode 05 applies a centralized party-character template

```text
05 template_id
```

selects one of CODE.2's ten static `0x248` templates and installs it through the party helper chain.

Template IDs:

```text
0 GLORIANNA
1 VELIKA
2 MISCHA
3 VUKO
4 TATIAN
5 IRMGARDE
6 ANSHELM
7 RYSZARD
8 VLADISLAV
9 FHALKEN
```

[RES12 wordcode](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES12_WORDCODE.md)

## 33. Template application becomes persistent character state

The party-template installation path produces a complete active `0x248` character image and marks recruited/template provenance bit:

```text
status 0x0080
```

That character then participates in:

- active party state;
- SAVE serialization;
- roster/provenance logic;
- story/finale predicates.

The script VM therefore crosses directly into long-lived party state.

## 34. Opcode 16 suppresses matching live NPC actors

```text
16 npc_type
```

scans live family-A records and sets a suppression bit on matching actors.

First-person family-A scene emission tests that bit as a skip gate. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

The operation is safely:

```text
suppress/hide matching NPC type
```

and does not mutate `NPCS.DAT`.

## 35. NPC suppression has encoded special operands

CODE.2 includes additional handling for opcode-16 operands:

```text
0xFFFF
0x0029
```

including a coupled type operation.

The exact cases are part of executable behavior and should remain explicit rather than normalized into a simple “for each actor with type == operand” loop.

## 36. Interaction programs have three mutable local resource bases

```text
17 value  -> archive-9 typed-resource sub-base 0x7FF0
19 value  -> indexed-text base                 0x7FF4
27 value  -> voice/sound sub-base              0x7FF8
```

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

Local IDs in later instructions are interpreted relative to these bases.

## 37. Archive-9 graphics use INIT interaction base 103

For ordinary interaction graphics:

```text
absolute RES9 typed resource =
    INIT interaction base 103
  + current wordcode resource sub-base
  + instruction resource delta
```

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

This namespace is separate from the raw program bank at RES9 entries 517..554.

## 38. Draw opcode 10 is the dominant interaction visual operation

```text
10 resource_delta, x, y
```

selects the archive-9 resource and renders it at the encoded coordinates.

The active bank contains:

```text
1,044
```

instances, making it the most frequent opcode in the shipped wordcode corpus. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

## 39. Randomized draw opcode 23 selects a resource variant, not a branch

```text
23 resource_delta, random_span, x, y
```

applies the executable's random selection within the encoded resource span before drawing.

It is a visual resource-selection operation rather than randomized control flow.

## 40. Draw variants 25 and 26 have separate renderer paths

Both consume:

```text
resource_delta, x, y
```

but use different graphics helper sequences.

Opcode 25 performs an explicit managed load/draw/release sequence.

Opcode 26 uses a separate temporary rectangle/display-state path. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

Their behavior is known, but the preservation archive does not invent original compositor-mode names.

## 41. Opcode 11 is a display/UI refresh operation

Opcode `11` has no operands and is heavily used between interaction draws.

It invokes a display/UI refresh/reset helper path.

Because the exact original source-level label is unavailable, the behavioral name remains conservative.

## 42. Wordcode can directly control interaction music

Opcode:

```text
1D music_id
```

enters the music-sequence subsystem.

The active bank uses this opcode 36 times. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

This is interaction-local music control layered over the default level-music architecture documented in Component 05.

## 43. Direct sound opcode exists even though the active bank does not use it

Opcode:

```text
1C sound_id
```

adds the INIT sound-effect base and enters the direct SFX playback path.

The active 38 programs contain zero opcode-1C instances, but the executable handler is complete and therefore remains part of the VM contract.

## 44. Cinematic opcode 24 terminates the active run

```text
24 cinematic_id
```

invokes the ANM cinematic subsystem and establishes the VM termination condition.

Only two active programs use it:

```text
program 16 / BURGOMASTER     cinematic 6
program 32 / VISTANI INSIDE  cinematic 1
```

[WORDCODE program index](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_PROGRAM_INDEX.md)

## 45. Interaction-loop exit is separate from VM termination

Opcode `15` sets runtime exit-request byte `0x8100`.

It is not the same mechanism as interpreter return code 3.

The outer interaction loop checks this byte at the relevant execution phase. [WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

## 46. NPC type maps directly to wordcode program ID

For an accepted live NPC interaction:

```text
if npc_type == 82:
    program_id = 37
else:
    program_id = npc_type - 34
```

Therefore:

```text
NPC types 34..70 -> programs 0..36
NPC type 82      -> program 37
```

[WORDCODE program index](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_PROGRAM_INDEX.md)

## 47. All 38 active programs have executable-proved NPC identities

Representative mappings:

```text
0   MAN IN WOODS
4   TATYANA
9   GHOST
16  BURGOMASTER
23  MAGIC MIRROR WHOLE
27  STRAHD DISGUISED
28  FINAL STRAHD
37  ADVENTURER
```

The complete program/NPC/resource/text/sound table is preserved in [WORDCODE_PROGRAM_INDEX.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_PROGRAM_INDEX.md).

## 48. Shared assets do not imply shared story identity

Alternate NPC/program pairs deliberately reuse setup bases:

```text
VISTANI / VISTANI INSIDE
ELVEN MAGE / ELVEN MAGE IN BAR
STRAHDS PRISONER / STRAHDS PRIS OUT
DANDY WOMAN / DANDY WOMAN IN BAR
MAN IN WOODS / MAN IN WOOD IN BAR
```

They retain separate:

- NPC type IDs;
- program IDs;
- live actor state;
- script content/state.

A port/editor should not deduplicate them because their resources match.

## 49. Active wordcode corpus is structurally closed

Across RES9 programs 0..37:

```text
programs                         38
program words                11,396
decoded instructions          4,237
parse errors                      0
invalid direct control targets    0
timer slots used                0..3
executable timer slots           0..4
```

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

Every validated:

- conditional branch;
- absolute jump;
- nested call;
- enabled timer handler;
- choice target list;
- dynamically inserted choice target;

lands at a decoded instruction start.

## 50. Five supported opcodes are unused by the active shipped bank

```text
03  set one choice target
04  set one choice text
0B  clear persistent global flag
0C  toggle persistent global flag
1C  direct sound-effect playback
```

[WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

These are latent executable-supported capabilities and belong in a faithful VM even though no supplied active program exercises them.

