# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 01 — Executable Containers, Protected-Mode Modules, and the Supervisor Cycle

**Status:** PRIMARY ARCHITECTURAL RECONSTRUCTION — VERIFIED AGAINST FROZEN REPOSITORY EVIDENCE AND IMMUTABLE EXECUTABLE IDENTITIES  
**Documentation branch:** `code1-decompilation`  
**Frozen pre-documentation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`  
**Container-map correction commit:** `b87ac1d6cb39c11b174824484a995b1b7cf6b5ee`

---

## 1. What the shipped program actually is

The DOS release is not one monolithic game executable.

At the outermost game-control level, `R.EXE` is a small 16-bit DOS supervisor. It creates working directories, launches protected-mode child modules, waits for their termination statuses, interprets two application-owned transition codes, deletes the transient `CHARS` handoff image when required, and repeats the module cycle. The two protected-mode application children are `CODE.1` and `CODE.2`. The launcher therefore implements a process-level state machine around two separate 32-bit application images. This architecture is proven directly from the shipped launcher and the two CODE modules rather than inferred from filenames. [Frozen runtime audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/FULL_THREAD_AUDIT_2026-10-06.md) [Canonical transition protocol](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/STATE_TRANSITION_PROTOCOL.md)

The architecture is:

```text
RLOFT.BAT
    |
    v
R.EXE                         16-bit DOS supervisor
    |
    +--> CODE.2               32-bit protected-mode application
    |       |
    |       +-- status 10 --> CODE.1
    |       |
    |       +-- status 12 --> delete CHARS, restart module cycle
    |
    +--> CODE.1               32-bit protected-mode application
            |
            +-- normal handoff:
                write CHARS
                return status 10
                resume CODE.2
```

This distinction matters to preservation. A future native implementation may choose to run these formerly separate modules inside one host process, but the original ownership boundaries, serialized transition state, and transition causes remain historically meaningful. Replacing the DOS process structure is an implementation decision; erasing it from the technical model would be a loss of original architecture.

**Evidence class:** CODE-PROVEN / CROSS-CORROBORATED.

---

## 2. `R.EXE`: supervisor, not game engine

The shipped `R.EXE` is 12,368 bytes and has SHA-256:

```text
a6d5874dc91fb068a3ed0a07290547c310ee7f4f50be4d8176c4788f251631b0
```

Its MZ-declared image size equals its physical file size. There is therefore no appended Ravenloft protected-mode application payload hidden after the DOS image. Its reconstructed main path begins with supervisor state 10, launches and waits for `CODE.2`, conditionally launches `CODE.1`, handles status 12 by deleting `CHARS` and returning the supervisor to the continuation state, and deletes `CHARS` again at final shutdown. It also creates the `WORK` and `AUTOMAPS` directories. [Frozen runtime audit §2](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/FULL_THREAD_AUDIT_2026-10-06.md) [R.EXE reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/R_EXE/01_STRUCTURAL_RECONSTRUCTION.md)

The process loop reconstructed from the shipped launcher is:

```text
status = 10

while status == 10:
    status = spawn_and_wait(CODE.2)

    if status == 10:
        status = spawn_and_wait(CODE.1)

    else if status == 12:
        delete("CHARS")
        status = 10

