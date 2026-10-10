# F790 source event projections in opt-in F12 diagnostics — 2026-10-10

## Scope

This checkpoint connects ORIGINAL SOURCE EVENT PROJECTION to native Win32 inventory mouse interactions and source feedback expiry **only inside opt-in F12 developer tracing**. It does NOT invoke original F790/2932C graphics, change the indexed framebuffer, observe extra IRQ ticks, reseed gameplay RNG, or claim that a native click always has complete DOS-equivalent behavior. The last Windows binary is older; this source remains UNBUILT on Windows.

At startup of run_exploration, only when the existing DevSessionTrace is enabled, read the unchanged original T archive once into a local OriginalTTextArchive. This costs no extra T decode in ordinary non-developer gameplay. Never put raw text, item descriptions, spell names, original file bytes or source paths into telemetry.

## Original ordinary-slot capture

At native PartyPanels ordinary-slot click, BEFORE swap_ordinary_slot:

- Capture the original source hand (object ID and known bit) from InventoryRuntimeState::held.
- Capture the selected actor's original CHARS slot WORD at +0x40 + 2*slot and corresponding known bit at +0x12B with InventoryRuntimeState::ordinary_slot_before_117a0.
- Execute the EXISTING swap_ordinary_slot normally. No new gameplay mutation is introduced.
- Only if F12 trace is enabled, project that source status through OriginalF790OrdinarySlotEventPlan::after_117a0 with the original T archive and the native post-action inventory.
- For each ordered event write a symbolic DevSessionTrace.event with the original E8 address and item ID, then a separate flags event for known and pending. Errors in passive tracing are captured without interrupting input/gameplay. The pending 759C text stays in the owned snapshot and is **never logged**.

Event-name contract:

| Logged name | Numeric a | Numeric b |
| --- | --- | --- |
| f790.source.ordinary | source E8 logical offset | caller-owned item ID (can be -1) |
| f790.source.expiry | 0x131BB | global held item ID after expiry |
| f790.source.flags | known (0/1) | pending 6DAC (0/1) |

A successful scroll must produce TWO ordered f790.source.ordinary records, source E8=0x1173B and 0x1184A. Their numeric item IDs must be the original scroll then -1; both flags records show pending=1. Rejection produces only 0x1178E. Tome fragment consumption produces 0x11882, successful automatic container insertion produces 0x118FF, and ordinary slot swap produces 0x119FC using the old slot-local ID/known. Unsupported/no-change actions produce no projected source event.

## Expiry

At the existing 13184 native modal tick check, preserve the boolean result from expire_scribe_feedback_13184. Only if it is true, capture the cleared-state source argument snapshot via OriginalF790FeedbackSourceSnapshot::after_expiry, and emit source E8=0x131BB plus flags (pending=0). Equality and unsigned timer wrap do not expire. The native redraw already shows ordinary text on the next repaint, but the original F790 artwork/RNG event is NOT live-dispatched.

## Tests and original evidence

Original immutable CODE.2 SHA-256: 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314; logical object-1 base 0x3AF24. Seven independent machine-byte windows and ten exact E8 targets in 117A0 status paths passed locally. The Python original-byte source contracts now require the trace-only host boundary and reject accidental source_event_f790 or dispatch_source_call from the Win32 refresh code. The existing event-plan C++ regression and synthetic event-to-indexed-VGA regression cover ordered two-call scribe success and 256 RNG-free host redraws.

Status: **210 registered CTests** (Python available). Last actually completed Windows Full #651: 182/182 passed at a substantially older revision. This checkpoint, including the C++ Win32 instrumentation, has NOT been Windows-built or manually tested. Original DOS visual/timing/IRQ parity remains unaccepted. Builds are manual only with the user's explicit authorization.

## User-assisted field acceptance after a manual Windows build

Launch the opt-in F12 developer trace, enter inventory, perform a valid spell-scribing success, repeat with a rejected scroll, interact with a Tome fragment and a container, wait for a pending feedback timeout, then supply the source event trace. Inspect ordered E8 item IDs and known/pending flags against original source. Separately capture DOS/native 320x400 indexed framebuffer and exact original 30700 tick/cursor timing; no F12 trace evidence alone is sufficient to certify DOS parity. Do not enable random ribbon drawing on each host frame.