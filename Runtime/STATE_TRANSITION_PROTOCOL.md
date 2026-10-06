# Ravenloft runtime state-transition protocol

Status: **CANONICAL CONSOLIDATION — binary-proven supervisor/module protocol.**

This document consolidates the certified runtime-control findings for `R.EXE`, `CODE.1`, `CODE.2`, and the transient `CHARS` transfer image. It supersedes provisional interpretations in earlier checkpoints where noted.

## 1. Runtime architecture

`R.EXE` is a 16-bit Borland DOS supervisor, not the protected-mode game engine. `CODE.1` and `CODE.2` are DOS/4G protected-mode application modules. The launcher creates `WORK` and `AUTOMAPS`, establishes the DOS/4G environment, spawns/waits for the CODE children, interprets their DOS termination statuses, and owns final/transition cleanup of `CHARS`.

```text
RLOFT.BAT
    -> R.EXE
         -> create WORK / AUTOMAPS
         -> CODE.2
         -> CODE.1 when CODE.2 returns 10
         -> repeat according to child status
```

The architecture is therefore a supervisor-controlled module cycle, not a single monolithic executable.

## 2. `CHARS`: transient cross-process state transfer

`CHARS` is a transient transfer artifact between CODE modules.

CODE.1's producer path deletes/reinitializes `CHARS`, reopens it, transfers exactly `0x920` bytes, and closes it. CODE.2 independently tests for the file, opens it when present, transfers the same `0x920` bytes into the active-party image, closes it, and performs post-load processing.

The payload is exactly four `0x248` character records:

```text
4 * 0x248 = 0x920
```

Consequently `CHARS` is not an ordinary persistent save file. It is the original DOS implementation of a module-boundary serialization contract.

## 3. Status 10: continue the module cycle

The certified protocol-level meaning is:

```text
STATUS_10 = CONTINUE_MODULE_CYCLE
```

The principal CODE.1 handoff is:

```text
completed character-state workflow
    -> serialize active party to CHARS (0x920)
    -> terminate CODE.1 with status 10
    -> R.EXE observes 10
    -> launch CODE.2
    -> CODE.2 imports CHARS
```

CODE.1's controlled termination machinery explicitly couples the normal status-10 transition with its CHARS producer. Status 10 should therefore become a typed module-transition result in a native port rather than an emulated DOS process exit.

## 4. Status 12: restart without transferred party state

The certified protocol-level meaning is:

```text
STATUS_12 = RESTART_CODE_CYCLE_WITHOUT_CHARS
```

When CODE.2 returns 12, R.EXE deletes `CHARS`, normalizes its supervisor state back to the continuation state, and restarts the CODE cycle. Status 12 describes the **mechanism**, not a single gameplay cause.

Two live gameplay causes are certified.

### 4.1 Party wipe / no active party

CODE.2 checks all four party records for unavailable states. When no active character remains it enters the terminal failure presentation, waits for acknowledgement, and requests status 12.

Relevant status semantics established independently include:

- `0x4000`: zero-HP/death state; lethal damage sets it and revival clears it while restoring HP;
- `0x8000`: empty/unoccupied party slot.

Thus the failure path is:

```text
NO ACTIVE PARTY
    -> terminal failure presentation
    -> status 12
    -> R.EXE deletes CHARS
    -> restart module cycle
```

### 4.2 Quest-completion finale

The live finale gate requires possession of five special objects:

- Lord Dhelt's Holy Symbol (`0x74`)
- Ancient Elven Crown (`0x5C`)
- Wereraven Feather (`0x76`)
- Vistani Tarokka Card (`0x8B`)
- Priceless Fire Ruby (`0x77`)

On success CODE.2 performs presentation teardown, invokes numbered event 10, then requests status 12. Event 10 deterministically selects `CINE10.ANM` or `CINE11.ANM` from party character status and provenance.

The exact qualifying predicate per party record is:

```c
status < 0x4000 && !(status & 0x0080)
```

`0x0080` is set by the predefined/recruited party-template installation path. Therefore `CINE10` is selected when at least one living original/non-recruited player-party character remains; `CINE11` is selected when none does.

```text
FIVE-OBJECT QUEST GATE
    -> event 10
    -> CINE10 or CINE11
    -> status 12
    -> R.EXE deletes CHARS
    -> restart module cycle
```

## 5. Unreferenced duplicate finale routine

CODE.2 routine `0x23DF0..0x23E1A` performs the same semantic finale tail: teardown, event 10, then status 12. However exhaustive static reachability work found no direct call/jump, recognized jump-table entry, stored absolute pointer, or LE relocation/fixup targeting `0x23DF0`.

It is therefore preserved as:

```text
UNREFERENCED DUPLICATE FINALE ROUTINE
```

No gameplay trigger should be invented for it. The five-object path remains the certified live finale producer.

## 6. Canonical supervisor state machine

A native reconstruction should model the observed behavior approximately as:

```cpp
enum class ModuleResult {
    ContinueModuleCycle,             // original DOS status 10
    RestartWithoutTransferredParty,  // original DOS status 12
    OrdinaryTermination
};

enum class RestartCause {
    PartyWipe,
    QuestCompletionFinale
};
```

Conceptually:

```text
START
  |
  v
CODE.2
  | status 10
  v
CODE.1 --status 10 / CHARS--> CODE.2
  ^                              |
  |                              | status 12
  |                              v
  +---------------------- delete CHARS
                                 |
                                 +--> restart cycle
```

The native implementation should preserve the `0x920` serialization boundary until the character schema and module ownership are fully reconstructed, even if the eventual port replaces the physical temporary file with an in-memory typed structure.

## 7. Preservation rules

1. Do not rename status 10 globally as a character-generation result; its protocol meaning is broader continuation.
2. Do not rename status 12 globally as `GAME_OVER`, `ENDING`, or `NEW_GAME`; both failure and successful finale use the same supervisor primitive.
3. Keep gameplay **cause** separate from supervisor **transition mechanism**.
4. Preserve `CHARS` as a documented compatibility serialization format even if a native runtime no longer needs the file itself.
5. Preserve unreferenced `0x23DF0` as original-code evidence, but do not expose it as a live gameplay transition without new reachability evidence.

## 8. Certification state

The following are now considered established for the shipped binary under audit:

- R.EXE supervisor role;
- CODE.1/CODE.2 child-module relationship;
- CHARS producer/consumer lifecycle and exact `0x920` transfer size;
- status-10 continuation protocol;
- status-12 restart-without-CHARS protocol;
- live party-wipe status-12 cause;
- live five-object quest-finale status-12 cause;
- event-10 CINE10/CINE11 selection predicate;
- `0x0080`, `0x4000`, and `0x8000` status distinctions relevant to that selection;
- `0x23DF0` preservation classification as an unreferenced duplicate absent contrary future evidence.

This closes the runtime state-transition protocol as an architectural reconstruction milestone. Further executable work should move outward to launcher/environment details, installer/audio-driver preservation, or integration of this protocol into the broader Technical Archive rather than reopening the state machine without contradictory binary evidence.