delete("CHARS")
```

That model should be read literally as control ownership. `R.EXE` owns *which module executes next*. The child module owns *why* it requests a transition.

Contemporary Watcom/Tenberry documentation independently confirms that DOS/4GW protected-mode programs can be spawned with wait semantics and that the parent receives the child's exit code. That documentation explains the platform mechanism, while Ravenloft's own launcher bytes prove that Ravenloft actually uses such a process-status protocol. [Open Watcom C/C++ Programmer's Guide, “Spawning Protected-Mode Applications,” pp. 26–27](https://openwatcom.org/ftp/manuals/current/pguide.pdf)

**Preservation rule:** do not collapse `R.EXE`, CODE.1, and CODE.2 into a single undocumented “main executable” abstraction.

**Evidence class:** BYTE/CODE-PROVEN for Ravenloft; OFFICIAL-DOCUMENTED for the general DOS/4GW spawn/wait mechanism.

---

## 3. CODE.1 and CODE.2 are DOS/4GW Professional bound applications

### 3.1 Shared outer bound runtime

A fresh byte-level comparison performed during this preservation pass closed an ambiguity left by earlier object-map work.

Both immutable CODE files contain the same DOS/4GW Professional bound outer image through physical file offset `0x25D23`. At physical `0x25D24`, both contain a nested MZ application-module header. That nested MZ header contains:

```text
e_lfanew = 0x1EB8
```

Therefore the protected-mode linear-executable header is at:

```text
0x25D24 + 0x1EB8 = 0x27BDC
```

in both CODE.1 and CODE.2.

The two files remain identical through the fixed beginning of that LE header and first differ at file `0x27BF0`, which is LE-header offset `+0x14`, the module-page-count field. The files therefore share the same outer bound extender/runtime image while containing differently sized Ravenloft application modules. [Corrected CODE object map](https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md)

The shared bound image contains shipped strings including:

```text
DOS/4GW Professional Protected Mode Run-time
Copyright (c) Rational Systems, Inc. 1990-1994
Feb 17 1994 11:54:25
1.96
DOS/4GW Professional
LINEXE_LOADER
RATIONAL DOS/4G
```

The string evidence, the nested protected-mode image geometry, and the runtime behavior all converge on DOS/4GW Professional 1.96 as the bound extender environment in the supplied files. [Corrected CODE object map](https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md)

Open Watcom's Programmer's Guide states that DOS/4GW uses the LE file format for a 32-bit protected-mode program with a flat address space, and that DOS/4GW Professional differs from ordinary DOS/4GW in part by binding the extender to the application program instead of requiring the extender as a separate file. Those descriptions match the physical Ravenloft layout. [Open Watcom Programmer's Guide, Chapter 7, “Linear Executables,” p. 41](https://openwatcom.org/ftp/manuals/current/pguide.pdf) [Open Watcom Programmer's Guide §14.2, pp. 102–103](https://openwatcom.org/ftp/manuals/current/pguide.pdf)

### 3.2 Why “bound image” matters

Earlier project work had already corrected the physical starts of the executable and data objects, but the reason for the original mapping error was not fully documented.

The LE header contains a `DATA PAGES OFFSET`. The IBM/Open Watcom-hosted Linear Executable specification defines that value as relative to the beginning of the EXE file containing the linear module. In Ravenloft's bound CODE files, the relevant application-module origin is physical `0x25D24`, not physical byte zero of the outer DOS/4GW-bound file. [IBM, *OS/2 16/32-bit Object Module Format and Linear eXecutable Module Format*, rev. 10, “Data Pages Offset,” pp. 58–62](https://openwatcom.org/ftp/devel/docs/lxomf.pdf)

The correct physical mapping is therefore:

```text
physical application page stream
    = bound application-module origin
    + LE DATA PAGES OFFSET
```

not:

```text
outer file byte zero
    + LE DATA PAGES OFFSET
```

That single origin distinction explains the historical xref failure that had placed data strings such as `CHARS` inside the wrong object.

**Evidence class:** BYTE-PROVEN for the Ravenloft offsets; OFFICIAL-DOCUMENTED for LE field semantics and DOS/4GW Professional binding.

---

## 4. CODE.1 container and object map

### 4.1 Immutable identity

```text
CODE.1 physical size        277,787 bytes / 0x43D1B
SHA-256                     fd9b709ceee190e6a70870f9012ebea3
                            bc437cae6bc74e0150d6f922fa750fed

