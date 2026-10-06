# Ravenloft: Strahd's Possession — Execution Lifecycle and CODE Relationship

Status: **PHASE I / BASELINE 02 — RECONSTRUCTED AND EVIDENCE-BOUND**  
Scope: DOS supervisor lifecycle, CODE.1/CODE.2 process relationship, CHARS transfer contract, application transition statuses, and preservation consequences.

## 1. Purpose

This unit establishes how the shipped Ravenloft DOS runtime is actually divided across `R.EXE`, `CODE.1`, and `CODE.2`. It replaces the misleading monolithic-executable model with the supervisor-controlled process cycle demonstrated by the original binaries.

The document describes the transition mechanism separately from gameplay causes. Numeric process statuses are historical transport primitives; they are not interchangeable with user-facing states such as "game over", "new game", or "ending".

## 2. Top-level runtime chain

Claim: Ravenloft uses a small 16-bit DOS supervisor to launch two protected-mode application modules as child processes.

Class: **CONFIRMED / RECONSTRUCTED**

Evidence:
- `R.EXE`: 12,368-byte 16-bit MZ executable; main launcher path begins at physical `0x27D1`.
- `Runtime/R_EXE/01_STRUCTURAL_RECONSTRUCTION.md`.
- CODE.1 and CODE.2 independently contain their protected-mode DOS/4G executable images and have separately certified object maps.

```text
RLOFT.BAT
    |
    v
R.EXE                         16-bit Borland supervisor
    |
    +-- prepare DOS/4G environment
    +-- create WORK
    +-- create AUTOMAPS
    +-- launcher prerequisite checks
    |
    +--> CODE.2               protected-mode application module
    |      |
    |      +-- launcher-visible status controls next transition
    |
    `--> CODE.1               protected-mode application module
           |
           +-- launcher-visible status controls next transition
```

`R.EXE` does not contain an appended protected-mode game image. Its MZ-declared size equals its physical file size. The CODE modules are spawned child programs, not overlays mapped into the launcher process.

## 3. R.EXE supervisor lifecycle

Claim: `R.EXE` owns startup directories, child-process orchestration, transition-status interpretation, and `CHARS` cleanup.

Class: **CONFIRMED**

Binary-proved launcher behavior:

```text
if argc != 1:
    return

perform prerequisite/runtime setup
prepare DOS4G environment state
create WORK
create AUTOMAPS

status = 10
while status == 10:
    status = spawn_and_wait("code.2")

    if status == 10:
        status = spawn_and_wait("code.1")
    else if status == 12:
        delete("CHARS")
        status = 10

