# R.EXE Structural Reconstruction — Phase 1

Status: **CONFIRMED / ACTIVE**

## Binary identity

- File: `R.EXE`
- Size: 12,368 bytes (`0x3050`)
- SHA-256: `a6d5874dc91fb068a3ed0a07290547c310ee7f4f50be4d8176c4788f251631b0`
- Format: 16-bit DOS MZ executable
- Compiler-family evidence: embedded `Borland C++ - Copyright 1991 Borland Intl.` runtime string and Borland-style runtime code.

This corrects an important early architectural assumption: **R.EXE itself is not the 32-bit game image. It is a small 16-bit DOS launcher/supervisor. `CODE.1` and `CODE.2` are the DOS/4G protected-mode game executables.**

## MZ header

Observed header fields:

| Field | Value |
|---|---:|
| signature | `MZ` |
| bytes in last page | `0x0050` |
| 512-byte pages | `0x0019` |
| relocation entries | 24 |
| header paragraphs | `0x0020` = 512 bytes |
| minimum allocation | 0 |
| maximum allocation | `0xffff` |
| initial SS | `0x02dd` |
| initial SP | `0x0080` |
| initial IP | `0x0000` |
| initial CS | `0x0000` |
| relocation-table offset | `0x003e` |

The MZ-declared image size is exactly `0x3050`, equal to the physical file size. There is no appended protected-mode payload hidden after the DOS image.

The executable entry instruction is therefore at physical file offset `0x0200` (512-byte header + CS:IP 0000:0000).

## Startup/runtime character

The entry path is conventional 16-bit DOS/Borland startup code. It obtains the DOS version with INT 21h/AH=30h, initializes data/runtime state, resizes its DOS memory block with INT 21h/AH=4Ah, and initializes C runtime facilities.

The binary also contains the messages:

- `Cannot find WORK subdirectory`
- `No Mouse Installed.  Ravenloft requires a mouse to run.`

These are launcher-level prerequisites, not strings recovered from CODE.1/CODE.2.

## Proven data-segment map

The launcher data region places the following strings at the offsets used by the recovered main routine:

| DS offset | File offset | String |
|---:|---:|---|
| `0x0238` | `0x2b18` | `WORK` |
| `0x024c` | `0x2b2c` | `DOS/4GW Professional Protected Mode Run-time  Version 1.96` |
| `0x0288` | `0x2b68` | `Copyright (c) Rational Systems, Inc. 1990-1994` |
| `0x02b8` | `0x2b98` | `DOS4G=QUIET` |
| `0x02c4` | `0x2ba4` | `WORK` |
| `0x02c9` | `0x2ba9` | `AUTOMAPS` |
| `0x02d2` | `0x2bb2` | `code.2` |
| `0x02d9` | `0x2bb9` | `code.2` |
| `0x02e0` | `0x2bc0` | `code.1` |
| `0x02e7` | `0x2bc7` | `code.1` |
| `0x02ee` | `0x2bce` | `CHARS` |
| `0x02f4` | `0x2bd4` | `CHARS` |
| `0x02fa` | `0x2bda` | `DOS4G=` |

The duplicated CODE and CHARS strings are not incidental: both members of each pair are passed to runtime process/file routines.

## Recovered launcher main path

The principal game-launch routine begins at physical `0x27d1` (load-module offset `0x25d1`). Its behavior can now be stated with high confidence:

```text
if argc != 1:
    return

perform launcher/runtime prerequisite checks
construct/prepare DOS4G environment state
print DOS/4GW Professional Runtime 1.96 banner/copyright
prepare DOS4G=QUIET state
create WORK directory
create AUTOMAPS directory
perform launcher prerequisite check

status = 10
while status == 10:
    status = spawn/wait("code.2", argv0="code.2", ...)
    perform launcher prerequisite check

    if status == 10:
        status = spawn/wait("code.1", argv0="code.1", ...)
    else if status == 12:
        unlink("CHARS")
        status = 10

perform launcher prerequisite check
unlink("CHARS")
clear/reset DOS4G environment state
return
```

Names above are semantic descriptions; C-library symbol names remain subject to final symbol matching. The control flow and string operands are binary-proven.

