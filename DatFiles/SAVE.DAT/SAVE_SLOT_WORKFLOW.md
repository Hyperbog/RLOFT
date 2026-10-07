# Ravenloft SAVE-slot directory workflow

Status: **executable-certified from immutable CODE.2 save/load slot paths.**

This document closes the outer save-slot workflow surrounding the already-certified `SAVE.DAT` serializer and standalone `LEVnnn.DAT` level snapshots.

## 1. Architecture

The runtime does not treat a numbered save slot as one monolithic file.

The live working game state is held under:

```text
WORK\
    SAVE.DAT
    LEVnnn.DAT
    ...other working files matched by *.*
```

A numbered save is a directory snapshot of that working set:

```text
SAVE00\   dedicated quicksave slot

SAVE01\
...
SAVE10\   ten ordinary selectable slots
```

The immutable CODE.2 data object contains the mutable pathname template:

```text
SAVE??\SAVE.DAT
```

at DGROUP offset `0x668C`, physical CODE.2 file offset `0x7C5B0`.

It also contains:

```text
WORK       DGROUP 0xC020
\         DGROUP 0x0398
SAVE.DAT   DGROUP 0x039C
??????????? DGROUP 0x03B0
\*.*      DGROUP 0x03BC
..         DGROUP 0x03C4
```

These strings and the routines below reconstruct the complete directory-copy workflow.

## 2. SAVE.DAT itself is written to WORK

Serializer `066E8..` constructs its output path by concatenating:

```text
WORK
+
\
+
SAVE.DAT
```

into the temporary path buffer at `0x12990`.

Therefore the primary serializer writes:

```text
WORK\SAVE.DAT
```

The serializer argument is the save-description/slot-label selector, not the output-directory selector.

Its special argument `-1` copies only five bytes from `TEMP` into the otherwise existing 50-byte label buffer before writing the full 50 bytes; this odd encoded behavior remains preserved by the certified decompilation.

Source reconstruction:
- `Code2/Code2.cpp`
- `Code2/FINAL_SEMANTIC_AUDIT.md`, checkpoints covering `066E8..0706D`
- `DatFiles/SAVE.DAT/GLOBAL_STATE.md`

## 3. Current level is snapshotted before saving

The save workflow at logical `077A0` begins:

```text
push current_level (DGROUP 0xC6C8)
A79D = 0
call 171FC
A79D = 1
call 066E8(0)
```

Routine `171FC(current_level)` is the standalone level writer documented in the LEVELS/LEV reconstruction.

Thus a quicksave first synchronizes the currently active level into:

```text
WORK\LEVnnn.DAT
```

then writes global/party state to:

```text
WORK\SAVE.DAT
```

The ordinary save UI follows the same persistence model before/while packaging the selected slot.

This establishes the user-save state as a coordinated pair of persistence layers:

```text
WORK\SAVE.DAT
    global/party/cross-level state

WORK\LEVnnn.DAT
    per-level mutable snapshots
```

## 4. Directory helper identities

CODE.2 wrappers:

```text
339A2 / 339CF   DOS find-first/find-next family
33A38           INT 21h AH=39h  mkdir
33A5A           INT 21h AH=3Bh  chdir
```

The path copier at `073A0..` is a wildcard file copier.

It receives:

```text
arg1 source_directory
arg2 destination_directory
```

and constructs:

```text
source_directory + "\*.*"
```

for DOS enumeration.

For every match, it constructs:

```text
source_directory + "\" + filename

destination_directory + "\" + filename
```

It reads the complete source file into allocated memory, creates the destination, writes the complete source size, and requires the destination write count to equal the source size.

The certified audit records one asymmetry: source short-read results are not independently enforced before the destination-size check.

## 5. Directory clearing uses FCB wildcard delete

Routine `07378` initializes an FCB using:

```text
???????????
```

and executes DOS `INT 21h / AH=13h` (FCB delete).

It is called only after changing into the directory to clear.

Thus save/load replacement clears a directory's existing files before copying the new snapshot into it.

## 6. Quicksave is SAVE00

Logical workflow `077A0..` explicitly sets:

```text
SAVE??\SAVE.DAT

? #1 = '0'
? #2 = '0'
```

and temporarily writes NUL at the template's backslash, producing the directory string:

```text
SAVE00
```

It then:

