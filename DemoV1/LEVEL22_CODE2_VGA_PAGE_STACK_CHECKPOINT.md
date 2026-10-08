# Original CODE.2 2E590 / 2E5D4 VGA page stack checkpoint

**Date:** 2026-10-08 · **Branch:** `demo-v1`  
**Original owned file:** `CODE.2`, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
**CODE.2 object-1 file base:** `0x3AF24` (all instruction
offsets below are logical object-1 positions).

## Verified original state and exact instruction order

Original `2E590` selects a new page while saving the page
already presented; `2E5D4` reverses that operation. Crucially,
the original uses **three different page globals**:

| Original DGROUP | Type | Meaning |
| --- | --- | --- |
| `F0D0` | BYTE | Page-stack depth, with original 8-bit wrap |
| `EF4D` | BYTE | Active/presented page saved on stack |
| `2B644 + BYTE[F0D0]` | BYTE | Saved previous `EF4D` page |
| `EFE9` | DWORD | Current `2AB6C` VGA page selector |
| `F001` | DWORD | Current physical VGA linear page base |
| `F005` | DWORD | Mutable original page stride used by `2AB6C` |

**`2E590(requested_page)`:**

1. `MOVZX EDX,BYTE[F0D0]` compares depth with exactly `10`.
   If it is equal, calls original `2F580(0x1358)` diagnostic.
   **There is no stack clamp at this branch.** If the
   diagnostic returns, the routine continues.
2. Re-reads `BYTE[F0D0]` *after* the diagnostic, then
   reads `BYTE[EF4D]` and writes it to
   `BYTE[2B644+unsigned F0D0]`.
   This stores `EF4D`, **not** `EFE9`. The two can differ
   because the earlier `2A078` routine selects pages through
   `2AB6C` without updating EF4D.
3. Increments AL modulo 256, writes `BYTE[F0D0]`,
   and invokes original `291B0(low8(requested_page))`.
4. `291B0` invokes `29FEC→2AB6C`, which zero-extends
   the page to a DWORD, reads **live** `F005`,
   computes `F001=((0xA000+page*F005)<<4)` with
   original 32-bit wrap, then `291B0` stores
   `BYTE[EF4D]=page`.

**`2E5D4()`:**

1. Compares `BYTE[F0D0]` with zero; if equal, calls
   original `2F580(0x136C)` diagnostic. If it
   returns, the instruction stream still continues.
2. Executes `DEC BYTE[F0D0]` modulo 256;
   original zero wraps to `0xFF`.
3. Zero-extends the now-current `F0D0`, reads
   `BYTE[2B644+F0D0]`, then invokes `291B0`
   to restore the saved presented page and update
   `EFE9/F001/EF4D`.

Both diagnostics may have their own original side effects;
their callbacks run at the original source locations, and the
stack depth is re-read afterward. Missing diagnostic handlers
or missing captured stack slots are **explicit unsupported
host states**, not assumed DOS-safe behavior. The original
executable does not necessarily guard out-of-bounds accesses
if a diagnostic unexpectedly returns.

## New reusable C++ components

- `include/rloft/combat_original_vga_page_stack.hpp`
- `src/combat_original_vga_page_stack.cpp`
- `tests/combat_original_vga_page_stack_tests.cpp`
- `include/rloft/combat_original_vga_page_stack_binding.hpp`
- `src/combat_original_vga_page_stack_binding.cpp`
- `tests/combat_original_vga_page_stack_binding_tests.cpp`

The pure original-state model has explicit captured
`F0D0/EF4D/EFE9/F001/F005/2B644` values and an
optional real `2F580` error callback. An observer can
inspect each completed original `291B0` selection.
The original `F005` stride may be observed via a
**late read** immediately before `2AB6C`, after any
diagnostic callback has returned.

`CombatOriginalVgaPageStackOverlayBinder::bind` installs
real `2E590`/`2E5D4` callbacks into the prior original
`1A288/1AB20` overlay service model, preserving all
other overlay collaborators. For coherent native graphics,
`bind_with_live_video` additionally shares the true
`EFE9/F001` projection and **live F005** with the
previous `CombatOriginalVgaCopyState` consumed by
`2A078`. The stack's EF4D remains separate; no
Level 22 page numbers or framebuffers are hardcoded.

The integrated CTest exercises:
`1A8C8 → 1A288 → 2E590 → 291B0 → backing capture
→ 2E5D4 → VGA-copy callback → 30CD4 → 1AB20
→ 2E590/2E5D4`, including restored original
`9E37/9E39` overlay globals, original saved `EF4D`,
and a deliberate initial `EF4D != EFE9` fixture.

## Tests and exact original-byte verifier

- All **23 new instruction anchors** at `2E590..2E607`
  directly matched the user-owned CODE.2; cumulative
  original executable audit now **826 named x86 byte
  checks at 818 distinct logical offsets**. This is
  not a whole-game code coverage percentage.
- The **exact GitHub source blob** for
  `combat_original_vga_page_stack.cpp` (SHA-1
  `7a046a7e97f62ea9acde87239324f59903ff3de6`)
  was locally compiled, with an interface-equivalent
  `CombatVgaPageSelectionModel`, and passed strict
  C++17 Debug, optimized Release and UBSan tests.
- The **exact GitHub source blob** for
  `combat_original_vga_page_stack_binding.cpp` (SHA-1
  `4434ca7b79c457b79dd77636659d9353d69a5578`)
  likewise passed local Debug, Release and UBSan
  with minimal interface-compatible overlay/video stubs.
  Those small harness tests validate real state
  mutation and live F005 synchronization but **do
  not replace the checked-in full overlay CTest**.
- Both new CTest targets are registered in
  `DemoV1/CMakeLists.txt` with assertions forced on
  in Release. They have **not** yet been executed
  in a full current-HEAD Windows build.

The last full Microsoft C++ build and 130/130 green
CTests were from the older, pinned
[Windows run #634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281),
source SHA `40e3f14b1072a7a0180ea1b0b4c1cea437137631`.
The page-stack changes are **not** included in that
old result. GitHub Actions remains manual-only.

## Next original framebuffer dependency: 292BC / 2932C

Initial original-byte analysis now confirms distinct
clip/restore paths, so generic Windows `memcpy` cannot
replace these services:

- `292BC` clips signed negative starting row/column to
  zero, decreases corresponding row/column counts,
  then clips to signed `WORD[EF3F]` and `WORD[EF41]`.
  It calls `2A114` only if BOTH resulting counts
  are **strictly positive**.
- `2932C` similarly clips, then delegates to `2A178`
  when row count is strictly positive and column count
  is **nonnegative**. The zero-column distinction
  matters for sequencer/graphics-controller state even
  when no byte is written.
- `2A114` captures four plane blocks after writing
  VGA graphics-controller read-map index `04h`.
  `2A178` restores the blocks via sequencer index
  `02h` plane-mask writes and ends with `0xFF02`.

Full native capture/restore pixel effects and source-driven
viewport clipping still require implementation and DOS
A/B frame comparisons.

**Formal acceptance:** 1 of 5 demo gates. Shared-engine
reconstruction has advanced; overall engineering completion
remains unmeasured.
