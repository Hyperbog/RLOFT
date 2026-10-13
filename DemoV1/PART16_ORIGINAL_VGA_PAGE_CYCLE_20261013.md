# Part 16 — Original C050/EF4D page transition source model

**Owner branch:** `demo-v1`. **Scope:** source-exact, independently
testable original CODE.2 page lifecycle, deliberately *not* yet bound
to live normal-game rendering.

## Three exact x86 routines verified

Original owner CODE.2 (558689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`)
object-1 instruction bytes were disassembled with i386 objdump,
physical mapping `0x3AF24 + logical`:

1. `1F1D6`: `MOV EAX,[C050]; SHL EAX,0Fh;
   ADD EAX,0A0000h; MOV [FDBC],EAX`. The segment continues
   into 1F1C8 perspective, actor and renderer callbacks.
   This extraction only models the four arithmetic/load/store instructions.
2. `1EA78`: `PUSH DWORD[C050]; CALL 1A510;
   XOR BYTE[C050],1; ADD ESP,4; RET`.
   The complete 22 bytes were matched to the uploaded DOS binary.
   The display callback receives the **full pre-call DWORD**, then
   the low byte flips AFTER the callback. Any callback-side
   C050 mutation is therefore visible to that XOR.
3. `291B0`: `MOVZX EAX,BYTE[ESP+4]; PUSH EAX;
   CALL 29FEC; ADD ESP,4; MOV AL,BYTE[ESP+4];
   MOV [EF4D],AL; RET`. The complete 24 bytes match the
   original. Selecting EF4D is separate from changing C050.
   Its `29FEC→2AB6C` callback projects EFE9/F001.

The original initial image has DWORD C050=1, BYTE EF4D=0.
Initial FDBC computed with the original instruction formula is
`0xA8000`. This is not a live DOS VGA page capture.

## Implementation

`OriginalVgaPageCycleModel` owns three page-state fields
`draw_page_c050`, `selected_page_ef4d`, `ground_base_fdbc`.
It implements three explicit source operations with callbacks for
`1A510` and `29FEC`. Missing callbacks reject BEFORE mutation.
Calls are made before source stores; no extra FDBC update, EF4D
alias, fullscreen blit, page mask, or IRQ timing is invented.

A portable C++17 unit test (with Release assertions enabled)
verifies argument width, exact callback/store order, page 0/1
arithmetic, 32-bit wrapped shifts, callback C050 changes,
low-byte-only XOR, no implicit FDBC update, independent
EF4D selection, callback-missing failures and 100 toggles.
CI also checks a read-only source contract. Optional
`--original /private/CODE.2` verifies full original identity,
all three instruction windows and both initial DGROUP fields.

## Verified original ground draw-entry prefix (follow-up)

Original object-1 `1F1C9..1F1D5` contains
`MOVZX EAX,BYTE[C050]; PUSH EAX; CALL 291B0`.
It then executes `1F1D6` and re-reads **DWORD[C050]** before
storing the calculated `FDBC`. The original `1F1C9` 13-byte
instruction window was authenticated separately.

`enter_ground_draw_1f1c8_prefix` now composes those two
operations in original order, including callback mutation
visibility. A non-callback initial page 1 chooses `EF4D=1`,
`FDBC=0xA8000`. A unit test also simulates the 29FEC
service changing C050 from 1 to 0; 291B0 still stores
EF4D=1 but the subsequent FDBC read sees 0 and computes
`0xA0000`. This test demonstrates ordering, **not evidence
that original 29FEC itself changes C050**.

This remains only the entry prefix; rest of 1F1C8 includes
1EDAC/334C4 and the actual raster and overlays, and still
requires DOS call-site and frame evidence before live binding.

## Remaining Alpha blockers

This model has **not** been wired to Win32 presentation. To
use it for normal frames, establish the exact original
event/scene call sequence into `1EA78`, first real mutable C050,
`1A510` output and `291B0→29FEC` page service behavior,
ES:EDI/VGA selector mapping, page masks and independent DOS
frame comparison. Unconditional flipping on native render frames
would be speculative and remains prohibited.

The separate Alpha field-test artifact is the supported
protected-copy path for collecting normal-game evidence;
none of the formal 10 Alpha release gates is accepted solely
from this source model.
