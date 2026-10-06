# R.EXE / CODE.1 / CODE.2 — CHARS and Exit-Protocol Audit

Status: **ACTIVE — evidence boundary recorded**

## Purpose

This checkpoint cross-correlates the launcher state machine with the immutable CODE binaries and the completed CODE reconstructions. It deliberately separates proven facts from still-unresolved gameplay labels.

## Proven launcher protocol

R.EXE initializes `status = 10` and supervises the protected-mode children as follows:

```text
while status == 10:
    status = run_and_wait("code.2")
    launcher_prerequisite_check()

    if status == 10:
        status = run_and_wait("code.1")
    else if status == 12:
        delete("CHARS")
        status = 10

launcher_prerequisite_check()
delete("CHARS")
```

Therefore the launcher-level meanings are proven without assigning gameplay names:

- `10`: continue the two-module cycle; specifically CODE.2 -> CODE.1 and CODE.1 -> CODE.2 when the returned status remains 10.
- `12` returned by CODE.2: delete `CHARS` and restart at CODE.2.
- any other status reaching the outer test: leave the module cycle and proceed to launcher shutdown.

## CHARS exists in all three runtime participants

Direct byte search of the immutable supplied files establishes:

| Binary | physical file offset | bytes |
|---|---:|---|
| `R.EXE` | `0x2BCE` / `0x2BD4` | `CHARS\0` |
| `CODE.1` | `0x3E7B8` | `CHARS\0` |
| `CODE.2` | `0x76634` | `CHARS\0` |
| `CODE.2` | `0x77064` | `CHARS\0` (preceded by a space in the adjacent string region) |

R.EXE's references are already proven operands of DOS file deletion. The CODE occurrences prove that CHARS is not merely a launcher-private filename: both protected-mode modules carry the name in their application data/string regions.

## Context evidence

The CODE.1 `CHARS` string is located in the Ravenloft application string region near `INIT`, `OBJECTS.DAT`, `.RES`, and `Ravenloft` rather than in the outer DOS/4G extender text.

CODE.2's first `CHARS` occurrence is likewise adjacent to application-level filenames including `NPCS.DAT`, `OBJECTS.DAT`, and `INIT`, followed by game/training text. Its second occurrence is in another application string cluster near `COLORS`, `RES`, and `Ravenloft`.

This materially strengthens the interpretation of CHARS as a game-owned transient data artifact shared across module boundaries. It does **not**, by itself, prove its record format or which module creates versus consumes it.

## Important correction to methodology

A textual search of the reconstructed C++ is insufficient for locating the child termination statuses. The faithful reconstructions frequently preserve machine behavior through boundary abstractions, runtime-tail models, and address-accountable functions rather than emitting source spelling such as `exit(10)` or `return 12`. Therefore absence of those literal strings in `Code1.cpp` / `Code2.cpp` is not evidence that the exit paths are absent.

The protocol must be resolved from the protected-mode termination/runtime paths and their callers in the immutable binaries, then mapped back to the reconstructed functions.

## Current evidence classification

### CONFIRMED

1. R.EXE is the supervisor; CODE.1 and CODE.2 are child protected-mode executables.
2. Status 10 is the launcher's continue/switch-module status.
3. CODE.2 status 12 causes CHARS deletion and a restart at CODE.2.
4. R.EXE deletes CHARS again at final shutdown.
5. `CHARS` is embedded in R.EXE, CODE.1, and CODE.2.
6. The CODE occurrences reside in Ravenloft application string/data regions, not merely generic DOS/4G runtime text.

### NOT YET CERTIFIED

The following labels remain prohibited until their producer paths are traced from the immutable CODE bytes:

- status 10 = "character generator transition";
- status 10 = "enter game" or "return to menu";
- status 12 = "new party", "reset characters", or equivalent;
- CODE.1 is exclusively the character generator;
- CODE.2 is exclusively the gameplay engine;
- CHARS record layout, ownership, or persistence semantics.

Some of these interpretations may ultimately be correct, but the archive will not promote them from hypothesis to fact without call-path evidence.

## Next binary work

1. Locate the protected-mode process-termination service/runtime tail in CODE.1 and enumerate all callers that can supply status 10.
2. Repeat for CODE.2, enumerating status 10 and status 12 producers.
3. Trace CODE.1's `CHARS` filename reference to its file-open/create/read/write callers.
4. Trace both CODE.2 `CHARS` references and classify each operation.
5. Correlate those file operations with the termination paths.
6. Only then assign semantic names to the protocol and reconstruct the CHARS layout if the file carries structured state.

## Preservation consequence

A source port should not model CODE.1 and CODE.2 as unrelated programs. Even before the remaining semantic labels are known, the shipped architecture proves a supervised state machine with filesystem-mediated state (`CHARS`) and child-process status-mediated transitions. A faithful port may collapse the processes into one executable, but it must preserve the equivalent state transitions and transient-state lifecycle.