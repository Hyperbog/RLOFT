# RES12 alternate wordcode-shaped bank

Status: **`RES12:370..406` is structurally aligned with the active RES9 interaction-wordcode bank but is not bytecode-compatible with the CODE.2 `0x00..0x29` VM recovered from `RES9:517..554`. No direct CODE.2 raw archive-12 loader is present in certified object-1.**

## Shared outer structure

All 37 records use the same container shape as the active RES9 programs:

```text
uint16 word_count
uint16 program_words[word_count]
uint16 zero_bank[word_count]
```

Every supplied second-bank word is zero. The RES12 records contain 4,820 program words total and have `word_count` values from 22 through 375.

They are ordinally aligned with the first 37 RES9 program IDs. In particular, the early opcode-`0x19`-shaped text-base operand matches at every shared ordinal position. This proves a real relationship between the banks, but not runtime compatibility.

## Active CODE.2 raw-loader evidence

The active loader `13F70` requests archive 9 through raw archive-handle accessor `2EC74`.

A complete call-target scan of the certified CODE.2 object-1 bytes finds exactly **one** direct call to `2EC74`:

```text
logical call site  13F76
preceding argument 9
```

No direct raw-handle request for archive 12 exists in object-1. This is stronger than merely failing to locate a second high-level loader in the reconstructed C++.

## Incompatible with the recovered RES9 VM grammar

`Resources/RES/WORDCODE_VM.md` reconstructs the active CODE.2 dispatcher as opcodes `0x00..0x29` with exact operand consumption.

Applying that exact grammar to the 37 RES12 records does not succeed. Fifteen programs reach values above `0x29` at would-be instruction boundaries:

```text
program 4   RES12:374   word 46   0x41
program 5   RES12:375   word 56   0x48
program 6   RES12:376   word 36   0x48
program 10  RES12:380   word 61   0x49
program 11  RES12:381   word 54   0x4C
program 12  RES12:382   word 36   0x55
program 13  RES12:383   word 36   0x55
program 14  RES12:384   word 67   0x52
program 15  RES12:385   word 40   0x4E
program 27  RES12:397   word 64   0x4C
program 29  RES12:399   word 54   0x45
program 30  RES12:400   word 60   0x59
program 33  RES12:403   word 36   0x4C
program 34  RES12:404   word 36   0x4E
program 35  RES12:405   word 60   0x45
```

These failures may reflect a different opcode set, different operand lengths, or another related compiled format. They are sufficient to rule out treating RES12 as a bytecode-compatible duplicate of the active RES9 bank.

## Safe classification

The current evidence supports:

**ID-aligned alternate/incompatible wordcode-shaped bank.**

It does **not** yet support a claim that RES12 is:

- selected by this CODE.2 executable;
- a newer or older VM version;
- a specific localization;
- a specific development/editor intermediate;
- unused by every executable associated with the shipped media.

Those possibilities require independent evidence.

## Remaining work

- search CODE.1 and other shipped executables/tools, if present in the preservation set, for a matching archive-12 raw loader or a dispatcher accepting the RES12 dialect;
- derive the alternate instruction grammar from a controlling executable/tool before assigning semantics;
- compare program-level control-flow/text/resource relationships only after that grammar is known;
- preserve the second all-zero companion bank exactly while its purpose remains unresolved.
