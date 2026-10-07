# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 02 — Protected-Mode Startup, DPMI, Selectors, and Runtime Memory Ownership

**Status:** PRIMARY ARCHITECTURAL RECONSTRUCTION — MACHINE-CHECKED AGAINST CODE.1/CODE.2 AND CORRELATED WITH SURVIVING WATCOM RUNTIME SOURCE  
**Documentation branch:** `code1-decompilation`  
**Frozen pre-documentation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`  
**Parent archive chapter:** `01_EXECUTABLE_CONTAINER_AND_MODULE_CYCLE.md`

---

## 1. Scope

The preceding chapter established the physical container and process architecture:

```text
R.EXE
  -> DOS/4GW Professional
      -> protected-mode CODE.1 or CODE.2
          -> LE object 1 (code)
          -> LE object 2 (data/BSS/stack)
```

This chapter documents what happens below the Ravenloft application layer once a CODE module is running:

- 32-bit startup state;
- extender detection;
- PSP and environment access;
- stack and BSS initialization;
- selector versus segment versus linear-address semantics;
- DPMI functions actually used by the runtime;
- DOS conventional-memory fallback;
- DPMI linear-memory allocation and handles;
- runtime near-heap arenas and free-list blocks;
- arena resize and release;
- program-break growth;
- compatibility paths for other DOS extenders;
- the relationship between Ravenloft's binary and surviving Watcom runtime source.

The critical preservation rule is:

> A selector, a real-mode segment, a linear address, a DPMI memory-block handle, and a Ravenloft/Watcom heap pointer are different entities even when the original runtime converts between them.

Flattening them into a generic host pointer destroys both the machine semantics and the ownership rules needed to reproduce allocation/free behavior.

---

## 2. Provenance: this layer is substantially Watcom runtime machinery

CODE.1's startup at logical `0xCC1C` jumps over a 120-byte embedded Watcom identification/data block and enters the active startup body at `0xCC96`. The binary then performs a highly distinctive sequence:

```text
align ESP to 4 bytes
save stack top
initialize current break
initialize PSP selector
query DOS/extender version
identify Phar Lap / CodeBuilder / Rational DOS/4G paths
capture environment location
copy command line/program name
scan environment
establish stack-low/dynamic-end values
clear BSS
run initializers
enter C/application main
```

This sequence is not merely “Watcom-like.” Surviving Open Watcom source derived from the Sybase/Watcom runtime contains the same startup algorithm and historical symbol names in `bld/clib/startup/a/cstrt386.asm`. The source identifies the routine as **DOS 32-bit startup code** and names state such as `_STACKTOP`, `_curbrk`, `_psp`, `_osmajor`, `_osminor`, `_STACKLOW`, `_dynend`, `_Extender`, `_Envptr`, `_LpCmdLine`, and `_LpPgmName`. [Surviving Watcom-derived startup source, 2017 revision](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/startup/a/cstrt386.asm)

This correlation is strong enough to recover runtime terminology, but the shipped Ravenloft bytes remain controlling. The surviving source is not asserted to be the exact compiler-library revision linked into Ravenloft. Where the source and binary differ, the binary wins.

**Evidence class:** CODE-PROVEN for Ravenloft behavior; OFFICIAL/SURVIVING-VENDOR-SOURCE corroboration for historical runtime names.

---

## 3. CODE.1 startup in machine order

### 3.1 Entry and initial state

The LE entry is `0xCC1C`. It jumps to `0xCC96`, after the embedded identification block. Fresh disassembly of immutable CODE.1 gives:

```asm
CC96  sti
CC97  and esp,FFFFFFFCh
CC9A  mov ebx,esp
CC9C  mov [5304],ebx
CCA2  mov [52F0],ebx
CCA8  mov ax,0024h
CCAC  mov [52FC],ax
```

The surviving Watcom startup performs the same operations as:

```text
align ESP
_STACKTOP = ESP
_curbrk   = ESP
_psp      = PSP selector
```

The address correlation supports the following reconstructed runtime identities:

| CODE.1 DGROUP offset | Runtime role | Confidence |
|---|---|---|
| `0x5304` | `_STACKTOP` | cross-corroborated |
| `0x52F0` | `_curbrk` | cross-corroborated |
| `0x52FC` | `_psp` selector/segment value | cross-corroborated |
| `0x5300` | `_STACKLOW` | cross-corroborated |
| `0x52EC` | `_dynend` | cross-corroborated |
| `0x5326` | DOS major version byte | cross-corroborated |
| `0x5327` | DOS minor version byte | cross-corroborated |
| `0x5320/+0x5324` | environment far-pointer offset/selector pair | cross-corroborated |
| `0x52F4` | command-line pointer | cross-corroborated |
| `0x52F8` | program-name pointer | cross-corroborated |
| `0x531E` | extender-family code (`_Extender`) | cross-corroborated |

Machine evidence: [CODE.1 final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md), canonical reconstruction [Code1.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp). Vendor-source correlation: [`cstrt386.asm`](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/startup/a/cstrt386.asm).

The adjacent byte `0x531F` is deliberately **not** assigned an exact source symbol here. Later heap code consumes it on a compatibility path, but the surviving runtime sources changed their extender/subtype support over time. Its behavior is preserved; its exact Ravenloft-era symbol remains bounded.

### 3.2 DOS/extender probe

The startup loads:

```asm
mov ebx,50484152h
xor eax,eax
mov ah,30h
int 21h
```

`0x50484152` corresponds to the four-byte marker used by surviving Watcom startup source to probe extender behavior. The returned DOS version is stored at `0x5326/0x5327`. [Watcom-derived startup source](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/startup/a/cstrt386.asm)

The binary then distinguishes three broad compatibility paths.

---

## 4. Extender-family detection

### 4.1 Phar Lap path

At `0xCCD1`, CODE.1 shifts the returned EAX and tests:

```asm
cmp ax,4458h       ; 'DX'
```

On this path it derives the extender version from the returned character in BL, uses selector `0x24` for the PSP, obtains a high-water address from PSP-relative state, rounds it to 4 KiB, updates the current break, shrinks the DOS block with `INT 21h/AH=4Ah`, and uses `0x2C` as the environment selector.

The surviving Watcom startup source identifies this as the Phar Lap 386|DOS-Extender path. [`cstrt386.asm`](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/startup/a/cstrt386.asm)

### 4.2 Intel CodeBuilder compatibility path

CODE.1 contains an additional branch absent from some later/surviving startup-source revisions:

```asm
CD17  cmp ax,4243h       ; 'CB'
...
CD4C  mov al,09h
```

The stored extender-family value becomes `9`.

Later runtime heap code explicitly compares the extender byte with `9` before invoking:

```asm
mov eax,80004800h
int 21h
```

A surviving pre-2022 Watcom extender table defines:

```text
X_INTEL = 9    ; Intel CodeBuilder
```

and the Watcom heap source's `CodeBuilderAlloc` uses exactly `EAX=80004800h; INT 21h`. [Watcom extender values before the 2022 reindexing](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/watcom/h/extender.inc) [Watcom heap source](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/heap/c/grownear.c)

Therefore the former project-neutral label “mode 9” can now be historically tightened:

> **Extender code 9 is the Intel CodeBuilder compatibility path in the runtime lineage matching Ravenloft's binary.**

This is cross-corroborated by both the shipped machine code and surviving Watcom source.

### 4.3 Why current Open Watcom numeric enums must not be used blindly

Open Watcom changed the extender enumeration in December 2022 while adding/fixing Phar Lap version handling. The commit inserted a Phar Lap V1 slot, shifting:

```text
old: X_INTEL = 9
new: X_INTEL = 10
```

and similarly shifting later values. [Open Watcom commit `b8af89cf...`, “review/fix pharlap version macros”](https://github.com/open-watcom/open-watcom-v2/commit/b8af89cf1002f150e6260be3c285b7b02dd7ed06)

Ravenloft's binary directly proves the older numeric convention through the `9 -> EAX=80004800h` path. Any future researcher consulting current Open Watcom headers without version awareness could misidentify Ravenloft's value 9 as Phar Lap V8. That would be historically wrong for this binary.

This is an example of why external source is corroboration, not controlling evidence.

### 4.4 Rational DOS/4G path

If the prior probes do not select Phar Lap or CodeBuilder, CODE.1 executes:

```asm
mov dx,0078h
mov ax,FF00h
int 21h
test al,al
```

The surviving Watcom source identifies this as the probe for Rational DOS/4G or compatible. On success, Ravenloft stores extender code `1`. [`cstrt386.asm`](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/startup/a/cstrt386.asm) [historical extender table](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/watcom/h/extender.inc)

This matches the shipped environment: the outer file is DOS/4GW Professional-bound, and the runtime identifies Rational/Tenberry-compatible DOS/4G behavior.

---

## 5. PSP and environment ownership

DOS programs conventionally receive a Program Segment Prefix containing process metadata, including a pointer to the environment. The protected-mode runtime must translate that DOS-era process structure into the selector/linear model used by the 32-bit application.

The startup stores the PSP selector/value at `0x52FC`, obtains the environment selector/segment, and publishes a far environment pointer at `0x5320/+0x5324`.

CODE.1 then:

1. reads the command-line length from the PSP command buffer;
2. skips leading spaces;
3. copies the command line into aligned application-owned memory;
4. walks the environment string block;
5. detects `no87=`;
6. walks past the double-NUL terminator;
7. copies the program pathname/name;
8. restores the application data selector.

This sequence is independently recognizable in surviving Watcom startup source. [CODE.1 binary audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md) [Watcom-derived startup](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/startup/a/cstrt386.asm)

The important preservation point is that environment memory is **selector-qualified**. A native reconstruction may normalize it to host strings after ingestion, but a faithful low-level model must not pretend the DOS PSP/environment pointers were ordinary flat host pointers at startup.

---

## 6. Stack, dynamic area, and BSS

After copying startup strings, CODE.1 stores:

```text
_STACKLOW  <- current destination pointer
_dynend    <- current ESP
```

The binary then computes the BSS length from its linker-defined boundaries.

A particularly important DOS/4G-specific optimization is encoded:

```text
if _Extender == 1 (Rational DOS/4G compatible)
    and BSS length > 0x1000:
        explicitly zero only first 0x1000 bytes
