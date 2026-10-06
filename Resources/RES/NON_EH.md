# Non-EH RES records

Status: **the 75 non-EH records in `RES9`/`RES12` are structurally identified as interaction/display wordcode containers. The active 38-entry `RES9` bank has an executable-proved loader and a completely recovered 42-opcode instruction grammar with corpus-wide control-flow validation. The two four-byte `RES1` records remain unresolved sentinels.**

The immutable archives, `INIT`, and original CODE.2 bytes are controlling evidence.

## Corpus

The 77 non-empty entries that do not begin with `EH` are:

- `RES1:381` and `RES1:427`: four bytes `60 00 00 00`;
- `RES9:517..554`: 38 active wordcode containers;
- `RES12:370..406`: 37 wordcode-shaped containers.

The two `RES1` records are each immediately preceded by a zero-length archive entry. No direct consumer has yet been proved, so they remain preserved as four-byte sentinels rather than being assigned a speculative type.

## `RES9` / `RES12` container shape

All 75 larger records have exactly:

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

The zero companion bank is **not read** by the proved active RES9 loader. Its intended serialization/runtime lifecycle remains unresolved and it must be preserved verbatim.

## Direct CODE.2 loader for active RES9 wordcode

Routine `13F70` bypasses the normal `EH` loader:

1. obtains archive 9's raw handle through `2EC74(9)`;
2. computes an entry index from two `INIT`-derived values plus program ID;
3. seeks that raw RES entry through `31334`;
4. reads `word_count`;
5. reads exactly `word_count * 2` bytes into VM word buffer `0x13020`;
6. resets word instruction pointer `0x7FEC` to zero.

The supplied `INIT` proves:

```text
INIT +0x1E = 103
INIT +0x22 = 414

entry = 103 + 414 + program_id
      = 517 + program_id
```

Therefore `RES9:517..554` are exactly wordcode program IDs `0..37`.

## Wordcode VM is now structurally reconstructed

CODE.2 `15534` dispatches 16-bit opcode words `0x00..0x29` through the 42-entry table at logical `1548C`.

`WORDCODE_VM.md` now documents:

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
- conservative behavior labels for the remaining renderer draw variants.

`analyze_wordcode.py` parses and validates the supplied active bank directly from `RES9` and `INIT`.

Supplied-corpus result:

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

See `WORDCODE_VM.md` for the full opcode specification and `analyze_wordcode.py` for reproducible validation/disassembly.

## Higher-level role

Routine `16524(program_id)` initializes interaction/display state, loads the requested raw RES9 program, draws resources, runs the VM, manages choices/timers/audio, and later restores surrounding display/music state.

Its callers include fixed story/gameplay IDs and world-object-derived IDs. `21B11`, for example, maps one eligible interactive world-record type to program `0x25` and otherwise converts eligible types using `type - 0x22` before invoking `16524`.

The safe class name remains **interaction/display wordcode** rather than NPC animation or generic scripting.

## `RES12` relationship

`RES12:370..406` has the identical count/program/zero-bank structure. The 37 shared ordinal positions also have matching early opcode-`0x19` text-base operands relative to the RES9 programs.

The proved active loader explicitly opens archive **9**. No corresponding active archive-12 raw-wordcode loader has yet been established. Therefore RES12 remains an **ID-aligned alternate wordcode bank**, not an asserted runtime-selected bank.

## Remaining work

The RES9 instruction grammar/control flow is closed. The remaining non-EH frontier is now:

- determine the intended purpose of the serialized zero companion bank;
- trace RES12 selection or establish that it is alternate/unused data for this executable;
- resolve the two `RES1` four-byte sentinels;
- map individual program IDs to named story/interactions where caller/level evidence makes those identities provable;
- continue deeper naming of renderer draw variants only when CODE.2 evidence supports stronger names.
