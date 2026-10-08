# Original CODE.2 1A288/1AB20 overlay suspension and restoration

**Date:** 2026-10-08 · **Branch:** `demo-v1`  
**Status:** Original-source-checked control flow, callback execution model,
CMake test registration and a buffered `1A8C8` integration test.
These results **do not certify** the native overlay pixels or the
latest Windows executable.

## Exact original evidence

User-owned `CODE.2` SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Executable object-1 begins at physical file byte `0x3AF24`;
all offsets below are logical object offsets. This investigation
re-used the original bytes and separately generated disassembly,
not guessed contemporary cursor/overlay semantics.

### Suspend/activate entry at 1A288

- `1A288` tests `WORD[85FC]`. When disabled, jumps to
  shared `1AC0B` RET without modifying `WORD[9E37]`.
- `1A296` compares the **full 16-bit WORD** at `9E37`
  against zero. If **nonzero**, jumps straight to `1AC04`:
  `INC WORD[9E37]` and return. This is not a boolean
  counter, nor a signed-positive clamp; overflow wraps
  from `0x7FFF` to `0x8000`.
- At **zero** reference depth, first call
  `2E590(BYTE[9E39])` to save/select the original VGA page.
  Then call `1A3B8(&9E31,&9E33,&9E35)`, which may
  mutate the live signed position WORDs.
- Call `292BC(9E3A+(BYTE[9E39]<<10),
  sign_extend(WORD[9E31]),sign_extend(WORD[9E33]),0x20,0x10)`
  to capture the original **32 rows × 16 columns** backing.
  All those global values are observed **after** earlier callbacks.
- Call `2AE30(sign_extend(row9E31),sign_extend(column9E33))`.
  Test `WORD[A6C0]` **after** `2AE30` returns.
  If zero, skip the optional `29804` drawing branches.
- If `A6C0` is nonzero, `2AE30` result equal to
  `2` draws `29804(1,row,column,0x880D)`; result
  equal to `1` draws `29804(2,row,column,0x880D)`.
  Otherwise, signed `WORD[85FE] != -1` draws
  `29804(0,row,column,0x880D)` followed by
  `29804(0,current_row,current_column,0x8603)`.
  When `85FE == -1`, the routine uses
  `WORD[A6BC]` if signed `>=3`, or mode `0`,
  with resource pointer `0x880D`.
- All first-activation branches join at `1ABFF`:
  `2E5D4` pops/restores the VGA page, then shared
  `1AC04` **increments the live** `9E37` WORD.

### Restore/deactivate entry at 1AB20

- `1AB20` first tests `WORD[85FC]`. If disabled,
  returns immediately without changing the reference count.
- If enabled, `1AB2A` executes
  `DEC WORD[9E37]` **unconditionally**; `1AB31`
  returns unless the post-decrement WORD is zero.
  Thus an original `0` underflows to `0xFFFF`
  and skips pixel restoration.
- Only on zero, call `2E590(BYTE[9E39])`,
  then `2932C(9E3A+(BYTE[9E39]<<10),
  caller_row,caller_column,0x20,0x10)`.
  Unlike `1A288`, these coordinates come from the
  original **arguments to 1AB20**, not a fresh read of
  DGROUP `9E31/9E33`.
- Original `1AB67` makes a **tail JMP** to `2E5D4`
  to restore the selected VGA page. There is **no final
  reference-count increment** in 1AB20 itself.

### Interaction with 1A8C8

Original `1A8C8` decrements `9E37` and temporarily
changes `9E39` before calling `1A288`.
It later calls `1AB20` with its **saved** row/column,
then increments `9E37` and restores `9E39`.
The refcount cycle must be tested in this complete
order; it is not equivalent to toggling one boolean.

## Native implementation and verification

Added shared-engine source:

- `DemoV1/include/rloft/combat_original_overlay.hpp`
- `DemoV1/src/combat_original_overlay.cpp`
- `DemoV1/tests/combat_original_overlay_tests.cpp`

`CombatOriginalOverlayGlobals` refers directly to
the **same mutable `CombatOriginalCompositeState`**
used by `1A8C8`, with the additional original
`85FE/A6C0/A6BC` branch-selector fields.
`CombatOriginalOverlayModel` executes all verified
entry gates, signed WORD wrap, graphics-callback and
resource address sequencing. Required unimplemented
original services `2E590`, `1A3B8`, `292BC`,
`2AE30`, `29804`, `2E5D4`, and `2932C`
are explicit injected callbacks, never invented
Windows cursor graphics.

`bind_1a8c8` supplies the original suspension and
restoration callbacks **without replacing** existing
`2A078` planar VGA pixel-copy and `30CD4` DGROUP
backing-buffer services. The regression exercises
the complete buffered `1A8C8 -> 1A288 -> 2A078
-> 30CD4 -> 1AB20` callback order and original
reference-count/page restoration. Further component
regressions cover zero/nonzero initial depth,
16-bit wraparound, activation without drawable mode,
all `2AE30` classification values,
`85FE/A6BC` special drawing choices and fail-closed
absence of required captured services.

**49 new instruction byte anchors** were independently
checked against the immutable user-supplied CODE.2.
They cover the actual instructions in `1A288/1AB20`
and their shared tail at `1AB6C..1AC0B`.
Cumulative original-byte verifier: **803 named checks
at 795 distinct logical code locations** (not a
runtime coverage ratio).

The source-equivalent overlay model and its stand-alone
branch tests passed strict `-std=c++17 -Wall -Wextra
-Werror -pedantic` Debug, optimized Release
(with assertions enabled), and UBSan.
The newly committed **full buffered-compositor CTest
integration case was added afterward**; a new complete
Windows CMake/CTest build has **not** been run for it.
Do not misattribute the historical Windows
[#634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281)
130/130 success at old source SHA
`40e3f14b1072a7a0180ea1b0b4c1cea437137631`
to this current branch HEAD.

## Next exact dependencies

1. Reconstruct `2E590/2E5D4` DOS page-stack
   operations including `F0D0`, `2B644`,
   and `291B0` active-page change.
2. Connect `292BC/2932C` raw overlay backing
   capture and restoration to authentic VGA planes
   and DGROUP `9E3A` buffer bytes; source-audit
   their gfx-controller state and clipping first.
3. Reconstruct `1A3B8`, `2AE30`, `29804`
   and any remaining asset/mouse-state callbacks
   that affect position and overlay graphics.
4. Run consolidated **current-HEAD Windows**
   compilation/CTests and reference-DOS
   framebuffer, IRQ, and game-state A/B tests.

**Formal demo acceptance: 1/5 gates.** Engineering
completion remains unmeasured; this is substantial
reusable engine progress, not a new accepted release gate.
GitHub Actions stays manual-only.


## Following shared original 2E590/2E5D4 page-stack checkpoint

[2E590 / 2E5D4 source checkpoint](LEVEL22_CODE2_VGA_PAGE_STACK_CHECKPOINT.md)
supersedes the previous statement that these VGA
page operations were only injected stubs.
They now have reusable source-backed native
F0D0/EF4D/2B644 stack logic with precise 291B0
selection, warning diagnostics and explicit
shared EFE9/F001/F005 synchronization to 2A078.
The cumulative opcode verifier is now at
**826 named / 818 distinct original locations**.
The remaining nontrivial dependencies are the
actual 292BC/2932C capture/restore, their
2A114/2A178 VGA backing raster, overlay
position/graphics services, and DOS validation.
