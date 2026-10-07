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
