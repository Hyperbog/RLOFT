# CODE.2 2A078 VGA latch-copy and 30CD4 backing-buffer move

**Date:** 2026-10-08 · **Branch:** `demo-v1`  
**Scope:** Original DOS screen compositor's two DIFFERENT kinds of copying.  
**Status:** Native source-backed reusable implementations, CMake-registered
tests and original opcode auditing. This checkpoint does **not** represent
a completed current-HEAD Windows build or DOS-versus-native frame comparison.

## Executable provenance

Owned original `CODE.2` SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Object-1 code begins at original physical file byte `0x3AF24`.
All subsequent procedure offsets are logical object-1 offsets.

## 2A078 is a VGA four-plane hardware-latch copy

Original `2A078..2A102` takes seven original DWORD parameters:

`(destination_page,source_row,source_column,destination_row,destination_column,row_count,column_count)`.

The original `2AB38` helper computes source byte address from
the current `DWORD[F001]` and source row/column, returning
the source column's two low plane bits. The routine adds those
**source** plane bits to `column_count` and rounds the resulting
unsigned DWORD **up in groups of four using x86 ADD 3 / SHR 2**.
Destination column alignment is **not** substituted when
calculating the copy width.

It saves the **low byte** of original `DWORD[EFE9]` in AH,
uses `2AB6C` to select the destination page, computes the
destination byte address via another `2AB38`, and calls
`2AB6C` again to restore the original source page (thereby
recomputing both `EFE9` and `F001`). Both page selectors
mask the page value to eight bits; `F005` remains the
observed original page-stride DWORD, not a hardcoded value.

Original port operations:

- `2A0BD..2A0C1`: VGA graphics controller
  `OUT 03CEh, AX=4105h`, setting index `05h` mode `41h`:
  graphics **write mode 1** with four-plane latch copying.
- `2A0C7..2A0CB`: VGA sequencer `OUT 03C4h, AX=0F02h`,
  enabling **all four** destination write planes.
- `2A0E8`: `REP MOVSB` in **forward** DF=0 order.
  A VGA source read loads four hardware latches. The
  destination write in graphics write mode 1 copies
  **all four latched source-plane bytes** to the new
  address, rather than copying a single packed color.
- The row loop advances each physical address by
  `DWORD[EFF9] - bytes_per_row` after copying a row;
  the original `DEC EAX / JNE` does **not** guard an
  initially zero row count.
- `2A0F6..2A0FA` restores graphics controller register
  `05h` to **0x40**, leaving sequencer register
  `02h` at **0x0F**. These post-copy registers remain
  observable by subsequent rendering instructions.

This is intentionally **not memmove**. If the source and
destination byte ranges overlap on the same VGA page,
the source for each successive `MOVSB` may already have
been overwritten; the exact original forward copy can
propagate an earlier pixel's plane values through multiple
destination bytes. Do not use `std::memmove` or an
intermediate whole-rectangle snapshot for this routine.

New `CombatOriginalVgaCopyModel::copy_2a078` executes
that source-ordered four-plane transfer directly through
`VgaPlanarMemory::read/write`, with caller-owned
`EFE9/F001/F005/EFF9/EFED/EFF1` globals and observed
graphics-controller mode. It preflights bounded host
captures before any VGA/global mutation; malformed
or out-of-aperture captures return unsupported rather
than reinterpreting original DOS unchecked behavior.

## 30CD4 is a distinct overlap-aware DGROUP memmove

Original `30CD4..30D25` is not a VGA copy. It reads the
`(destination,source,length)` original DWORD arguments,
compares original **unsigned pointers** and computes
`source+length` using original wrapping 32-bit `LEA`.

When `src < dest < src+length` by the original unsigned
comparisons, it sets DF and copies **backward** using
`REP MOVSW` and a possible trailing byte. Otherwise,
it copies **forward** with `REP MOVSD` plus residual
bytes. The backward path restores DF with `CLD`
before returning. Identical source/destination pointers
are a no-op.

Original `1A8C8` uses this to move exactly `0x400`
bytes of the buffered overlay between DGROUP blocks:

`source=9E3Ah+(page<<10)`,  
`destination=9E3Ah+((page^1)<<10)`.

New `CombatOriginalDgroupMoveModel::move_30cd4`
operates on an explicitly captured original DGROUP
byte slice and preserves original unsigned overlap
branch choice. It fails closed for uncaptured memory
without replacing missing original bytes.

## Native integration

New files:

