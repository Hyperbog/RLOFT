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

## Genuine original RES graphics smoke test

[Windows milestone #39088036213](https://github.com/Hyperbog/RLOFT/actions/runs/39088036213) passed **68/68 CTests** plus an additional read-only source test on preserved original `RES0:15` and `RES1:336` (real palette `COLORS`, validated original SHA-256). It captured 90 and 2,992 nonzero indexed pixels respectively and logged only output hashes; no original art/captures are in artifacts. [Windows engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39088036213/artifacts/12121950557) and [JUnit/evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39088036213/artifacts/12121069884) expire December 17, 2026.

To inspect your own legally held original graphic, run the packaged diagnostic:

```powershell
.\rloft_original_code2_graphics_frame.exe --res "C:\Games\RLOFT\RES1" 336 1 0 "C:\Temp\portrait" "C:\Games\RLOFT\COLORS" 0 0
```

This produces owner-local `portrait.idx` and `portrait.bmp` from authentic source art. The ZIP contains only synthetic-frame examples. Original CODE.2 2FD9C unloaded resource/DOS IO is **not** wired, and R04 is unaccepted; [details](../Code2/graphics/REAL_RES_TYPE1_SOURCE_RENDER.md).

## Unloaded original CODE.2 resource-loader milestone

**Latest validated Windows package (obsolete preloading removed):** [Milestone #39090358644](https://github.com/Hyperbog/RLOFT/actions/runs/39090358644) passed **68/68 CTests** and original RES0:15/RES1:336 read-only image validation. Both indexed SHA-256 outputs were unchanged after deleting the legacy cache-prepopulation function. [Engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39090358644/artifacts/12122332063) and [test evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39090358644/artifacts/12122287353) expire December 17, 2026. **R04 remains unaccepted.** No proprietary image, RES archive or palette bytes are distributed.



[Windows build #39089791043](https://github.com/Hyperbog/RLOFT/actions/runs/39089791043) **passed 68/68 tests** and exercised real owner-preserved RES0/RES1 through original `2ECB4`/ `2FD9C` file-backed loading and original image-cache population. The authenticated frame hashes remain unchanged. [Windows engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39089791043/artifacts/12121853934) and [test evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39089791043/artifacts/12121759678), retained until December 17, 2026. Only the synthetic preview frame is packaged; original raster and palette bytes stay on the runner. The `37700` compressed-stream stop condition is not yet reconstructed and is supplied by a bounded host callback. R04 is **not accepted**.
