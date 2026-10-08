# Original CODE.2 292BC/2932C overlay backing clip checkpoint

**Date:** 2026-10-08 · **Branch:** `demo-v1`.  
**Original CODE.2:** SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`;
object-1 byte base `0x3AF24`.

## Exact original assembly behavior

The original overlay wrapper `1A288` calls
`292BC(pointer,row,column,row_count,column_count)`
for 32×16 backing capture. Its paired `1AB20` calls
`2932C(pointer,row,column,row_count,column_count)`
for backing restoration. The two functions share most of
their signed clipping logic, but **not** the final
zero-width gate.

1. Original row `EDX` and column `EBX` are **signed
   32-bit** values. If either start coordinate is negative,
   its size is adjusted by that negative value and its
   start coordinate is set to zero, with original
   **wrapping DWORD ADD** semantics.
2. The original derives `end = row+rows` and
   `end = column+columns` with x86 wrapping LEA.
   It compares them using **signed DWORD CMP/JGE**
   against the sign-extended original **WORD[EF3F]**
   maximum row and **WORD[EF41]** maximum column.
   Values beyond the limit are clipped by original
   `limit-start` DWORD subtraction.
3. The backing **pointer argument is unchanged** when
   the start row/column is clipped. Do not move a
   host buffer pointer forward as a modern pixel
   clipping API might.
4. `292BC` invokes `2A114` only when BOTH final
   row and column counts are **strictly positive**.
   In original `292BC:29313`, a `JLE` vetoes zero
   column count.
5. `2932C` invokes `2A178` when final row count
   is strictly positive but final **column count is
   nonnegative**. The original `2932C:29383` uses
   `JL`, not `JLE`. A zero-column restore is
   therefore **not a no-op**: the downstream `2A178`
   still gets invoked, potentially producing VGA
   sequencer writes or a physically rounded byte.
6. The wrappers do **not** themselves rasterize the
   four-plane backing memory. They pass the clipped
   original five DWORD arguments to the lower-level
   `2A114` read-map routine or `2A178` write-map
   routine respectively.

## Native implementation and integration

New shared source and tests:

- `include/rloft/combat_original_overlay_backing_clip.hpp`
- `src/combat_original_overlay_backing_clip.cpp`
- `tests/combat_original_overlay_backing_clip_tests.cpp`
- `include/rloft/combat_original_overlay_backing_clip_binding.hpp`
- `src/combat_original_overlay_backing_clip_binding.cpp`
- `tests/combat_original_overlay_backing_clip_binding_tests.cpp`

`CombatOriginalOverlayClipModel` provides pure
`inspect_292bc`/`inspect_2932c` projections and
bounded `capture_292bc`/`restore_2932c` callback
executors. They use actual caller-supplied signed
EF3F/EF41 bounds, exact original unsigned DWORD
arithmetic and signed branch conditions.

`CombatOriginalOverlayBackingClipBinder::bind`
attaches those clipping paths to the previously
implemented `CombatOriginalOverlayServices`, thereby
moving original `1A288/1AB20` closer to its authentic
full rendering chain. The injected **2A114 and 2A178**
services still need their own source-backed VGA
plane/buffer implementations. Missing required backing
services are explicit **unsupported host states**;
the original binary did not necessarily reject them.

Both CTest targets are CMake-registered with assertions
enabled even in optimized Release builds. Tests cover
negative starts, positive edge clipping, zero row,
the **zero-column capture/restore distinction**,
unaltered resource pointers, original 32-bit wrap,
live bounds mutation and first-activation plus
last-reference integration with the overlay model.

## Verification and current status

- All **30** sampled original instructions matched
  the immutable owned executable bytes; 29 were
  **new** verifier anchors. Original `2939A:RET`
  had already been audited in an earlier sprite
  checkpoint and was not duplicated.
- Cumulative source ledger: **855 named x86
  checks at 847 distinct instruction addresses**.
  This is not a percentage of total DOS code or
  whole-game behavioral coverage.
- The **exact committed** clipping source and binding
  source Git blobs passed independently compiled
  strict C++17 Debug, optimized Release and
  UndefinedBehaviorSanitizer tests, using compatible
  overlay interface stubs. This is **not** a
  current-HEAD complete Windows build/CTest run.
- The last complete Windows build is the earlier
  [run #634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281)
  at source commit
  `40e3f14b1072a7a0180ea1b0b4c1cea437137631`,
  with 130/130 CTests at **that** historical SHA,
  not these newer renderer additions.
- The private GitHub Actions workflow remains
  **manual-only**; no runner was used here.

## Next fidelity gates

Original `2A114` uses `2AB38` addressing then
graphics-controller **read-map index 04h** for
four successive VGA planes and writes backing bytes
into the provided DGROUP buffer. Original `2A178`
uses the sequencer **index 02h** plane masks and
copies the backing bytes into VGA planes, with a
final `0xFF02` port write. Their exact buffer
layout, byte-span rounding, zero-width side effects,
sequencer state and original forward-copy order
still require native reconstruction and source
verification.

After that, remaining `1A3B8/2AE30/29804`
position, hit-test and graphics routines must be
connected to authentic original assets. An
end-to-end DOS-versus-native VGA/IRQ/runtime
comparison is still needed for formal fidelity.

**Formal demo acceptance: 1/5 release gates.**
Engineering progress continues but is not
assigned an unsupported single percentage.