1. creates `SAVE00` if possible/needed;
2. changes into `SAVE00`;
3. deletes all files in that directory;
4. changes to `..`;
5. copies `WORK\*.*` into `SAVE00\`.

The quicksave serializer call is:

```text
066E8(0)
```

and description table entry 0 at DGROUP `0x6461` is the shipped string:

```text
QUICKSAVE
```

Therefore slot 0 is the dedicated quicksave slot.

## 7. Quickload restores SAVE00 into WORK

Logical workflow `07874..` builds directory `SAVE00`.

It then:

1. verifies/chdirs into `SAVE00`;
2. returns to `..`;
3. changes into `WORK`;
4. deletes all existing WORK files;
5. returns to `..`;
6. copies:
   ```text
   SAVE00\*.* -> WORK\
   ```
7. restores the template backslash;
8. falls into the common SAVE.DAT load path.

The common load path at `07938..` explicitly constructs:

```text
WORK\SAVE.DAT
```

and opens that file.

Thus quickload does not load `SAVE00\SAVE.DAT` in place. It first makes the selected slot the new complete `WORK` snapshot, then restores the game from WORK.

## 8. Ordinary slots are SAVE01 through SAVE10

The save-slot selector at `079E0..` maps its ten visible rows to zero-based row index `0..9`.

Before serialization it increments that selection:

```text
slot_number = row_index + 1
```

so the ordinary domain is:

```text
1..10
```

The directory template digits are generated conventionally:

```text
tens = slot_number / 10
ones = slot_number % 10
```

giving:

```text
SAVE01
SAVE02
...
SAVE09
SAVE10
```

The save description passed to `066E8` is the corresponding 50-byte slot-label table entry.

## 9. Ordinary save workflow

After the user selects/edits a normal slot label:

1. `066E8(slot_number)` writes the new `WORK\SAVE.DAT`;
2. CODE.2 builds `SAVE##`;
3. it creates/chdirs into that directory;
4. existing slot files are deleted;
5. it returns to the parent directory;
6. wildcard copier performs:
   ```text
   WORK\*.* -> SAVE##\
   ```
7. UI resources/handles are torn down and the save UI returns.

Thus the numbered slot is a complete copy of the current working persistence directory.

## 10. Ordinary load workflow

The load selector at `07B94..` similarly converts visible row `0..9` to slot number `1..10`, constructs `SAVE##`, and then:

1. verifies the selected slot directory;
2. changes into `WORK`;
3. deletes existing WORK files;
4. returns to the parent;
5. wildcard-copies:
   ```text
   SAVE##\*.* -> WORK\
   ```
6. invokes common loader `07938..`;
7. common loader opens `WORK\SAVE.DAT`;
8. the SAVE deserializer restores global/party state;
9. final load restoration invokes the normal level load path, which now sees the selected slot's restored `WORK\LEVnnn.DAT` snapshots.

This is the key persistence ownership chain:

```text
numbered slot directory
    |
    | restore all files
    v
WORK directory
    |
    +-- SAVE.DAT       global/party state
    |
    +-- LEVnnn.DAT     mutable level state
    |
    v
runtime reconstruction
```

## 11. Save descriptions are 50 bytes per slot

The save/load UI holds a 50-byte description table beginning at DGROUP `0x6461`.

Entry 0 is `QUICKSAVE`; ordinary slots use the following entries.

Editor routine `075CC..` edits a selected label in place. The binary preserves several non-obvious behaviors:

- ESC cancels;
- Enter accepts;
- Backspace removes a character only when length is nonzero;
- display-width checks are performed before append;
- on the first edit, the inherited 50-byte label is cleared **before** character classification once the key reaches the printable path;
- maximum accepted string length is bounded below 50 bytes.

The certified semantic audit corrected the exact first-edit clear ordering; a native UI should preserve the saved 50-byte field even if it presents a modern text editor.

## 12. Save/load error behavior is asymmetric

The final semantic audit records:

- SAVE serializer short-write failures close the SAVE handle and return failure;
- wildcard-copy errors have dedicated message paths;
- save and load UI workflows do not report every failure identically;
- the right-click/cancel exits in the two slot-selection loops intentionally preserve encoded handle-leak behavior in the original runtime.

A native port should reproduce user-visible success/failure semantics in its faithful mode without deliberately leaking host resources. The leak itself is an implementation artifact; the observable control-flow/result behavior is the compatibility target.

## 13. Relationship to MAKSAV.BAT

The supplied `MAKSAV.BAT` creates `SAVE01`, copies `LEV022.DAT` and `SAVE.DAT` into it, then deletes the working copies.

That batch file is installation/quick-start packaging evidence and is consistent with the runtime directory architecture reconstructed above:

```text
SAVE## directory
    contains SAVE.DAT plus LEV snapshots
```

The runtime CODE.2 workflow is nevertheless the controlling source for interactive save/load behavior.

## 14. Native compatibility rule

A faithful port may represent a save slot as a modern directory, archive, or managed object internally, but its legacy compatibility layer should preserve the original ownership model:

```text
WorkingPersistenceSet
    SAVE.DAT
    LEV snapshots

SaveSlot[0]
    quicksave

SaveSlot[1..10]
    ordinary user slots
```

Saving must synchronize the current level before packaging the slot. Loading must restore the selected slot's per-level snapshots before/with global game-state restoration so the normal level loader observes the correct mutable state.

Do not treat `SAVE.DAT` alone as a complete Ravenloft saved game.