else:
        explicitly zero full BSS
```

The surviving Watcom source explains the assumption: under Rational DOS extender operation, the remaining pages are expected to be zeroed by the extender. [Watcom-derived startup source](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/startup/a/cstrt386.asm)

CODE.1's direct instructions at `0xCE42..0xCE66` preserve exactly that first-4-KiB cap.

A native loader that constructs the original object model should therefore guarantee zero-initialized non-file-backed object/BSS state before application code runs. Reliance on host-language static initialization happens to produce the same common result, but the historical reason is the DOS extender/LE loader contract.

---

## 7. Segment, selector, and linear-address domains

### 7.1 Real-mode segment

A DOS real-mode segment denotes a base in 16-byte paragraphs:

```text
linear_base = segment << 4
```

DPMI function `0100h` returns both:

- AX = real-mode segment base;
- DX = protected-mode selector for the block.

The DPMI 1.0 specification explicitly documents this dual result. [DPMI 1.0, Int 31h Function 0100h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

### 7.2 Protected-mode selector

A selector indexes a descriptor that contains a linear base, limit, and access attributes. It is not itself a linear address.

DPMI function `0006h` takes a selector in BX and returns the descriptor's 32-bit linear base in CX:DX. [DPMI 1.0, Int 31h Function 0006h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

CODE.1 uses this service whenever it needs to convert selector-backed memory into a flat linear address.

### 7.3 Linear address

A linear address is the 32-bit address seen by protected-mode code before paging translation. DPMI `0501h` allocates a linear block and returns its base directly as BX:CX. It does **not** allocate an LDT descriptor for the block. [DPMI 1.0, Int 31h Function 0501h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

### 7.4 DPMI memory-block handle

DPMI `0501h` additionally returns an opaque memory-block handle in SI:DI. That handle, not the linear address, is supplied to `0502h` and `0503h` for freeing/resizing. [DPMI 1.0 functions 0501h–0503h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

The Ravenloft-linked runtime preserves both values.

---

## 8. CODE.1 DPMI/DOS-memory wrapper family

The runtime wrapper block at approximately `0x6F10..0x71C7` exposes the core memory primitives used above the extender.

### 8.1 `0x6F10`: allocate DOS memory

The routine converts the requested byte count to 16-byte paragraphs and invokes DPMI:

```text
INT 31h
AX = 0100h
BX = paragraphs
```

On success the DPMI specification returns:

```text
AX = real-mode segment
DX = selector
```

CODE.1 preserves the selector separately, computes the real-mode segment form, and also derives the flat linear base as:

```text
segment << 4
```

This is not redundant bookkeeping. The three representations are useful for different consumers.

The Watcom Programmer's Guide likewise describes low-memory allocation through DPMI `0100h` and the segment-to-linear conversion for first-megabyte access. [Open Watcom Programmer's Guide, DOS/4GW low-memory discussion](https://openwatcom.org/ftp/manuals/current/pguide.pdf)

### 8.2 `0x6F74`: free DOS memory by selector

This wrapper invokes:

```text
INT 31h
AX = 0101h
DX = selector
```

DPMI `0101h` frees the DOS block and descriptors created by `0100h`. [DPMI 1.0, Function 0101h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

The runtime consumes only the low 16 bits of the promoted cdecl selector argument; preserving this width was part of the semantic audit.

### 8.3 `0x6FC0`: selector to linear base

This wrapper invokes DPMI `0006h` and combines CX:DX into a 32-bit linear address.

This is the canonical bridge:

```text
selector
   |
   | DPMI 0006
   v
