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


## 51. RES9 setup instructions form an interaction context

A program typically establishes three independent bases:

~~~text
0x17 resource_base
     -> runtime 0x7FF0

0x19 text_base
     -> runtime 0x7FF4

0x27 voice_sound_base
     -> runtime 0x7FF8
~~~

The selection equation for ordinary interaction graphics is:

~~~text
RES9_typed_entry = 103 + resource_base + instruction_delta
~~~

The corresponding dialogue text ID is relative to the independently mutable T base. The voice/sound base is a third namespace.

This is why a program cannot be safely reduced to a single resource ID. [Wordcode VM](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

## 52. Three wordcode return/exit mechanisms must not be conflated

The interpreter exposes:

~~~text
return 1    nested subroutine return (opcode 0x07)
return 2    choice UI yield (opcode 0x0E)
return 3    program termination (opcode 0x1B)
~~~

Separately, opcode 0x15 writes an interaction-loop exit-request flag at runtime 0x8100.

Additionally, cinematic opcode 0x24 reaches the higher-level termination condition after playback.

A faithful caller must preserve these distinctions rather than using one boolean ended flag. [Wordcode VM](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_VM.md)

## 53. T is a separate indexed text file, not embedded RES9 dialogue

Physical T file identity:

~~~text
bytes            133584
stored count       1073
active offsets     1073
first record       4572 / 0x11DC
SHA-256         a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315
~~~

The indexed reader is CODE.2 logical 1C368..1C408.

RES9 script instructions reference T by index using their text-base setup; LEVELS opcode 7 and ANM cinematic text operations also call the same indexed reader.

Sources: [T format](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/MISC/T/FORMAT.md), [wordcode program index](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_PROGRAM_INDEX.md).

## 54. Exact T record decoding

The file begins with:

~~~text
uint32 stored_count
uint32 absolute_offset[stored_count]
unreferenced carrier bytes
record payloads at absolute_offset[]
~~~

Lookup:

~~~text
seek(T, 4 + index*4)
read uint32 absolute_record_offset
seek(T, absolute_record_offset)
read uint16 encoded_length
read encoded_length payload bytes
append runtime NUL
for each payload byte:
    byte ^= 0xDC
~~~

This is not LZ or Huffman decompression. It is bytewise XOR obfuscation.

The runtime NUL is not physically stored. CODE.2 follows the offset table, not the end of the previous record. Do not parse contiguous strings by stepping from one length to the next.

## 55. Unreferenced T bytes remain historical evidence

The active offset table ends at byte 4296, but the first referenced record begins at byte 4572:

~~~text
pre-payload slack = 276 bytes
~~~

Thirty gaps after active records total another 1804 bytes of unreferenced material. Gaps vary from 4 to 1012 bytes and include fragments of unrelated previous content.

Twenty-five active records have zero encoded length. All 1073 supplied active records contain no embedded decoded NUL.

These are archive facts, not authorization to:
- erase slack during patch-only repacking;
- treat residue as hidden/active dialogue;
- collapse legitimate empty records;
- inject a runtime NUL into the file.

Source: [T archive reconstruction](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/MISC/T/FORMAT.md).

## 56. RES9 NPC dialogue bases are independently established

NPCS.DAT interaction identity, RES9 script ID and T base are three separate identifiers.

The direct caller near CODE.2 31ADC..31B20 maps NPC types 34..70 to program IDs 0..36. Special NPC type 82 maps to program 37.

Then opcode 0x19 establishes the T base.

Examples from the shipped program table:

| RES9 program | NPC type | Name | T base |
|---:|---:|---|---:|
| 0 | 34 | MAN IN WOODS | 239 |
| 4 | 38 | TATYANA | 302 |
| 9 | 43 | GHOST | 343 |
| 16 | 50 | BURGOMASTER | 420 |
| 27 | 61 | STRAHD DISGUISED | 527 |
| 28 | 62 | FINAL STRAHD | 544 |
| 37 | 82 | ADVENTURER | 396 |

The T records decode to dialogue families coherent with these identities, cross-checking the executable's numeric mapping.

Source: [program index](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/WORDCODE_PROGRAM_INDEX.md).

## 57. NPC interaction dispatch and scripted fixed calls coexist

A clicked/interacted NPC normally goes through a type eligibility predicate and:

~~~text
if npc_type == 0x52:
    program = 0x25
else:
    program = npc_type - 0x22

16524(program)
~~~

Story and level-event routines can independently call 16524 with a fixed ID (for example GHOST program 9 or FINAL STRAHD program 28).

Consequently, the same script program may be reached by:
- an NPC interaction;
- a fixed story event;
- a level-event reserved branch.

Do not infer unique NPC proximity as a precondition for every invocation.

## 58. Choice state uses four *paired* slots

The runner owns four choices, each represented by a text ID and wordcode destination:

~~~text
choice[i]:
    text_id
    target_word_ip
~~~

The active implementation stores these as interleaved words with a four-byte slot stride at runtime 0x7FD8/0x7FDA.

Sentinel 0xFFFF means an unused entry.

The VM provides list replacement (opcodes 00, 01), individual setter opcodes 03/04, append opcode 13, remove/compact opcode 14, prepend opcode 1A, and choice-yield opcode 0E.

A native UI may draw more elaborate widgets, but the strict scripting model has exactly four slots and original overflow/compaction rules.

## 59. Conditional modal dialogue is not a choice instruction

Opcode 02 selects a text record conditionally and presents it modally, optionally with voice/sound. It does not itself mean that the player selected an option from the four-choice list.

Its variable-length word grammar terminates on condition 0x21 **plus its own paired default text ID**.

This distinction matters for a decompiler because parsing opcode 02 as a conventional fixed-arity choice instruction shifts the positions of all following opcodes and invalidates branches.

## 60. Nested wordcode saves and restores caller IP

Opcode 12 receives a target word IP. The runner:

1. saves current VM IP;
2. switches to the target;
3. invokes wordcode until a nonzero interpreter return;
4. restores the caller IP.

Opcode 07 supplies the corresponding nested-return code.

A C++ translation may use structured call frames, but preserving original word-IP semantics and intermediate result handling is mandatory.

## 61. Wordcode has five recurring callback slots

Opcode 0F encodes:

~~~text
slot_index
handler_word_ip
interval
~~~

Runtime holds five 12-byte timer entries, with target IP, interval, and next absolute deadline.

When due, the poller temporarily executes the handler script and restores the main IP, then advances the deadline.

A handler IP of 0xFFFF disables/normalizes the timer slot.

Active programs use slots 0..3, but slot 4 is executable-supported capacity; do not remove it in the faithful engine.

This timer mechanism is not character effect expiration and not the LEVELS fixed 50-slot event table.

## 62. Two kinds of story flags use overlapping syntax

RES9 global flag instructions:

~~~text
09 id,target    branch if persistent bit set
0A id           set persistent bit
0B id           clear persistent bit
0C id           toggle persistent bit
0D id,target    branch if persistent bit clear
~~~

Call the same 256-bit SAVE-owned flag services used by LEVELS event opcodes 8/9/10.

RES9 VM-local instructions:

~~~text
1E bit          set local bit
1F bit          clear local bit
20 bit          toggle local bit
21 bit,target   branch if local bit set
22 bit,target   branch if local bit clear
~~~

Operate on a separate wordcode state dword at runtime 0x7FD0.

Global flags persist in SAVE.DAT. Local wordcode flags are interaction-state values. A native API must require the caller to name which flag domain is being accessed.

## 63. Wordcode item search is recursive

Opcode 08 tests party possession of an OBJECTS item ID, including items nested inside persistent container contents.

It jumps to the encoded word IP when found. Otherwise execution proceeds after the instruction.

Opcode 28 removes the item through a related recursive inventory/container operation.

These calls share the distributed item-state model documented in Component 10: definitions are in OBJECTS.DAT; actual references live in character slots, containers, cursor, and/or world records.

## 64. Special 0xFFFF item predicate counts Barovian Coins

Opcode 08 operand 0xFFFF is a **special case**, not an invalid object ID.

The underlying helper recursively counts OBJECTS ID 0x75 (BAROVIAN COIN) and succeeds only if exactly 15 are found.

A compatibility implementation must reproduce the hard-coded count and ID. It must not turn the special value into “has any item” or “always false.”

## 65. Grant and loose-item spawn have different semantics

Opcode 06 attempts to grant an object to the party. Its helper has object-specific cases and may fall back to instantiating a family-C loose world item when insertion fails.

Opcode 29 directly creates a family-C loose item at current party position.

The distinction is:

~~~text
06 item ID:
    try inventory
    -> possibly world fallback

29 item ID:
    explicitly create world item
~~~

Both preserve the original OBJECTS type ID and use the world family-C 22-byte record layout for spawned instances.

## 66. Opcode 05 uses centralized party templates in active RES9

Active opcode:

~~~text
05 template_id
~~~

The callee copies/installs one of ten predefined 0x248-character images through party/roster state helpers. Template IDs range 0..9 in the supplied active bank.

Final template identities:

~~~text
0 GLORIANNA   1 VELIKA   2 MISCHA   3 VUKO
4 TATIAN      5 IRMGARDE 6 ANSHELM 7 RYSZARD
8 VLADISLAV   9 FHALKEN
~~~

The interpreter operation is conservatively named **apply/add party-character template**. Calling every occurrence “recruit NPC” would overfit some program contexts.

This operation links interaction wordcode to the exact 0x248 character ABI shared by CODE.1, CODE.2, CHARS, and SAVE.DAT.

## 67. Opcode 16 modifies live actors, not NPCS templates

Given an NPC type operand, opcode 16 scans family-A live records, selecting matching types, then sets:

~~~text
live_actor.byte[+0x0B] |= 0x02
~~~

This bit is part of the actor runtime-flags dword and suppresses class-0 scene emission. The NPCS.DAT definition itself is not rewritten.

Two special encoded operands also exist: 0xFFFF and 0x29, including a coupled type operation. They require exact executable handling rather than a generic equality-only loop.

The FINAL STRAHD story path later clears the same actor-suppression bit for type 8.

## 68. Wordcode music, direct sound, and cinematics

- Opcode 1D transitions/selects music-sequence ID through the 0x28Cxx/0x290xx manager.
- Opcode 1C plays an SFX ID after applying the INIT base. It contains two ID exclusions, and has zero occurrences in active RES9.
- Opcode 24 invokes a cinematic ID, then terminates the active script run.

Only two shipped active programs invoke the cinematic opcode:

~~~text
program 16 BURGOMASTER      cinematic 6
program 32 VISTANI INSIDE   cinematic 1
~~~

These connect RES9 dialogue programs to the ANM decoder/timing system without implying that cinematics themselves are wordcode scripts.

## 69. Historical duplicate interaction identities are not deduplicable

Several NPC type pairs intentionally reuse graphics/text/sound setup bases while retaining separate program IDs:

~~~text
VISTANI / VISTANI INSIDE
ELVEN MAGE / ELVEN MAGE IN BAR
STRAHDS PRISONER / STRAHDS PRIS OUT
DANDY WOMAN / DANDY WOMAN IN BAR
MAN IN WOODS / MAN IN WOOD IN BAR
~~~

Other shared setups include prisoner chained/unchained and both maiden statues.

A preservation editor must retain each entry; apparent duplicate resources are a gameplay/story state distinction, not garbage.

## 70. RES9 active corpus boundaries are closed

The supplied active bank validates to:

~~~text
programs                         38
program words                11396
decoded instructions          4237
parse errors                     0
invalid direct targets           0
timer slots used              0..3
timer slots available          0..4
~~~

Opcode frequencies (hex opcodes) are recorded exhaustively in WORDCODE_VM.md. All decoded explicit branch, nested-call, timer-handler and dynamically constructed choice destinations land on instruction starts.

Five handlers are supported but unused in active RES9:

~~~text
03 choice target setter
04 choice text setter
0B global bit clear
0C global bit toggle
1C direct SFX playback
~~~

A runtime faithful to CODE.2 must retain them.

## 71. RES12 is a related older scripting generation, not the active bank

RES12 raw entries 370..406 contain 37 programs. They use the same outer form as active RES9:

~~~text
uint16 word_count
uint16 program_words[word_count]
uint16 zero_companion[word_count]
~~~

They are grammar-compatible with the 0x00..0x29 VM except for opcode 05's operand encoding.

The recovered legacy form is:

~~~text
05 inline_character_template[141 words]
~~~

rather than one final static template ID.

With this grammar:
- 37/37 programs parse;
- 4820 program words;
- 1079 instructions;
- zero invalid direct targets;
- 16 occurrences of inline template opcode 05.

Sources: [legacy wordcode](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES12_WORDCODE.md), [resource lineage](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/RES11_RES12_LINEAGE.md).

## 72. Exact legacy inline template geometry

RES12 opcode 05's 141 word payload contains:

~~~text
0..15     name characters / one low ASCII byte per word
16        portrait
17        sex
18        race
19        age
20..22    class codes
23        alignment
24        base armor class
25..26    current/max HP
27..29    class levels
30..32    class progress / XP
33        Strength
34        exceptional Strength
35..39    Dexterity/Constitution/Intelligence/Wisdom/Charisma
40..60    object IDs in item[21]
61..140   two 8x10 spell matrices (80 words)
~~~

The data deterministically maps to the later CODE.2 fixed character-record fields through +0x109. It does not encode the complete later 0x248 live character image, because post-inventory timers/effects/UI state are absent.

This is a concrete format-lineage finding, not conjecture based on visual similarity.

## 73. Historical opcode 05 data was migrated to centralized templates

The active executable stores ten static 0x248-byte party templates and the active wordcode references them by numeric template ID.

The legacy RES12 programs embedded these templates inline.

Specific migration: legacy program 14 embedded IRMGARDE. Active program 14 no longer applies a template, while active program 37 applies template ID 5 twice and shares program 14's interaction graphics/text/sound identity.

This is evidence of *content evolution*, not a second unrelated game script format.

## 74. RES12 is shipped but not selected by this CODE.2 VM loader

The direct raw-handle audit finds only one CODE.2 raw archive-handle request path:

~~~text
13F76 -> 2EC74(archive 9)
~~~

No equivalent raw archive-12 request is observed. CODE.1 also initializes only 11 indexed RES groups.

Therefore:

~~~text
active supplied executable -> RES9 wordcode
shipped historical resource -> RES12 legacy wordcode
~~~

The original editor or executable revision that used the legacy RES12 generation remains unknown. Its files remain preservation material, not candidates for deletion as unused assets.

## 75. Graphics/text continuity corroborates the lineage

Across 37 ordinal program pairs:
- all 37 preserve the same starting text-base value;
- historical base resource alignment indicates legacy graphics base 51 versus active INIT base 103;
- 23 of 37 aligned initial graphics are byte-identical.

The old graphic base is inferred from matching records; it is not claimed as an original recovered INIT value.

These constraints provide substantial evidence for a pre-split/legacy producer format and a restructured final resource index.

## 76. Equal-length zero companion word array stays UNKNOWN

Both active RES9 and legacy RES12 script containers store a second array of word_count 16-bit zeros.

The supplied CODE.2 runtime reads only the first bank. Its whole-object raw-resource access scan finds no alternative path that consumes the second bank.

This establishes **no direct runtime consumer in the supplied executable**, not an explanation of why the producer wrote it.

The historical purpose must remain UNKNOWN; a lossless tool must preserve the array, not strip it.

## 77. LEVELS opcode 11 is a reserved branch plus external-resource dispatcher

The one-byte arg3 passed to LEVELS event opcode 11 selects:

~~~text
0x0B   fixed final-Strahd story routine
0x0C   fixed GHOST/Vengeful-Spirit possession routine
0x0D   fixed Irmgarde story/cell routine

all other values
       numbered external interactive-resource processor
~~~

The three reserved cases are direct CODE.2 address-distinct functions. The default path calls 13EF8(arg3) and terminates that event slot's remaining command processing.

The default path must not be casually modeled as another RES9 wordcode ID: it opens an externally numbered file.

Source: [event callees](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_CALLEES.md).

## 78. Numbered event-resource path is INFO.DAT dependent

Default opcode-11 resource setup:

~~~text
13EF8(number)
   -> 13AD8()
   -> 139AC(number)
   -> path construction
~~~

Path format:

~~~text
INFO.DAT resource path
+ backslash
+ two decimal digits of number
~~~

The supplied INFO.DAT is exactly 17 bytes:

~~~text
13 ASCII configuration bytes "0000000001111"
uint16 path_length = 2
path bytes = "D:"
~~~

Consequently numbered event 7 resolves to the DOS path D:\07 for the supplied configured installation.

This is not the same as the indexed RES9 resource opener and not the same as the SAVE/LEV filename path.

Source: [INFO.DAT format](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/INFO.DAT/FORMAT.md).

## 79. Numbered event processor has its own lifecycle

The confirmed logical call chain is:

~~~text
13EF8
  |
  +-- 13AD8       select/open numbered file
  +-- 13834       run interactive processor
  +-- 13C88       apply/cleanup resulting transition
  |
  v
interaction result
~~~

Its relationship to LEVELS is proved through opcode 11's default branch.

A faithful native architecture should model this as a **separate external numbered-resource interaction interface**, rather than asserting without evidence that its on-disk format is the same as raw RES9 VM records.

The exact grammar of the separate numbered files must be tied to actual supplied file bytes before a universal script decompiler is claimed.

## 80. The six actual LEVELS opcode-11 calls

All six active opcode-11 commands in the 32-level corpus are:

| Level | Slot | arg3 | Resolved behavior |
|---:|---:|---:|---|
| 5 | 23 | 13 / 0x0D | Irmgarde-dependent fixed-cell story branch |
| 10 | 3 | 12 / 0x0C | Vengeful-Spirit possession lifecycle |
| 12 | 10 | 7 | numbered interactive resource |
| 13 | 8 | 9 | numbered interactive resource |
| 29 | 28 | 11 / 0x0B | final-Strahd activation |
| 31 | 1 | 2 | numbered interactive resource |

These data instances demonstrate both script-to-hardcoded-story and script-to-external-resource dispatch.

Source: [event audit](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/FINAL_AUDIT_2026-10-06.md).

## 81. Final-Strahd activation mutates both global and live-actor state

Opcode 11, arg3=0x0B invokes logical 00074.

The story handler:

1. invokes RES9 program 28, mapped to FINAL STRAHD;
2. sets global event flag 0x3A;
3. scans the live family-A actor array;
4. finds type 8, which NPCS.DAT names STRAHD;
5. clears the +0x0B suppression bit 0x02 on matching live actors;
6. requests world/scene refresh through 1F1C8.

It does **not** mutate the immutable NPCS type definition to reveal Strahd.

This is the precise cross-system chain:

~~~text
LEVELS script
  -> fixed story routine
  -> RES9 dialogue program 28
  -> SAVE global story flag
  -> mutable LEV actor suppression state
  -> renderer/scene refresh
~~~

Source: [event reserved branches](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_CALLEES.md).

## 82. Vengeful-Spirit story uses wordcode, flags, and character status

Opcode 11 arg3=0x0C calls story handler 000B8, whose dialogue invocation uses RES9 program 9 (GHOST).

On its onset branch:

- event flag 0x3C gates an already-active possession;
- an eligible party character is selected;
- program 9 runs;
- character status bit 0x0100 is set;
- event flag 0x3C is set.

The GHOST dialogue group has T base 343.

The bit 0x0100 changes gameplay/presentation state and selects the alternate possessed-portrait bank. These are serialized character effects, not wordcode-local scratch flags.

Source: [possession audit](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/STATUS_POSSESSION.md).

## 83. GHOUL LORD death bridges combat back into possession resolution

NPC death handler logical E284..E553 tests type 0x12 (18), the supplied GHOUL LORD entry.

On that actor's death it sets persistent event flag 0x20.

The GHOST wordcode/T narrative identifies Victor Grymig / Ghoul Lord as the target of the possessing spirit's demand.

On resolution, handler 000B8:

- invokes GHOST program 9 again;
- sets flag 0x3D;
- clears flag 0x3C;
- clears character status mask 0x0140 (both possession 0x0100 and warning/coercion 0x0040).

A level-transition path also checks 0x20 before calling the resolution sequence and clearing that flag.

This is a **combat death -> persistent story flag -> scripted character-status cleanup** sequence and must not be reduced to a dialogue-only branch.

## 84. Vengeful-Spirit warning is a separate recurring presentation state

A separate code path scans active party records, requires possession 0x0100, and clears companion status bit 0x0040 before its warning logic.

When warnings apply, it selects one of four T records 351..354 and displays a coercion message with the possessed portrait, then sets 0x0040.

It derives the warning variant through:

~~~text
(rand * 4) >> 15   // 0..3 from the shipped LCG result
~~~

This is a repeatable story-presentation/action-blocking effect, not the HELD condition (0x0200).

A faithful native engine needs two separate character status views for 0x0100 and 0x0040. [Possession status](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/STATUS_POSSESSION.md).

## 85. Irmgarde branch tests recruited identity and ability to act

Opcode 11 arg3=0x0D invokes logical 001FC.

If persistent global event flag 0x46 is already set, the routine returns false.

Otherwise it searches the active party for exact name IRMGARDE and additionally requires the recruited/template-provenance status bit 0x0080.

The character is rejected if either:
- paralysis 0x1000;
- death 0x4000.

A character merely named IRMGARDE but lacking provenance bit 0x0080 does not satisfy the proved predicate.

Source: [Irmgarde event reconstruction](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_CALLEES.md).

## 86. Irmgarde branch changes a fixed map cell

On success, handler 001FC computes:

~~~text
cell_index = 18 * map_width + 54
cell[cell_index].low_byte &= 0xC0
set global event flag 0x46
BF44 = 0x33
return success
~~~

This clears only the cell's low six-bit code while preserving its other cell flags.

On failure to find a qualified Irmgarde, it writes BF44=0x4A and returns false.

When opcode 11 sees true, it can additionally clear cell-record control bit 0x04 on its command cell.

This is a hard-coded story topology change tied to named recruited party state. It is **not** evidence for a general “open door with party member” opcode.

## 87. The persistent flag namespace crosses all three script/story layers

The 0x22200 global 32-byte vector is manipulated by:

- LEVELS event opcodes 8/9/10;
- RES9 wordcode opcodes 09/0A/0B/0C/0D;
- hard-coded Strahd/Ghost/Irmgarde and other story routines;
- NPC death and transition logic.

SAVE.DAT serializes the same 32 bytes.

The interaction between these producers matters more than each opcode name. A faithful port must provide a **single persistent flag service** shared by event VM, wordcode VM, and native story routines.

Wordcode-local 0x7FD0 flags must not be stored into that service.

## 88. A dialog program can have graphics, audio, item, story, and party side effects

Consider the actual instruction-family architecture, not a hypothetical linear conversation file:

~~~text
NPC interaction
   |
   v
wordcode script
   |
   +-- choose T text by character condition
   +-- draw graphics at encoded coordinates
   +-- queue/change music/sound
   +-- append/remove four-choice options
   +-- follow selected word-IP target
   +-- set/test SAVE-backed story flags
   +-- test/remove/grant object IDs
   +-- apply party-template character
   +-- suppress live actor
   +-- spawn family-C item
   +-- call cinematic
   +-- schedule repeating callback
~~~

Therefore reconstructing a dialogue screen as a list of strings and hyperlinks would lose gameplay and temporal behavior.

## 89. Wordcode graphics and text IDs are relative, not absolute

The VM changes the current sub-base during program execution.

A draw instruction operand cannot always be interpreted from the fixed INIT interaction base alone.

Likewise a local dialogue text ID is interpreted relative to the current T base.

A preservation-grade disassembler should print both:
- raw encoded operand;
- derived resolved identifier under current execution context.

For conditional or branching base changes, that resolution may be path-dependent. Preserve raw operands regardless of any symbolic annotation.

## 90. Replay and VM scheduling are tied to game runtime time

The five 12-byte wordcode timer records store next deadlines, intervals and script handler word IPs. Their poller executes due handlers by changing the VM instruction pointer temporarily.

Other runtime systems already use distinct clocks:

~~~text
interrupt clock
pausable gameplay/deadline tick
16.16 time-of-day
~~~

The VM's actual deadline comparator and scheduling logic must be preserved in its original time domain, not implicitly advanced by modern UI render frames.

Where temporal details are not yet independently simulated, the original CODE.2 timer helper is controlling evidence.

## 91. Source-preserving modding needs an executable grammar

A correct RES9 wordcode editor requires more than an opcode listing:

1. decode each raw 16-bit program word;
2. consume variable-length opcode 02 through condition 0x21 plus text ID;
3. account for indirect operand consumption at opcode 05;
4. identify every decoded instruction start by word index;
5. classify absolute jumps, conditions, nested-call targets, choice destinations, and timer handlers;
6. validate all targets against instruction starts;
7. retain unknown/unused opcodes in a future-proof AST representation;
8. preserve the serialized companion bank exactly;
9. keep the raw program code and relocatable symbolic views separate;
10. distinguish legacy RES12 template-bearing grammar from final active RES9 grammar.

The supplied audit achieves zero parse errors and zero invalid direct targets for both active and legacy banks under their respective grammars.

## 92. Proposed native runtime boundaries

Suggested C++ integration types (not original source names):

~~~text
LevelEventTable
    50 serialized slots
    active count + five five-byte commands

LevelEventInterpreter
    executes opcode 0..12
    calls cell/world/audio/story services

InteractionWordcodeProgram
    raw words + preserved companion bank

InteractionWordcodeVM
    word-index instruction pointer
    four choices
    local flags
    nested call stack
    five repeating callbacks
    presentation/resource context

InteractionRunner
    NPC/fixed program selection
    modal choice UI
    exit/cinematic restoration

IndexedTextArchive
    T offsets, XOR decode, preserved slack

ExternalNumberedEventRunner
    INFO.DAT rooted resource selection
    13EF8/13834/13C88 lifecycle

StoryStateServices
    SAVE persistent flag vector
    character/status mutations
    live NPC suppression
    level-cell mutations
~~~

These boundaries mirror distinct original data owners without pretending the decompilation recovered DreamForge project classes.

## 93. Preserve literal widths, sentinels, and address domains

A faithful engine must retain:

~~~text
LEVELS:
  uint8 opcode/args
  5-byte command width
  26-byte slot
  int8 event index -1 sentinel

RES9:
  uint16 opcode/arguments
  word-index instruction pointer
  0xFFFF empty choice/timer sentinel
  variable opcode 02
  indirect operand consumption opcode 05

T:
  absolute uint32 offsets
  uint16 payload lengths
  XOR byte 0xDC

SAVE:
  exactly 32 bytes persistent story flags
~~~

One 16-bit/8-bit truncation or pointer-domain mixup can silently corrupt branch destinations, resource references, or persistent state.

## 94. Test: physical LEVELS script geometry and corpus census

For all 32 levels:

~~~text
50 * 26 = 1300 bytes per level
1600 physical script slots total
394 active slots
555 active command instances
max count 5
opcode range 0..12
~~~

Verify the entire per-level script region is round-trippable, including inactive commands and nonzero tails.

Test interpreter -1 sentinel behavior separately.

## 95. Test: level event dispatcher

Construct one event for each opcode 0..12, including those absent from the supplied corpus.

Verify:
- exact number of operands and field widths;
- record kind branches for opcodes 0/1/2;
- class-3 spawning for 3;
- six-bit-only cell mutation for 4;
- queued digital SFX for 5;
- music transition order for 6;
- modal indexed T lookup for 7;
- persistent flag bit operations for 8/9/10;
- reserved/default branch split for 11;
- global state-code assignment for 12.

Also test the unsupported opcode skip path.

## 96. Test: unaligned event sound/music arguments

For opcodes 5 and 6 construct byte arrays where:

~~~text
command opcode at +0
arg3 at +3
uint32 requested at +3 (not guaranteed within +0..+4)
~~~

Change trailing adjacent bytes and verify the read follows certified CODE.2 behavior.

A safe host implementation should bound malformed external inputs while preserving valid shipped-byte behavior; do not silently change the operand width for ordinary valid scripts.

## 97. Test: active RES9 grammar/control flow

Parse RES9:517..554 and assert:

~~~text
38 programs
11396 program words
4237 decoded instructions
0 invalid branch/choice/nested/timer targets
~~~

Test:
- variable conditional dialog end marker;
- count-prefixed choice lists;
- opcode 05 operand consumed by its callee;
- target word-index arithmetic;
- return codes 1/2/3;
- nested caller-IP restoration;
- jump targets and choice-edit compaction.

## 98. Test: wordcode local/global flag isolation

Construct a script setting a VM-local bit and a different global bit.

Verify:
- local bit changes affect only VM-local branch predicates;
- persistent flag affects LEVELS and story consumers;
- SAVE round-trip preserves the global bit only;
- a fresh interaction context does not invent SAVE persistence for the local bit.

Include unused but executable-supported global clear/toggle handlers.

## 99. Test: four-choice assembly

Exercise:
- replace all text IDs;
- replace all target IPs;
- individual setters 03/04;
- append opcode 13;
- remove/compact opcode 14;
- prepend opcode 1A;
- choice return opcode 0E;
- 0xFFFF sentinel and full-table edge conditions.

Verify selected target resumes at the encoded word instruction pointer and the displayed text resolves via the current T base.

## 100. Test: conditional dialogue

Build condition lists with:
- matching first condition;
- matching later condition;
- no preceding match and default condition 0x21;
- voice/sound selector;
- default text ID after 0x21.

Require the parser to place the following opcode at the correct word position for every case.

Only infer semantic names for condition constants if CODE.2's condition helper proves them.

## 101. Test: timer callbacks

For all five timer slots verify:
- encoded handler word IP;
- interval and next deadline;
- disable sentinel 0xFFFF;
- due/not-due paths;
- caller-IP restore;
- callback result handling;
- successive rescheduling;
- slot 4 despite no current corpus use.

Keep the VM callback timer distinct from character spell deadlines and HUD combat-feedback timers.

## 102. Test: item and character script mutations

Exercise:
- ordinary item possession;
- nested container possession;
- 0xFFFF/15 BAROVIAN COIN predicate;
- recursive item removal;
- grant into available party slot;
- full-party fallback to family-C world item;
- explicit family-C spawn;
- all ten centralized party templates;
- recruited/template provenance status;
- actor suppression including special opcode 16 operands.

Verify save/level persistence owners are updated, not static OBJECTS/NPCS definitions.

## 103. Test: external numbered event interface

For INFO resource path D: and event values 2, 7, 9, verify two-decimal-digit pathname construction, open/error handling, and cleanup around the numbered resource lifecycle.

Do not fake success by treating the value as a RES9 wordcode program ID.

Where numbered external resource bytes are unavailable, mark end-to-end format tests as **blocked by missing corpus**, not passed by VM parser tests.

## 104. Test: hard-coded story chains

At minimum verify:

**Final Strahd**
- program 28 invoked;
- persistent flag 0x3A set;
- live NPC type 8 suppression cleared;
- scene refresh requested.

**Vengeful Spirit**
- program 9 used on onset/resolution;
- possession status 0x0100 installed;
- global flag 0x3C set;
- GHOUL LORD death sets 0x20;
- resolution clears 0x0100/0x0040, clears 0x3C, sets 0x3D.

**Irmgarde**
- exact recruited identity test;
- dead/paralyzed rejection;
- cell (row 18,column 54) six-bit-code clear only;
- global flag 0x46 and BF44=0x33 on success;
- BF44=0x4A on relevant failure.

## 105. Test: T archive byte-exact preservation

For the supplied 133584-byte T file:

~~~text
1073 active records
25 zero-length records
276 pre-payload slack bytes
30 post-record gaps
1804 skipped gap bytes
last indexed payload ending at EOF
~~~

Ensure:
- XOR round-trip exact;
- absolute offsets followed;
- runtime NUL not serialized;
- inactive/residue bytes preserved;
- no implicit Unicode rewriting;
- no fabricated records from apparent stale strings.

## 106. Test: legacy RES12 decoder and migration

Parse all 37 legacy programs with opcode 05 arity 141 words.

Assert:

~~~text
4820 program words
1079 decoded instructions
16 inline-template occurrences
0 invalid direct targets
~~~

For each compact template:
- decode identity, class, abilities, inventory and spell matrices;
- map to expected static 0x248 template prefix through +0x109;
- retain raw inline representation when round-tripping RES12;
- do not replace it on disk with a one-word final template ID unless intentionally converting formats.

Validate 37/37 shared ordinal text bases and the documented IRMGARDE program migration.

## 107. Correction C-28 — LEVELS and RES9 do not share a script grammar

The LEVELS VM has 5-byte command records and 13 opcodes; RES9 VM has 16-bit words and 42 opcodes.

They are linked by shared services but are not interchangeable.

Attempting to decode LEVELS event commands with the RES9 opcode parser, or RES9 instructions with a fixed five-byte parser, destroys control flow and semantics.

## 108. Correction C-29 — dialogue is executable story logic, not merely text

RES9 wordcode includes choices, conditional branches, timers, inventory state changes, party-template installation, live NPC suppression, global flags, audio, and cinematics.

An editor that preserves only displayed T records but drops the surrounding wordcode does not preserve original gameplay logic.

## 109. Correction C-30 — legacy RES12 is an earlier VM generation

RES12's apparent invalid opcodes were misidentified embedded character-template data inside opcode 05.

The corrected legacy 141-word opcode-05 grammar parses all 37 programs with zero invalid direct targets and independently corroborates final centralized CODE.2 templates.

This strengthens historical preservation without claiming RES12 is selected by the supplied CODE.2.

## 110. Correction C-31 — separate script-local and SAVE-persistent flags

The 0x7FD0 interaction-local state dword is distinct from the 0x22200, 32-byte SAVE global event vector.

Merging them causes:
- state leakage across interactions;
- incorrect SAVE persistence;
- incorrect branching;
- story progression defects.

## 111. Correction C-32 — reserved event branches are code, not data-driven lookups

LEVELS opcode 11 values 0x0B/0x0C/0x0D invoke hard-coded story routines.

Treating every value as a numbered file path bypasses the Strahd, possession, and Irmgarde character/world mutations.

## 112. Historical unknowns retained

The active wordcode grammar, data boundaries and major semantics are closed for the supplied build, while these historical questions remain:

1. Original DreamForge source/type names for both script engines and every opcode.
2. Historical purpose/producer of the equal-length zero companion bank in raw RES9/RES12 program records.
3. Whether and how another executable/editor consumed the shipped legacy RES12 scripts.
4. Specific original build/editor version that produced the active RES9 versus legacy RES12 change.
5. Historical meaning of RES1's two inactive 0x00000060 non-EH markers.
6. Exact compositor/source names for draw operations 0x10, 0x11, 0x23, 0x25 and 0x26.
7. Complete player-facing names for every conditional-dialog code without independent executable predicates.
8. Source-level intent of encoded NPC-suppression operands 0xFFFF and 0x29 beyond reconstructed behavior.
9. Full grammar/provenance of separate numbered external interactive files without every historical file available.
10. Any alternative wordcode/runtime handling present only in another executable revision.

Those questions must not be disguised as invented facts, nor used to prevent implementation of the established shipped behavior.

## 113. Source ledger

### Certified executable and DAT behavior

- [Code2/Code2.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)
- [Code2/CODE2_CONTINUITY_ADDENDUM.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)
- [DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md)
- [DatFiles/LEVELS.DAT/EVENT_CALLEES.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/EVENT_CALLEES.md)
- [DatFiles/LEVELS.DAT/FINAL_AUDIT_2026-10-06.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FINAL_AUDIT_2026-10-06.md)
- [DatFiles/INFO.DAT/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/INFO.DAT/FORMAT.md)
- [DatFiles/SAVE.DAT/STATUS_POSSESSION.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_POSSESSION.md)

### Resource and wordcode evidence

- [Resources/RES/WORDCODE_VM.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/WORDCODE_VM.md)
- [Resources/RES/WORDCODE_PROGRAM_INDEX.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/WORDCODE_PROGRAM_INDEX.md)
- [Resources/RES/NON_EH.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/NON_EH.md)
- [Resources/RES/RES12_WORDCODE.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/RES12_WORDCODE.md)
- [Resources/RES/RES11_RES12_LINEAGE.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/RES11_RES12_LINEAGE.md)
- [Resources/MISC/T/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/T/FORMAT.md)
- [Resources/RES/analyze_wordcode.py](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/analyze_wordcode.py)
- [Resources/RES/analyze_legacy_archives.py](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/analyze_legacy_archives.py)

### Cross-system chapters

- [Component 05 world state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/05_WORLD_LEVEL_STATE_AND_EVENTS.md)
- [Component 07 UI](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md)
- [Component 08 persistence](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md)
- [Component 09 character state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/09_PARTY_CHARACTER_GAMEPLAY_MODEL.md)
- [Component 11 NPCs](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/11_NPC_CREATURE_AI_AND_LIVE_ACTOR_MODEL.md)
- [Component 13 spells](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/13_SPELL_MAGIC_EFFECT_AND_PREPARATION_SYSTEM.md)

## 114. Architectural checkpoint

~~~text
LEVEL EVENT SCRIPT
LEVELS.DAT / LEVnnn.DAT
    50 x 26-byte slots
          |
          v
    13-opcode event VM
          |
          +-- cell control/geometry
          +-- class-3 effect spawn
          +-- SFX/music/T message
          +-- SAVE global flags
          +-- story routine
          +-- numbered external processor
                    |
                    +-- INFO.DAT path
                    +-- numbered resource
                    +-- separate interaction lifecycle


INTERACTION WORDCODE
NPC type or fixed story call
          |
          v
      program ID
          |
          v
      RES9 517..554
          |
          +-- program words (runtime-consumed)
          +-- equal-length zero bank (preserved, inactive)
          |
          v
      42-opcode 16-bit VM
          |
          +-- word IP, nested calls, jumps
          +-- four choices / conditional dialogue
          +-- five recurring callbacks
          +-- local state flags
          +-- global SAVE flags
          +-- item search/grant/remove/world spawn
          +-- party templates
          +-- live NPC suppression
          +-- RES9 graphics
          +-- T indexed dialogue
          +-- SFX/music/cinematics

HISTORICAL LINEAGE
RES12 legacy scripts
          |
          +-- same VM opcode domain
          +-- opcode 05 has 141-word inline party template
          +-- not loaded by supplied CODE.2


PERSISTENCE
SAVE.DAT global flags
SAVE.DAT party status/roster
LEVnnn.DAT live cell/NPC/item mutation
T/RES raw file bytes preserved separately
~~~

**Component 14 closes the active script/dialogue architecture to the supplied-build evidence boundary.** The next preservation component should cover the **sound/music/driver interface architecture**: SOUND.CFG, SFX queues, RES6/RES10 VOC, RES5 XMIDI, ANM audio interaction, period DOS audio driver ABIs, mute/transition controls, and the still-bounded ARIA GM1/GM2 support-bank grammar.