bound module origin         0x25D24
LE header                   0x27BDC
application entry object    1
application entry offset    0xCC1C
```

The canonical decompilation accounts for the complete 70,402-byte executable object: 69,361 translated instruction bytes plus 1,041 proven non-code/data bytes. [CODE.1 continuity baseline](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/DECOMPILATION_CONTINUITY.md) [CODE.1 final binary audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)

### 4.2 LE header fields relevant to loading

The directly parsed CODE.1 LE image records:

| Field | CODE.1 value |
|---|---:|
| Module pages | `0x18` |
| EIP object | `1` |
| EIP | `0xCC1C` |
| ESP object | `2` |
| ESP | `0x9E90` |
| Page size | `0x1000` |
| Object table offset | `0xC4` |
| Object count | `2` |
| Data-pages offset | `0x6A00` |
| Automatic data object | `2` |

The object table is:

| Object | Virtual size | Relocation base | Raw flags | First page-table index | Page-table entries |
|---|---:|---:|---:|---:|---:|
| 1 | `0x11302` | `0x10000` | `0x2005` | 1 | 18 |
| 2 | `0x9E90` | `0x30000` | `0x2003` | `0x13` | 6 |

[Corrected CODE object map](https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md)

Using the bound-module origin:

```text
CODE.1 first physical data page
    = 0x25D24 + 0x6A00
    = 0x2C724
```

Object 1 occupies 18 physical 4 KiB page slots, so:

```text
CODE.1 object 2 physical start
    = 0x2C724 + 18 * 0x1000
    = 0x3E724
```

This reproduces the independently certified decompilation boundaries exactly. [Corrected CODE object map](https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md)

Object 2 has a virtual extent larger than its six stored pages. That is normal linear-executable behavior: an object's virtual address range need not be entirely represented by bytes stored in physical data pages. The loader can provide non-file-backed/zero-initialized capacity. The IBM format documentation distinguishes an object's virtual size and page-table-backed physical data. [IBM LX/LE specification, Object Table and Object Page Table, pp. 66–73](https://openwatcom.org/ftp/devel/docs/lxomf.pdf)

### 4.3 CODE.1 application entry

The LE entry point is object 1 offset `0xCC1C`.

At that address the executable immediately performs a short jump to `0xCC96`, crossing the range:

```text
0xCC1E..0xCC95
```

Those 120 bytes are proven embedded Watcom runtime identification text/data, not executable fall-through. That classification was necessary to close CODE.1's byte accounting correctly. [CODE.1 residual ledger](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/RESIDUAL_LEDGER.md) [CODE.1 final binary audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)

The runtime startup later reaches the Ravenloft application entry routine at logical `0x508C`; the startup call occurs at `0xFCD9`. `0x508C` performs application setup, calls `0x4978`, and passes that routine's result unmodified to the controlled-termination helper `0x5014`. [CODE.1 status-10 propagation](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CODE1_STATUS10_PROPAGATION.md)

This gives an important conceptual layering:

```text
LE EIP 0xCC1C
    |
    v
Watcom / protected-mode startup machinery
    |
    v
Ravenloft application entry 0x508C
    |
    v
character-generation / handoff application state
```

“Entry point” can therefore mean two different things in documentation. The LE entry is the runtime's machine entry. `0x508C` is the application-level entry reached after the runtime startup path. Both must be named explicitly to avoid conflation.

---

## 5. CODE.2 container and object map

### 5.1 Immutable identity

```text
CODE.2 physical size        558,689 bytes / 0x88661
SHA-256                     853e0a4dfcabebcfeecb975cf2a92b92
                            eb4d94806d835ad30a0bab3e265bd314

bound module origin         0x25D24
LE header                   0x27BDC
application entry object    1
application entry offset    0x353A0
```

CODE.2 object 1 is 239,698 bytes. The certified reconstruction accounts for 235,022 translated instruction bytes and 4,676 proven non-code/data bytes, with no unresolved object-1 byte. [CODE.2 final certification](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md)

### 5.2 LE header fields relevant to loading

| Field | CODE.2 value |
|---|---:|
| Module pages | `0x4E` |
| EIP object | `1` |
| EIP | `0x353A0` |
| ESP object | `2` |
| ESP | `0x2DD10` |
| Page size | `0x1000` |
| Object table offset | `0xC4` |
| Object count | `2` |
| Data-pages offset | `0x15200` |
| Automatic data object | `2` |

Object table:

| Object | Virtual size | Relocation base | Raw flags | First page-table index | Page-table entries |
|---|---:|---:|---:|---:|---:|
| 1 | `0x3A852` | `0x10000` | `0x2005` | 1 | 59 |
| 2 | `0x2DD10` | `0x50000` | `0x2003` | `0x3C` | 19 |

[Corrected CODE object map](https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md)

Physical mapping:

```text
first CODE.2 physical data page
    = 0x25D24 + 0x15200
    = 0x3AF24

