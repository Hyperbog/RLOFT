# Character-status ending predicate reconstruction

Status: **CORRECTED / BINARY-PROVEN GAMEPLAY SEMANTICS.**

This document consolidates the later status-bit ownership work and supersedes the earlier conservative checkpoint.

## 1. Certified record geometry

The active-party image begins at runtime `0x218E0`; each character record is `0x248` bytes. The 16-bit status word is at:

```text
runtime 0x218F0 = record +0x10
```

Four records are examined across exactly `0x920` bytes.

## 2. Exact event-10 predicate

CODE.2 logical `0x13B6E..0x13BA5` counts a party record only when:

```c
uint16_t status = character.status_word;
status < 0x4000u && (status & 0x0080u) == 0
```

The `< 0x4000` behavior is effectively unsigned because the machine first zero-extends the 16-bit status into 32-bit `EBX` before the compare. An `int16_t` cast is not equivalent.

## 3. Certified status meanings

Independent producer/consumer audits establish:

```text
0x0080  party-template / recruited-character provenance
0x4000  zero-HP death state; lethal HP depletion sets it, revival clears it
0x8000  empty/unoccupied active-party slot
```

Current HP is runtime `0x218FC`, i.e. record `+0x1C`.

## 4. Finale selection

If at least one record satisfies the predicate, event 10 remains 10 and opens `CINE10.ANM`. If none satisfies it, the event number is rewritten to 11 and opens `CINE11.ANM`.

Therefore:

```text
CINE10.ANM
    at least one living original/non-recruited party character remains

CINE11.ANM
    no living original/non-recruited party character remains
```

The selector is not a generic good/bad-ending flag. It specifically tests survival of the original/non-template party-character class.

## 5. Preservation consequence

Preserve the original 16-bit status word for compatibility and debugging, but model the proven meanings as distinct typed state: character provenance, zero-HP death state, and slot occupancy.
