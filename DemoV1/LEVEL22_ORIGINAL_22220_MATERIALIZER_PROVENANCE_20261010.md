# Original CODE.2 22220 native materializer provenance — 2026-10-10

**Workstream:** `demo-v1` native game engine; original-version exact `CODE.2` SHA256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. The active `demo-v1` full build source baseline before this edit is certified Windows #658 214/214, with later isolated span/replay tests authored, not Windows-built. **This change is read-only diagnostics and record provenance, not a renderer-mode change.**

## Source chain and bounded native states

The immutable original CODE.2 materializer tails at logical `330E7,33278,3337F,33487` compare source-materialized record `+8` against `0x93`, write zero to global `22220` if equal, then compare signed depth at `+0` against the mutable nearest and optionally update it. Native traversal currently has two different families of result: the source-backed `ViewSweepRecordModel::materialize` and special `biased_record`, versus `empty_record()` returned from native bounds/empty-state fallback. The latter produces `0x93` resource fields **without proving the original four materializers executed**.

To avoid conflation, append a default-false `original_materializer_executed` boolean to the native `ViewSweepRecord`. Both original source-backed result constructors set true; the native-only empty placeholder explicitly stays false. The newly verified pure [22220 replay model](LEVEL22_ORIGINAL_22220_MATERIALIZER_SOURCE_REPLAY_20261010.md) refuses any not-materialized source record instead of treating it as a genuine 22220=0 write.

New read-only renderer-capture TSV fields count `ground_sweep_source_materialized` and `ground_sweep_native_synthetic`; they sum to the diagnostic ray count. Existing `view_sweep_record_tests`, `view_sweep_traversal_tests` and `exploration_frame_capture_tests` now assert their provenance and capture separation using synthetic fixtures. No existing material/resource/depth computation is modified; no branch calls `OriginalGroundNearest22220SourceModel::replay` in gameplay. The official source parser/FF50 dynamic clipping remains pending, and original DOS ram/framebuffer/tick parity remains pending.

The pre-existing full #658 native forest-floor result (64/64 successful full 34A74 128-row frames, 32768 indexed writes) is preserved; we make no DOS equivalence claim from source-only provenance. **Windows compilation of this commit and all 218 current CTests requires a separately authorized manual Full run.** Workflow remains manual-only.