32-bit linear base
```

It must not be replaced with an assumption that selector numerically equals its base.

### 8.4 `0x6FF4`: DOS-backed runtime allocation header

This higher-level allocator:

1. rounds the size to paragraphs;
2. rejects requests exceeding `0xFFFF` paragraphs;
3. allocates with the DOS-memory service;
4. resolves the selector to a linear base;
5. writes a six-byte ownership header at the returned linear base.

The header is:

```text
+0  uint16  DOS/DPMI selector
+2  uint32  0
```

The zero dword distinguishes this backing type from a DPMI linear-memory allocation.

### 8.5 `0x709C`: DPMI linear-memory allocation header

This path invokes:

```text
INT 31h
AX    = 0501h
BX:CX = requested byte count
```

On success DPMI returns:

```text
BX:CX = linear address
SI:DI = memory-block handle
```

CODE.1 writes:

```text
+0  uint16  0
+2  uint32  DPMI memory-block handle
```

at the linear base.

Thus the same six-byte header has two ownership variants:

```text
DOS-backed block:
    selector != 0
    DPMI handle == 0

DPMI-linear block:
    selector == 0
    DPMI handle != 0
```

This is one of the most important implementation details in the original heap runtime. Release logic later examines the ownership header to choose the correct operating-system/extender free primitive.

### 8.6 `0x705C`: free DPMI linear block

The handle is split back into SI:DI and passed to:

```text
INT 31h
AX = 0502h
```

DPMI 1.0 defines `0502h` as Free Memory Block and explicitly takes the memory-block handle rather than the linear address. [DPMI 1.0, Function 0502h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

### 8.7 `0x70FC`: largest available DOS block query

This routine was initially easy to misname because it deliberately calls DOS-memory allocation with an impossible/maximum-size request. The final audit corrected it.

It requests `0xFFFF` paragraphs via DPMI `0100h`. On the expected insufficient-memory failure, DPMI returns the largest available block size in BX. The wrapper converts that paragraph count to bytes.

This behavior is explicitly specified for DPMI `0100h`: on allocation failure, BX reports the largest available DOS-memory block in paragraphs. [DPMI 1.0, Function 0100h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

It is therefore a **DOS low-memory availability query**, not a selector allocator.

---

## 9. DPMI linear allocation: `0501h / 0502h / 0503h`

### 9.1 Allocate — 0501h

DPMI 1.0 specifies:

```text
AX    = 0501h
BX:CX = requested size

