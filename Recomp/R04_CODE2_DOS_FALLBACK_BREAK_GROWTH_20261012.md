# R04 checkpoint: original CODE.2 DOS fallback, 39E1C growth, and free-list fidelity

**Date:** 2026-10-12. **Canonical branch:** `code1-decompilation`.
**Milestone:** R04 — DOS/DPMI memory runtime — **not accepted**.

## Verified implementation

Source files:
- `Recomp/src/dos_paragraph_memory.hpp` and `test_dos_paragraph_memory.cpp`
- `Recomp/src/code2_original_dos_fallback_gate.cpp`
- `Code2/Code2.cpp` (specific `398E0` semantic correction)
- `Recomp/CMakeLists.txt` (new original CODE.2 source CTest)

Original CODE.2 remains tied to SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`;
the original file bytes were not modified.

### Original 39C7B / 39D4D DOS fallback

The regression executes the compiled canonical CODE.2
`37B42 → 39D4D → 39C7B`. An explicitly rejected DPMI `0501h`
request causes the **original CODE.2 source**, not a replacement allocator,
to request a DOS `48h` probe block, request the full DOS paragraph
allocation, free the probe with `49h`, and initialize its arena with
original `39BBC/39838/398E0`. It verifies:

- a source-owned arena, opaque DOS allocation selector, and the
  actual host-owned shared guest bytes;
- release of the probe, allocation boundaries, and no preseeded arena;
- original `37C37/398E0` full free/coalescence followed by
  original `39A2C → DOS 49h` reclamation, which invalidates the selector;
- source rejection of requests larger than the 64-KiB fallback ceiling;
- failure of the *second* DOS allocation request and original probe
  rollback without a leaked segment or invented successful arena.

Conventional DOS allocation and original CODE.2 static DGROUP are
**different address spaces**. The test host's `DosParagraphMemory`
provides an optional shared guest aperture, but maps DOS conventional
`segment × 16` through an explicit `0x300000` guest alias selected by
the test's selector-base callback. This is an **adapter policy**, not
measured DOS/4GW LDT descriptor parity. Independent tests cover shared
AH=`48h/49h/4Ah` allocate, grow, shrink, failure rollback, bytes
preserved, zero initialization, stale addresses, and strict isolation
from static DGROUP.

### Original 39E1C extender limit/break growth

A separately initialized guest executes
`37B42 → 39E1C → 39BBC/39838/398E0` in source MODE=2, using a
bounded host DS-limit and break-extension service. The first source
extension creates a new arena without preseeded blocks; the second
source extension grows the **adjacent** arena while preserving an
already-allocated payload. The test checks:

- source ownership, live payload contents, break extension counts,
  free-list stitching and adjacent coalescence;
- rejected over-budget extension with unchanged live state;
- complete original `398E0` free/coalescence after multiple extensions;
- a subsequent 7,000-byte original `39838` first-fit reuse requiring
  **zero** additional host break allocations.

### Byte-grounded 398E0 reconstruction correction

The newly added multi-growth regression first failed because
the preexisting C++ implementation of original `398E0` skipped the
circular free-list sentinel when inserting a free block *above* the
highest existing free block, leaving contiguous space fragmented.

The supplied authentic `CODE.2` bytes, disassembled as x86 32-bit
code object address `39939h–39948h`, include:

```asm
39939  8B 7F 08       MOV EDI,[EDI+08h]
3993C  39 FE          CMP ESI,EDI
3993E  72 5E          JB 3999Eh
39940  8D 7B 20       LEA EDI,[EBX+20h]   ; circular-list sentinel
39943  3B 73 24       CMP ESI,[EBX+24h]   ; first free entry
39946  77 56          JA 3999Eh           ; insert before sentinel
```

The source `398E0` adapter is now a monotonic sorted-successor
traversal for a **well-formed** circular free list, preserving the
binary's insertion/coalescing semantics. This is not a claim that
the adaptive x86 search heuristic has been replicated instruction
for instruction. In the old implementation the new high block
incorrectly advanced from sentinel to the low free entry, missing
the adjacent high free block.

## Windows evidence

- [Build #110](https://github.com/Hyperbog/RLOFT/actions/runs/38259753905):
  **50/51**; new `39E1C` multi-growth test exposes the bug.
- [Build #111](https://github.com/Hyperbog/RLOFT/actions/runs/38259979852):
  **50/51**; diagnostic confirms repeated source break growth with a
  fragmented high-end free-list instead of a usable contiguous block.
- [Build #112](https://github.com/Hyperbog/RLOFT/actions/runs/38260569907):
  **51/51** after byte-grounded `398E0` correction.
- [Build #113](https://github.com/Hyperbog/RLOFT/actions/runs/38260777313):
  **51/51** including stronger full-arena coalescence and 7,000-byte
  source first-fit *reuse without new allocations*.

## R04 acceptance decision

**Still OPEN: 3 of 12 gates accepted, 25.00%.** This verifies the
bounded source-backed memory paths, not a complete faithful DOS/4GW
host runtime or DOS-observed behavior.

Remaining: integrate authentic CODE.1 `DB5B → 10590` free-list semantics
without test-owned released-bit shortcuts; connect the DOS and
`39E1C` alternative CODE.2 branches into continuous startup (rather
than only independent source regressions); unify selectors/descriptor
limits/ownership with CODE.1; compare real original DOS memory behavior
against the chosen host policies; retain explicit hardware and
gameplay-unimplemented boundaries. Do not mark R04 accepted until
its full acceptance scope is independently satisfied.
