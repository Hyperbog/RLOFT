# Event 10 Cinematic Correlation and Status-12 Finale Path

Status: **BINARY-PROVEN — numbered event 10 selects `CINE10.ANM` or conditionally `CINE11.ANM`; status 12 follows this event on the five-item quest gate**

This checkpoint closes the previously withheld correlation between the `0x13EF8(10)` call in the CODE.2 status-12 transition and the supplied late-game ANM resources.

## 1. Cinematic filename constructor at `0x139AC`

CODE.2 contains the literal template `CINE00.ANM` at object-relative `0x131D0`. Routine `0x139AC` receives an integer in `EBX`, copies that template to a local buffer, divides the integer by 10, and replaces the two zero characters with decimal tens and units digits.

Therefore its argument maps directly to the shipped cinematic namespace:

```text
0  -> CINE00.ANM
1  -> CINE01.ANM
...
10 -> CINE10.ANM
11 -> CINE11.ANM
```

The routine then opens the constructed filename through the runtime file-open wrapper and initializes the ANM stream.

## 2. `0x13EF8` carries the numbered event into cinematic selection

`0x13EF8` passes its numbered-event argument through `0x13AD8`. In `0x13AD8`, the event number is loaded into `ESI`; after setup it reaches:

```text
0x13BAA: push esi
0x13BAB: mov [0x7F78],esi
0x13BB1: call 0x139AC
```

Thus the numbered-event value is the base cinematic number supplied to the proven `CINE00.ANM` filename constructor.

This establishes directly that `0x13EF8(10)` is not merely adjacent to cinematic playback: it selects cinematic number 10, subject to the event-10 adjustment below.

## 3. Special event-10 adjustment selects CINE10 or CINE11

Immediately before the call to `0x139AC`, CODE.2 has explicit special handling when `ESI == 10`:

```text
0x13B69: cmp esi,0x0A
0x13B6C: jne 0x13BAA

scan four records at 0x218F0
record stride = 0x248
total span    = 0x920

0x13BA1: test edx,edx
0x13BA3: jne 0x13BAA
0x13BA5: mov esi,0x0B
0x13BAA: push esi
0x13BB1: call 0x139AC
```

The four-record span is exactly the already-proven transferred character block (`4 * 0x248 = 0x920`). Each record is tested through its leading word/flag state. If at least one record satisfies the tested condition, `ESI` remains 10 and `CINE10.ANM` is opened. If none satisfy it, CODE.2 changes `ESI` to 11 and opens `CINE11.ANM` instead.

The precise gameplay label for the tested character flag is not assigned here; the binary fact is sufficient: **event 10 branches between two finale cinematics according to party/character state.**

## 4. Connection to the five-item status-12 producer

The already classified producer at `0x26BD9` requires all five inventory predicates to succeed:

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
cleanup/presentation teardown
call 0x13EF8(10)
call exit-helper(12)
```

Combining that path with the filename constructor now proves:

```text
possess all five required quest objects
        |
        v
run numbered event 10
        |
        +-- qualifying party state --> CINE10.ANM
        |
        `-- alternate party state  --> CINE11.ANM
        |
        v
CODE.2 exits with status 12
        |
        v
R.EXE deletes CHARS and restarts CODE.2
```

This is a deliberate **finale/major quest-completion cinematic transition**, not an error path and not a character-generation failure.

## 5. Status-12 naming boundary

Status 12 still must not be globally renamed to a finale-specific enum because two other CODE.2 producers also emit it. The certified protocol name remains:

**`RESTART_CODE_CYCLE_WITHOUT_CHARS`**

The `0x26BD9` producer can now be semantically annotated as:

**`QUEST_COMPLETION_FINALE -> EVENT_10_CINEMATIC -> RESTART_CODE_CYCLE_WITHOUT_CHARS`**

## Next checkpoint

Classify the event-10 character-record predicate sufficiently to explain the `CINE10` versus `CINE11` ending variant, then correlate the remaining status-12 producers (`0xCCE0`, `0x23E10`) against their numbered events/UI/resource identities.