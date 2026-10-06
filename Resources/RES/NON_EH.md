# Non-EH RES records

Status: **the active RES9 interaction/display wordcode bank is structurally and behaviorally reconstructed; RES12 is a legacy generation of the same VM with an older 141-word opcode-05 inline-template encoding; and the two RES1 four-byte records are proved inactive/unconsumed by the supplied CODE.2 resource paths. The serialized zero companion bank in every wordcode container is likewise not consumed by this executable and remains producer-side `UNKNOWN`.**

The immutable archives, `INIT`, and certified original CODE.2 object are controlling evidence.

## Corpus

The 77 non-empty entries that do not begin with `EH` are:

- `RES1:381` and `RES1:427`: four bytes `60 00 00 00`;
- `RES9:517..554`: 38 active wordcode containers;
- `RES12:370..406`: 37 wordcode-shaped alternate containers.

## RES1 raw four-byte records

The two RES1 records are each immediately preceded by a zero-length entry:

```text
RES1:380  empty
RES1:381  60 00 00 00
...
RES1:426  empty
RES1:427  60 00 00 00   // final archive entry
```

They cannot be consumed by normal typed resource loader `2EB60`, because that path requires a five-byte-or-longer `EH` envelope before dispatching types 1..7.

A direct call-target scan of the complete certified CODE.2 object finds exactly one call to raw-archive-handle helper `2EC74`: `13F76`, inside the active wordcode loader, with archive number 9. There is no raw archive-1 request.

Therefore these two serialized `0x00000060` records are **inactive/unconsumed by the supplied CODE.2 resource paths**. Their historical producer/tool meaning remains unknown and must not be invented. See `RES1_SENTINELS.md`.

## RES9 / RES12 container shape

All 75 larger raw records have exactly:

```text
uint16 word_count
uint16 program_words[word_count]
uint16 zero_bank[word_count]
```

Thus:

```text
entry_size == 2 + 4 * word_count
```

Every supplied `zero_bank` word is zero.

- `RES9`: 38 records, 29..606 program words, 11,396 program words total;
- `RES12`: 37 records, 22..375 program words, 4,820 program words total.

## Direct CODE.2 loader for active RES9 wordcode

Routine `13F70` bypasses the normal `EH` loader:

1. obtains archive 9's raw handle through `2EC74(9)`;
2. computes an entry index from two `INIT`-derived values plus program ID;
3. seeks that raw RES entry through `31334`;
4. reads `word_count`;
5. reads **exactly `word_count * 2` bytes** into VM word buffer `0x13020`;
6. resets word instruction pointer `0x7FEC` to zero.

The supplied `INIT` proves:

```text
INIT +0x1E = 103
INIT +0x22 = 414

entry = 103 + 414 + program_id
      = 517 + program_id
```

Therefore `RES9:517..554` are exactly wordcode program IDs `0..37`.

## Zero companion bank: executable-negative closure

The on-disk zero bank begins immediately after the first `word_count` words, but the active loader stops after step 5 above. It does not seek across, copy, initialize from, compare, or write the companion bank.

The stronger whole-object raw-access audit matters here: `13F76 -> 2EC74(9)` is the **only** direct raw-handle request in the certified CODE.2 object. No second raw reader exists that could reopen these same records and consume their trailing halves through another archive path.

Consequently, for the supplied executable:

```text
program_words   runtime-consumed by 13F70
zero_bank       serialized but not runtime-consumed
```

This closes the executable-side question. It does **not** establish why DreamForge's producer pipeline wrote a second equal-length zero word array, nor whether another build/editor used it. The correct evidence classification for its historical purpose is therefore `UNKNOWN`.

Preservation/repacker rule: retain the full second bank verbatim and require its stored length to match `word_count`; do not omit it merely because this executable ignores it.

## Wordcode VM

CODE.2 `15534` dispatches 16-bit opcode words `0x00..0x29` through the 42-entry table at logical `1548C`.

`WORDCODE_VM.md` documents:

- all 42 opcode handlers;
- exact operand consumption for every opcode;
- variable-length opcode `0x02` condition/text grammar;
- word-addressed jumps and nested execution;
- the three nonzero interpreter return codes;
- four-choice table construction and selection;
- recurring VM timer callbacks;
- persistent SAVE-backed global flags versus private wordcode-local flags;
- inventory possession, grant, remove and loose-item spawn operations;
- the special 15-`BAROVIAN COIN` predicate;
- NPC suppression operations;
- archive-9 graphics/text/sound sub-bases;
- sound, music and cinematic operations;
- conservative behavior labels for remaining renderer draw variants.

`analyze_wordcode.py` validates the supplied active bank directly from RES9 and INIT:

```text
programs                         38
program words                11,396
decoded instructions          4,237
parse errors                      0
invalid direct control targets    0
timer slots used                0..3
```

All encoded conditional branches, absolute jumps, nested calls, enabled timer handlers, choice target lists and dynamically inserted choice targets land on decoded instruction starts.

Five executable-supported opcodes are absent from the shipped active bank but remain part of the recovered grammar:

```text
03  set one choice target
04  set one choice text
0B  clear persistent global flag
0C  toggle persistent global flag
1C  direct sound-effect playback
```

## Higher-level role and named programs

Routine `16524(program_id)` initializes interaction/display state, loads the requested raw RES9 program, draws resources, runs the VM, manages choices/timers/audio, and restores surrounding display/music state.

The active bank maps directly to NPC definitions:

```text
NPC types 34..70 -> programs 0..36
NPC type 82      -> program 37
```

`WORDCODE_PROGRAM_INDEX.md` records the complete mapping and each program's resource/text/sound/music relationships.

The safe class name is **interaction/display wordcode** rather than NPC animation or a universal gameplay scripting language.

## RES12 relationship

RES12:370..406 has the identical outer count/program/zero-bank shape and is now grammar-closed as a legacy generation of the active interaction/display VM.

The former apparent invalid opcodes were operand data inside opcode 0x05. Legacy opcode 0x05 consumes a **141-word inline compact character template**; active RES9 opcode 0x05 consumes a **one-word template ID** into CODE.2's centralized static character-template table.

With the recovered legacy arity:

    programs                         37
    program words                 4,820
    decoded instructions          1,079
    invalid direct targets            0

Same-ordinal legacy/active text bases are preserved 37/37. All ten final static template identities appear in the legacy inline payloads.

The whole-object raw-access audit still proves no direct archive-12 raw-handle request exists in supplied CODE.2, and supplied CODE.1 initializes only 11 indexed RES groups. RES12 is therefore historically related but inactive in the certified executable pair.

See RES12_WORDCODE.md and RES11_RES12_LINEAGE.md.

## Closed versus intentionally unknown

Closed for the supplied executable:

- active RES9 bank location and program IDs;
- complete active VM instruction grammar/control flow;
- named NPC/program mapping;
- RES12 is not selected by the certified CODE.2 raw loader;
- the serialized zero companion banks are not runtime-consumed;
- RES1 raw records are not reachable through established CODE.2 resource paths.

Intentionally unresolved historical/producer questions:

- why the wordcode producer serialized a same-length zero bank;
- what tool/build originally used the two RES1 `0x00000060` markers;
- exact historical executable/editor/build that consumed or produced the legacy RES11/RES12 composite generation.

Those are preservation `UNKNOWN`s, not blockers for a faithful reconstruction of this executable's runtime behavior.
