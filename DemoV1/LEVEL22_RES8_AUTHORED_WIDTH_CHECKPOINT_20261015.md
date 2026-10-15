# Level 22 — RES8 authored-width clipping

**Date:** October 15, 2026  
**Branch:** `demo-v1`  
**Source commit:** [`362d66a49c6fa9f83392ead4d1ff9bd7713cbdda`](https://github.com/Hyperbog/RLOFT/commit/362d66a49c6fa9f83392ead4d1ff9bd7713cbdda)

## Verified source discrepancy

`EnvironmentProjectionModel::build` computes x-position using `EnvironmentPlacement::draw_width_pixels` and places that exact width in the typed `EnvironmentDrawCommand`. The randomized sky/background path similarly emits 32 commands, each with an **8-pixel** width, at viewport x = 32 + 8*i.

Before this change, both `EnvironmentPresenter::draw_commands` and `ExplorationPresenter::draw_environment` passed every command's x, y and shading to `draw_subtracted`, but **dropped `command.width`**. The raster loop then iterated the decoded `Type1Raster::width`, which can exceed the plan's intended width. This means that any oversized source resource spills into the next column of scenery, and subsequent draws overwrite/obscure previous tiles. The source contract was inconsistent even though exact Level 22 screenshot impact is **not yet verified**.

## Implementation

- `EnvironmentPresenter::draw_subtracted` has an optional `authored_width` parameter. The default `std::nullopt` preserves the legacy full-raster source drawing behavior.
- When a native command supplies a width, the renderer draws at most `min(authored_width, raster.width)` source columns; it does not stretch, sample outside decoded media or invent padding.
- Both the reusable command presenter and the enhanced exploration presenter now pass `command.width`.
- A zero/negative explicitly supplied width is rejected before any framebuffer change. Extreme signed x/y placement uses widened 64-bit accumulation to avoid wrapping into the viewport.
- Transparency, palette index subtraction, authentic RES8 content, placement x/y, SINTAB/world geometry, viewport clip and existing 60 Hz timing are not otherwise modified.
- Synthetic tests cover narrow width, transparency inside the crop, authored width larger than the decoded stride, invalid width rejection without partial writes and near-integer-limit coordinates.
- The enhanced-level22 source contract requires native command width forwarding and adds a mutation that must be rejected if it is removed.

## Verification and limitations

**Verified:** [Windows CI #885](https://github.com/Hyperbog/RLOFT/actions/runs/38823907665) successfully compiled exact source commit `362d66a49c6fa9f83392ead4d1ff9bd7713cbdda`. The enhanced-Level22 source verifier rejected **16/16 negative mutations**; all **38/38 focused CTests passed**, including `environment_presenter_tests`, `original_ground_texture_raster_tests` and frontend-preservation contracts. Continuous Alpha ZIP publication, intro and field-test bundles, and checked-in EXE refresh succeeded.

**Source-matched Windows Alpha artifact:** `Ravenloft-Continuous-Alpha-Windows-x64`, ID `12014805145`. Field-test package ID `12014495566`; original intro package ID `12013199775`. Checked-in PE build source is stored in `DemoV1/Windows/build-source-sha.txt`, and its SHA-256 is `43e7d32ea3b4245338de4d0f6c6d1710de3567ede6a100247c984a5cf3664cf5`.

Without comparing actual RES8 source dimensions at each Level 22 pose, we cannot claim how many pixels this prevents from being overwritten in the owner's installation. The correction protects the draw-command contract and resolves the source-level overdraw possibility, **not** all wrong environment color or tree placement. Real-game screenshot/field validation, palette accuracy, NPC placement, inventory, automap and menu acceptance remain separate and open.

**Mod-forward boundary:** authors can eventually supply validated resources with independent dimensions; the planned native draw width is a typed compositing constraint rather than a hardcoded RES archive assumption. No published mod package schema or loader is implied.