CODE.2 object 2 physical start
    = 0x3AF24 + 59 * 0x1000
    = 0x75F24
```

Again, those values independently reproduce the already-certified extraction boundaries. [CODE.2 README](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/README.md) [CODE.2 final certification](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md)

### 5.3 CODE.2 application entry

The LE entry is `0x353A0`.

The first instruction jumps over `0x353A2..0x35417`, 118 bytes of proven non-code runtime data, and execution continues at `0x35418`. The following startup body establishes stack/runtime state, distinguishes DOS/extender cases, handles PSP/environment state, processes runtime environment strings, and eventually reaches the game application path. [CODE.2 continuity addendum, batches 111–112](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)

The application-main path reached through this runtime is logical `0x20608`. The controlled application exit helper is `0x205B4`. The ultimate process termination path reaches DOS `INT 21h / AH=4Ch` at `0x35666..0x35668`. [Exit-status producer reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/EXIT_STATUS_PRODUCERS.md)

Conceptually:

```text
LE EIP 0x353A0
    |
    +-- jump over embedded runtime data
    |
    v
startup body 0x35418...
    |
    v
runtime initialization
    |
    v
Ravenloft application path 0x20608
    |
    v
gameplay / world / UI state
```

---

## 6. What CODE.1 and CODE.2 own

The binary evidence allows a stronger statement than simply saying there are “two executables.”

CODE.1 is the module that owns the reconstructed character-generation/presentation workflow leading to the normal party handoff. Its application state produces the transient `CHARS` image and requests continuation status 10. CODE.2 is the principal gameplay/world module: it conditionally imports `CHARS`, owns the active party image at runtime `0x218E0`, and produces the certified restart/finale transition status 12. [CODE.1 status-10 propagation](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CODE1_STATUS10_PROPAGATION.md) [CHARS reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CHARS_XREF_RECONSTRUCTION.md) [CODE.2 status-12 ownership](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CODE2_STATUS12_OWNERSHIP.md)

That does **not** justify inventing original DreamForge source-module names. “Character-generation module” and “gameplay/world module” are preservation descriptions of observed ownership, not recovered historical filenames or class names.

**Evidence class:** CROSS-CORROBORATED for functional ownership; UNKNOWN for original DreamForge source-level module terminology.

---

## 7. `CHARS`: the serialized bridge between processes

### 7.1 File geometry

`CHARS` is exactly the size of four active character records:

```text
4 * 0x248 = 0x920 bytes
```

It is not a normal persistent save file. It is a temporary process-boundary serialization image used to move party state from CODE.1 to CODE.2. [Canonical transition protocol](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/STATE_TRANSITION_PROTOCOL.md)

### 7.2 CODE.1 side

The CODE.1 `CHARS` pathname is physically located at:

```text
file offset               0x3E7B8
object 2 displacement     +0x94
mapped linear address     0x30094
```

[Corrected CODE object map](https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md)

The producer routine at `0x4EF0`, with its late file-operation sequence around `0x4FBF..0x500D`, performs:

```text
delete old CHARS
create/reinitialize CHARS
close
reopen CHARS
transfer 0x920 bytes from party state
close
```

The DOS-file wrapper chain was traced to delete, open/create, close, and handle-I/O behavior. [CHARS xref reconstruction §2](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CHARS_XREF_RECONSTRUCTION.md)

### 7.3 CODE.2 side

CODE.2 contains two `CHARS` pathname occurrences because startup uses separate existence/access and open/read paths:

```text
#1 file 0x76634
   object 2 +0x710
   linear 0x50710

