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



## 51. RES12 preserves an earlier generation of the same interaction VM

The supplied CD also contains:

```text
RES12:370..406
37 raw wordcode programs
```

with the same outer shape:

```text
uint16 word_count
uint16 program_words[word_count]
uint16 companion_bank[word_count]
```

[RES12 wordcode](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES12_WORDCODE.md)

RES12 is historically related but not loaded by the certified CODE.1/CODE.2 pair.

## 52. RES12 is not an incompatible opcode dialect

Earlier parsing with the active grammar appeared to produce invalid opcodes above `0x29`.

Those values are actually operand data inside legacy opcode `0x05`.

Recovered rule:

```text
active RES9 opcode 05
    05 template_id
    one operand word

legacy RES12 opcode 05
    05 inline_character_template[141]
    141 operand words
```

Every other recovered opcode grammar remains compatible. [RES12 wordcode](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES12_WORDCODE.md)

## 53. The recovered legacy grammar parses all 37 programs cleanly

With opcode `0x05` assigned 141 operand words:

```text
programs parsed                   37 / 37
program words                      4,820
decoded instructions               1,079
invalid direct control targets         0
timer slots used                      0,1
opcode-05 occurrences                 16
```

[RES12 wordcode](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES12_WORDCODE.md)

This is strong evidence of format evolution rather than corruption.

## 54. Legacy opcode 05 stores a compact character template inline

The 141 words encode:

```text
0..15      name words
16         portrait
17         sex
18         race
19         age
20..22     class codes
23         alignment
24         base AC
25..26     current/max HP
27..29     class levels
30..32     class progression
33         Strength
34         exceptional Strength
35..39     Dex/Con/Int/Wis/Cha
40..60     item[21]
61..140    two spell matrices
```

[RES12 wordcode](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES12_WORDCODE.md)

It is a compact predecessor structure, not a verbatim `0x248` runtime character record.

## 55. Legacy templates deterministically expand into the later record prefix

The 141-word payload expands into the final character-record layout through:

```text
+0x109
```

covering:

- name/identity;
- HP and class progression;
- duplicated base/current abilities;
- 21 item slots;
- both 80-byte spell matrices.

Later runtime/effect state beginning at `+0x10A` is absent from the legacy payload. [RES11/RES12 lineage](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES11_RES12_LINEAGE.md)

## 56. The final executable centralizes those templates

Active RES9 replaces the inline 141-word character data with one static template ID.

CODE.2 stores ten static `0x248` templates:

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

[RES11/RES12 lineage](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES11_RES12_LINEAGE.md)

The format evolution is therefore:

```text
legacy script
    opcode 05 + 141-word inline template

final script
    opcode 05 + template ID
        |
        v
centralized CODE.2 0x248 template table
```

## 57. Legacy and final templates preserve real content revisions

FHALKEN and MISCHA expand byte-identically through the mapped 266-byte prefix.

Other characters differ in bounded fields.

This proves the later generation is not only a structural repack; some content was revised between producer generations. [RES11/RES12 lineage](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES11_RES12_LINEAGE.md)

## 58. Same-ordinal text identity survives 37/37 programs

For legacy program IDs `0..36`, the initial text-base operand matches the corresponding active program in:

```text
37 / 37
```

cases.

[RES12 wordcode](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES12_WORDCODE.md)

This is strong continuity evidence between the two script generations.

## 59. Graphics namespace moved while preserving large exact content runs

Exact archive alignment reconstructs:

```text
legacy interaction graphics base   51
final RES9 interaction base       103
```

Using those bases, 23/37 same-ordinal programs resolve their configured base graphic to byte-identical resources. [RES11/RES12 lineage](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES11_RES12_LINEAGE.md)

The old base is an alignment reconstruction, not a recovered old INIT constant.

## 60. IRMGARDE records a concrete script migration

Legacy program 14 embeds IRMGARDE inline.

In the active generation:

- program 14 no longer applies template 5;
- new program 37 applies template ID 5 twice;
- programs 14 and 37 share the same major interaction graphics/text/sound identity.

[RES12 wordcode](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES12_WORDCODE.md)

This preserves a specific story/content restructuring between generations.

## 61. Certified executables do not load RES12 wordcode

CODE.2 raw wordcode loader requests archive 9 only.

CODE.1 also initializes only 11 indexed RES groups.

Therefore:

```text
RES9 interaction wordcode
    active in supplied executable

RES12 interaction wordcode
    historical/pre-split predecessor generation
```

[RES11/RES12 lineage](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES11_RES12_LINEAGE.md)

The exact older executable/editor that consumed RES12 remains unknown.

## 62. LEVELS opcode 11 bridges the level VM into story/interaction systems

LEVELS event opcode 11 reads `arg3`.

Three values are reserved for hard-coded story routines:

```text
0x0B  final-Strahd story activation
0x0C  Vengeful-Spirit possession lifecycle
0x0D  Irmgarde-dependent fixed-cell trigger
```

All other values enter the separate numbered interactive-resource processor. [LEVELS event callees](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_CALLEES.md)

## 63. Final-Strahd event invokes wordcode program 28

Reserved value:

```text
arg3 = 0x0B
```

calls a routine that:

1. invokes RES9 program 28;
2. program 28 is independently mapped to `FINAL STRAHD`;
3. sets persistent global flag `0x3A`;
4. scans live family-A actors;
5. finds NPC type 8 / STRAHD;
6. clears its runtime suppression bit;
7. refreshes world/scene state.