success:
BX:CX = linear address
SI:DI = block handle
```

The block handle is a lifetime token. The linear address is a location. They are deliberately separate. [DPMI 1.0, Function 0501h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

### 9.2 Free — 0502h

```text
AX    = 0502h
SI:DI = block handle
```

No linear address is required by the API. [DPMI 1.0, Function 0502h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

### 9.3 Resize — 0503h

DPMI `0503h` accepts the old handle and new size. On success it returns a **new linear address and new handle**. The specification explicitly warns that the old handle becomes invalid and that the linear address may change. [DPMI 1.0, Function 0503h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

This detail directly explains why CODE.1's arena-resize routine cannot simply keep using the old pointer.

---

## 10. CODE.1 arena resize at `0x1083F`

The final semantic audit corrected this path after the earlier structural reconstruction had normalized it incorrectly.

The routine searches the runtime arena list for an arena satisfying the resize constraints. In the DPMI-backed case it:

1. identifies the six-byte DPMI ownership header preceding the arena;
2. computes the new rounded 4-KiB size;
3. invokes DPMI `0503h` using the stored handle;
4. receives a potentially relocated linear base and new handle;
5. rewrites the ownership header at the new base;
6. unlinks the old arena-list identity;
7. rebuilds the miniheap at the relocated base;
8. reconstructs/splits the requested allocation and trailing free block.

The final audit specifically corrected the successful return value to the relocated allocation position based on the new DPMI linear base, rather than returning a stale/intermediate arena pointer. [CODE.1 final audit, checkpoints 10n–10o](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)

This behavior follows the DPMI `0503h` contract exactly: relocation is permitted and the returned handle replaces the old one. [DPMI 1.0, Function 0503h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

---

## 11. Runtime near-heap structure

Above those system allocations sits an ordinary runtime allocator. It must be understood as a second layer:

```text
DPMI/DOS backing allocation
        |
        v
miniheap / arena descriptor
        |
        v
free-list blocks
        |
        v
