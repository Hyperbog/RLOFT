# CODE.2 117A0 native F790 batch-dispatch boundary — October 10, 2026

**Branch:** `demo-v1`. **Status:** isolated native source integration, not yet connected to live Windows input. No automatic build or DOS parity claim.

## Original binary verification

Immutable supplied `CODE.2`: 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, executable object-1 physical base `0x3AF24`.

These six direct E8 calls were independently checked byte-for-byte against the uploaded original file:

| Original E8 | Instruction bytes | Target | Meaning |
| --- | --- | --- | --- |
| `1173B` | `E8 50 E0 FF FF` | `F790` | Scribe success, pre-consumption hand |
| `1178E` | `E8 FD DF FF FF` | `F790` | Rejected scribe |
| `1184A` | `E8 41 DF FF FF` | `F790` | Second successful-scribe call, explicit empty |
| `11882` | `E8 09 DF FF FF` | `F790` | Consumed Tome fragment |
| `118FF` | `E8 8C DE FF FF` | `F790` | Inserted item into container |
| `119FC` | `E8 8F DD FF FF` | `F790` | Ordinary exchange, saved local slot |

Argument ownership and double-call semantics are established in `LEVEL22_F790_ORDINARY_SLOT_EVENT_SEQUENCE_20261010.md`. Only actual original events, never 70-Hz frame redraws, may consume the shared source RNG.

## Source change

Added `include/rloft/original_f790_ordinary_slot_dispatch.hpp`. This batch boundary composes the existing original `117A0` source-event plan, F790 text resolver, saved-page renderer, original `RES2:3` ribbon and font, and **one shared** `OriginalRandom`. It does not add a new modal PRNG or alter normal game input.

It preflights the entire event sequence, exact E8 source sites and original argument origins, scribe pair order/feedback text, explicit-empty arguments, available RES2 frames and font widths **before the first framebuffer restoration or RNG/IRQ side effect**. A successful scribe dispatches both `1173B` and `1184A` in order. The generic native redraw path remains separately cache-only.

The existing `original_f790_runtime_description_tests.cpp` regression now exercises the composed batch and explicitly rejects malformed second calls and fabricated explicit-empty arguments with no partial event/RNG/tick mutation. The existing 256-frame replay test remains.

## Verification status

- Original binary E8 targets: **6/6 locally verified** from the immutable supplied `CODE.2`.
- Existing registered CTests: **210** when Python available; this change adds no new target.
- Last authorized Windows Full: **#651, 182/182 passed**, older source.
- **New code not yet compiled or tested in Windows CI**.
- Original DOS indexed graphics, IRQ timings, source-event parity and whole-Level22 gameplay remain unaccepted.
- No completion percentage adjustment. No GitHub Actions build dispatched.

**Next:** review and bind original `112EC` modal-entry capture plus the real Win32 original-source event lifecycle, then obtain a manually authorized Windows build and paired DOS/native event/pixel evidence. Do not dispatch F790 from repaint, invent triggers, or proceed to independent Level23.
