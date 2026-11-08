# Part 21 — Original CODE.2 181B4 direct saved-level restoration checkpoint

**Branch:** `recomp`. **Validated commit:** [`8f3deb5`](https://github.com/Hyperbog/RLOFT/commit/8f3deb5a1a33a0c3b7bbcd4258df0664bd741e6a). [Original saved-level CI](https://github.com/Hyperbog/RLOFT/actions/runs/39133894933): **success — 17/17 preserved LEVnnn.DAT**. The Windows, Linux, canonical source-link, and authenticated world graphics regressions are independently maintained.

## Why the new save snapshot matters

The owner's `SAVE10.zip` contains a Level 5 active `SAVE.DAT` with position approximately (6.349, 6.920), facing 252°, plus 17 standalone `LEVnnn.DAT` files, including Levels 11 and 22. These files **do not** imply the camera last used on Level 11; the `LEV011.DAT` file is saved per-level mutable state, not a camera save. There are **67,773** cell words across those 17 standalone maps. Differences from the unmodified archived world include only 12 topology-code changes across Levels 5 (10), 10 (1), and 25 (1); the remaining many grid differences reflect exploration/current visibility flags. This comparison is an independent host audit, not a claim about runtime parity.

## Actually executed original source

The existing archive-level loader diagnostic executed `function_181b4` only via its `LEVELS.DAT` fallback. `rloft_original_code2_level_loader --saved-levels <dir>` now executes its **other original branch**: direct open of original `WORK\\LEVnnn.DAT` using the authenticated `181B4` pathname-building bytes `81A4..81A6`, mapped to an allowlisted **read-only** DOS file provider rooted at the selected save directory.

For every found LEVnnn file, an independent bounded parser validates the standalone 0x1D-byte seven-section directory, 0x2A4-byte header, row-major 16-bit grid, 0x514-byte event script block, the four-byte post-event gap, the width×height-byte automap, and the final length-prefixed annotation bytes. Original compiled source `181B4` reads the file and fills guest memory; it is not populated from the parser. The verifier then checks actual guest `0x25618` header, `0x198E0` cells, `0xCF44` script pointer/content, `0x1760` automap, `0x5760` annotations, and the original scale-word normalization. Source `334C4` is executed on the same loaded memory; it must preserve authored topology/exploration flags and only clear transient `0x0040` visibility marks before the deliberately intercepted raycast.

The [new Linux CI gate](https://github.com/Hyperbog/RLOFT/blob/recomp/.github/workflows/recomp-saved-levels-linux.yml) builds the canonical-source C++ executable and uses a **sparse, read-only checkout** of the pre-existing `DnD-Classics/MASTER/Ravenloft - Strahd's Possession/cloud_saves/RLOFT/SAVE10`. It checks all 17 direct saves and does not publish any original user game media, save or serialized bytes.

## Reproduce with a local copy

Extract `SAVE10.zip` to a private folder and execute:

```sh
rloft_original_code2_level_loader --saved-levels /private/SAVE10
```

The non-save `LEVELS.DAT` 32-level gate and `--raycast LEVELS.DAT SINTAB` source Level 22 gate remain separate. The authentic Level 11 full RES4 multi-material compositor gate also remains separate and relies on its source-defined **Level 11 transition camera**, not on any unrecorded location in the saved `LEV011.DAT`.

## What is not proved

- Mutable live NPC/actor records, moving-object semantics, event **execution**, dynamic-door behavior and complete save-game re-entry have not yet been independently compared byte-for-byte against original DOS running state. Their serialized bytes pass through the original source loader, but this gate verifies only the header, grid, 0x514 script, automap and annotation guest regions.
- Continuous `SAVE.DAT` character/global restoration, the Level 5 active camera in a full game boot, outdoor sky/ground/trees, HUD overlays and sprite occlusion still require source-to-runtime integration.
- Independent original DOS VGA `.idx`/RGB6 `.pal` reference capture is still missing; neither an ordinary screenshot nor the native frame's own hash satisfies DOS pixel parity or R04 memory acceptance.

**Next engineering boundary:** reconstruct global `SAVE.DAT` restore and the associated `WORK/LEV005.DAT` level state in one CODE.2 session; verify source camera (Level 5, x≈6.349, y≈6.920, 252°), author-selected scenery/resource initialization and actual world output against independent original-game observations when available.