application malloc-style allocations
```

The canonical CODE.1 reconstruction covers the arena allocator/free/coalescing core around `0x104E0..0x1083E`. [Code1.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp) [CODE.1 final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)

### 11.1 Allocation

The allocation core:

- rejects zero-size requests;
- adds allocator overhead;
- checks arithmetic wrap;
- rounds to the runtime's alignment;
- enforces a minimum block size;
- consults cached largest-free-block state;
- scans free lists;
- splits a free block when the remainder is large enough;
- otherwise consumes the complete free block;
- marks allocation state in the low bit of the size/tag field;
- returns the user pointer after the block header.

The final audit found that the failure scan's “largest candidate” calculation had originally been reconstructed incorrectly; the encoded `SUB/CMC/SBB/AND/ADD` sequence computes a maximum, not an arbitrary subtraction result. That correction matters because the arena caches influence later allocation decisions. [CODE.1 final audit checkpoint 10m](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)

### 11.2 Free/coalesce

The free path:

1. subtracts the user-header offset;
2. confirms the in-use bit;
3. clears/normalizes the allocation state;
4. coalesces with an adjacent free successor when possible;
5. inserts the block into address order;
6. coalesces with the preceding free block when contiguous;
7. updates arena allocation/free counters and largest-block caches.

Address ordering is therefore part of the heap algorithm, not incidental debugging state.

### 11.3 Arena list

The runtime maintains an address-sorted list of arenas/miniheaps. Surviving Watcom source names the corresponding helper concept `__LinkUpNewNHeap`. The source initializes:

- previous/next arena links;
- circular free-list sentinel;
- rover;
- allocation/free counts;
- the first free region;
- an end-of-arena sentinel.

[Watcom `grownear.c`, older source lineage](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/heap/c/grownear.c)

The CODE.1 machine behavior matches this structure closely enough that the source-level vocabulary **arena/miniheap/free list** is justified for documentation, while exact C struct layout remains governed by the binary.

---

## 12. System allocation strategy: DPMI first, DOS fallback

One of the most revealing runtime routines is CODE.1 `0x109C1`, which closely matches the surviving Watcom helper historically called `RationalAlloc`.

### 12.1 First choice: DPMI linear memory

The routine first attempts a DPMI memory block allocation. If successful, it records:

```text
DPMI handle
DOS segment = 0
```

and constructs an arena from the returned linear base.

### 12.2 Fallback: reserve/probe conventional DOS memory

If the DPMI allocation fails and the request can fit within the DOS-memory constraints, the runtime performs a carefully ordered fallback.

It first allocates a DOS block representing the minimum-real-memory reservation/probe. While that block is still allocated, it attempts the requested DOS block. Only **after** making the requested allocation attempt does it free the probe block.

This ordering was explicitly corrected during CODE.1's semantic audit because freeing the probe before the second allocation changes the DOS allocator's available space and can alter which allocation succeeds. [CODE.1 final audit checkpoint 10n](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)

Surviving Watcom `RationalAlloc` source shows the same sequence:

```text
save_DOS_block = allocate(minreal reservation)
DOS_block      = allocate(requested size)
free(save_DOS_block)
if DOS_block succeeded:
    convert DOS block to linear address
    record DOS ownership
```

[Watcom `grownear.c`](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/heap/c/grownear.c)

This is unusually strong source/binary corroboration.

---

## 13. Why the runtime keeps an ownership header

The backing allocator can return memory from different mechanisms:

```text
A. DPMI linear memory
   -> lifetime owned by DPMI handle
   -> free with 0502h

B. DOS conventional memory
   -> lifetime owned by DOS/DPMI selector/segment allocation
   -> free through DOS-memory/selector path

C. contiguous program-break growth
   -> part of process data-space extent
   -> resized through extender/DOS block semantics

D. Intel CodeBuilder compatibility allocation
   -> alternate INT 21h EAX=80004800h API
