# CODE.2 34A74 → 342E4/34954 → 34350 exact planar raster audit — 2026-10-10

**Original authority:** uploaded `CODE.2` 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, object-1 physical base `0x3AF24`. Independently tested exact source bytes. This is a **bounded original sampler/callback audit**, not original DOS pixel equivalence or a proof of runtime ES alias safety.

## Original planar source loop and non-obvious control flow

- Original `34350..3494F` is exactly **64 identical 24-byte instruction sequences** (1,536 bytes). Each sequence resets ESI, adds EBX/EBP to V/U accumulators, computes a 64×64 source index with two `SHLD ESI,...,6` operations, adds source base **`FF54`**, reads one indexed texel with `LODS BYTE DS:[ESI]`, subtracts shade `AH`, and writes **`STOS BYTE ES:[EDI]`**. Then `34950` returns. The exact 1,536-byte sequence and function bound are checked by an original binary oracle.
- First-phase helper `342E4..3434F` selects VGA sequencer port **`03C4`**, register 2 plane masks **`08,04,02,01`** (written AX values `0802,0402,0202,0102`). It reloads output pointer **`FF40`** for each plane. Calls at `34302/34319/34330` target `34350` for the first three planes. The **fourth plane is NOT a fourth E8**: at `34347` it pushes its original caller return from scratch `10258`, then passes `XCHG EBX,EBX; NOP` at `3434D/3434F` and **falls through to `34350`**. Its sampler RET returns to the original `34A74` caller.
- Second-phase helper `34954..34994` uses paired VGA plane masks **`0C,03`** (AX `0C02,0302`), and **two** E8 calls `34972/34989→34350`. Its own RET at `34994` returns to `34A74`. Both phases temporarily store/restore their return address through source `10258`.
- There are therefore **five explicit E8 calls + one controlled fall-through**, each executing the same 64 texel sampler for six distinct VGA mask phases. One logical first-phase row covers four masks; a paired-plane row covers two paired masks, not a fabricated 64-pixel texture.
- Outer `34A74` sets destination `FF40=FF48+FDBC` at `34A97..34AA6`, and texture base `FF54=C040+2` at `34AA7..34AB3`. It reads mutable `FF50` at `34AB9` for first `min(80,FF50)` rows and again at `34B47` for the paired `FF50-80` rows. The live native `GroundTextureRasterSpec` correctly distinguishes **64 plane bytes/row, first 80 rows and max 128** in its source contract.

## Exact source evidence and bounded alias result

The new `verify_original_ground_raster_unrolled_planes.py` checks original CODE.2 hash, exact 64×24 sampler bytes, both helper SHA-256 bounds, all VGA masks/port writes, the five near-call destinations, original `10258` return-address protocol, first fourth-plane fall-through, `FF40/FF54` source setup, and both `FF50` operand reads. It also checks no *direct literal FF50 operand* in either helper. This independently passes against the original uploaded file.

**This narrows but does NOT close write aliasing.** `34350` writes by `ES:[EDI]`, and no current field trace proves the segment/base pointer cannot alias original DGROUP `FF50` for every possible mode. Additionally, source `34A74` modifies four immediate opcode bytes, and original upper callbacks may read IRQ/modify RNG. Therefore existing `ground_projection_lower_ff50_if_upper_unchanged` remains conditional, and variable-height live terrain remains unaccepted until callback/segment state is proven. Original 64/64 Level22 full-height native #658 frame evidence remains accepted; original DOS frame/timing/RNG acceptance remains pending.

**Build discipline:** Registered standalone Python CTest `original_ground_raster_unrolled_plane_tests` adds one test to this source branch. No Windows Full CI dispatched; workflow must remain manual-only. No original game bytes, artwork, gameplay pixels or source timing were changed.

## Next investigation

Trace original destination `ES` and `FF48/FDBC/FF44` update paths through `34A74` callers and 34350 helpers, resolve indirect `F0C8` installed target from original startup/callback lifecycle, and then independently capture matched original DOS indexed framebuffer and tick state. Only after those gates can dynamic FF50 pixel rendering be integrated without speculative substitutions.
