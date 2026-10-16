# Level22 enhanced-game renderer + player-hit foundations

**Date:** October 16, 2026 · **Game branch:** `demo-v1` · **Status:** implemented/committed, native field acceptance open.

## One continuous C++ game, no duplicate engines

This checkpoint changes the **actual** `DemoV1/` game and its Windows build. Original DOS source files remain read-only media on the archive branch; their bytes are never embedded into a player executable or recreated as commercial artwork. The indexed/software presenter stays authoritative and the default until owner field capture accepts the enhanced 3D overlay.

## Authored RES8/RES4 backdrop now reaches the optional OpenGL world

The prior OpenGL world first displayed the full original indexed framebuffer, then erased its entire 256×256 first-person viewport to one palette index before drawing the 3D scene. That discarded the original RES8 sky and scenery backdrop and could expose flat brown/blank bands.

The game presenter now captures the 256×256 world rectangle at source coordinates **(x=32,y=4)** immediately after the original `draw_environment()` and RES4 ground pass, but **before** source SINTAB walls, RES3 silhouettes or final HUD. The resulting 65,536 **indexed bytes** travel through the live exploration frame to `Host::bind_gpu_frame` and the existing `Win32GpuScene` context.

The new platform-independent `EnhancedGpuBackdrop` projects source indices through the current 6-bit palette into the bottom-up 320×400 RGBA OpenGL canvas, without altering the source assets. Crucially, the live host now passes **the exact `ExplorationPresentedFrame::palette` of that same frame**, instead of letting the GPU reload a static level palette. This maintains RGB consistency during in-game palette changes. Normal F2 3D now draws that original panorama and ground underneath the genuine RES4 OpenGL mesh and live RES3 billboards. The original top 64 menu rows are preserved when the menu is active. The code clears GL **depth only** before drawing geometry, rather than erasing the source sky again.

If OpenGL geometry/resource drawing fails, the error handler restores the **actual original full software scene** from the same frame into the RGBA canvas, uploads the restored pixels, and redraws the viewport immediately. Subsequent frames fall back to the indexed world. No invented solid-color sky or duplicate software silhouettes are used as GPU artwork.

`enhanced_gpu_backdrop_tests` checks 6-bit index→RGB, GL vertical row inversion, viewport sides/bottom, top-menu preservation, full software-frame restoration and invalid dimensions. This is included in the ordinary Windows workflow. The source-integrity guard now explicitly rejects backdrop capture after wall/sprite composition or failure to hand it to the live host.

**Boundary:** This materially corrects the 3D background composition, but it does not prove accurate perspective, sprites, tree sizes, floor texture phase, shading, draw order, actual GPU frame pacing or audible audio. F2 remains optional and Windows screenshots/play remain the acceptance gate.

## Original C7C4 Strength hit source and C80C live character inputs

The supplied `CODE.2` SHA-256 is
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Its original LE object-1 physical source base `0x3AF24` contains logical `C7C4..C80A`, and the actual signed 26-byte DGROUP `0x6832` table begins at physical file offset **`0x7C756`**.

The source-authenticated table for Strength 0..25 is:
`[2,-5,-3,-3,-2,-2,-1,-1,0,0,0,0,0,0,0,0,0,1,1,3,3,4,4,5,6,7]`.

The new `CombatStrengthAttackModel::from_code2()` reads the **user's own installed CODE.2**, preserving signed-byte arithmetic without implementation-defined casts. `score()/inspect()` enforce exactly the C7C4 Strength-18 exceptional ranges (1..50→1, 51..99→2, 100→3) and reject scores outside the original table rather than reading arbitrary adjacent DOS heap bytes.

The new `CombatPlayerHitInputsModel` connects existing, already-audited models using the live data sources, NOT temporary/demo data:
- original actor +0x10A base attack target, current Strength +0x33 and exceptional +0x3F
- NPCS definition signed armor class and live NPC byte+8 flags
- OBJECTS weapon enchantment +0x37, and slot20 `0x54` Stone Giant Gauntlets
- original `InventoryEncumbranceModel::actor()` burden rank from current inventory weight and CODE.2 threshold table
- actor primary-effect +0xF3 flags bit0 Bless and bit3 Aid
- caller-supplied original 1d20, not randomly rolled by the input-projection helper

On a held NPC, C80C returns before actor, weapon, burden or RNG work. The adapter preserves that ordering. The existing `CombatHitCheckModel::resolve()` then applies the already-proven threshold, auto-hit, auto-miss and die-modifier order.

Synthetic C++ tests verify table endpoints, exceptional Strength boundaries, range rejections, actual actor/equipment/NPC source bytes, and 15+5=20 modified auto-hit with threshold 13. A new source integrity test checks seven deliberately altered combat inputs. Both the portable C++ test and source guard are now in the normal Windows build/test pipeline.

**Boundary:** These components provide an accurate, reusable calculation foundation. The actual hand click still needs the full **214B8 → 24478 → 26018 → E5F0** dispatcher, target selection, on-demand RNG, character action-state/cooldowns, weapon creature gates, injury/death sounds, XP and animations. None of those are pretended to be complete here, and no new damaging click is introduced prematurely.

## Verification

- The supplied original `CODE.2` and table/opcodes were checked independently offline.
- Current GitHub branch contains the source, tests, CMake registrations, normal Windows workflow target names, and negative-mutation source checks.
- A compiled-source-matched Windows run or owner game screenshot **has not been confirmed for this patch**. Preserve Pre-Alpha status until a new build and Windows play evidence exist.
