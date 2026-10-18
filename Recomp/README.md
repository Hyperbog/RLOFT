# Ravenloft CODE.1/CODE.2 — recompilation workstream

**Owning branch:** `recomp`. Original-source recompilation, DOS/4GW compatibility, native link/startup experiments and independent original-image tests. **Not** the canonical enhanced Windows game: that is [`demo-v1`](https://github.com/Hyperbog/RLOFT/tree/demo-v1/DemoV1).

## Build and test (Windows x64 / MSVC)

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

**Linux GCC/Clang:** run `bash dev/linux.sh test` from the repository root; [Linux-first workstream setup](../dev/README.md) covers dependencies. [Verified Linux C++ run #39082582548](https://github.com/Hyperbog/RLOFT/actions/runs/39082582548) passed **63/63 CTests** on its recorded revision.

**Windows milestone builds:** [schedule, manual selection, artifact downloads and limits](WINDOWS_MILESTONE_BUILDS.md). [First R04 probe #39082555268](https://github.com/Hyperbog/RLOFT/actions/runs/39082555268) compiled and passed **63/63 MSVC CTests** while R04 remains unaccepted. Artifacts are engineering tests, not playable-game releases.

The owning [.github/workflows/recomp-source-link-gate.yml](../.github/workflows/recomp-source-link-gate.yml) is scoped to `recomp` and packages **engineering executables, not a playable game**. The reconstructed `Code1/` and `Code2/` directories remain at their established paths because these are compile inputs. [Workstream/branch guide](BRANCH_GUIDE.md) documents ownership.

## Acceptance boundary

Historical research reached R01–R03 of 12 planned gates, with R04 DOS/DPMI memory/startup investigation incomplete; the older milestone count is **not a current measure of enhanced-game completeness**. The last historical README claimed Windows source run #38325531978 with 63/63 CTests at its stated revision. Retest current HEAD before updating any verification claim. Neither a successful C++ linker nor bounded source entry tests proves original DOS boot, complete original-machine equivalence or player gameplay.

## Evidence and historical notes

The previous extensive milestone log remains in [README_HISTORY_20261017.md](README_HISTORY_20261017.md), in its original directory to preserve relative links to source-address audits. Original-file identity checks, DOS memory selectors and initializers must remain independently attributable to CODE.1/CODE.2, with no proprietary originals in build artifacts.

[Current GitHub branch/CI observation](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/LIVE_STATUS.md) is automated, but does not establish recompilation semantic acceptance.

## Graphics recompilation — original CODE.2 VGA

[Graphics source map, binary SHA witnesses, and remaining integration boundaries](../Code2/graphics/ORIGINAL_VGA_SOURCE_MAP.md) document the original 0x29953 mode setup, 0x2AB38 planar pixel address, 0x29CA0 DAC6 stream and 0x2ABF4 CRTC page start. The separate [portable host component](src/original_vga_host.hpp) and [CTest](src/test_original_vga_host.cpp) implement and test four 64 KiB video planes, palette bytes, latches, GC read and write modes 0–3, bit-mask/rotate/AND/OR/XOR and synthetic indexed scanout. The original CODE.2 image was independently authenticated for the six cited instruction ranges, and the proof helper is shared exactly with `code1-decompilation`.

**Important:** the source graphics calls are not yet hooked to the host adapter, original VGA timing/CRTC modes are incomplete and no real game frame has been produced by recompilation. This is a graphics *hardware-layer regression milestone*, not a playable graphics release.

## Original Ravenloft RES image rendering (verified)

[Actual RES0/RES1 type-1 graphics source integration](../Code2/graphics/REAL_RES_TYPE1_SOURCE_RENDER.md) now decodes owner-owned RES bytes via a bounded read-only adapter and calls **original reconstructed CODE.2 `2A20C` image packing, `2F29C` cached item lookup, `2975C` resource drawing and `2A380` VGA output**. Authentic `RES0:15` and `RES1:336` rendered on a Windows CI runner with original `COLORS` bank 0, with original source hashes checked, and 68/68 CTests passed. [Windows milestone #39088036213](https://github.com/Hyperbog/RLOFT/actions/runs/39088036213) provides the current engineering executable. Original resources and their decoded frames are never bundled in CI downloads. Full DOS resource file opens, composed 3D/gameplay rendering, and visual parity remain open.