final prerequisite/cleanup path
delete("CHARS")
reset DOS4G environment state
return
```

Evidence details:
- `R.EXE` physical `0x2852` and `0x285D` call the DOS `INT 21h/AH=39h` wrapper with `WORK` and `AUTOMAPS`, proving directory creation.
- The launcher contains duplicate `code.2`, `code.1`, and `CHARS` pathname strings used by process/file routines.
- Its `CHARS` deletion wrapper executes DOS `INT 21h/AH=41h`.
- The CODE dispatch reaches the Borland spawn/wait process family and returns child termination status to the supervisor.

The launcher is therefore architecturally significant even though a native port need not reproduce it as a DOS executable.

## 4. CODE.1 and CODE.2 relationship

Claim: CODE.1 and CODE.2 are distinct process phases connected by explicit supervisor transitions and a serialized party-state handoff.

Class: **RECONSTRUCTED from confirmed process/file paths**

The shipped architecture does **not** justify treating the CODE files as interchangeable halves of one flat address space. They have:
- different executable identities and object maps;
- different startup/termination paths;
- independent static data regions;
- a process boundary between executions;
- an explicit serialized transfer artifact (`CHARS`).

The safest functional distinction currently established is:

- **CODE.2** owns the main gameplay-side runtime state and is the first protected-mode module launched by the supervisor cycle.
- **CODE.1** contains the character-state workflow used by the principal handoff path back to CODE.2.

This is a runtime-role statement, not a claim about original DreamForge source directory names or that CODE.1 contains only character-generation behavior.

## 5. CHARS is the module-boundary serialization contract

Claim: `CHARS` is a transient cross-process transfer file containing exactly four serialized character records.

Class: **CONFIRMED**

### 5.1 CODE.1 producer

CODE.1 routine `0x4EF0` / late file sequence `0x4FBF..0x500D`:

1. deletes an old `CHARS` pathname;
2. creates/reinitializes the file;
3. closes it;
4. reopens it;
5. transfers exactly `0x920` bytes from the character-state image;
6. closes the file.

Relevant low-level wrappers resolve to DOS delete/open/create/close and handle-I/O services.

### 5.2 CODE.2 existence gate and consumer

CODE.2 logical `0x204B3` tests the second `CHARS` pathname using DOS file-attribute query behavior:

```text
CHARS absent   -> alternate initialization path at 0x19888
CHARS present  -> consumer at 0x16CD0
```

CODE.2 `0x16CD0`:

1. opens `CHARS`;
2. transfers `0x920` bytes into active-party storage at DGROUP `0x218E0`;
3. closes the file;
4. performs post-load processing.

### 5.3 Payload geometry

The transfer size closes exactly against the live character record geometry:

```text
4 party records * 0x248 bytes = 0x920 bytes
```

Thus `CHARS` is not a persistent save format. It is the original process-boundary serialization mechanism for active party state.

Preservation consequence: even if a port replaces the physical file with an in-memory typed object, compatibility tooling should retain the exact `0x920` legacy representation and four-record boundary.

## 6. Status 10 — continue the module cycle

Claim: DOS child status `10` (`0x0A`) is the supervisor protocol value **CONTINUE_MODULE_CYCLE**.

Class: **CONFIRMED / RECONSTRUCTED**

### 6.1 Launcher behavior

`R.EXE` uses status 10 as the loop-continuation value:

- CODE.2 returns 10 -> launch CODE.1;
- CODE.1 returns 10 -> loop and launch CODE.2 again.

### 6.2 CODE.1 producer and CHARS coupling

CODE.1 application entry routine `0x508C` calls state routine `0x4978`; its return value is passed unchanged to non-returning controlled termination routine `0x5014`.

`0x4978` has a certified path returning `0x0A`. `0x5014` explicitly special-cases status 10 and, when the required two character-state presence flags are set, invokes the `CHARS` producer before forwarding the same status into process termination.

CODE.1 also has an independent direct `push 0Ah ; call 5014` path at `0x9B1E..0x9B20`.

The principal handoff is therefore:

```text
CODE.1 character-state workflow
    -> completed transfer condition
    -> serialize 4 x 0x248 bytes to CHARS
    -> process status 10
    -> R.EXE continues module cycle
    -> CODE.2 tests/imports CHARS
```

Status 10 remains a protocol-level continuation result; not every producer should be renamed to a single user-facing action.

## 7. Status 12 — restart without transferred party state

Claim: DOS child status `12` (`0x0C`) is the supervisor primitive **RESTART_CODE_CYCLE_WITHOUT_CHARS**.

Class: **CONFIRMED / RECONSTRUCTED**

CODE.2 controlled-exit helper `0x205B4` explicitly compares its requested status with both `0x0A` and `0x0C`; those two transition values bypass an additional ordinary-termination reset and are passed unchanged into the process-termination path.

The Watcom termination path ultimately exposes the result through DOS `INT 21h/AH=4Ch` at CODE.2 `0x35666..0x35668`, making the value visible to `R.EXE`'s spawn/wait result.

When `R.EXE` receives status 12 from CODE.2 it:

```text
deletes CHARS
normalizes supervisor state to 10
restarts the module cycle
```

The same transition primitive has at least two certified gameplay causes:

### 7.1 No-active-party / party-wipe path

CODE.2 tests the four party records. When no active character remains, it performs the terminal failure presentation and requests status 12.

Relevant independently reconstructed character-status meanings include:
- `0x4000`: death/zero-HP state;
- `0x8000`: empty/unoccupied party slot.

### 7.2 Quest-completion finale

The live finale path checks possession of five required objects, performs presentation teardown, invokes event 10, then requests status 12.

Event 10 selects `CINE10.ANM` or `CINE11.ANM` based on the party predicate documented in `Runtime/STATE_TRANSITION_PROTOCOL.md` and `Runtime/CINE10_CINE11_VARIANT_PREDICATE.md`.

Therefore status 12 must not be globally renamed `GAME_OVER`, `ENDING`, or `NEW_GAME`: both failure and successful completion use the same supervisor mechanism.

## 8. CODE.2 startup and controlled termination anchors

Claim: the CODE.2 application lifecycle can be anchored independently from the launcher protocol.

Class: **CONFIRMED**

Key logical addresses:

```text
0x353A0   Watcom/protected-mode startup entry
0x20608   application-main path reached through startup/runtime
0x204B3   CHARS existence/access gate
0x16CD0   CHARS 0x920-byte consumer
0x205B4   controlled application exit helper
0x35666   DOS AH=4Ch process-termination tail
```

These anchors demonstrate that launcher-visible state transitions are application-owned exits carried through the CRT/DOS boundary, not launcher guesses about crashes or error codes.

## 9. Lifecycle state machine

Class: **RECONSTRUCTED**

```text
                         +-------------------------+
                         |          R.EXE          |
                         |  supervisor / cleanup   |
                         +------------+------------+
                                      |
                                      | initial / continue
                                      v
                               +-------------+
                               |   CODE.2    |
                               |  gameplay   |
                               +------+------+ 
                                      |
                       status 10      |      status 12
                                      |             |
                                      v             v
                               +-------------+   delete CHARS
                               |   CODE.1    |       |
                               | char-state  |       |
                               |  workflow   |       |
                               +------+------+       |
                                      |              |
                           status 10 + CHARS          |
                                      |              |
                                      +-------> R.EXE +
                                                 |
                                                 +--> CODE.2
