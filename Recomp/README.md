# Ravenloft: Strahd's Possession — original-source recompilation

**Active branch:** [`Recomp`](https://github.com/Hyperbog/RLOFT/tree/Recomp) (case-sensitive). This is the reconstructed original DOS CODE.1/CODE.2 workstream, distinct from `demo-v1` and from `DnD-Classics`, which preserves the original game files.

## Current status — 2026-10-26 UTC

The original CODE.2 runtime retains the same guest memory, heap, source VGA, resource caches and input loop across compiled gameplay callbacks. The experimental Windows host presents and accepts input on original Level 22. The saved Level 23 → Level 26 → Level 23 original-source portal diagnostic now **passes a strict native full-cycle acceptance**. This does not establish physical portal playability, general save-system coverage, complete DOS devices, or retail DOS frame/audio parity.

**Retained original Level 23 save restoration: VERIFIED.** The preserved original `SAVE01` was loaded through compiled `0x07938 → 0x181B4(23)` on one guest. Its source-authored `C6C8=0xFFFFFFFF` no-departing-level sentinel is now supported, with 156 source save reads, 219 source saved-level reads, and zero departing-world writes. The [strict Windows evidence](https://github.com/Hyperbog/RLOFT/actions/runs/40271379104) contains `07938_RETAINED_LEVEL23_RESTORE_PASS`. Original `0x1BDAC` actors and `0x2D5D0` collision callbacks also execute and return.

**Saved Level 23 restoration and authentic Level 23 → 26 → 23 native source frames: STRICT PASS.** [Windows retained-game acceptance #40320888276](https://github.com/Hyperbog/RLOFT/actions/runs/40320888276) at implementation [`b9c07cea`](https://github.com/Hyperbog/RLOFT/commit/b9c07cea4d440af1c9b790588e590e2943f4cc2a) succeeded, including `CODE2_07938_RETAINED_LEVEL23_PORTAL_26_23_FULL_CYCLE_PASS`, native process exit code zero, 3 original `181B4` level transitions, 3 audited original `1FC14` post-level scanout frames, final Level 23 state and balanced scene/page/HUD/audio source cycles. Both portal directions invoke authentic `LEVELS.DAT` type-8 records through compiled `2D3D8`; the diagnostic's private keyboard command is **not a physical in-game walk**. Source `34DE8` Level 26 mode-4 polygon traversal legitimately produces zero polygon quads and visible sprites; the host acceptance now requires completed original cell/projection scans, actor, wall, scene, compass and raster-accounting returns rather than fabricated pixels. The [strict workflow](../.github/workflows/recomp-retained-07938-level23.yml) independently checks retained guest ownership, native 320×400 indexed scanout, RGB6 palette, and a complete post-scene `30B40` re-entry. **Retail DOS synchronized framebuffer/audio equivalence, physical input parity, general world coverage and release-mode fast-fail `0xC0000409` remain open.** See the [retained Level23/26 continuity report](ORIGINAL_RETAINED_07938_LEVEL23_20261026.md).

The historical R01–R12 acceptance ledger remains in [`ACCEPTANCE_GATES.json`](ACCEPTANCE_GATES.json); an MSVC build or passing CTests never automatically accepts an unverified gameplay gate.

## Build from source

Windows x64 / Visual Studio 2022:

```powershell
cmake -S Recomp -B build/recomp -A x64 -DBUILD_TESTING=ON
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

Linux: `bash dev/linux.sh test` from the repository root. The [Windows milestone workflow](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-milestone-windows.yml), [canonical source-link workflow](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-source-link-gate.yml), and [Linux regression](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-linux-ci.yml) produce independent evidence. For the strict retained-save native acceptance, use the [separate source-backed Level 23 workflow](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-retained-07938-level23.yml).

## Download the latest Windows engineering build

Open [Windows engineering builds](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-windows-player-package.yml) and choose an actual **successful** run for `Recomp`, then download its artifact. It contains rebuilt experimental executables, launch scripts and a source SHA, **never original game media**. The comprehensive [Windows milestone workflow](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-milestone-windows.yml) also provides a larger diagnostics package. Read individual nonblocking portal steps; overall green does not imply playable cross-level success.

## Owner-local Windows testing

Extract the **whole** engineering package. Supply your own original GOG `RLOFT` directory containing `CODE.2`; the launch script auto-detects standard paths or accepts `-GameFolder`. Supply `-CdRoot` for an already-extracted original CD `RLOFT` folder, or leave it unset to use the owner-local `game.gog` extractor.

```powershell
.\Launch-RLOFT-Level22.ps1 -GameFolder 'C:\Games\RLOFT'
.\Launch-RLOFT-Level22.ps1 -GameFolder 'C:\Games\RLOFT' -Smoke
.\Launch-RLOFT-Level22.ps1 -GameFolder 'C:\Games\RLOFT' -CdRoot 'C:\Games\CD_Extracted\RLOFT' -SavedLevel23Folder 'C:\Saves\SAVE01'
```

For a **strict** experimental Level 23 → 26 → 23 portal check, add `-SavedPortalSmoke`. The authentic `07938 → 181B4` no-departing-level sentinel now works; a passing restored Level 23 frame is not proof of the later portal. Require the strict workflow's explicit native full-cycle marker and zero exit code before claiming acceptance. The saved-game slot must contain unchanged `SAVE.DAT` and `LEV023.DAT`. No original game files, retail saves or `WORK` paths are rewritten. Original source guest and indexed VGA buffers remain private to the owner.

**Controls:** W/S move forward/backward; A/D strafe; J/L and left/right arrows rotate; Esc quits. Windows-translated keyboard input is not certified physical DOS IRQ9 parity. Fully playable cross-level rendering, Miles AIL audio, all scripted events and independent DOS frame synchronization are open work.

## Repository organization

- `Code1/`, `Code2/`, and `Recomp/src/`: retained original reconstruction and host adapters. `*.inc` fragments are compiled source dependencies, **not** disposable temporary files.
- `Recomp/tests/`, `Recomp/tools/`: source regression and private-media diagnostics.
- `Recomp/ACCEPTANCE_GATES.json`: authoritative historical 12-gate research acceptance record.
- [Documentation index](DOCS_INDEX.md): the current entry points and selected technical evidence.
- [Historical research log](README_HISTORY_20261017.md): consolidated prior README, branch guide, Windows milestone history and Level 22 playtest notes. Older dated source-address reports remain in their original paths to preserve cross-references.

**Branch ownership:** keep recompilation changes in `Recomp`; do not relocate `Code1/` or `Code2/` without updating compilation and ABI contracts. Original commercial files stay immutable on `DnD-Classics` and are never embedded in CI packages. The `demo-v1` enhanced port is a separate workstream.
