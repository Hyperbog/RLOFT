# Level 22 DDA4 timed-message glyph/sprite renderer — original-source checkpoint

**Date:** 2026-10-08  
**Branch:** `demo-v1`  
**Scope:** Original CODE.2 `1C258 -> 3060C -> 303C8 -> 2939C/293EC
-> 2A380/2A584 -> 2E758` synchronous call dependencies.

This checkpoint does **not** claim pixel-perfect DOS text or sprite output,
full framebuffer integration, or a rebuilt Windows EXE after these new commits.

## Immutable source

Original user-owned CODE.2 SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

Executable object-1 starts at original file byte `0x3AF24`.
All original code offsets below are logical positions within that object.

- `3060C:30614` writes its **first** argument (the row)
  to original DWORD `2B684`; `30621` writes its **second**
  argument (the column) to DWORD `2B680`, then calls `303C8`.
  For the level-up queue this begins at row `0x107` (263)
  and column `0x22` (34), not the other way around.
- `303CD..303DB` always requests original `2F29C(style,0)`
  and stores its returned font-record pointer at `2B67C`,
  even when the message starts with NUL.
- `303E8` tests the **current** source byte against zero before
  each glyph. There is no host string conversion or ahead-of-time
  NUL traversal. Renderer callbacks may change later observed bytes.
- `303F1..3041F` calls **shaded** `293EC` when
  `DWORD[F56C] != 0`. Its five arguments are the
  sign-extended original glyph byte, current row, current column,
  DWORD at the active font record, and observed shade DWORD.
  `30421..3043F` calls normal `2939C` with the same
  first four parameters when `F56C=0`.
- Original DOS bytes `0x80..0xFF` are treated as **signed
  negative glyph indexes** using `MOVSX BYTE`. Example:
  `0xE9 -> -23`. This is not the same as indexing an
  ordinary modern 0..255 Unicode/CP437 table.
- `30440..30455` re-reads the **live** `2B67C` font
  record after the glyph renderer returns. It loads the
  **unsigned** width byte at `font_record+4+signed_glyph`,
  increments the source pointer, then performs an exact
  wrapping DWORD `ADD` to `2B680` (the column).
  Rendering and width lookup must not reuse an outdated
  font pointer or invent a safe high-character width.

## Normal and shaded sprite dispatch

Original `2939C` and `293EC` both check their
resource pointer argument at `[ESP+0x10]`. If zero, they
jump to shared `2939A` RET **without rasterization or refresh**.

When nonzero:

1. Normal `2939C:293B3` forwards four original arguments
   to `2A380`, then uses a **tail JMP** to `2E758`.
2. Shaded `293EC:29407` forwards five original arguments
   to `2A584`, then uses a **tail JMP** to `2E758`.
   Original `2A584:2A590/2A595` writes the shade into
   two original self-modifying operand locations.
3. `2E758` reads original function pointer DWORD `F0C8`
   **after the raster routine has completed**. If nonzero it
   calls that *currently observed pointer*; if zero it returns.
   A raster routine may have changed F0C8 in the interim.
4. The sprite routines themselves execute original VGA plane
   sequencer I/O and proprietary sprite-format raster behavior.
   Their complete pixel effects **are not yet reimplemented
   by these callback drivers**.

## New native source and explicit boundaries

New source interfaces and tests:

- `include/rloft/combat_original_text_runtime.hpp`
- `src/combat_original_text_runtime.cpp`
- `tests/combat_original_text_runtime_tests.cpp`
- `include/rloft/combat_original_sprite_runtime.hpp`
- `src/combat_original_sprite_runtime.cpp`
- `tests/combat_original_sprite_runtime_tests.cpp`

Both sources and both tests are registered in DemoV1 CMake and
in the global assertion-enabled CTest target list.

`CombatOriginalTextRuntimeModel::draw_3060c_303c8` executes the
verified original **font selection, glyph dispatch and advancing
column** sequence against caller-owned mutable source globals.
`bind_display_3060c` can be assigned to the existing
`CombatIrqDisplayServices::draw_text_3060c` callback, connecting
the previously implemented timer-driven `1C258` service to the
original per-glyph traversal. The callback's state reference must
outlive its use.

`CombatOriginalSpriteRuntimeModel` executes the bounded
normal/shaded `2939C`/`293EC` dispatch calls and invokes the
live `F0C8` refresh callback after rasterization only when
the pointer is nonzero. The underlying `2A380` and
`2A584` sprite rasterizers remain injected, not fabricated.