[LEVELS event callees](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_CALLEES.md)

This is a direct LEVELS→RES9→live-NPC story transition.

## 64. Vengeful-Spirit event invokes the GHOST interaction program

Reserved value:

```text
arg3 = 0x0C
```

calls story routine `000B8`.

That routine uses:

```text
RES9 program 9
NPC identity GHOST
```

and persistent flags:

```text
0x20
0x3C
0x3D
```

to control onset/resolution. [Possession status](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/STATUS_POSSESSION.md)

## 65. Possession becomes persistent character state

On onset the story routine selects an eligible active party character and installs:

```text
status 0x0100
    Vengeful-Spirit possession

persistent flag 0x3C
    possession event active
```

[Possession status](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/STATUS_POSSESSION.md)

This state survives through the ordinary `0x248` character SAVE image.

## 66. Possession also drives recurring coercion state

A later maintenance routine requires `0x0100`, chooses one of four T messages, displays it with the affected portrait, and sets:

```text
status 0x0040
```

the Vengeful-Spirit warning/coercion presentation/action-blocking state.

[Possession status](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/STATUS_POSSESSION.md)

Thus a story script can produce long-lived gameplay/UI restrictions through character status.

## 67. Ghoul Lord death closes the possession story loop

NPC type:

```text
0x12 / 18
GHOUL LORD
```

sets persistent event flag:

```text
0x20
```

on death.

The next possession-resolution path:

- re-invokes program 9;
- sets flag `0x3D`;
- clears active-possession flag `0x3C`;
- clears character status bits `0x0100 | 0x0040`.

[Possession status](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/STATUS_POSSESSION.md)

This is a complete cross-subsystem chain:

```text
LEVELS event
 -> RES9 interaction
 -> character status
 -> NPC death
 -> global flag
 -> RES9 resolution
 -> character status clear
```

## 68. Irmgarde event is hard-coded story logic, not a generic door command

Reserved value:

```text
arg3 = 0x0D
```

requires a recruited/template party member named:

```text
IRMGARDE
```

and rejects her if:

```text
paralyzed 0x1000
dead      0x4000
```

[LEVELS event callees](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_CALLEES.md)

On success the routine clears the six-bit code of fixed cell:

```text
row 18
column 54
```

sets global event flag `0x46`, writes state code `0x33`, and returns success.

This must remain a named hard-coded story hook rather than being generalized into an ordinary “open cell” opcode.

## 69. Opcode-11 default path runs numbered interactive resources

Any `arg3` other than `0x0B..0x0D` calls:

```text
13EF8(arg3)
```

and stops the current LEVELS event slot afterward. [LEVELS event callees](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_CALLEES.md)

This is a **third interaction layer**, separate from both:

- LEVELS fixed command slots;
- RES9 wordcode.

## 70. Numbered resources are opened through INFO.DAT's shared path

Setup calls `139AC(number)`, which constructs:

```text
resource_path + "\" + two decimal digits
```

The supplied INFO.DAT stores:

```text
resource_path = "D:"
```

so numbered interaction 7 resolves to:

```text
D:\07
```

[INFO.DAT README](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/INFO.DAT/README.md)

## 71. The numbered-resource processor is kept distinct because its inner format is not yet equivalently closed

The wrapper path is proved:

```text
13EF8(number)
    |
    +-- 13AD8 initialize/open
    |
    +-- 13834 interactive processor
    |
    +-- 13C88 apply/cleanup result
    |
    v
interaction result
```

[LEVELS event callees](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_CALLEES.md)

The surrounding executable establishes its interactive/story role, but the preservation archive deliberately does not label its internal resource grammar as RES9 wordcode without equivalent format evidence.

## 72. Supplied LEVELS opcode-11 uses six values

```text
0x02  numbered resource
0x07  numbered resource
0x09  numbered resource
0x0B  final-Strahd special
0x0C  Vengeful-Spirit special
0x0D  Irmgarde special
```

[LEVELS event scripts](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md)

This demonstrates that opcode 11 is a dispatcher across both data-driven external interactions and hard-coded story logic.

## 73. LEVELS modal text and RES9 dialog both use indexed text mechanisms

LEVELS opcode 7 calls the indexed XOR-decoding reader directly.

RES9 wordcode establishes a text base and uses local text IDs for modal/choice interactions.

Both ultimately depend on indexed encoded text resources, but they use different script-level addressing conventions.

Do not force both into one script instruction format merely because their presentation is text.

## 74. Persistent story state belongs to SAVE, not the script files

Script resources define behavior and transitions.

The resulting long-lived state is stored in:

- SAVE global event flags;
- active character records/status;
- roster/party state;
- LEV world state;
- inventory/containers;
- live world actors.

The scripts themselves are immutable resources.

This separation permits the same interaction program to behave differently on later visits by querying persistent flags and current world/party state.

## 75. Script-driven world changes pass through normal subsystem state

Examples:

```text
grant/remove item
    -> normal inventory/container/world-item system

suppress NPC
    -> live family-A runtime state

spawn loose item
    -> family-C world state

set global flag
    -> SAVE-persistent flag vector

apply character template
    -> active party 0x248 record

clear cell code
    -> live level cell word

play cinematic
    -> ANM subsystem
```

The VM does not maintain shadow copies of these systems.