```

Ordinary non-transition termination values leave this continuation/restart cycle and proceed through launcher final cleanup.

## 10. Ownership and persistence boundaries

### Supervisor-owned

**CONFIRMED**
- creation of `WORK` and `AUTOMAPS`;
- child-process sequencing;
- interpretation of child status 10/12;
- deletion of `CHARS` on status-12 restart and final launcher exit;
- DOS/4G launcher environment preparation/reset.

### CODE.1-owned in the proved handoff

**CONFIRMED / RECONSTRUCTED**
- completion tests for its character-state workflow;
- production of the four-record `CHARS` image;
- status-10 process transition back to the supervisor.

### CODE.2-owned

**CONFIRMED / RECONSTRUCTED**
- CHARS existence test/import;
- main gameplay-side runtime state represented by the reconstructed CODE.2 systems;
- controlled transition helper recognizing statuses 10 and 12;
- party-wipe and finale status-12 causes;
- ordinary save/level/resource systems documented elsewhere in the repository.

## 11. Porting boundary

A faithful native implementation should preserve the **state machine**, not necessarily the DOS process mechanics.

Safe modernization boundary:

```cpp
enum class ModuleResult {
    ContinueModuleCycle,             // legacy DOS status 10
    RestartWithoutTransferredParty,  // legacy DOS status 12
    OrdinaryTermination
};
```

A native runtime may:
- combine R.EXE/CODE.1/CODE.2 into one host process;
- replace spawn/wait with typed transitions;
- replace `CHARS` file I/O with an in-memory four-character transfer object.

It must not erase:
- the historical distinction between the CODE modules;
- the 4 × 0x248 serialization contract;
- restart behavior that deliberately discards `CHARS`;
- the fact that multiple gameplay causes share status 12;
- any compatibility import/export path requiring original serialized state.

## 12. Historical unknowns retained

The following remain intentionally unresolved:

- the original DreamForge source/module names corresponding to CODE.1 and CODE.2;
- the development/build rationale for splitting the application into two protected-mode child programs;
- the exact historical symbolic names for supervisor results 10 and 12;
- whether other Ravenloft releases used additional transition causes or different launcher rules;
- the exact intended source-level abstraction for `CHARS` beyond its proved four-character transfer role.

These unknowns do not block a faithful reconstruction of the shipped runtime lifecycle.

## 13. Canonical evidence

Primary repository evidence for this unit:

- `Runtime/R_EXE/01_STRUCTURAL_RECONSTRUCTION.md`
- `Runtime/R_EXE/02_CHARS_AND_EXIT_PROTOCOL.md`
- `Runtime/CHARS_XREF_RECONSTRUCTION.md`
- `Runtime/CODE1_STATUS10_PROPAGATION.md`
- `Runtime/CODE2_STATUS12_OWNERSHIP.md`
- `Runtime/EXIT_STATUS_PRODUCERS.md`
- `Runtime/STATE_TRANSITION_PROTOCOL.md`
- `Runtime/CINE10_CINE11_VARIANT_PREDICATE.md`
- `Code1/FINAL_BINARY_AUDIT.md`
- `Code2/FINAL_CERTIFICATION.md`
- `Resources/FINAL_RESOURCE_AUDIT.md`

## 14. Next evidence unit

`TechnicalArchive/03_RENDERING_RESOURCE_AND_DATA_ARCHITECTURE.md`

The next Phase-I unit should integrate the now-certified `Resources/` workstream with CODE.2 rendering/resource consumers and the reconstructed DAT structures. It should preserve the original renderer architecture — including RES3 compiled scene shapes, SINTAB executable micro-rasterizers, INIT source-bank translation, texture/profile resources, palettes, cinematics, and resource namespace bases — rather than reducing those mechanisms to generic modern "sprites" and "assets".