An incomplete captured NUL-terminated string, missing
required font record/glyph width memory readers, or missing
actually executed original rendering services is an explicit
host-side unsupported state. The original DOS executable did
not necessarily reject these states safely.

## Regression and audit

- **31** additional `2F29C/303C8/293EC` code signatures,
  and **17** `2939C/293EC/2A380/2A584/2E758` signatures
  matched original uploaded CODE.2: 48 new distinct byte
  locations. Cumulative original combat byte ledger:
  **557 named x86 checks at 549 distinct code locations**;
  one direct Strength data anchor, nine SHA256-checked
  DGROUP spans and one SHA256-checked CS span.
- Source-equivalent strict C++17 local
  `-Wall -Wextra -Werror -pedantic` tests **passed**
  in Debug `-O0`, optimized Release `-O2 -DNDEBUG` with
  assertions active, and `-fsanitize=undefined`.
  Cases cover signed glyph `0xE9 -> -23`, mutable
  per-glyph F56C/2B67C observations, column wrapping,
  early NUL, callback mutation of next source byte,
  two sprite variants, zero pointer early return,
  post-raster F0C8 changes, and an end-to-end
  `1C258 -> 3060C -> 303C8` callback sequence.
- The locally compiled source is source-equivalent to the
  committed implementations but was not a fresh
  full checked-out repository build of these new CTests.
  The last actual Windows green CTest run was
  [#634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281)
  at earlier source SHA `40e3f14b1072a7a0180ea1b0b4c1cea437137631`,
  passing **130/130 tests**. **Do not** ascribe that older
  green build to these later source additions.

## Next required rendering fidelity gate

The existing `VgaPlanarMemory` provides four-plane byte writes
and page import/export for 320x400 indexed frames, while the
native `IndexedFontPresenter` supplies convenient higher-level
glyph drawing. Neither automatically matches the original
`303C8` negative signed font indexing, palette/shade self-patch
routines, `2A380/2A584` VGA mask/stride/clipping logic, or
`2E758` callback timing.

Next reconstruct sprite 2A380/2A584 plane operations and
their original resource headers, validate against DOS VGA
A0000/A8000 bytes, then implement the 1A8C8 composite path.
Only connect the complete native HUD/message rendering when
reference-DOS pixels, IRQ ticks, and mutation order match.

**Overall faithful-demo acceptance remains 20% (1/5).**
The GitHub Actions workflow remains manual-only.


## Next source-backed pixel-address dependency: 2AB38

Subsequent to the glyph/sprite callback integration above, the
original `2AB38..2AB6B` address calculation used by
`2A380` and `2A584` has been modeled independently:

```text
first = (uint32_t(row) << (DWORD[EFED] & 31))
second = (first << (DWORD[EFF1] & 31))
physical_linear_EBX = DWORD[F001] + first + second
                    + (uint32_t(column) >> 2)
plane_ECX = uint32_t(column) & 3
```

This is the original x86 register arithmetic, **not** a
hardcoded 320x400 Windows framebuffer formula. Both row
shifts and F001 are mutable original runtime globals.
The supplied CODE.2 DGROUP image initially serializes
the involved globals as zero; their **runtime** values
must be observed from original initialization or live
game state, not inferred from those zero-filled bytes.

An observed `EFED=4`, `EFF1=2` combination produces
`16+64=80` VGA bytes per row, while `F005=0x800`
and page1 produce F001 `0xA8000`. These are useful
specific test inputs, **not universal assumed constants**.

New source:
`include/rloft/combat_original_vga_address.hpp`,
`src/combat_original_vga_address.cpp` and
`tests/combat_original_vga_address_tests.cpp`.
The test exercises all 320 columns, both visible VGA
pages, last displayed pixel, unusual runtime row shifts,
x86 low-five-bit CL shifts, unsigned column SHR2,
negative-looking input coordinates, and original DWORD
overflow. Focused standalone strict C++17 Debug,
Release and UBSan tests **passed**. CMake registers the
new model and CTest with assertions enabled, but it
has not yet run in another complete Windows CI build.

**20 more exact original 2AB38 instruction signatures**
were independently verified, bringing the cumulative
audit to **577 named x86 checks at 569 distinct logical
instruction addresses**. The direct Strength table,
nine hashed DGROUP data spans and one hashed CS span
remain unchanged.

This completes the **pixel-address/plane selection math
only**. It does not emulate the source 2A380/2A584
sequencer OUT instructions, run-length/transparent pixels,
sprite clipping, shade self-patches, hardware write-mask
composition, or 1A8C8 screen compositing. Those remain
the next source-fidelity gate and must be checked against
reference DOS VGA contents.