#2 file 0x77064
   object 2 +0x1140
   linear 0x51140
```

[Corrected CODE object map](https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md)

At logical `0x204B3`, CODE.2 performs a DOS file-attribute/existence test. If `CHARS` is absent, control goes to the alternate initialization path at `0x19888`. If present, CODE.2 calls the consumer at `0x16CD0`, which opens the file, transfers exactly `0x920` bytes into active-party storage at runtime `0x218E0`, closes it, and performs post-load processing. [CHARS xref reconstruction §§3–4](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CHARS_XREF_RECONSTRUCTION.md)

The complete original bridge is therefore:

```text
CODE.1 active character state
        |
        | serialize 0x920
        v
      CHARS
        |
        | process terminates with status 10
        v
      R.EXE
        |
        | launches CODE.2
        v
CODE.2 existence gate
        |
        +-- absent --> alternate startup
        |
        +-- present --> read 0x920 --> active party at 0x218E0
```

A native port may replace the physical temporary file with an in-memory typed transfer object, but a compatibility layer should remain capable of reproducing the exact `0x920` serialized form.

---

## 8. Status 10 is a protocol value, not merely “character creation finished”

### 8.1 CODE.1 producer

CODE.1 application routine `0x4978` can return decimal 10. Its caller `0x508C` passes that return value unchanged to the non-returning controlled-termination helper `0x5014`. `0x5014` explicitly special-cases status 10. When the two required party-presence state flags are set, it invokes the `CHARS` producer before passing the same status into process termination. CODE.1 also has an independent direct `push 0Ah ; call 5014` route at `0x9B1E..0x9B20`. [CODE.1 status-10 propagation](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CODE1_STATUS10_PROPAGATION.md)

The strongest protocol-level name is therefore:

```text
10 = CONTINUE_MODULE_CYCLE
```

The normal completed character-generation workflow is one concrete producer/cause of that transition. The numeric value itself belongs to the supervisor protocol and should not be globally renamed `CHARACTER_CREATION_COMPLETE`.

### 8.2 Supervisor interpretation

When CODE.2 returns 10, `R.EXE` launches CODE.1. When the CODE.1 invocation returns 10 after its normal handoff, the launcher loop naturally returns to CODE.2 on the next iteration. [Canonical transition protocol](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/STATE_TRANSITION_PROTOCOL.md)

The same number is therefore meaningful at more than one edge of the cycle.

---

## 9. Status 12 is restart-without-transferred-party state

CODE.2 controlled-exit helper `0x205B4` explicitly compares its requested status against both `0x0A` and `0x0C`. Those two values bypass an extra ordinary-cleanup/reset operation and proceed to the process-termination machinery. This proves that CODE.2 itself recognizes 10 and 12 as privileged inter-process transition values rather than arbitrary DOS error codes. [Exit-status producer reconstruction §2](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/EXIT_STATUS_PRODUCERS.md)

At the supervisor layer:

```text
12 = RESTART_CODE_CYCLE_WITHOUT_CHARS
```

because `R.EXE` responds by deleting `CHARS`, resetting its supervisor state to continuation, and restarting the module cycle. [Canonical transition protocol](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/STATE_TRANSITION_PROTOCOL.md)

Two live gameplay causes are certified:

1. **Party wipe / no active party.**
2. **Five-artifact quest completion**, followed by numbered event 10 and `CINE10.ANM` or `CINE11.ANM`.

[CODE.2 status-12 ownership](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CODE2_STATUS12_OWNERSHIP.md)

The standalone CODE.2 routine `0x23DF0..0x23E1A` contains the same event-10/status-12 finale sequence but has no certified incoming static application edge, no recognized jump-table entry, no stored absolute callback pointer, and no LE relocation targeting the routine. It is therefore preserved as an **unreferenced duplicate finale helper**, not fabricated into a third gameplay transition. [Final producer xref audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/STATUS12_FINAL_PRODUCER_XREF.md)

The design principle is:

```text
gameplay cause != supervisor mechanism
```

A party wipe and successful game finale are very different gameplay events but intentionally converge on the same supervisor primitive.

---

## 10. DOS process termination is the physical transport for the status value

The supervisor protocol ultimately crosses the child-process boundary through ordinary DOS process termination.

For CODE.2, the Watcom/runtime termination path reaches DOS:

```text
INT 21h
AH = 4Ch
```

at logical `0x35666..0x35668`. The application result therefore becomes the DOS child-process status that `R.EXE` receives from its wait/spawn mechanism. [Exit-status producer reconstruction §1](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/EXIT_STATUS_PRODUCERS.md)

CODE.1 similarly forwards its application result through its audited controlled-termination/runtime path. The critical preservation point is not the compiler library name of every wrapper but the end-to-end contract:

```text
Ravenloft application state
    -> requested transition value
    -> protected-mode runtime termination
    -> DOS child exit status
    -> R.EXE wait result
    -> next module/supervisor action