```

The six-byte pre-arena ownership record allows release/shrink code to distinguish A from B. Separate extender-family branches distinguish C/D where relevant.

A modern port should retain equivalent typed ownership even if all actual host memory comes from one allocator:

```cpp
enum class BackingKind {
    DpmiLinear,
    DosMemory,
    ProgramBreak,
    CodeBuilderCompat
};
```

The point is not to emulate obsolete DOS APIs for their own sake. The point is to preserve the original lifetime and failure semantics, especially when reproducing bugs or low-memory behavior.

---

## 14. `_curbrk`, selector limits, and contiguous DGROUP growth

Not all extender modes grow memory by obtaining independent DPMI blocks.

The runtime also maintains a current program break, correlated with Watcom's `_curbrk`. In modes where the data selector represents a resizable contiguous address range, heap growth can extend the process data area rather than create a separate miniheap backing allocation.

CODE.1 `0x10974` checks the last free block against the current break. For a range of Phar Lap extender-family values, it issues `LSL DS` to obtain the data-selector limit, adds one because x86 segment limits are inclusive, and updates the break value.

DPMI documentation explicitly notes that a client uses the `LSL` instruction to query a descriptor's limit. [DPMI 1.0, Function 0006h notes](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

The surviving Watcom heap source uses the same conceptual operation in `__LastFree`: for Phar Lap it refreshes `_curbrk` from the data-selector size before deciding whether the last free block reaches the current end. [Watcom `grownear.c`](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/heap/c/grownear.c)

This is why `_curbrk` cannot be modeled as merely “bytes allocated so far.” Depending on extender family it is tied to the current addressable data-segment extent.

---

## 15. `__ExpandDGROUP`-class behavior

CODE.1 `0x10AA9` matches the architecture of surviving Watcom `__ExpandDGROUP`:

1. verify heap growth is enabled;
2. reject terminal/sentinel current-break state;
3. round/adjust the requested amount;
4. account for an existing final free block where appropriate;
5. impose allocator structure overhead;
6. enforce an arena growth minimum;
7. round system requests to 4 KiB;
8. choose extender-specific system-growth mechanism;
9. if using program-break growth, try to merge the newly exposed address range into the final existing arena when physically contiguous;
10. otherwise create and link a new arena;
11. convert the new space into a normal free-list block.

[CODE.1 canonical source](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp) [Watcom `grownear.c`](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/heap/c/grownear.c)

The old source explicitly distinguishes:

- Rational zero-base;
- Intel CodeBuilder;
- Phar Lap;
- Rational non-zero-base / contiguous data-space behavior.

Ravenloft's ordinary shipped DOS/4GW path is Rational-compatible, but preserving the alternate branches matters because they are real code shipped in the executable and form part of the historical compiler/runtime environment.

---

## 16. Intel CodeBuilder allocation path

The compatibility branch for extender code 9 calls:

```asm
mov eax,80004800h
int 21h
```

with the requested size passed in the expected register.

Surviving Watcom heap source names the wrapper:

```text
CodeBuilderAlloc
```

and defines the same instruction sequence. [Watcom `grownear.c`](https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/heap/c/grownear.c)

Combined with the older extender table's `X_INTEL=9`, this closes the historical identity of a path that the prior Ravenloft reconstruction appropriately left as a neutral “mode-9 allocation.”

No evidence indicates Ravenloft normally runs this branch in the supplied DOS/4GW Professional configuration. It is compiler/runtime compatibility capacity that ships inside the program.

**Classification:** runtime-supported alternate extender path, not normal Ravenloft runtime path.

---

## 17. DOS `AH=48h/49h` versus DPMI `0100h/0101h`

Two superficially similar low-memory mechanisms occur in the runtime and must not be merged.

### DPMI DOS-memory service

```text
INT 31h / AX=0100h
INT 31h / AX=0101h
```

These provide a DPMI-client interface to DOS memory and descriptor management.

### Direct DOS allocation/free

```text
INT 21h / AH=48h
INT 21h / AH=49h
```

These are DOS memory-block services. They appear in fallback/growth paths and in compatibility logic.

The DPMI 1.0 specification itself notes that DPMI `0100h` ultimately uses DOS allocation function `INT 21h/AH=48h`, but the caller-visible contracts are different: `0100h` supplies protected-mode descriptor state, while direct DOS calls work in the DOS memory-control-block/segment domain. [DPMI 1.0, Function 0100h](https://openwatcom.org/ftp/devel/docs/dpmi10.pdf)

A faithful model should preserve the API layer actually called by each machine-code path.

---

## 18. CODE.2 contains the same runtime architecture

CODE.2 is much larger as an application, but its late runtime contains the same architectural families.

The certified reconstruction identifies:

- DOS-memory wrappers;
- DPMI memory wrappers;
- descriptor/stream runtime;
- arena allocation/free core at `0x39838..0x39A2B`;
- arena cleanup at `0x39A2C..`;
- sorted arena initialization around `0x39BBC`;
- final growth/allocation helpers through `0x39D4C`.

[CODE.2 continuity addendum, batches 128–143](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md) [CODE.2 final certification](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md)

Fresh binary inspection during this preservation pass rechecked CODE.2's DPMI-family instructions around `0x30D30..0x31012` and its heap/arena tail around `0x39838..0x39D4C`. The same distinctions recur:

```text
DOS-memory allocation
selector -> linear-base conversion
DPMI 0501 linear allocation
DPMI-handle lifetime
DOS conventional-memory fallback
arena ownership header
sorted miniheap list
free-list splitting/coalescing
```

The duplicated runtime architecture is expected for separately linked child executables. It should not be mistaken for DreamForge intentionally implementing two independent custom heap managers.

---

## 19. Startup differences between CODE.1 and CODE.2

The two applications share compiler/runtime ancestry but are not byte-identical after linking.

CODE.1's LE entry is `0xCC1C`; CODE.2's is `0x353A0`.

CODE.2 entry `0x353A0` jumps over `0x353A2..0x35417`, 118 bytes of proven embedded runtime data, with active startup beginning at `0x35418`. Its certified audit preserves:

- 4-byte stack alignment;
- DOS/extender branch distinctions;
- selector-backed PSP/environment handling;
- environment scanning including `no87=`;
- program-name/command-line termination behavior;
- Rational-mode BSS zeroing behavior.

[CODE.2 continuity addendum, batches 111–113](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)

The correct preservation approach is therefore:

> Reuse source-level runtime identities when the binary independently matches them, but never map CODE.1 addresses or globals onto CODE.2 by assumption.

That is the same standard used during the certified CODE.2 reconstruction.

---

## 20. Initial LE loading and relocation versus runtime allocation

Two different “loader” concepts exist and must remain separate.

### 20.1 Process-image loading

Before the CODE entry point executes, DOS/4GW Professional loads the LE application objects, establishes their mapped bases, prepares the initial stack/data environment, zero-fills virtual object state as required, and applies LE fixups/relocations.

This is **extender/LE-loader ownership**.

The relevant evidence is:

- the bound DOS/4GW Professional container;
- LE object table;
- object page table;
- LE fixup tables;
- mapped object bases;
- executable entry object/offset.

See `01_EXECUTABLE_CONTAINER_AND_MODULE_CYCLE.md` and `Runtime/CODE_OBJECT_MAP.md`.

### 20.2 Runtime allocations after entry

DPMI `0100/0501/0502/0503`, DOS `AH=48/49`, near-heap arenas, and `_curbrk` growth occur **after process startup** and service the running program.

These are **C runtime/application-process memory ownership**.

The distinction matters because an LE relocation handle or object fixup is not the same thing as a DPMI memory-block handle allocated later by the application.

---

## 21. Failure semantics are part of fidelity

A high-level reimplementation could allocate “effectively unlimited” host memory and thereby remove most original allocation failure paths. That would not be behaviorally identical.

The shipped runtime includes meaningful low-memory behavior:

- paragraph-size limits;
- DOS-block fragmentation effects;
- maximum-available DOS block reporting;
- DPMI carry/error codes;
- DPMI handle failures;
- 4-KiB rounding;
- arithmetic overflow rejection;
- arena-growth retry;
- DOS conventional-memory fallback;
- minimum-real-memory reservation behavior;
- program-break resize failure;
- allocator cached-largest-block behavior.

For an ordinary modern playthrough these may rarely matter. For preservation, debugging, fuzzing, save corruption analysis, or compatibility testing they are real parts of the executable.

A native port can choose a “modern unlimited memory” operational mode while retaining an **original-runtime compatibility model** for deterministic tests and edge behavior.

---

## 22. Modder/programmer guidance

### 22.1 Do not expose raw guest pointers as stable mod IDs

Addresses such as `0x218E0`, `0x52F0`, or arena addresses are runtime implementation details. Mods should use stable semantic IDs/handles.

### 22.2 Preserve serialization and resource IDs, not heap addresses

Persisted formats such as SAVE/LEVELS/OBJECTS/NPCS and resource IDs are meaningful cross-run identities. Heap pointers are not.

### 22.3 Keep guest address width explicit

Where a compatibility layer reproduces original state, use explicit 16-bit/32-bit guest types:

```cpp
using DosSegment   = uint16_t;
using Selector     = uint16_t;
using Linear32     = uint32_t;
using DpmiHandle32 = uint32_t;
using GuestOffset  = uint32_t;
```

Do not typedef all of them to a single opaque pointer and lose conversion boundaries.

### 22.4 Model ownership

An allocation record should retain enough information to know whether the original block was:

- selector/DOS backed;
- DPMI-handle backed;
- contiguous DGROUP growth;
- alternate-extender backed.

This makes debugging and faithful release behavior possible.

---

## 23. Historical corrections/enrichments from this pass

### C-03 — CODE.1 extender byte `0x531E` identified

**Previous archive state:** generic extender “mode” byte.

**New classification:** cross-corroborated Watcom runtime `_Extender` value.

Supporting evidence:

- startup stores the detected extender code at `0x531E`;
- heap/runtime branches consume that byte exactly like surviving Watcom extender-family tests;
- code `1` is Rational DOS/4G-compatible;
- code `9` selects the CodeBuilder allocator path.

### C-04 — Extender code 9 historical identity closed

**Previous archive state:** “mode-9 allocation” / neutral compatibility path.

**New classification:** **Intel CodeBuilder compatibility path**.

Evidence:

- CODE.1 startup explicitly selects value 9 on the `'CB'` branch;
- heap code value 9 executes `EAX=80004800h ; INT 21h`;
- surviving older Watcom extender table defines `X_INTEL=9`;
- surviving Watcom `CodeBuilderAlloc` uses the exact same interrupt sequence.

This is a documentation/terminology correction only. No executable semantics change.

### C-05 — Modern Open Watcom enum incompatibility documented

A 2022 Open Watcom maintenance change inserted Phar Lap V1 into the numeric sequence and shifted Intel CodeBuilder from 9 to 10. Therefore **current Open Watcom extender-number constants are not valid historical numeric labels for Ravenloft's binary**. [2022 enum-change commit](https://github.com/open-watcom/open-watcom-v2/commit/b8af89cf1002f150e6260be3c285b7b02dd7ed06)

### C-06 — DPMI/DOS six-byte ownership header semantically classified

The existing machine reconstruction already preserved the bytes. This archive promotes their architectural meaning:

```text
WORD selector + DWORD 0
    = DOS-memory-backed ownership

