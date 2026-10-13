# Part 16 — original FF50 upper-stage boundary proof

**Branch:** `demo-v1`. **Scope:** independent original-source model
and original-file oracle for the first *provably callback-free* dynamic
lower-floor span. No live renderer or gameplay path was changed.

## Original machine code

The owner-supplied original CODE.2 is 558689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`,
LE code physical base `0x3AF24`.

- `1F255..1F2AF` computes initial signed FF50 and persistent EBX
  from verified original 22220 and SINTAB 15618. Existing
  `OriginalGroundFF50SourceModel` conservatively rejects outside
  the original C738=127 lookup span.
- With original selection BYTE[258A8]==FF and C027 !=0,
  `1F30E` compares the just-written FF50 to zero.
  `1F315: 7E 52` branches **directly to 1F369** when it is
  nonpositive. No upper raster, VGA fill or overlay runs on that
  branch, so FF50 is unchanged *at the lower-stage boundary*.
- `1F369..1F384` reads C026, then computes lower FF50 using
  `0x100-(EBX+post_upper_FF50)` with 32-bit wrap and signed branch.
  When C026==0, `1F3CA` uses original `33DE0` fill instead.
- `33E28..33E6A` was separately byte-audited as a complete
  67-byte source function, SHA-256
  `8e9622be88f5265cddb916d4cc41ff0ee1c93714952ce31d7a37863495844da0`.
  It contains neither a direct FF50 operand nor nested E8 call,
  **but it DOES execute REP STOS through ES:EDI.** DOS selector
  mapping has not been independently matched, so the ordinary
  `33E28` fill is **not** certified as FF50-preserving.
  Neither `34A74`, `29B40` nor `314B0` is admitted.

## Newly implemented tested model

`OriginalGroundSurfaceDispatch::plan_lower_after_skipped_upper`
requires an explicit upper-stage `SkipNonpositiveFF50` result.
Only on that exact source branch does it pass the proven unchanged
initial FF50 to the already source-backed `plan_lower`; all upper
draw/overlay paths return `nullopt`. The caller still must provide
authentic 22220, 15618, selected actor and C026/C027 state.
The native function is constexpr, side-effect-free and does
not consume RNG or write indexed framebuffer bytes.

The C++17 regression checks original nearest 127*8 (upper FF50=-63,
lower=-189), C026=0 flat fill, and rejects 33E28, 34A74,
29B40 and 314B0. The existing source-only and optional authentic
original binary verifier now protects the exact branch and complete
33E28 source bytes. Existing CTest registrations remain unchanged.
The original `plan_lower(..., actual_post_upper_ff50,...)`
continues to represent all other stages.

## Unaccepted

This is a real dynamic-depth *source-model* step, **not a live
FF50 or new rendered terrain span**. Original selector-to-VGA
mapping, actual normal-level mutable mode/callback state,
original raster clipping and matched DOS pixels are still
required before altering runtime draw behavior.