```

Contemporary Watcom documentation's `P_WAIT` example explicitly demonstrates a parent protected-mode application receiving the spawned program's exit code. This is corroborating platform documentation; Ravenloft's actual status values and branch behavior are established from its own binaries. [Open Watcom Programmer's Guide, pp. 26–27](https://openwatcom.org/ftp/manuals/current/pguide.pdf)

---

## 11. Extender/runtime code must not be mistaken for DreamForge game logic

CODE.1 and CODE.2 contain several distinct provenance layers in one physical file:

```text
outer DOS/4GW Professional bound extender/runtime
    |
    +-- nested MZ/LE protected-mode module
            |
            +-- startup / compiler-runtime machinery
            +-- embedded runtime data/tables/identification blocks
            +-- Ravenloft application code
            +-- Ravenloft application data
```

Recognizable Watcom runtime patterns do not prove that every application routine was originally authored in a particular source language, and runtime copyright/identification blocks are not evidence of DreamForge's own naming conventions. The project's decompilation policy correctly requires binary evidence to outrank compiler conventions and explicitly warns against inferring authorship/compiler-language facts merely from runtime signatures. [CODE.1 continuity methodology](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/DECOMPILATION_CONTINUITY.md)

For preservation documentation, each routine should therefore be classified by observed role rather than by an unsupported historical source-language story:

- **platform/extender machinery**;
- **compiler/runtime machinery**;
- **application-owned wrapper**;
- **Ravenloft gameplay/UI/resource logic**;
- **embedded data/table**;
- **unresolved provenance**.

This becomes especially important in later chapters covering DPMI allocation, selector operations, DOS wrappers, formatted I/O, interrupt handlers, and heap/stream internals.

---

## 12. Implications for a faithful native implementation

A programmer reimplementing Ravenloft should preserve the following semantics even if the physical DOS process model is modernized.

### 12.1 Model module transitions explicitly

A suitable typed representation is:

```cpp
enum class ModuleResult {
    ContinueModuleCycle,             // DOS status 10
    RestartWithoutTransferredParty,  // DOS status 12
    OrdinaryTermination
};
```

Gameplay causes should remain separate:

```cpp
enum class RestartCause {
    PartyWipe,
    QuestCompletionFinale
};
```

This mirrors the binary architecture: mechanism and cause are separate state dimensions. [Canonical transition protocol](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/STATE_TRANSITION_PROTOCOL.md)

### 12.2 Preserve `CHARS` compatibility geometry

The native engine may use a typed in-memory party structure internally, but the historical transfer format should remain representable as exactly four `0x248` records / `0x920` bytes. That enables regression tests, save-state archaeology, compatibility tooling, and byte-for-byte comparison with the original transition artifact. [CHARS xref reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CHARS_XREF_RECONSTRUCTION.md)

### 12.3 Do not build modern architecture from wrong physical offsets

Any utility that maps original file addresses must incorporate the bound-module origin:

```text
CODE module origin = 0x25D24
```

and then interpret the LE tables from the nested module. Hard-coding only `0x2C724` and `0x3AF24` without documenting why they are correct is insufficient for preservation-grade tooling because it cannot generalize to another build or explain a mismatch. [Corrected CODE object map](https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md)

### 12.4 Retain address-domain terminology

At least four address domains recur throughout the project:

1. **outer physical file offset**;
2. **nested module-relative offset**;
3. **object-relative logical offset**;
4. **mapped linear/runtime address**.

For example, CODE.1 `CHARS` can legitimately be described as:

```text
outer file               0x3E7B8
object 2 displacement    0x94
linear address           0x30094
```

Those numbers are not interchangeable. Every later chapter should state which domain an address belongs to.

---

## 13. Corrections and clarifications introduced by this preservation pass

### C-01 — Bound application-module origin established

**Previous state:** the final object starts were correct, but the documentation primarily treated them as certified extraction boundaries after correcting an earlier LE-offset interpretation.

**Correction:** both CODE files are now explicitly documented as containing a nested application module beginning at physical `0x25D24`. Its nested MZ `e_lfanew=0x1EB8` resolves to LE header `0x27BDC`.

**Consequence:** the LE `DATA PAGES OFFSET` equations directly reproduce both certified object-1 starts:

```text
CODE.1: 0x25D24 + 0x6A00  = 0x2C724
CODE.2: 0x25D24 + 0x15200 = 0x3AF24
```

[Corrected CODE object map](https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md)

### C-02 — Shared bound-runtime identity made explicit

The two CODE files are now documented as sharing the same outer DOS/4GW Professional runtime image up to the nested linear-executable module's differing size/loader metadata. This prevents future researchers from treating duplicated extender bytes as two independently authored Ravenloft subsystems. [Corrected CODE object map](https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md)

No game behavior changed in either correction. These are structural/documentation corrections that make the already-certified object mappings reproducible from the actual container geometry.

---

## 14. Open questions carried forward

This chapter closes the high-level executable/container/module-cycle foundation, but it intentionally does **not** claim several things that require later work.

### 14.1 Original build-toolchain precision

Watcom runtime identity is strongly present, but the exact compiler/linker version and original DreamForge source-language breakdown for every application subsystem are not yet promoted to historical fact merely from runtime signatures. [CODE.1 decompilation methodology](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/DECOMPILATION_CONTINUITY.md)

### 14.2 Detailed startup memory construction

This chapter identifies the startup entries and object geometry but does not yet fully document:

- selector creation and limits;
- PSP/environment aliases;
- stack construction;
- DPMI host interactions;
- conventional/DOS memory ownership;
- protected-mode linear allocations;
- LE relocation application;
- runtime heap growth.

Those belong to the next architectural chapter.

### 14.3 Historical reason for the CODE.1/CODE.2 split

The runtime ownership relationship is machine-proven. The *development-history reason* DreamForge chose the two-process design is not presently established by developer documentation. Unless a period source or developer testimony is recovered, the archive should distinguish “what the split does” from “why the developers chose it.”

### 14.4 Alternate builds

The exact container equations in this chapter apply to the supplied immutable CODE.1/CODE.2 pair. Another retail revision may have different application-module size, page tables, object counts, or offsets. A preservation parser should therefore derive them from the embedded module/LE metadata rather than assuming this build's physical constants.

---

## 15. Source ledger

### Primary repository evidence

1. **CODE.1 canonical identity and methodology**  
   `Code1/DECOMPILATION_CONTINUITY.md`, frozen baseline commit `373c7f10...`  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/DECOMPILATION_CONTINUITY.md

2. **CODE.1 final byte/semantic audit**  
   `Code1/FINAL_BINARY_AUDIT.md`, frozen baseline  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md

3. **CODE.1 residual/non-code classification**  
   `Code1/RESIDUAL_LEDGER.md`, frozen baseline  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/RESIDUAL_LEDGER.md

4. **CODE.2 canonical baseline**  
   `Code2/README.md`, frozen baseline  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/README.md

5. **CODE.2 final certification**  
   `Code2/FINAL_CERTIFICATION.md`, frozen baseline  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md

6. **CODE.2 startup/non-code reconstruction provenance**  
   `Code2/CODE2_CONTINUITY_ADDENDUM.md`, frozen baseline  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md

7. **Corrected bound-module/object map**  
   `Runtime/CODE_OBJECT_MAP.md`, correction commit `b87ac1d6...`  
   https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md

8. **R.EXE / complete runtime consistency audit**  
   `Runtime/FULL_THREAD_AUDIT_2026-10-06.md`, frozen baseline  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/FULL_THREAD_AUDIT_2026-10-06.md

9. **Canonical module state-transition protocol**  
   `Runtime/STATE_TRANSITION_PROTOCOL.md`, frozen baseline  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/STATE_TRANSITION_PROTOCOL.md

10. **CHARS producer/consumer xrefs**  
    `Runtime/CHARS_XREF_RECONSTRUCTION.md`, frozen baseline  
    https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CHARS_XREF_RECONSTRUCTION.md

11. **CODE.1 status-10 propagation**  
    `Runtime/CODE1_STATUS10_PROPAGATION.md`, frozen baseline  
    https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CODE1_STATUS10_PROPAGATION.md

12. **CODE.2 status-12 ownership**  
    `Runtime/CODE2_STATUS12_OWNERSHIP.md`, frozen baseline  
    https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CODE2_STATUS12_OWNERSHIP.md

13. **Unreferenced finale reachability audit**  
    `Runtime/STATUS12_FINAL_PRODUCER_XREF.md`, frozen baseline  
    https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/STATUS12_FINAL_PRODUCER_XREF.md

14. **Exit-status machinery**  
    `Runtime/EXIT_STATUS_PRODUCERS.md`, frozen baseline  
    https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/EXIT_STATUS_PRODUCERS.md

### Official platform/format documentation

15. **Open Watcom C/C++ Programmer's Guide, v1.8**  
    Relevant sections: 32-bit DOS/4GW applications; spawning protected-mode applications; Chapter 7 Linear Executables; §14.2 DOS/4GW Professional. Portions of the manual are reprinted from Tenberry Software documentation.  
    https://openwatcom.org/ftp/manuals/current/pguide.pdf

16. **IBM, OS/2 16/32-bit Object Module Format and Linear eXecutable Module Format (LX), Revision 10**  
    Relevant sections: 32-bit linear EXE header, object table, object page table, data-pages-offset rules. This is used for field-layout/offset terminology alongside Open Watcom's explicit statement that DOS/4GW uses LE.  
    https://openwatcom.org/ftp/devel/docs/lxomf.pdf

---

## 16. Architectural checkpoint

The following are now the canonical foundation for all later executable-side documentation:

```text
R.EXE
    = 16-bit DOS supervisor

CODE.1 / CODE.2
    = DOS/4GW Professional-bound protected-mode application files

common nested application-module origin
    = physical 0x25D24

common LE header
    = physical 0x27BDC

CODE.1 object 1
    = physical 0x2C724
    = mapped base 0x10000
    = virtual size 0x11302
    = LE entry 0xCC1C

CODE.1 object 2
    = physical 0x3E724
    = mapped base 0x30000

CODE.2 object 1
    = physical 0x3AF24
    = mapped base 0x10000
    = virtual size 0x3A852
    = LE entry 0x353A0

CODE.2 object 2
    = physical 0x75F24
    = mapped base 0x50000
    = virtual size 0x2DD10

CHARS
    = transient four-character cross-process image
    = 0x920 bytes

status 10
    = CONTINUE_MODULE_CYCLE

status 12
    = RESTART_CODE_CYCLE_WITHOUT_CHARS
```

The next executable-side document should begin below this layer: **protected-mode startup, selectors, DPMI, DOS memory, linear memory, PSP/environment state, relocation, and runtime memory ownership.**
