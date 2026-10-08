# Original CODE.2 overlay position, hit testing and draw-record services

**Date:** 2026-10-08 · **Branch:** `demo-v1`  
**Source provenance:** User-owned `CODE.2` SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`;
object-1 original file base `0x3AF24`.
Instruction addresses below are original logical object-1 offsets.

**Scope:** Previously outstanding overlay services `1A3B8`,
`2AE30`, and `29804`, invoked inside the native original
`1A288` activation path.

## 1A3B8 — overlay pointer position and three WORD outputs

The original `1A3B8..1A50C` code reads `BYTE[D680]`,
two independent WORD mode flags `A6BE` and `A6BC`,
signed mouse-coordinate WORDs `A6B4` and `A6B6`,
and a third signed output WORD `A6B8`.

1. When `BYTE[D680]!=0`, set **both** mode WORDs to zero
   and call original `1F820` before returning the live
   three output WORDs. The return-status register EBX
   began as zero and is not set by this branch.
2. When `A6BE!=0`, clamp the signed `A6B4` row into
   inclusive `[0x124,0x187]` and the signed `A6B6`
   column into `[0x7E,0xC3]`. **Write the clamped
   globals before** calling `33C90(row,column)`,
   and set EBX to one.
3. Next, **re-read `A6BC` after the possible 33C90
   callback**. When nonzero, compute new target clamps:
   row `[4,0x103]`, column `[0x11,0x11F]`.
   Only if the targets differ from the live globals,
   call `33C90` with those targets, then write target
   WORDs to A6B4/A6B6 **after** the callback returns.
   EBX becomes one even if no movement was necessary.
4. Write three original **16-bit output pointer targets**
   from the **current** A6B4, A6B6 and A6B8, respectively,
   then return EBX. The original `1A288` passes
   `DGROUP 9E31`, `9E33`, and `9E35` as these
   three pointers. The native shared compositor now keeps
   an explicit 9E35 WORD rather than dropping it.

The model retains authentic original `33C90` and `1F820`
effects as synchronous callbacks. It never substitutes
modern Windows mouse coordinates or an assumed cursor.

## 29804 — 24-byte draw-record stack with original 29414 call

Original `29804..29837` is a compact wrapper, not a
sprite rasterizer itself. It reads `DWORD[EFD4]`,
calculates `DGROUP:EF74 + (EFD4 * 0x18)` with wrapping
32-bit multiplication, **increments EFD4 before**
calling `29414`, and **decrements live EFD4** on
return. The original four drawing arguments are forwarded
unchanged to 29414, followed by the computed record pointer.

This requires a stack-like original `EFD4` depth, with
nested calls receiving different 24-byte record addresses.
Critically the final decrement is of **live** EFD4, not
restoration of a host-saved original value, so nested
callback effects are observable. `29414` remains
an actual original drawing callback: it has its own
clipping and refresh logic and must be reconstructed
separately before declaring end-to-end pixel equivalence.

## 2AE30 — original opposite-page scene record hit test

Original `2AE30..2B085` tests D680 and immediately
returns 0 (without table reads) if it is nonzero.
Otherwise it toggles only the low bit of
`DWORD[C050]`, taking the **other scene page**.
The scene records start at original logical address
`0x27148 + (other_page * 0x2260)`. The entry count
is `WORD[0xF038 + (other_page * 2)]` and is reread
at the original loop condition. Each record has
**22 bytes** and is scanned in increasing index order.

For records with `WORD[record+8]==0`, the routine
uses original record ID at `+6`, reads a WORD type
from `0x14D8E + (id*22)`, and, for types
`<0x22` or `==0x50`, performs an early filter on
the raw WORD at `record+2`. The original instruction
`SAR ((word - 0x7D00),3)` followed by signed
`JL 0x39` skips that record when
`(word-0x7D00)<456`.

For records not skipped, the test compares the
input row and column, as **signed DWORDs**, against
the *inclusive* four unsigned WORD boundaries
at offsets `+0xE, +0x10, +0x12, +0x14`.

Once a flag-zero rectangle matches, it calls source
`2AD18(type)` first, then `2ADE0(type)` only if
the first result was false. Either true result
returns classification **2**. If both are false,
types `<0x22` or `==0x50` return **1**,
otherwise scanning continues.

For `WORD[record+8]==1`, matching rectangle records
read a second WORD type from
`0x22246+(id*22)`; types `0x52`, `0x71`,
`0x6E`, `0x91` return **2**, all other types
continue scanning. Other record flags do not produce
a classification. Exhausting the record list returns
**0**.

The bounded host model accesses only original supplied
WORD bytes through `original_word` and source predicate
callbacks `2AD18/2ADE0`; missing memory or a missing
needed predicate returns **unsupported**, not an invented
record type or arbitrary hit-test score. There is no
Level 22-specific location, coordinate or object type
hardcoded into the implementation.

## Native work and CTest registration

New shared engine:

- `include/rloft/combat_original_overlay_position.hpp`
- `src/combat_original_overlay_position.cpp`
- `tests/combat_original_overlay_position_tests.cpp`
- `include/rloft/combat_original_overlay_probe.hpp`
- `src/combat_original_overlay_probe.cpp`
- `tests/combat_original_overlay_probe_tests.cpp`
- `include/rloft/combat_original_overlay_draw.hpp`
- `src/combat_original_overlay_draw.cpp`
- `tests/combat_original_overlay_draw_tests.cpp`
- `tests/combat_original_overlay_chain_tests.cpp`

All **four** CTests are registered with assertions enabled even
under Release. The source adapters attach to the previous
`CombatOriginalOverlayModel::suspend_1a288` without replacing
existing real `2E590/2E5D4` page state, `292BC/2932C`
and `2A114/2A178` four-plane backing memory, or
`1A8C8/2A078/30CD4` composition operations.

The new integrated test drives first activation through
original `33C90` clamped positioning, original
`2AE30` alternate-page record classification,
`29804` EFD4 record dispatch to `29414`, then
`2E5D4` page restoration and the existing final
`1AB20` backing-restore callback.

## Verification and scope limits

**96 new original x86 signatures** (43 for 1A3B8,
13 for 29804, 40 for 2AE30) matched the immutable
original CODE.2 object's actual bytes. Original-byte
ledger: **1004 named checks, 996 distinct logical
instruction offsets**. These counts are source checks,
NOT a share of engine source lines or a percentage
of gameplay coverage.

Independent local strict C++17
`-Wall -Wextra -Werror -pedantic` Debug, optimized
Release with assertions active, and UndefinedBehaviorSanitizer
checks passed against **Git-blob-matching exact source files**:

- Position source Git SHA-1
  `ba8bcf6662beaef75c27ab729ab7daec9334ced0`.
- Drawing wrapper source Git SHA-1
  `7baad45c36e43b4138fed6774413a3c6a5553a9f`.
- Scene probe source Git SHA-1
  `89b51f755711ae63fc6ad0f0f88a1534fc9d463b`.

The local harness used a minimal interface-compatible
overlay state to test real source operations. The four
**checked-in CTests** have been registered but not yet
run in the full repository Windows configuration. The
last full Windows CI result, successful
[run #634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281)
with 130/130 tests at older source commit
`40e3f14b1072a7a0180ea1b0b4c1cea437137631`,
does **not** cover these new changes. The workflow
continues to be manual-only.

## Next dependency / acceptance boundary

The deepest remaining overlay-specific external effects
are actual `33C90` and `1F820` cursor services,
`2AD18/2ADE0` predicate bodies, and the
`29414` draw-record compositor with its resource and
refresh logic. A faithful runtime must supply original
DOS memory at `C050/F038/27148/14D8E/22246` and
actual source resources. Test DOS scene-record values,
VGA output planes, register mutations and IRQ traces
against the original executable before accepting
full-game overlay fidelity.

**Release acceptance remains 1 of 5 major demo gates.**
Engineering completion is measured separately and
has no supported numerical percentage.