## Directory creation is proven

The calls at physical `0x2852` and `0x285d` pass DS offsets `0x02c4` and `0x02c9`. Their target resolves to a Borland DOS wrapper issuing:

```asm
mov ah,39h
mov dx,[bp+6]
int 21h
```

INT 21h/AH=39h is DOS `mkdir`, proving that `R.EXE` attempts to create:

1. `WORK`
2. `AUTOMAPS`

This is launcher behavior, not CODE-module behavior.

## CHARS deletion is proven

The routine invoked with DS offset `0x02ee` / `0x02f4` resolves to a DOS wrapper issuing:

```asm
mov ah,41h
mov dx,[bp+6]
int 21h
```

INT 21h/AH=41h deletes a file. Therefore the launcher explicitly deletes `CHARS`:

- when CODE.2 returns status 12, before restarting the module cycle;
- again during final launcher shutdown.

This establishes `CHARS` as a transient/inter-module/runtime file whose lifecycle is partly owned by `R.EXE`.

## CODE.1 / CODE.2 are child processes, not overlays loaded by R.EXE

The CODE dispatch calls enter the Borland process-spawn family. The wrapper at physical `0x1aff` selects an internal process routine based on mode and passes the child path/argument vector. The lower process implementation ultimately uses DOS child-process services and returns the child's termination status to the launcher.

Thus the architecture is now:

```text
RLOFT.BAT
    |
    v
R.EXE                    16-bit Borland launcher/supervisor
    |
    +-- creates WORK
    +-- creates AUTOMAPS
    +-- prepares DOS4G environment
    +-- checks launcher prerequisites
    |
    +--> CODE.2           DOS/4G protected-mode executable
    |       |
    |       +-- exit status drives R.EXE state machine
    |
    +--> CODE.1           DOS/4G protected-mode executable
            |
            +-- exit status drives R.EXE state machine

R.EXE also owns cleanup/restart behavior involving CHARS.
```

This means a modern source port does **not** need to reproduce R.EXE as a DOS executable. It needs to reproduce its orchestration semantics wherever those semantics remain relevant: startup directories, prerequisite behavior, CODE.1/CODE.2 phase transitions, transient CHARS cleanup, and any state communicated through child exit codes/files.

## Exit-code protocol — current proven boundary

Two child return values have explicit launcher meanings:

- **10 (`0x0a`)** — continue/re-enter the CODE module cycle. After CODE.2 returns 10, R.EXE runs CODE.1; after CODE.1 returns 10, the outer loop returns to CODE.2.
- **12 (`0x0c`) from CODE.2** — delete `CHARS`, force status back to 10, and restart the cycle.

The exact gameplay meaning of statuses 10 and 12 must be correlated against the already reconstructed exit sites in CODE.1 and CODE.2 before assigning names such as `CHANGE_MODULE`, `NEW_CHARACTER`, etc. Until then those semantic labels are intentionally withheld.

## DOS/4GW relationship

The DOS/4GW 1.96 banner/copyright is embedded in R.EXE because the launcher participates in preparing/announcing the protected-mode child runtime. The actual uploaded `CODE.1` and `CODE.2` binaries independently identify as DOS/4G-bound DOS executables. They share the same MZ prefix bytes at their beginning and carry the protected-mode game images already reconstructed elsewhere in this repository.

Contemporary/successor Watcom documentation is consistent with the observed architecture: DOS/4GW protected-mode applications can be spawned and waited upon, with the child's exit code returned to the parent. This is supporting platform context only; the Ravenloft control flow above is established directly from the shipped bytes.

## Next reconstruction targets

1. Cross-reference every CODE.1 and CODE.2 termination site that returns 10 or 12 and assign the protocol's actual gameplay semantics.
2. Resolve the prerequisite/check routine called around every module transition, including the mouse requirement.
3. Recover the exact DOS4G environment construction/reset sequence.
4. Determine the complete `CHARS` producer/consumer contract across R.EXE, CODE.1 and CODE.2.
5. Produce a compact C/C++ launcher reconstruction after the above semantics are named from evidence.
