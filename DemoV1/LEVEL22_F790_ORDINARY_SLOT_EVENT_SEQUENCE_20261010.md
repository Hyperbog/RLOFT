# CODE.2 117A0 original F790 event fanout — 2026-10-10

## Binary-backed correction

Original immutable CODE.2 SHA-256: 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314. Object-1 physical file base 0x3AF24.

The previous source-only feedback snapshot incorrectly used the **post-transaction held cursor** for the success-path source call 1173B. Original 11630 sets pending byte 6DAC and deadline 6DA8=30700()+0x36, and then calls F790 at 1173B **before** the scroll is consumed. The caller at 117A0 then invokes 1A344(-1,0) at 1183E and calls F790 **again** at 1184A with literal arguments (-1,0), while pending feedback is still active. These are two separate pending F66C ribbon events that can each consume interior tile draws from the one shared RNG. The prior single-event interpretation was incomplete.

Rejection instead runs F790 once at 1178E and returns zero; 11834 takes the conditional branch to 11AD0 before the second call.

| Native ordinary-slot outcome | Source F790 call sequence | Argument ownership |
| --- | --- | --- |
| LearnedMageSpell | 1173B, then 1184A | Pre-consumption global held scroll/known, then explicit empty (-1,0) |
| MageScrollRejected | 1178E | Retained global scroll/known |
| ConsumedStoryFragment | 11882 | Explicit empty (-1,0) after 1A344 |
| InsertedInContainer | 118FF | Explicit empty (-1,0) after successful 11404 |
| Swapped | 119FC | Previous actor slot local ID/known; source EBP and stack word |
| NoChange, Unsupported | none | No F790 event |

An older pending 6DAC message still overrides any of those inputs during F790, including explicit-empty calls. The known bit in a vacated native container slot may be stale even when its item ID is -1; 118FF explicitly passes known=0 regardless. Conversely normal 119FC retains the source actor-slot known bit even with previous ID -1.

## Committed source implementation

The corrected OriginalF790FeedbackSourceSnapshot::after_scribe API REQUIRES pre-transaction HeldItemState and rejects mismatched post-state; this prevents recording consumed (-1,0) state as the first scribe F790's actual arguments. New OriginalF790OrdinarySlotEventPlan::after_117a0 returns up to TWO owned event snapshots, with source E8 addresses, pending messages and exact held/known tuple. It does not sample IRQ or RNG, draw, or assume any frame is an original F790 event.

A new read-only InventoryRuntimeState::ordinary_slot_before_117a0 takes a CHARS actor/slot snapshot from word +0x40+2*slot and bit in DWORD +0x12B prior to the native swap. It preserves stale known bits and rejects invalid actors/slots.

The separate C++ event sequence regression checks all status families, two-event success, rejection, stale known container insertion, invalid state and party slot snapshot. The existing synthetic indexed renderer + one-shared-RNG integration test was expanded to dispatch both pending success events and replay 256 host frames with no extra RNG or IRQ observations.

The independent verifier tools/verify_original_f790_ordinary_slot_event_plan.py covers SEVEN exact original instruction windows, TEN near-call source/target edges and the crucial reject/success branch. All seven windows and all ten rel32 targets passed directly against the supplied immutable CODE.2 in this session. It is registered in CTest and the deterministic F790 feedback runner.

## Acceptance and next gate

210 registered CTests with Python, up from 208. Current changed native C++ source remains NOT BUILT or run on Windows; last completed Full Windows #651 passed 182/182 at an older source revision. Original DOS rendering, IRQ timing and gameplay parity are NOT accepted; engineering readiness percentages remain unchanged. No Windows build was triggered.

Next: at the native ordinary-slot input boundary, capture the held item and old actor slot known/ID BEFORE mutation. Only after the real status is returned, create the ordered event plan and ultimately dispatch events through the one shared ExplorationRuntime RNG following original 112EC capture and 2932C restore. Never dispatch F790 in the 70 Hz host repaint path. Remaining container/equipment/hand source calls and DOS pixel tests remain separate gates.