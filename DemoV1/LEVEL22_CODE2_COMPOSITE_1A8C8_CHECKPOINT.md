# Original CODE.2 1A8C8 overlay compositor — source and integration checkpoint

**Date:** 2026-10-08. **Branch:** `demo-v1`.
**Scope:** Original logical `1A8C8..1A9F7` and its immediately called
`2A078`, `1A288`, `30CD4`, and `1AB20` dependencies.

## Source evidence and correct coordinate convention

Immutable user-owned original `CODE.2` SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Object-1 executable byte base: `0x3AF24`.
Original code addresses below are object-local offsets, not virtual
objectbase+0x10000 addresses.

`1A8C8(page,row,column,rows,columns)` delegates to
`2A078(page^1,row,column,row,column,rows,columns)`.
This argument convention was verified against **2A078's two
2AB38(row,column) calls** and its later
`EDX=(columns+source_plane+3)>>2`, followed by `rows`
scanlines. Earlier informal references to “left/top/width/height”
are misleading in this DOS routine. The first coordinate is
a VGA **row**; the second is a **column**.

Original overlay state is `WORD[85FC]` enabled,
signed `WORD[9E37]` refcount, signed `WORD[9E31]`
overlay **row**, signed `WORD[9E33]` overlay
**column**, `BYTE[9E39]` active page, and
`WORD[A6BA]` redraw-busy flag.

## Exact 1A8C8 branching

1. If overlay enabled `85FC==0`, or signed refcount
   `9E37<1`, take the **fallback direct** `2A078`
   call. The original does **not touch A6BA** on this path.
2. Otherwise set `WORD[A6BA]=1`, then perform the four
   original **signed DWORD** comparisons. The overlay
   spans `[row,row+32]` and `[column,column+16]`
   with **inclusive** touching boundaries; the copy
   spans `[row,row+rows]` and
   `[column,column+columns]`. Copy rectangle additions
   use original 32-bit wrap; the x86 `JL/JG/JLE`
   decisions interpret each result as signed. If
   disjoint, call `2A078` directly and clear `A6BA`.
3. If intersecting, preserve the original `BYTE[9E39]`
   and original `9E31/9E33` row/column registers,
   decrement `WORD[9E37]` with original WORD wrap,
   temporarily store `BYTE[9E39]=low8(page)`, and
   synchronously call **1A288**.
4. Call `2A078(page^1,row,column,row,column,rows,columns)`.
5. Move exactly **0x400 bytes** with original `30CD4`:
   source `DGROUP:9E3A + (page<<10)`, destination
   `DGROUP:9E3A + ((page^1)<<10)`; both calculations
   are original DWORD arithmetic. These are original
   DOS addresses, **not host pointers**.
6. Call **1AB20(saved overlay row, saved overlay column)**.
   The row/column arguments were captured from EBP/EDI
   before nested callbacks; they are not late rereads of
   possibly mutated `9E31/9E33`. Then increment the
   **live** `WORD[9E37]`, restore the saved active-page
   byte `9E39`, and finally set `WORD[A6BA]=0`.

The original code does not expose a normalized screen rectangle
abstraction: preserving signed comparisons and tick-dependent
callback effects is mandatory, including odd high-bit coordinates.

## New native source and tests

New reusable components:

- `include/rloft/combat_original_composite.hpp`
- `src/combat_original_composite.cpp`
- `tests/combat_original_composite_tests.cpp`

`CombatOriginalCompositeModel::inspect_1a8c8` returns one of
three exact branch classifications without mutating state:
`DirectWithoutOverlay`, `DirectOutsideOverlay`,
`BufferedOverlayOverlap`.

`run_1a8c8` executes the original routine's **caller-visible
wrapper state writes and synchronous callback order**. The
four lower-level DOS services are deliberately injected:
`2A078`, `1A288`, `30CD4`, `1AB20`.
A missing service needed for the selected branch yields an
explicit host error **before wrapper-side writes**; the
unbounded original binary did not provide that safety contract.

`bind_irq_display_1a8c8` connects this reusable wrapper
to `CombatIrqDisplayServices::composite_1a8c8`, so the
existing exact message update can follow
`1C258 -> 1A8C8 -> 2A078 -> return -> 30700`
without eagerly sampling the independent IRQ clock.

**31 new original x86 signature checks passed 31/31**
against the immutable supplied executable. Cumulative
audited executable offsets: **684 named signatures,
676 distinct logical instruction addresses**, plus
the prior data-table anchors. These check counts are
not runtime executable coverage percentages.

The isolated source-equivalent C++17 model passed local
`-Wall -Wextra -Werror -pedantic` Debug, optimized
Release with assertions explicitly enabled, and
UndefinedBehaviorSanitizer runs. The checked-in CTest
adds full branch/argument state fixtures, inclusive
bounds, signed coordinate wrap, captured overlay
coordinates, original backing-buffer addresses,
missing-service rejection and one
`1C258 -> 1A8C8 -> 2A078 -> post-render 30700`
call-order case.

The **full checked-in CTest and a consolidated Windows
CMake build have not yet been executed on this branch
HEAD**. The last complete Windows run
[#634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281)
passed **130/130** at earlier source SHA
`40e3f14b1072a7a0180ea1b0b4c1cea437137631`.
Do not assign that historic result to this newer source.

## Next fidelity boundary

The `1A8C8` wrapper is **not** the complete compositor.
Original `2A078` must perform its actual VGA
graphics-controller/sequencer port operations and original
source/destination four-plane rectangle copying.
The related `1A288` and `1AB20` overlay services
also branch into additional presentation/resource callbacks
that are not replaced by the wrapper model.

Next reconstruct `2A078` plane-copy and its hardware
port writes using the existing native `VgaPlanarMemory`;
preserve `2AB38` address calculations and the original
graphics-controller read/write modes. Then bind original
overlay buffers and DOS resource data and run DOS-reference
frame, copy, and IRQ trace comparisons. Neither the
sprite machine-state work nor this compositing wrapper
proves pixel-for-pixel runtime equivalence yet.

**Progress reporting:** Formal release acceptance remains
**1/5 gates**, while substantive shared-engine reconstruction
has advanced independently. The engineering-completion
percentage is not objectively measured; see
[demo scorecard](DEMO_PROGRESS_SCORECARD_20261008.md).
GitHub Actions remains manual-only.
