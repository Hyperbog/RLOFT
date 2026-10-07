# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 02 — Protected-Mode Startup, DPMI, Selectors, Relocation, and Runtime Memory Ownership

**Status:** PRIMARY RUNTIME RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

This chapter documents the machinery below Ravenloft's gameplay layer but above the bound DOS/4GW container described in Chapter 01:

- protected-mode startup;
- PSP and environment handling;
- selector-qualified memory;
- DPMI services;
- DOS/conventional-memory allocation;
- linear-memory allocation and handles;
- executable-image relocation;
- runtime heap/arena ownership;
- program-break growth;
- the point where compiler/runtime memory management hands usable storage to Ravenloft.

The important preservation rule is that the original program does **not** treat every address as one undifferentiated flat pointer. Even though DOS/4GW presents a 32-bit protected-mode environment, the machine code still carries distinct selectors, linear addresses, DOS segments, DPMI handles, object-relative addresses, and runtime arena headers. Flattening those distinctions too early caused several reconstruction errors during the original audits. Sources: [CODE.1 continuity rules](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/DECOMPILATION_CONTINUITY.md), [CODE.1 final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

---

## 2. Startup begins in extender/runtime code, not Ravenloft gameplay

CODE.1 enters executable object 1 at logical `0xCC1C`. CODE.2 enters at `0x353A0`. Both entries belong to protected-mode/runtime startup rather than the first game-specific main-loop instruction.

For CODE.1, the entry aligns ESP to four bytes, records the initial stack/break values in DGROUP, queries DOS/extender state through `INT 21h`, and branches according to the detected execution environment before continuing through PSP/environment processing. The certified reconstruction preserves separate linear-memory and selector-qualified reads because the original code accesses both kinds. [Code1.cpp, startup reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp), [CODE.1 final audit, startup checkpoint](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

CODE.2 independently reconstructs the same broad runtime concerns but is not assumed identical. Its entry at `0x353A0` jumps over `0x353A2..0x35417`, a proven 118-byte embedded runtime-data block, and continues at `0x35418`. The audited startup preserves stack alignment, DOS/extender branches, selector-backed PSP/environment access, environment parsing, `no87=` handling, and bounded zero-fill behavior. [CODE.2 continuity addendum, batches 111–113](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md).

**Evidence class:** CODE-PROVEN.

---

## 3. PSP and environment state remain selector-sensitive

The startup code obtains command-line and environment information through DOS process structures rather than from a modern argv object.

CODE.1's audited startup specifically preserves:

- PSP command-tail scanning;
- the command tail beginning at the conventional PSP location;
- environment traversal to its double-NUL terminator;
- the encoded three-byte transition after that terminator before copying the executable pathname;
- selector-qualified reads when the PSP/environment is not represented by an ordinary flat application pointer.

The `CD97` startup continuation was corrected because an earlier lift advanced only two bytes after the environment double-NUL. The immutable binary performs `LODSB` plus two `INC ESI` operations, producing an effective three-byte advance before the executable pathname copy. [CODE.1 final audit, startup continuation](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

This matters because environment/PSP memory is part of the DOS process contract. A native port can translate this into a host argument/environment representation, but preservation code and executable-analysis tooling must retain the original addressing model.

---

## 4. DPMI is the protected-mode service boundary

The runtime uses DPMI services through interrupt `31h`.

Open Watcom's DOS/4GW documentation lists the services supplied by DOS/4GW/DOS/4GW Professional, including:

```text
0006  Get Segment Base Address

0100  Allocate DOS Memory Block
0101  Free DOS Memory Block
0102  Resize DOS Memory Block

0500  Get Free Memory Information
0501  Allocate Memory Block
0502  Free Memory Block
0503  Resize Memory Block
```

Source: Open Watcom C/C++ Programmer's Guide, DOS/4GW DPMI services:  
https://openwatcom.org/ftp/manuals/current/pguide.pdf

The DPMI 1.0 specification separately defines the `0501h/0502h/0503h` family as linear-memory allocation, free, and resize operations; these operations return or consume both a linear address and an allocation handle. DPMI's own documentation explicitly distinguishes ownership of a linear block from descriptor-based addressability. Source: DPMI Committee, *DOS Protected Mode Interface Specification 1.0*, mirrored by PCjs:  
https://docs.pcjs.org/specs/dpmi/1991_03_12-DPMI_Spec_v10.pdf

This distinction appears directly in Ravenloft's reconstructed runtime.

**Evidence class:** OFFICIAL-DOCUMENTED for service semantics; CODE-PROVEN for Ravenloft usage.

---

## 5. DOS-memory allocation and DPMI linear allocation are different mechanisms

CODE.1 routines `0x6F10..0x71C7` are a compact set of extender/memory wrappers. Their final reconstruction deliberately exposes host-side services as explicit boundaries rather than replacing them with malloc/free. [Code1.cpp `function_6f10` family](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

### 5.1 DOS/conventional-memory path

`0x6F10` rounds a requested byte quantity to paragraphs and invokes the DOS-memory allocator. It returns multiple forms of the result: selector ownership, segment-derived information, and a linearized real-mode-style address. The promoted selector arguments in the related free/base wrappers remain 32-bit cdecl slots even though only the low 16 bits are consumed. [CODE.1 final audit, DPMI wrapper checkpoint](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

`0x6FF4` performs another DOS-memory allocation path. On success it obtains the selector's base and writes a six-byte ownership header at the returned memory base:

```text
+0x00  WORD  selector
+0x02  DWORD  0
```

That is direct evidence that the allocation's selector is not disposable metadata: it is persisted so the corresponding release path can later distinguish the backing mechanism. [Code1.cpp `function_6ff4`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

### 5.2 DPMI linear-memory path

`0x709C` uses the linear-memory allocator and receives a DPMI handle. On success it writes:

```text
+0x00  WORD  0
+0x02  DWORD  DPMI allocation handle
```

The zero word distinguishes the ownership form from the DOS-memory selector header. [Code1.cpp `function_709c`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp), [CODE.1 final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

Therefore the same six-byte prefix can encode two backing types:

```text
DOS-backed:
    WORD selector != 0
    DWORD 0

DPMI-linear-backed:
    WORD 0
    DWORD DPMI handle
```

That ownership convention is a reconstruction-level invariant and should be retained in compatibility tests.

### 5.3 Largest-DOS-block query

`0x70FC` was initially at risk of being mislabeled as another selector allocator. The final audit proves that it performs the DOS-memory query pattern and returns the largest available DOS block in bytes on the expected failure/query path. [CODE.1 final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

---

## 6. Segment base and segment limit are live runtime concepts

The runtime does not assume that a selector numerically equals a linear pointer.

In the heap fallback path, `0x109C1` can allocate conventional DOS memory and then uses DPMI function `0006h`, Get Segment Base Address, to obtain the corresponding linear base before constructing an arena there. The final audit explicitly removed a fabricated null-result branch after this encoded segment-base query. [Code1.cpp `function_109c1`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp), [CODE.1 post-closure checkpoint 10o](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

The runtime also executes `LSL DS` in selected extender modes to obtain the current byte-granular data-segment limit and then stores limit+1 as a current boundary value. Intel defines `LSL` as loading the effective segment limit from the descriptor selected by its source operand after descriptor/access validation. Sources: [Intel SDM Volume 3A, §5.10.3](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-system-programming-manual-325384.pdf), [Intel Instruction Set Reference, LSL](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2a-manual.pdf).

Ravenloft's runtime uses this at `0x10974` and again in the `0x10AA9` heap-extension logic. [Code1.cpp heap-growth reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp), [CODE.1 final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

This is why a faithful decompilation must retain a selector-limit boundary instead of replacing the operation with the host process's address-space size.

---

## 7. The runtime heap is layered over multiple backing allocators

Ravenloft's C/CRT-style heap is not identical to the OS/extender allocator. It owns arenas and free blocks built over lower-level memory acquired from several mechanisms.

### 7.1 Arena reclamation

`0x107CB` walks the arena list and detects arenas whose terminal free block spans the whole arena. Release depends on the stored arena ownership field:

- DPMI-backed arena -> DPMI `0502h` free using the stored handle.
- DOS-backed arena -> DOS memory free using the stored segment/selector ownership.

[Code1.cpp `function_107cb`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

### 7.2 DPMI resize path

`0x1083F` attempts to resize an eligible DPMI-backed arena using `0503h`. The service may return both a relocated linear address and a new handle. The final audit found a critical normalization error here: the return exposed to the runtime is the relocated usable pointer at `new_arena + 0x24`, not the arena base and not the free-block pointer produced by an internal splitter. [Code1.cpp `function_1083f`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp), [CODE.1 final audit checkpoints 10n/10o](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

### 7.3 New-arena acquisition

`0x109C1` first attempts DPMI `0501h`. On success it records:

```text
arena +0x00  DPMI handle
arena +0x04  allocation size
arena +0x08  next arena
arena +0x10  0  (marks non-DOS backing)
```

and returns a pointer past the arena-management prefix.

If the DPMI allocation fails, a bounded conventional-memory fallback is possible. That path deliberately performs a DOS AH=48h probe allocation, attempts the requested allocation while the probe is still held, frees the probe, then tests the requested result. The ordering is semantically significant because freeing the probe early changes DOS allocation availability; this exact error was found and corrected during the semantic audit. The successful fallback converts the DOS segment to a linear base with DPMI `0006h` and records the segment in the arena ownership field. [Code1.cpp `function_109c1`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp), [CODE.1 final audit checkpoint 10n](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

This demonstrates a general rule:

> runtime heap block != DPMI block != DOS memory block.

The runtime heap owns suballocation metadata over a lower-level backing allocation whose release method must remain known.

---

## 8. Heap-extension modes must remain numerically preserved

The heap-extension driver `0x10AA9` branches on runtime mode byte `DGROUP 0x531E`.

The supplied reconstruction proves separate encoded behavior for:

- mode 1;
- mode 9;
- modes 2..8;
- the remaining/default path.

Mode 1 uses the DPMI/DOS arena-acquisition family. Mode 9 uses a separate legacy/extender allocation boundary represented in the reconstruction as the encoded `80004800h` mechanism. Modes 2..8 explicitly refresh the DS selector limit before program-break-style growth. [Code1.cpp `function_10aa9`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

The historical source enum names for these numeric runtime modes are **not** established here. They should remain numeric until primary toolchain documentation or stronger binary identification recovers their exact meaning.

**Evidence class:** CODE-PROVEN behavior; UNKNOWN original enum nomenclature.

---

## 9. Program-break growth is distinct from block allocation

`0x10D3A..0x10E80` implements a program-break-style layer separate from the DPMI block allocator.

`0x10DD7` chooses between two resize mechanisms according to runtime mode. It preserves the old break, attempts a resize, stores the requested new break on success, and reports an allocation-style error on failure. One audit correction established that the nonzero/extender resize path receives the encoded rounded 4 KiB **page count**, not the raw requested byte address. [Code1.cpp `function_10dd7`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp), [CODE.1 final audit checkpoint 10o](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

A native port may implement all of this with host virtual memory, but a faithful compatibility model should keep the original interfaces distinct so failure behavior, growth thresholds, and ownership metadata can be tested independently.

---

## 10. Ravenloft also contains an LE-style image loader/relocator

CODE.1 routine `0xF8C4..0xFC96` is a resource image loader/relocator. It is important because it demonstrates that LE-style object/page/fixup concepts are not merely passive properties of the outer executable: similar executable-image structures are actively parsed at runtime.

The loader:

1. obtains or opens a source;
2. determines required image size;
3. optionally allocates destination memory;
4. zero-fills the destination;
5. follows the header pointer at source offset `0x3C`;
6. validates the expected two-byte signature;
7. reads a fixed header;
8. walks object records;
9. walks page-map entries;
10. applies object alignment;
11. copies bounded page data into guest memory;
12. walks fixup-page/fixup-record data;
13. validates supported fixup forms;
14. writes relocated 32-bit addresses into the loaded guest image.

[Code1.cpp `function_f8c4`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp), [CODE.1 final audit checkpoint 10l](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

The final audit corrected its page-data calculation because x86 variable shifts mask the count; the faithful reconstruction uses `page_shift & 31` before the 32-bit shift. This is another case where “equivalent-looking” host C++ could invoke undefined behavior even though the x86 instruction has defined masked-shift semantics. [CODE.1 final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

The adjacent one-byte routine at `0xFC97` is a genuine executable `RET`, not padding: an original LE fixup record targets it. [Code1.cpp `function_fc97`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

---

## 11. CODE.2 independently contains the same runtime classes

CODE.2 was not certified by copying CODE.1. Its audit independently reconstructed a DPMI/memory-wrapper region through `0x31012`, selector-bearing interrupt/vector state, selector-aware startup at `0x35418`, and later arena cleanup/growth routines. The continuity record explicitly required each CODE.1-derived hypothesis to be re-established from CODE.2 bytes. [CODE.2 continuity addendum, batches 71–72, 112, 141–143](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md).

Therefore these runtime families are not accidental CODE.1-only artifacts. They are part of the common protected-mode runtime architecture carried into both application modules, even though addresses and application data differ.

---

## 12. Programmer-facing invariants

A preservation-grade engine or emulator bridge should retain these distinctions:

```text
Selector
    != linear address

DOS segment
    != DPMI handle

DPMI block
    != runtime heap arena

runtime heap arena
    != user allocation

object-relative address
    != mapped linear address

guest linear address
    != host-native pointer
```

The certified C++ intentionally keeps “guest” reads/writes behind explicit boundaries in sensitive routines instead of casting original 32-bit addresses directly to host pointers. [Code1.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp).

For a modern Ravenloft engine, a useful architecture is to preserve these as typed concepts at the compatibility layer even if they ultimately map to ordinary host allocations:

```cpp
struct GuestLinearAddress;
struct GuestSelector;
struct DosSegment;
struct DpmiHandle;
struct RuntimeArena;
```

This prevents historical compatibility code from accidentally treating an ownership handle as an address or bypassing selector/limit behavior.

---

## 13. Historical unknowns retained

This pass does not manufacture names for the runtime mode byte at `0x531E` or claim an exact original Watcom source symbol for every allocator wrapper.

Open questions:

1. exact original source-level enum names for runtime modes 0..9;
2. exact compiler/runtime library version correspondence for every heap routine;
3. whether any Ravenloft-specific code directly depended on allocation failure fragmentation characteristics beyond the audited wrapper behavior;
4. whether alternate shipped builds exercise currently rare fallback modes differently.

These are historical/toolchain enrichment questions. They do not block faithful behavior for the supplied build.

---

## 14. Source ledger

### Repository evidence

- [Code1/Code1.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)
- [Code1/FINAL_BINARY_AUDIT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)
- [Code1/DECOMPILATION_CONTINUITY.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/DECOMPILATION_CONTINUITY.md)
- [Code2/CODE2_CONTINUITY_ADDENDUM.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)
- [Code2/FINAL_CERTIFICATION.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md)

### Official technical documentation

- Open Watcom C/C++ Programmer's Guide, DOS/4GW DPMI services and protected-mode programming:  
  https://openwatcom.org/ftp/manuals/current/pguide.pdf
- DPMI Committee, *DOS Protected Mode Interface Specification 1.0* (archival mirror):  
  https://docs.pcjs.org/specs/dpmi/1991_03_12-DPMI_Spec_v10.pdf
- Intel 64 and IA-32 Architectures Software Developer's Manual, System Programming Guide, selector/segment-limit semantics:  
  https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-system-programming-manual-325384.pdf
- Intel Instruction Set Reference, `LSL`:  
  https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2a-manual.pdf

---

## 15. Architectural checkpoint

The protected-mode memory stack can now be summarized as:

```text
DOS/4GW Professional / DPMI host
    |
    +-- selectors and descriptor bases/limits
    |
    +-- DOS conventional-memory blocks
    |       ownership: segment/selector
    |
    +-- DPMI linear-memory blocks
            ownership: DPMI handle
            |
            v
      runtime arena layer
            |
            v
      runtime heap/free blocks
            |
            v
      Ravenloft/application allocations
```

In parallel, the runtime can parse and relocate LE-style loaded images into guest linear memory.

The next preservation chapter should move from memory ownership into the **resource/archive manager and the loader/cache system that Ravenloft builds on top of this runtime memory layer**.
