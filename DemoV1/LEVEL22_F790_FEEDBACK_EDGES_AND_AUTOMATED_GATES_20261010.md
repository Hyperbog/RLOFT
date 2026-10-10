# CODE.2 F790 scribe success, failure and expiry: automated parity contract
**Date:** 2026-10-10. **Scope:** three proved original feedback events only. Original DOS acceptance and full native event wiring remain pending.

## Source machine-code evidence

Original shipped CODE.2 SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object-1 logical base at physical `0x3AF24`. The independent local script `verify_original_f790_feedback_edges.py --binary-only --original-code2 /mnt/data/CODE.2` matched **five original instruction windows** at `116F5`, `11717`, `1176A`, `13184`, `131AA` and three E8-rel32 targets into `F790`: `1173B`, `1178E`, `131BB`.

- `11717` writes byte `6DAC=1`, samples `30700`, adds `0x36` to deadline `6DA8`, then calls `F790` at `1173B`; this is the successful spell-scribing path.
- `1176A` performs the **same pending/deadline updates** and calls `F790` at `1178E`; this covers the rejected capacity/already-known branches.
- `13184` compares pending byte and then checks `30700 > 6DA8` **unsigned and strictly** (not equality, not wrap-corrected). If expiry occurs, `131AA/131B4` clears both deadline and pending byte BEFORE the `131BB` call back into `F790`.
- Existing original `F790` at `F7B4` prioritizes pending `759C` feedback over the ordinary held object; the last expiry call returns to normal item text.

The existing native `InventoryRuntimeState::swap_ordinary_slot` returns `LearnedMageSpell` and `MageScrollRejected`, assigns the same `MageScribeFeedbackKind` values and optionally captures the IRQ deadline, while `expire_scribe_feedback_13184` returns a boolean only when it clears the message. The Win32 modal currently discards that boolean and **does not yet dispatch F790 artwork on expiry**. This is preserved while the original saved-page capture, RNG reseeds and DOS visual matching remain unproved in the host.

## Automated feedback-loop components

`OriginalF790FeedbackEdges` provides two **read-only event translators**: `after_ordinary_slot_117a0` and `after_expire_13184`. The scribe translator requires valid pending `6DAC`, source `30700+0x36` deadline and consistent success/rejection kind. It maps success to `1173B`, both rejection reasons to `1178E`; all unrelated ordinary-slot outcomes yield no event. The expiry translator emits `131BB` only when actual native `expire_scribe_feedback_13184` returned true and the pending state has been cleared. Every result is checked against the existing 17-site `OriginalF790CallerCatalog`.

`OriginalF790FeedbackSourceSnapshot` uses these verified edges to copy the **source-call** held-ID/known args plus original `759C` message into owned storage. A successful scribe gets the original T spell-name record plus the source-proven suffix; rejection uses its original message without adding a spell name. After expiry the snapshot records no pending message and the current global held source arguments. A short-lived `arguments_now()` view can be used at synchronous future dispatch, without retaining a dangling `string_view` into mutable `MageScribeFeedback`.

New C++17 regression CTests cover unrelated actions, success, both rejection kinds, pending/empty held cursor, deadline equality and wrap, one-shot expiry, invalid/mismatched state, owned feedback text after live state mutation, synthetic XOR-DC T spell resource, and 1,000 no-op frame iterations. **These C++ tests are registered but not yet executed on Windows.** The Python CTest needs no copyrighted original game files by default, checking native source and CMake contracts; when supplied original CODE.2 it executes the full byte-accurate oracle. Its local binary-only original-file mode **passed**.

**CTest total: 205 registered when Python is available**, up from 202. Last completed Full Windows build remains #651 (182/182 on older revision). No Windows build, runtime DOS comparison, gameplay parity increase, or independent Level23 work was performed.

## Remaining work

Source-defined inventory event routing for the other **14** direct F790 sites remains incomplete. Even the three feedback event translators are isolated and **not wired to Win32**. Subsequent integration must bind the one existing `ExplorationRuntime::original_random_` only on authentic source events; sequence original 112EC capture, 2932C background restore and source `RES2:3` F66C selected tiles; and compare live Windows vs DOS event timing and pixels. Do not infer a source F790 call from a mere 70-Hz framebuffer refresh, and do not trigger manual Windows builds without explicit authorization.
