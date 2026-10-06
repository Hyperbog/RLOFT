# CODE.2 Status-12 State/UI Ownership

Status: **CORRECTED / CONSOLIDATED — two certified live gameplay causes; one unreferenced duplicate routine.**

This document supersedes the earlier checkpoint that counted all three direct `push 0Ch ; call 205B4` sites as live status-12 producers.

## 1. Protocol meaning

CODE.2 helper `0x205B4` explicitly treats status `10` and `12` as privileged application transition results. R.EXE independently proves the supervisor action for status 12:

```text
CODE.2 status 12
    -> R.EXE deletes CHARS
    -> supervisor state normalized to 10
    -> CODE cycle restarts without transferred CHARS state
```

Canonical protocol name:

```text
RESTART_CODE_CYCLE_WITHOUT_CHARS
```

This is a mechanism, not a gameplay cause.

## 2. Live producer: party wipe / no active party

The direct call at `0xCCE0` is reached through `0xCBC8` only after all four party records satisfy the disabling mask test:

```c
status & 0xD040
```

for every slot. The mask includes the certified zero-HP/death bit `0x4000` and empty-slot bit `0x8000`, plus two additional disabling bits.

`0xCBC8` performs the terminal presentation, waits for input acknowledgement, and then calls the controlled-exit helper with 12.

Certified cause:

```text
PARTY_WIPE / NO_ACTIVE_PARTY
    -> terminal presentation
    -> status 12
```

## 3. Live producer: five-object quest-completion finale

The direct call at `0x26BD9` is reached only after all five inventory predicates succeed:

```text
0x74  LORD DHELT'S HOLY SYMBOL
0x5C  ANCIENT ELVEN CROWN
0x76  WERERAVEN FEATHER
0x8B  VISTANI TAROKKA CARD
0x77  PRICELESS FIRE RUBY
```

Its success tail is:

```text
clear BF38
cleanup/teardown
13EF8(10)          ; numbered cinematic event 10
205B4(12)          ; supervisor transition
```

Event 10 selects `CINE10.ANM` or `CINE11.ANM` according to the certified party predicate.

Certified cause:

```text
FIVE-ARTIFACT QUEST COMPLETION
    -> CINE10/CINE11
    -> status 12
```

## 4. `0x23DF0` is not a certified live producer

The standalone routine at `0x23DF0..0x23E1A` contains the same finale tail and includes the direct status-12 instruction at `0x23E10`. However the later reachability audit found:

- no direct CALL/JMP target;
- no recognized switch/jump-table target;
- no stored absolute function pointer;
- no LE relocation/fixup targeting `0x23DF0`;
- no relevant application callback construction.

It is therefore classified as:

```text
UNREFERENCED DUPLICATE FINALE ROUTINE
```

It remains valid executable evidence but must not be counted as a third live gameplay trigger unless future dynamic evidence demonstrates a caller.

## 5. Canonical status-12 taxonomy

```text
LIVE CAUSE 1
PARTY_WIPE / NO_ACTIVE_PARTY
    -> status 12

LIVE CAUSE 2
FIVE-ARTIFACT QUEST COMPLETION
    -> event 10
    -> CINE10/CINE11
    -> status 12

ARCHIVAL / UNREFERENCED CODE
0x23DF0 duplicate finale helper
    -> event 10
    -> status 12
    -> no certified incoming edge

SUPERVISOR MEANING
status 12 = RESTART_CODE_CYCLE_WITHOUT_CHARS
```

For a native port, keep the gameplay cause separate from the transition result. Do not globally rename status 12 to `GAME_OVER`, `ENDING`, `NEW_GAME`, or character-generation terminology.
