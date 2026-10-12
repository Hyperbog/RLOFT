# R04 checkpoint: CODE.1 original DB5B/10590 free-list execution

**Date:** 2026-10-12. **Branch:** `code1-decompilation`.
**Scope:** canonical `Recomp/` tests exercising the reconstructed CODE.1
heap core, not the separate `demo-v1` or `native-engine` workstreams.

## Source changes

- `src/code1_original_heap_growth_gate.cpp`: the host-DPMI path now
  routes `10AA9 -> DB5B -> 10590` through the original reconstructed
  `Code1/Code1.cpp` functions, using the same bounded guest-word
  callbacks for original static DGROUP and host-owned dynamic pages.
  The prior test-owned allocated-bit clearing adapter is removed.
- The same regression now requests a real source `104E0` first-fit split,
  frees through `DB5B/10590`, checks adjacent coalescence and the
  circular free-list sentinel, and then lets original `107CB` reclaim
  the DPMI 0501 allocation using 0502. The original `1083F` relocation
  scenario now acquires its terminal *allocated* block through `104E0`,
  not a manually written allocated-bit test precondition.
- `src/code1_original_dos_fallback_gate.cpp`: identical source-backed
  free-list integration for original `109C1` DOS 48h fallback, with
  split/coalesce/reclaim assertions against the separate host-owned DOS
  paragraph backing store and a non-overlapping static DGROUP.

Original code-object bytes were also independently disassembled from the
authenticated file: `DB5B` loads `EBX=30B8h` at `DB66h`,
calls `10590h` at `DB71h`, then clears byte `8E78h` at
`DB76h`. The original free-core entry `10590h` starts with
`PUSH ESI; PUSH EDI; PUSH ECX; PUSH DS; MOV DS,EDX`. This confirms
the reconstructed `AllocBoundary` callback order and the DS/arena
dispatch, not a complete x86-versus-C++ equivalence proof.

The asset-free initial DGROUP fixture seeds *only four* original CODE.1
LE data-object-2 words: `[30C0]=30D4`, `[30C8]=FFFFFFFF`,
`[30D8]=30D4`, `[30DC]=30D4`. These were independently read from
the supplied original CODE.1 LE image, SHA-256
`fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`.
No copyrighted image is added to the repository. This is still a
bounded reconstructed-source fixture and not original DOS initialization.

## Verification status and acceptance

The new CODE.1 source changes are in:

- `2d228d14`: original DPMI source free-list, 104E0 split/coalescence,
  and source-acquired relocation terminal;
- `00c8668c`: correct original sentinel-cursor assertion;
- `e902268e`: DOS 48/49 source free-list split/coalescence.

A local transcription-level check of the existing `104E0` and
`10590` source algorithms passed a split/coalesce and terminal-full-
allocation invariant. **This is not a compilation of the entire
original CODE.1 reconstruction.** The latest independently recorded
Windows result predates these changes: build #113, **51/51 CTests**.
A Windows compilation and complete CTest run on this new head are
required before the CODE.1 test changes can be called independently
verified. Do not increase the CTest pass count or major milestone score
based on this document.

**R04 remains NOT ACCEPTED; major recompilation gates: 3/12 = 25.00%.**
Still outstanding: actual Windows CTest confirmation for this change,
CODE.2 alternate DOS/39E1C modes under continuous startup, shared
CODE.1/CODE.2 selector and descriptor ownership, and independently
captured original DOS memory behavior. File I/O, DOS hardware and
gameplay parity remain distinct later gates.