WORD 0 + DWORD DPMI handle
    = DPMI-linear-memory ownership
```

The classification is backed by the actual free/resize APIs and by surviving Watcom heap-source architecture.

---

## 24. Remaining bounded questions in this layer

### 24.1 Exact compiler/runtime release

The runtime source correlation is extremely strong, but this document does not assert an exact Watcom C/C++ release number for the complete Ravenloft application merely from current/surviving source similarity.

The embedded DOS/4GW Professional runtime identifies itself as 1.96; that is the extender identity, not automatically the compiler version.

### 24.2 Exact symbol for CODE.1 `0x531F`

The byte is consumed by compatibility logic, but surviving Watcom source revisions altered extender subtype and X32VM handling. Until a source revision is found that instruction-matches the shipped startup closely enough to establish layout, the byte remains behaviorally documented but not assigned a historical symbol.

### 24.3 Non-Rational alternate-extender behavior

Phar Lap and Intel CodeBuilder paths are real shipped runtime code but are not the normal execution path under the supplied bound DOS/4GW Professional environment. Dynamic behavior under those historical extenders has not been separately regression-tested.

### 24.4 Exact LE-loader internal implementation

The application-side evidence proves the LE format and fixup data; the bound DOS/4GW Professional loader performs initial image mapping before application entry. A full decompilation of the extender's internal LE loader is unnecessary for understanding Ravenloft gameplay but remains possible historical middleware work.

---

## 25. Source ledger

### Ravenloft primary evidence

1. **CODE.1 canonical reconstruction**  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp

2. **CODE.1 final binary/semantic audit**  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md

3. **CODE.1 methodology/identity**  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/DECOMPILATION_CONTINUITY.md

4. **CODE.2 canonical reconstruction**  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp

5. **CODE.2 final certification**  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md

6. **CODE.2 runtime reconstruction ledger**  
   https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md

7. **Corrected CODE object/container map**  
   https://github.com/Hyperbog/RLOFT/blob/b87ac1d6cb39c11b174824484a995b1b7cf6b5ee/Runtime/CODE_OBJECT_MAP.md

### Official/specification sources

8. **DPMI Committee, DOS Protected Mode Interface Specification, Version 1.0**  
   Functions used here include 0006h, 0100h, 0101h, 0501h, 0502h, and 0503h.  
   https://openwatcom.org/ftp/devel/docs/dpmi10.pdf

9. **Open Watcom C/C++ Programmer's Guide, Version 1.8**  
   Contains Watcom/Tenberry DOS/4GW documentation, protected-mode memory guidance, DPMI function descriptions, and DOS/4GW Professional information.  
   https://openwatcom.org/ftp/manuals/current/pguide.pdf

### Surviving Watcom/Sybase-derived runtime source

These sources carry Open Watcom copyright plus portions copyright 1983–2002 Sybase. They are used as historical/runtime corroboration, not as a substitute for Ravenloft bytes.

10. **32-bit DOS startup: `cstrt386.asm`**, pre-2022 extender numbering  
    https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/startup/a/cstrt386.asm

11. **Extender numeric table: `extender.inc`**, pre-2022 numbering with `X_INTEL=9`  
    https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/watcom/h/extender.inc

12. **Near-heap growth/system allocation: `grownear.c`**  
    Includes `RationalAlloc`, `CodeBuilderAlloc`, `__LastFree`, `__AdjustAmount`, `__CreateNewNHeap`, and `__ExpandDGROUP`.  
    https://github.com/open-watcom/open-watcom-v2/blob/7dc56fae4e6a1b9b1eb937f12bab97cfd0d096de/bld/clib/heap/c/grownear.c

13. **2022 extender-enum reindex commit**  
    Demonstrates why current Open Watcom numeric constants cannot be projected backward without version control.  
    https://github.com/open-watcom/open-watcom-v2/commit/b8af89cf1002f150e6260be3c285b7b02dd7ed06

---

## 26. Architectural checkpoint

The memory/runtime model established by this chapter is:

```text
DOS/4GW Professional loads LE image
        |
        v
Watcom-style 32-bit startup
        |
        +-- stack metadata
        +-- PSP/environment
        +-- extender-family detection
        +-- BSS initialization
        +-- runtime initializers
        |
        v
Ravenloft application
        |
        v
near-heap allocator
        |
        v
miniheap / arena
        |
        +-- DPMI linear block
        |      linear address + opaque handle
        |      0501 allocate / 0502 free / 0503 resize
        |
        +-- DOS-memory block
        |      real-mode segment + selector
        |      selector -> linear via 0006
        |
        +-- contiguous DGROUP/program-break growth
        |
        +-- Intel CodeBuilder compatibility path
```

The next preservation chapter should move one layer upward into the **resource manager and archive-loading architecture**: indexed RES groups, cache-entry state, resource types, allocation/lifetime behavior, decompression, and how CODE.1/CODE.2 resolve resource IDs into loaded objects.
