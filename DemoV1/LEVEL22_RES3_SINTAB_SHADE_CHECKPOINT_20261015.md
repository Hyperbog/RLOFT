# Level 22 — Native RES3 SINTAB sprite color correction

**Date:** October 15, 2026 · **Branch:** `demo-v1`  
**Implementation source through:** [`86230b5a0db09ce1312a3b9d1afed4ed6d91526e`](https://github.com/Hyperbog/RLOFT/commit/86230b5a0db09ce1312a3b9d1afed4ed6d91526e)  
**Windows verification:** [Source-matched CI run](https://github.com/Hyperbog/RLOFT/actions/runs/38825500152) — report its result once concluded.

## Source defect and evidence

`ExplorationPresenter` previously passed decoded `Type7NativeRaster` pixels and masks directly to `EnhancedType7Compositor`, and the native draw wrote the unmodified RES3 palette index for every opaque pixel. It ignored the already reconstructed `Type7ColumnTraversalPlan::lookup_value`.

In contrast, the shipped SINTAB-generated code used by CODE.2 369D8 executes:

1. `MOV AL,[ESI+displacement]`
2. `MOV AH,AL`
3. `SUB EAX,ECX` (two packed shade bytes, with low-byte borrow)
4. `MOV [EDI+row],AL` or `MOV [EDI+row],AH`

The existing original-source metadata model `Type7VgaRasterDispatchModel` also confirms that CODE.2 36B14 **swaps the packed ECX bytes after each grouped source-column run**, whereas C029 individual-column mode swaps for odd destination X and restores the lookup between runs.

The user-supplied authentic `SINTAB` (333,757 bytes) was independently audited without publishing its bytes. Across **127 scale blocks and 23,910 generated stores**, the AL/AH selector is consistent with the 80-byte VGA row parity. **105 blocks** have the ordinary phase and **22** the inverted phase. This validates that a universal unshaded sprite or one constant shade is wrong. It does not by itself certify pixel-perfect Level 22 placement.

The physical destination address adds the runtime rasterizer row phase and `C6E0` row base. The native shader maps this relative AL/AH phase to the framebuffer Y coordinate before applying the packed lookup.

## Implemented

- A new narrow, typed `EnhancedType7ShadePlan` carries a packed lookup per already accepted visible source column and the SINTAB-derived native row-parity inversion. It operates on the existing scene plan without modifying game state or original archive bytes.
- `EnhancedType7ShadeModel` validates the original generated-code instruction grammar **without executing x86**, parses original store row parity, checks source run/column identity, and applies exactly the grouped/individual lookup byte-swapping rules.
- `EnhancedType7Compositor` optionally receives a shade plan. Ordinary standalone callers still draw unshaded sprites unless opting in. The actual enhanced game presenter supplies the plan on every visible RES3 scene call.
- The native shade calculation accounts for carry from low-byte subtraction into AH. Like the enhanced RES4 ground and RES8 backdrop, a result below zero clamps to palette index zero rather than wrapping into unrelated high-palette accents. This is an intentional enhanced PC color treatment, **not exact DOS SUB underflow behavior**.
- Per-source bounds/palette diagnostics continue to capture *written* indexed colors, with a new `scene_native_draw` shade-underflow-pixel count appended to existing TSV records.
- Regression cases cover AL/AH byte selection, grouped lookup swaps, individual-column parity, source row-phase inversion, low-byte borrow, underflow clamps, error rejection before any draw and preservation of the unshaded standalone API.
- The active enhanced-Level22 source contract guards the integration against silently removing the shade plan.

## Verification boundary

The original CODE.2/RLOFT source assets remain on the `DnD-Classics` branch. This implementation does **not** embed copyrighted raster bytes or use source machine code as a public mod API. It preserves the original RES3 masks, x/y projection and occlusion. Future mods can provide validated replacement images without changing the color lookup or gameplay rules.

The CI run for the exact native source must finish successfully before describing the Windows executable as verified. In-game effect on actual Level 22 tree/NPC RGB and vertical position has **not** been visually measured or owner accepted; the existing `Capture-Level22.cmd` diagnostic can identify which sprite entries and original shade indices matter at a reported pose.

**Still open:** tree positions and scale, wall/ground interactions, palette appearance beyond this confirmed unshaded path, inventory, automap, top menu, save/load and campaign progression. No release gates are changed.
