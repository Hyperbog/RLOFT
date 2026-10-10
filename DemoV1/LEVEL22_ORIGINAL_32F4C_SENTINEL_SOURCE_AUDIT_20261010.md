# Original CODE.2 32F4C source sentinel initialization — 2026-10-10

**Scope:** Original Strahd `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, 558,689 bytes; executable object-1 file base `0x3AF24`. Source-authenticated correction to the provenance terminology; **no change to gameplay pixels, ray material IDs, FF50 clipping, IRQ or RNG**.

## Exact source evidence

Original `32F4C..32F8A` is a **63-byte seven-field source sentinel writer**, not an invented native placeholder:

1. `PUSH EAX; IMUL EAX,[C6FC],0x1C; ADD EAX,15A18` computes the original 28-byte ray record address.
2. `record+0 = 0` depth, `record+4 = 0x93` outer and `record+8 = 0x93` inner.
3. `record+0x0C,+0x10,+0x14,+0x18 = 0` flags, texture, cell and shading fields.
4. `POP EAX; RET`; **no write to nearest-wall global `22220`**.

Two exact independent near-call targets are: `32C2F → 32F4C` (left-side ray outside the indexed level), and `32D19 → 32F4C` (right-side out of bounds). In contrast, original `330E7,33278,3337F,33487` materializer tails may reset or minimize `22220`. The original source thus intentionally distinguishes a sentinel-filled slot from a materialized 0x93 slot **even though the two share the same resource identifiers**.

The standalone byte oracle is `tools/verify_original_ground_sentinel_fill.py --original-code2 /path/to/CODE.2`. It checks original file identity, all 63 routine bytes and both original E8 rel32 call targets, as well as native provenance separation. It is registered as `original_ground_sentinel_fill_binary_contract_tests` in CMake.

## Faithful native representation

Native `ViewSweepRecord::original_materializer_executed` remains true only for the source four materializer variants. New separate `original_sentinel_initializer_executed` marks `empty_record()` from the exact **original 32F4C**, without implying a `22220` side effect. Default fixture records have neither bit; a source filler must not be confused with an unknown native fixture. The `ExplorationFrameCapture` TSV reports three exclusive categories: `ground_sweep_source_materialized`, `ground_sweep_source_sentinel_initializer` and `ground_sweep_native_synthetic` (unclassified test/default records). The existing `original_ground_nearest_22220.hpp` replay continues to refuse source filler/unknown records, rather than synthesizing non-original nearest-depth updates.

Regression tests now distinguish original materializer vs source sentinel in view traversal, as well as source sentinel vs unclassified fixture in an indexed TSV capture; independent actual `CODE.2` byte check covers the source routine and both calling branches. Source-only metadata and tests do not alter Level 22 gameplay raster. All 64/64 open-field #658 frames and 32768 indexed writes remain the previous **verified native** checkpoint.

## Remaining fidelity gates

This source correction does not itself reconstruct partial-depth 22220/FF50 wall occlusion or the order of original sentinel and materializer calls for all world states. It also does not provide an independently captured original DOS framebuffer/IRQ/RNG oracle. Continue original 1F1C8 mode integration after complete source-provenance/near-wall trace handling; do not infer 22220 from synthetic or sentinel writer records.

**No Windows Full CI was triggered** by this source-only commit. The source revision includes one additional Python-only registered CTest beyond the current branch's prior registry; Windows compilation/CTest remain a separate explicitly authorized gate, and GitHub Actions stays manual-only.

## 2026-10-10 follow-up: bounded original 32F4C replay supported

The isolated `OriginalGroundNearest22220SourceModel::replay` now accepts exactly two provenance classes in ray order: the four ordinary source materializers (which mutate `22220`), and source-owned `32F4C` sentinel-filled records (which do **not** modify `22220`). Unknown native fixture rays and contradictory provenance still return `std::nullopt`. A verified `32F4C` slot must have zero depth and ordinary resource `0x93`; its source-sentinel counter is separate from signed-depth minima and materializer-zero-reset counters.

A deterministic regression places an original `32F4C` sentinel after a materialized 256-depth wall; the result stays **256** rather than incorrectly becoming zero. It then proves the opposite behavior for an actual materialized `0x93` ray (nearest resets to zero), tests invalid provenance, and preserves all existing signed-nearest checks. The original-byte 32F4C CTest oracle and four-materializer oracle remain independent. This is still an isolated mathematical/source-order model, **not yet a live renderer or original DOS pixel acceptance**. All new tests remain Windows-build pending.
