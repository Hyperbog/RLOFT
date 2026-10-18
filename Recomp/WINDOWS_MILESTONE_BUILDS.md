# Recompilation Windows milestone builds

**Owning branch:** `recomp`. The regular [source-link Windows workflow](../.github/workflows/recomp-source-link-gate.yml) runs as sources change. The independent [milestone Windows workflow](../.github/workflows/recomp-milestone-windows.yml) offers longer-lived, downloadable MSVC x64 test builds.

## Frequency and manual runs

- **Automatic:** the first and fifteenth of each month at **13:37 UTC**, scheduled by the copy of the workflow on the repository's default branch. The scheduled run checks out the then-current `recomp` branch.
- **On milestone acceptance-file changes:** pushing `Recomp/ACCEPTANCE_GATES.json` on `recomp` triggers a run using that exact branch commit.
- **On demand:** use [Actions → RLOFT Recomp Windows milestone validation](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-milestone-windows.yml), click **Run workflow**, then select `next` or one of `R01`–`R12`. For `next`, the report selects the first not-yet-accepted gate.

Manual selection is a **test focus label**, not acceptance. No build automatically changes `ACCEPTANCE_GATES.json`.

## Contents and evidence

On successful MSVC Debug build and CTest, Actions produces **RLOFT-Recomp-Windows-Milestone-<run>** with a ZIP containing `rloft_recomp_link_gate.exe`, CODE.1 and CODE.2 startup gate executables, original-image verifier, SHA256SUMS, exact source commit and R01–R12 evidence-labeled report. The independent `RLOFT-Recomp-Windows-Evidence-<run>` contains JUnit CTest results, progress manifest and failure diagnostics (when available). Both artifact types are retained **60 days**. No original proprietary binaries or playable Windows game are bundled.

**Compiled original CODE.2 indexed graphics and palette milestone — verified:** [Windows MSVC milestone #39086139502](https://github.com/Hyperbog/RLOFT/actions/runs/39086139502), [Linux GCC #39086139521](https://github.com/Hyperbog/RLOFT/actions/runs/39086139521), and [Windows source-link #39086139589](https://github.com/Hyperbog/RLOFT/actions/runs/39086139589) each passed **67/67 CTests**, including compiled CODE.2 planar image rendering, DAC banks, 200/400 modes, page/split timing adapters, original 32150 brightness and palettized 320×400 BMP validation. Download [Windows compiled graphics engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39086139502/artifacts/12120312840) or [MSVC/JUnit evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39086139502/artifacts/12120417572). The artifact includes `rloft_original_code2_graphics_frame.exe` and synthetic-source `code2-compiled-frame.idx`/`.bmp`. **This is not real-game artwork or DOS gameplay parity; R04 remains unaccepted.** [Source execution map](../Code2/graphics/COMPILED_GRAPHICS_PATH.md).

**Graphics VGA milestone — verified:** [Windows MSVC milestone #39084747886](https://github.com/Hyperbog/RLOFT/actions/runs/39084747886) passed **65/65 CTests**, including the CODE.2 planar address/DAC/CRTC/latch/write-modes regression and binary-span verifier self-test. The artifact now contains `rloft_original_vga_host_tests.exe` alongside the original-source startup diagnostics. Download [Windows graphics engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39084747886/artifacts/12119994330) and [JUnit/milestone evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39084747886/artifacts/12119804960). **R04 remains unaccepted**. This build does not launch the real CODE.2 renderer or show game frames.

**First verified milestone run:** [#39082555268](https://github.com/Hyperbog/RLOFT/actions/runs/39082555268): **MSVC x64 63/63 CTests passed**, artifact [milestone engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39082555268/artifacts/12119900798), [test evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39082555268/artifacts/12119940619). The selected probe was **R04**, which **remains unaccepted** because independent original DOS memory/selector behavior is still unproven. That run was not a playable game build.

**Linux cross-check:** [Ubuntu/CMake/Ninja C++ run #39082582548](https://github.com/Hyperbog/RLOFT/actions/runs/39082582548) passed **63/63 CTests** on its exact source commit. Run `bash dev/linux.sh test` locally from the `recomp` branch. Windows WPF/Win32 gameplay acceptance belongs to the distinct `demo-v1` branch.

## Future milestone policy

R01–R03 have separately recorded historical research acceptance. R04–R12 remain open until their evidence is certified in the source manifest. Passing the same 63 tests at a future milestone does **not** establish a new gate; genuinely deeper source behavior, host compatibility and independent original-machine comparisons must be added and tested. Do not distribute an engineering executable as a playable game or overwrite original retail saves.
