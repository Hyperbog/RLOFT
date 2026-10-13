# Native Alpha 0.6 — authentic original RES4 textures in interactive Windows preview

**Date:** 2026-10-12 (US Eastern; verified Windows UTC execution 2026-10-13).  
**Product:** independent `rloft_native.exe` replacement-engine technology preview.  
**Owning source:** `native-engine:0cdcfd40523b818d7410859f970d8dec6668f798`.  
**Windows acceptance branch:** `native-alpha-textured-acceptance-20261012:e031a2ad875e1073bd5cc20b78844451967bc47e` (the native source is identical; only the one-time Windows workflow is changed).  
**Acceptance:** source-only Linux successful; real-owner-original Windows x64/MSVC **32/32 CTests PASS** plus end-to-end original media smoke **PASS**. The user's direct GOG GUI field test has **not** occurred.

## Product-facing improvement

The existing independent Windows `rloft_native.exe` now exposes **Explore LEVELS.DAT (preview)**, a separate native first-person window. It opens the original player's installation through `NativeMedia`, reads authentic `LEVELS.DAT`, `RES4` and `COLORS`, and uses:

- Original game 32-level map cells, CODE.2-proven normal collision categories, level resource lookup tables and optional state-derived variation.
- Original `RES4` EH6 64×128 decoded wall texture pixels (cached per referenced source ID, loaded on demand).
- Original CODE.2 per-level selected COLORS DAC6 palette bank; exact source indexed pixels converted to Windows BGRA.
- Native W/S/Up/Down walk, A/D strafe, Left/Right turn, PageUp/PageDown level choice, R respawn and Esc close, with source-owned media immutable.
- A mandatory on-screen disclosure: **ORIGINAL RES4 + COLORS / PROVISIONAL HOST CAMERA — NOT DOS FRAME**.

**Fidelity boundary:** This is NOT a reconstructed DOS raycaster, original wall-side bank choice, original depth or wall-height scheduling, floor/ceiling rasterizer, sprite animation, actor/interaction simulation, native save-game or full DOS frame parity. The view uses deliberately provisional DDA and host bank/texture-column/projected-height choices, and a host-selected original palette background. Reusing original pixels is *not* proof of original scene equivalence. Existing original VGA EH6/EH7 compositor proofs remain independent and should ultimately replace this provisional host path.

## Authentic Windows source/media test — PASS

[Owner-authorized isolated build #38325198679](https://github.com/Hyperbog/RLOFT/actions/runs/38325198679) compiled Windows x64 with MSVC, passed **32/32 native CTests**, and executed newly compiled `rloft_native_original_preview_audit.exe` on exact authentic source data sparse-checked-out from `DnD-Classics`, directory `MASTER/Ravenloft - Strahd's Possession/CD_Extracted/RLOFT`.

The acceptance program loaded **all 32** authentic maps, selected source-passable camera cells, performed original-world-backed host ray casts and original-texture render draws, turned the camera and attempted bounded native moves for each level. The C++ application produced:

| Result | Windows measured |
| --- | ---: |
| Loaded authentic original levels | **32 / 32** |
| Original maps with wall hits | **32 / 32** |
| Original EH6 RES4 textured wall columns | **5,048** |
| Original RES4 sampled texels | **360,182** |
| Distinct real EH6 textures decompressed | **95** |
| Native Windows source-only CTests | **32 / 32 passed** |
| Authentic original media before/after SHA | **Identical** |

Aggregate full-frame BGRA digest (32 original-source preview images, width160×height200):
`e059d63c1f0dd477b25c67364bd57637617295b2bbee4275e970209457b0ff05`.

The acceptance workflow checked only six required authentic source files: `LEVELS.DAT`, `RES4`, `COLORS`, `INIT`, `RES0`, and `T` (the latter three satisfy the original loose installation signature). The workflow's first exploratory run #38325001159 intentionally omitted the installation signature and was refused by `NativeMedia`, then was corrected without weakening source validation. **No original game data was published in the Windows artifact.**

## Exact reproducible package

[GitHub Windows Actions artifact #11780003145](https://github.com/Hyperbog/RLOFT/actions/runs/38325198679/artifacts/11780003145) was downloaded and independently checked.

| File | SHA-256 |
| --- | --- |
| CI-built Win64 GUI `rloft_native.exe` | `6fd4f26f3255f89b5d575de1ffda8ca187b2168931f06e5067433b9575ee3234` |
| CI-produced inner six-file distribution ZIP | `ee4ef5802b6c49d8dea131b2de7bae38982883d23f8fa625b78cdccd7210f9b4` |
| GitHub Actions outer artifact ZIP | `f0001cb089c41ffc5d8158a7b90ff3d34a7818cb4252567652f01a722196f372` |
| Native bank audit PE | `0c78b35f7364fdbd4ca7a0e6af3acd8d797247a8196b19fa8565f334122dd8e0` |
| Native original-world audit PE | `8733a352f65540c5d281234d5239fe001928854d07ff50ffc3ba402e15fc287f` |

Separate user-oriented Alpha 0.6 package also adds an explanatory README without modifying CI-built PE bytes. Its full packaged SHA is `183ddc70616d2b9e8f1a4b3839f0dfe4c730a266333f9bc19ff868b2c1aa0064`; this is NOT the same ZIP as GitHub's original inner six-file package.

## Remaining acceptance and engine milestones

The real original-source headless Windows program and the Windows game EXE build have passed. **A user pressing keys in the actual Windows exploration window using the GOG ISO remains unverified**. This must be distinguished from Windows CI, which cannot exercise the interactive desktop as the player.

Next priorities are original CODE.2 live camera/ray depth and automatic wall/material dispatch into the already verified original EH6/EH7 indexed VGA and DAC6 compositor, then original DOS frame-reference comparison. Only then promote the visualization from authentic-texture host preview to DOS-faithful Alpha 1. Add gameplay and normal party/event save support later; do not claim first-playable milestone closed now.