- `include/rloft/combat_original_vga_copy.hpp`
- `src/combat_original_vga_copy.cpp`
- `tests/combat_original_vga_copy_tests.cpp`
- `include/rloft/combat_original_dgroup_move.hpp`
- `src/combat_original_dgroup_move.cpp`
- `tests/combat_original_dgroup_move_tests.cpp`
- `include/rloft/combat_original_vga_copy_binding.hpp`
- `src/combat_original_vga_copy_binding.cpp`
- `tests/combat_original_vga_copy_binding_tests.cpp`

All six source/test files are registered through `DemoV1/CMakeLists.txt`
and all three new test targets are in the assertion-enabled
CTest list.

`CombatOriginalVgaCopyBinder::bind_direct_copy` supplies
the exact `2A078` collaborator for the existing
`CombatOriginalCompositeModel::run_1a8c8`, so its
direct path produces **actual four-plane VGA bytes**.

`bind_copy_and_backing` additionally supplies `30CD4`
with captured DGROUP backing memory. The integration
test exercises the original compositor's full buffered
**copy→0x400 transfer→restore callback order** with
injected `1A288` and `1AB20` observers.

The `1C258` timed-message path also can invoke
the original compositor adapter, perform real planar
rectangle copying, and read `30700` **only after**
the compositing collaborator returns. The source
of the drawn font/glyph and the overlay services
must still be original runtime components.

## Verification

- **40 new original `2A078` opcode anchors** independently
  matched the user-owned `CODE.2`.
- **30 new original `30CD4` opcode anchors** independently
  matched the same immutable executable.
- Cumulative source ledger: **754 named x86 byte signatures
  at 746 distinct logical instruction sites**. This is
  evidence of original source provenance, not an engine
  coverage or completion percentage.
- The **exact committed** `combat_original_vga_copy.cpp`
  and header were independently reproduced in a local
  minimal interface-compatible VGA harness. Git blob
  hashes matched the committed GitHub source. Focused
  test runs passed strict C++17 Debug, optimized Release
  with assertions enabled, and UndefinedBehaviorSanitizer.
- The **exact committed** `combat_original_dgroup_move.cpp`
  and header were likewise Git-blob matched and passed
  strict Debug, Release, and UBSan source tests covering
  real original `0x400` backing transfer, odd-byte reverse
  overlap, and forward copy.
- New checked-in CTests additionally exercise four-plane
  source/destination bytes, source-page restoration,
  graphics-controller post-copy state, original
  forward *non-memmove* VGA aliasing, explicit unsupported
  boundaries, `1A8C8 → 2A078` native pixels,
  `1C258 → 1A8C8 → 2A078 → 30700` timing,
  and buffered overlay DGROUP transfer order.
- The exact checked-in CTest targets and the full
  Windows executable **have NOT been rebuilt** at this
  newer branch HEAD. The last verified full Windows run
  [#634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281)
  passed **130/130 tests** at the much earlier source
  commit `40e3f14b1072a7a0180ea1b0b4c1cea437137631`.

## Remaining end-to-end fidelity gates

Original `1A288` overlay suspension and `1AB20`
overlay restoration still require source-backed native
implementations. The full game must supply authentic
VGA/DGROUP globals, sprite/font assets, pending HUD
state and original IRQ ticks; compare the resulting
four-plane output, state changes, and elapsed original
time with a reference DOS trace. The native tests
are not proof of total game behavior.

**Formal acceptance: 1 of 5 major demo gates.**
Engineering completion: **not yet objectively weighted**.
GitHub Actions: **manual-only; no runner triggered here**.


## Next audited overlay entry points: 1A288 and 1AB20

A preliminary byte-level audit of original `1A288` and
`1AB20` confirms that neither is a plain no-op or
generic mouse overlay call:

- `1A288` first tests `WORD[85FC]` and `WORD[9E37]`.
  Its zero-reference branch calls `2E590`, then `1A3B8`
  with pointers to 9E31/9E33/9E35, then `292BC`
  with a `0x20` by `0x10` overlay capture and an
  original backing source at `9E3A+(BYTE[9E39]<<10)`.
  Subsequent `2AE30` calls and the long mode-dependent
  branch tree remain to be reconstructed, and cannot be
  replaced by an unconditional copy.
- `1AB20` also checks `85FC`, decrements `9E37`,
  conditionally invokes `2E590`, and uses `2932C`
  to restore the 32×16 backing image when its gate allows.
  Later branches depend on additional overlay/mouse globals
  and call `29804`/`2E5D4`. The authoritative
  implementation must follow the original guards and
  per-branch callback sequence.

This audit narrows the next work package; it does **not**
claim those two procedures are implemented or verified.
