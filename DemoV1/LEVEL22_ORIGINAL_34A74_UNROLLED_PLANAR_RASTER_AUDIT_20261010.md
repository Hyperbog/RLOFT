# CODE.2 34A74 → 342E4/34954 → 34350 exact planar raster audit — 2026-10-10

**Original authority:** uploaded `CODE.2` 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, object-1 physical base `0x3AF24`. Independently tested exact source bytes. This is a **bounded original sampler/callback audit**, not original DOS pixel equivalence or a proof of runtime ES alias safety.

## Original planar source loop and non-obvious control flow

- Original `34350..3494F` is exactly **64 identical 24-byte instruction sequences** (1,536 bytes). Each sequence resets ESI, adds EBX/EBP to V/U accumulators, computes a 64×64 source index with two `SHLD ESI,...,6` operations, adds source base **`FF54`**, reads one indexed texel with `LODS BYTE DS:[ESI]`, subtracts shade `AH`, and writes **`STOS BYTE ES:[EDI]`**. Then `34950` returns. The exact 1,536-byte sequence and function bound are checked by an original binary oracle.
- First-phase helper `342E4..3434F` selects VGA sequencer port **`03C4`**, register 2 plane masks **`08,04,02,01`** (written AX values `0802,0402,0202,0102`). It reloads output pointer **`FF40`** for each plane. Calls at `34302/34319/34330` target `34350` for the first three planes. The **fourth plane is NOT a fourth E8**: at `34347` it pushes its original caller return from scratch `10258`, then passes `XCHG EBX,EBX; NOP` at `3434D/3434F` and **falls through to `34350`**. Its sampler RET returns to the original `34A74` caller.
- Second-phase helper `34954..34994` uses paired VGA plane masks **`0C,03`** (AX `0C02,0302`), and **two** E8 calls `34972/34989→34350`. Its own RET at `34994` returns to `34A74`. Both phases temporarily store/restore their return address through source `10258`.
- There are therefore **five explicit E8 calls + one controlled fall-through**, each executing the same 64 texel sampler for six distinct VGA mask phases. One logical first-phase row covers four masks; a paired-plane row covers two paired masks, not a fabricated 64-pixel texture.
- Outer `34A74` sets destination `FF40=FF48+FDBC` at `34A97..34AA6`, and texture base `FF54=C040+2` at `34AA7..34AB3`. It reads mutable `FF50` at `34AB9` for first `min(80,FF50)` rows and again at `34B47` for the paired `FF50-80` rows. The live native `GroundTextureRasterSpec` correctly distinguishes **64 plane bytes/row, first 80 rows and max 128** in its source contract.

## Original VGA page and destination-register provenance

Original `1F1C8` first reads **BYTE `C050`** at `1F1C9` to pass the page to `291B0`, but then reads the complete **DWORD `C050`** at `1F1D6`, shifts it by 15 and adds `0xA0000`. At `1F1E6` it stores the result to **`FDBC`**: `FDBC = 0xA0000 + (DWORD[C050]<<15)`. Original shipped DGROUP has `DWORD[C050]=1`. The distinction between BYTE and DWORD source reads is preserved; runtime writers of the full C050 value and ES state are separate acceptance questions.

The original upper surface at `1F32C/1F336/1F343` assigns `FF4C=+1`, `FF44=+80`, `FF48=0x148`. The lower surface at `1F39D/1F3A7/1F3B4` assigns `FF4C=-1`, `FF44=-80`, `FF48=0x50F8`. Original `34A97` computes **`FF40=FF48+FDBC`**, and `34B2D/34B95` advance FF40 by the signed FF44 stride on subsequent rows. These are **13 exact original instruction windows** verified against the uploaded CODE.2 and now covered by the same source oracle.

For a **source-valid page number 0 or 1**, 128 rows and an 80-byte logical row width, the computed offsets remain inside the selected 32,768-byte VGA page. The upper segment covers relative `0x148..0x2947`; the lower relative `0x2948..0x5147`; for page 0 these become `0xA0148..0xA5147`, for page 1 `0xA8148..0xAD147`. This limits the **computed linear destination** to original VGA memory rather than directly addressing DGROUP `FF50`; it is *not* proof of the original runtime ES segment-base or external callback-installed graphics pointer. The branch remains safely read-only in native engine source.

## Exact source evidence and bounded alias result

The new `verify_original_ground_raster_unrolled_planes.py` checks original CODE.2 hash, exact 64×24 sampler bytes, both helper SHA-256 bounds, all VGA masks/port writes, the five near-call destinations, original `10258` return-address protocol, first fourth-plane fall-through, `FF40/FF54` source setup, and both `FF50` operand reads. It also checks no *direct literal FF50 operand* in either helper. This independently passes against the original uploaded file.

**This narrows but does NOT close write aliasing.** `34350` writes by `ES:[EDI]`, and no current field trace proves the segment/base pointer cannot alias original DGROUP `FF50` for every possible mode. Additionally, source `34A74` modifies four immediate opcode bytes, and original upper callbacks may read IRQ/modify RNG. Therefore existing `ground_projection_lower_ff50_if_upper_unchanged` remains conditional, and variable-height live terrain remains unaccepted until callback/segment state is proven. Original 64/64 Level22 full-height native #658 frame evidence remains accepted; original DOS frame/timing/RNG acceptance remains pending.

**Build discipline:** Registered standalone Python CTest `original_ground_raster_unrolled_plane_tests` adds one test to this source branch. No Windows Full CI dispatched; workflow must remain manual-only. No original game bytes, artwork, gameplay pixels or source timing were changed.

## 2026-10-10 source-clipped raster 80/81 row transition regression

The original `34ABF` compares current `FF50` with `0x50` and caps the **first 4-plane phase** at 80 rows; exact instruction bytes `34ABF..34AC8 = 83 F9 50 7C 05 B9 50 00 00 00`. Original `34B4D` subtracts `0x50` and `34B50/34B53` enters the **second paired-plane phase only for signed positive** residual: exact `34B4D..34B54 = 83 E9 50 83 F9 00 7E 55`. These bounded branches were reverified against the immutable original and added to the existing Python binary oracle.

The pre-existing native `OriginalGroundTextureRaster::draw` already supports 1–128 source-authenticated rows. The expanded existing C++ raster regression now **explicitly** checks positive `FF50` budgets **1, 79, 80, 81 and 128** without inventing wall geometry. It requires exactly `min(rows,80)` single-plane rows, `max(rows-80,0)` paired-plane rows, sample counts `256*single+128*paired`, output indexed writes `256*rows`, and source texel/shading samples at the final drawn row. Crucially, it checks the next *undrawn* row and side gutters remain unchanged. This guards against a regression at the original 80→81 plane-mask transition and premature drawing outside a source-proven clipped band. Full-height existing field acceptance remains separate.

**This is not live partial-ground acceptance**: the original runtime `22220`, post-upper callback `FF50`, C026/C027, mutable graphics callback and ES state remain unproven across all DOS scenes. This is a **test-only** change; current native source has not undergone Windows Full compilation. The original game binary remains outside Git.

## Next investigation

Trace original destination `ES` and `FF48/FDBC/FF44` update paths through `34A74` callers and 34350 helpers, resolve indirect `F0C8` installed target from original startup/callback lifecycle, and then independently capture matched original DOS indexed framebuffer and tick state. Only after those gates can dynamic FF50 pixel rendering be integrated without speculative substitutions.
