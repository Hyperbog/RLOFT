# Level 22 ground tile — original INIT base and signed level selector resolved

**Branch:** `demo-v1` · **Date:** 2026-10-08 EDT · **Source-only,
not yet compiled by current-HEAD Windows CI.** Actions remain
manual-only. Last observed green run: #640, 160/160 at older SHA.

## Critical correction to earlier research

Earlier checkpoints described `DGROUP:258F8` as an unresolved
resource base and treated Level 22's relative selector 55 as
insufficient to identify the real RES4 ground resource.

Direct instruction-level review of original `CODE.2`
SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
closes that **particular** uncertainty:

1. The original resource-configuration loader reads a two-byte
   value at source `INIT+0x0E`. Disassembly:
   `16E55` sets read position `0x0E`,
   `16E68` seeks using `3131C`,
   `16E70..16E80` reads two bytes into a
   zero-initialized local via `312EC`.
2. `16E86 MOV EAX,[ESP+24h]` retrieves that word,
   and `16E90 MOV [258F8],EAX` stores the actual
   original RES4 base. `16E8A` sets the **NEXT**
   field to offset `0x10`; that value must NOT
   be confused with the current base at `0x0E`.
3. At `1F388`, `MOVSX EAX,WORD [2561F]` loads
   signed level header `+7` because CODE.2 copies
   the level header to DGROUP `25618`.
   `1F391` adds `DWORD [258F8]`,
   then `1F398` calls `2F29C` with bank 4.
4. `2F29C` checks resource type exactly 1;
   `34A74` receives the loaded decoded resource body
   and advances two bytes before 64x64 texel reads.

The original **fresh Level 22** header's selector is
`0x0037 = +55`. Therefore the absolute ground
RES4 entry for the original supplied file set is:

```text
base = little_endian_u16(original_INIT[0x0E:0x10])
relative = signed_little_endian_i16(LevelState.header[7:9])
absolute_resource_id = base + relative
```

This is a **source-defined mapping**, not a texture
candidate guessed from screenshots. The actual numeric base
and resulting resource entry depend on the original INIT
file and are intentionally read at runtime, not hardcoded
without raw file evidence.

## Source integration now committed

`OriginalGroundRes4Audit::base_from_init` checks the
source length and parses the exactly specified unsigned
little-endian word at `INIT+0x0E`.

`OriginalGroundRes4Audit::resolve(INIT,LevelState,ResArchive)`
reads the signed relative selector from original header
bytes `7:9`, checks the complete arithmetic and archive
bounds, and demands that `ResArchive::decode_type1`
decode an authentic one-image 64x64/4096-byte source.
On success it keeps exact source base, signed selector,
absolute resource index and original decoded indexed
tile (with two unused host alignment bytes for the
`C040+2` sampler contract).

`ExplorationRuntime::initialize_gameplay_state`
now performs this source lookup at fresh Level 22
startup using its original `GameSource::read("INIT")`
and `GameSource::read("RES4")`. A genuine source
decoding failure is **reported**, not replaced with
fabricated ground art and not allowed to crash the
existing diagnostic walkaround.

Every later runtime frame reports:

- `ground_source_verified`;
- the exact `INIT` base and signed header selector;
- selected absolute RES4 entry;
- the source decode error if validation failed.

Opt-in native capture TSV now exports these as
`ground_original_res4_*` rows, alongside the
previous original FF58/SINTAB row and camera-phase
diagnostics. The standalone RES4 console probe
now also reads `LEVELS.DAT` Level 22 itself and
prints that **source-selected** resource rather than
merely enumerating visually similar candidates.

## Regression boundaries

The 164th registered CTest (expected on Python-enabled
Windows runner) now exercises:

- Original packed RES offset table and type-1 descriptor
  layout; raw, LZSS and DC90-transposed 4096-byte tiles.
- A synthetic `INIT+0x0E` little-endian base.
- Explicit signed relative selector from preserved
  `LevelState.header[7:9]`, successful and out-of-range
  cases and no arbitrary fallback.
- Lossless resolved texture bytes and one-row output
  of the existing original 34A74 sampler.
- Capture metadata with exact resource ID and failure
  values in separate successfully rendered frames.

The test source and integration have been committed,
but **not compiled or run on new Windows CI**. A passing
original-resource fixture is not evidence of parity
with an actual DOS runtime frame.

## Still open: true ground pixels in the live viewport

Even after source lookup is resolved, the current live
`ExplorationPresenter` still uses original flat
`33DE0` as an incomplete fallback.

To replace it faithfully, the remaining state includes:

- `FF50` dynamic rendered row count. Original
  `CODE.2 1F255..1F3B4` branches on `22220`
  and upper-surface state to determine how much
  lower texture to draw; the 34A74 raster must not
  blindly draw 128 rows in every state.
- Runtime `C026/C027` mode, top-surface branch,
  fill fallbacks, sprite compositing and `34DE8`
  follow-on output order.
- Comparison against real DOS indexed 320x400
  frames with matched original fresh-party state,
  palette and simulation time.

**Do not call this a finished live terrain renderer.**
The verified original RES4 source will be available
to the runtime, but the actual viewport still needs
state-accurate activation and pixel validation.

**Formal acceptance stays 1/5 gates (20%).**
No automatic build was started.


## Subsequent guarded live Level22 composition

[Original CODE.2 34A74 now conditionally used in
native first-person output](LEVEL22_LIVE_ORIGINAL_34A74_OPENFIELD_GROUND_20261008.md).
Verified INIT+0x0E and signed header+7 RES4 tile
is passed to the original SINTAB texture raster,
but **only** on exact full 256-ray 0x93
outdoor source records with original C026
textured mode. Original 335C8/1F2A5/1F372
then prove original FF50=128. Other modes
remain on an explicitly reported flat fallback.

This is an unbuilt source change, not DOS
framebuffer pixel equivalence.
