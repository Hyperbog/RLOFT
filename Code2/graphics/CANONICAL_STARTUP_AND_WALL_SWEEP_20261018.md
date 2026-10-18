# Canonical CODE.2 graphics/runtime integration — 2026-10-18

## Scope and acceptance

This checkpoint distinguishes **executed reconstructed source** from **test fixture/host adapters** and **unreached game runtime**. Three user goals: canonical `37700` LZSS, original `INIT` and eleven-archive startup, and RES4 wall geometry/composition.

**The graphics engineering executable is not a game build. R04 is unaccepted.** The full DOS/4GW CODE.1→CODE.2 gameplay startup and authentic level-render projection have not been demonstrated.

## 1 — Canonical `37700` corrected and invoked

The canonical [`Code2/Code2.cpp`](../Code2.cpp) no longer contains `function_37700(Runtime126Boundary&,u8 stop_flag)`. It exports `function_37700(Machine&,Runtime126Boundary&,u32 output_limit)`, which samples the live guest memory flag `[2B6C2h]` at the original four stop checks. A new bounded-read callback can signal EOF, and the implementation reuses the [instruction-derived dictionary/control loop](original_37700_control.hpp). Production decompression in [Recomp's CODE.2 graphics adapter](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/code2_compiled_graphics_frame.cpp) invokes **this canonical function** using original compiled `2F9E4` input and `2FA30` output; no parallel count-only decoder remains in that pathway.

Source `2FA30` changes guest flag `2B6C2` when the declared output completes. The ring is 4096 bytes, initial write offset `0xFEE`; source only initializes positions below `0xFEE` to `0xFE`. Compressed backreference loops are inclusive, even when the flag changes during a backreference; host limits reject input overrun/uninitialized byte reads. Source byte witness: `37700..3780D`, SHA-256 `61c42dd14fd8db7a22e439ff433ed851343e160d38b1b0d88ac5548ecc7f9915`.

## 2 — Actual CODE.2 INIT and multi-archive resource subsystem startup

A separately compiled [`Recomp/src/code2_original_res_startup.cpp`](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/code2_original_res_startup.cpp) invokes:
- **`16D04`**: the original `INIT` header reader for all 18 initialization words and original guest target fields, with read-only seek/read/close callbacks; an explicit expectation checks the preserved 1,879-byte file and source word values. `Application29Boundary`'s scene-descriptor and environment-profile follow-on callbacks are **not yet connected**, and are not claimed read by compiled source.
- **`2E768`**: original `INFO.DAT` load of the 13 resource flag bytes and DOS path prefix. The authentic installed INFO.DAT is 23 bytes with prefix `D:\RLOFT`, while a separately supplied source copy was 17 bytes with prefix `D:`. Both use the same flag bytes `0000000001111`. The host never uses these historical drive names as arbitrary filesystem access.
- **`2E7F0`**: original source initializes exactly **11 active slots `RES0..RES10`**, reads each archive's entry count, allocates source 16-byte archive slot entries and original nine-byte unloaded resource records, and confirms the full table is valid. Host virtual DOS handles resolve only allowlisted names into two explicitly supplied read-only directories: installed `RES0..RES8` and CD `RES9..RES10`.

This provides a *resource subsystem startup checkpoint*, not a full-game launcher. The separate reconstructed CODE.1 startup gate and CODE.2 executable-entry gate already exist, but **no CODE.1-to-CODE.2 continuous game path reaches this new resource initializer**. The correct complete boot requires connecting original handoff, game-state initialization, initial camera/world records, graphics and audio/device services.

On the owner's installation, run:

```powershell
.\rloft_original_code2_init_resources.exe --root "C:\RLOFT" --cd-root "D:\RLOFT"
```

Without arguments it tests matching synthetic `INIT`, `INFO.DAT` and eleven tiny source-format archives. Original files are never altered or added to a download.

## 3 — Original RES4 type 6 + CODE.2 two-pass compositor

The type-6 archive path now **executes actual compiled `37590`**, which reads the encoded-size word, allocates the original decoded-size+2 layout, initializes `2F960/2F96C` buffered stream, and invokes original `2FB04` → corrected `37700` → `2FA30`. The source `2ECB4` caches the resulting resource; `2F29C` handles subsequent cached lookups. No longer uses a host stand-in for `37590`.

First-person diagnostic `--res4-compositor` feeds synthetic *wall sweep records* with original source fields (depth, near/far material indices, 128-byte column selector and mask) into **compiled `335DC`**'s two-pass record coalescing, then **compiled `3263C`**'s VGA mask/shade/height compositor and **compiled `338C8`**'s vertical 128-texel resampler. A host shadow-VRAM mirror writes that selected column's bytes to the covered VGA aperture. The program checks *indexed pixel equality* against its independent direct `3395C`/`338C8` column diagnostic using identical RES4 inputs. The source's near-wall special path using self-modifying SINTAB code is not exercised in this test.

```powershell
.\rloft_original_code2_graphics_frame.exe --res4-compositor "C:\RLOFT\RES4" 600 128 0 "C:\Temp\wall-sweep" "C:\RLOFT\COLORS" 0 0
```

The result is a 320×400 palettized engineering image from authentic source texture bytes, **not a camera-projected world screenshot**. Source CODE.2 `334C4` view preparation and `32954` ray/grid sweep are mapped by original-byte witnesses but have **not** been connected to the full LEVELS/INIT camera, depth/occlusion state and scene compositor. Thus complete first-person projection, occlusion, world wall selection, overlays, sprites and HUD remain missing.

## Original-byte evidence and regression gates

[Original `CODE.2` SHA manifest](verify_original_vga.py) covers **32** original-source instruction regions including:
`16D04..17097` INIT (`41af04fcfe969f2dbccbcd830eded9c35ce6e758c4b203ac49379e34ea62fee7`);
`2E768..2E7F0` INFO.DAT (`7ab9c6d1ce7c170e33bd913d6a27cf85fe020fa13c969ad125f1ffa91ee8a5e0`);
`3263C..32952` compositor (`f13246466f26e83b31e3f6800a4a3a76a940ccc71d598ae11dfa3ce74d6782ee`);
`32954..32F4A` camera sweep (`c8f30081beebcd0bcc31d057d4a72f94abb5730a74a9d7d967720b816eb07416`);
`334C4..335DC` view preparation (`66a8a82aeffae8ff115ae4e314d766582c159ec629d6dce222dd319d03a2586d`);
`335DC..337C7` two-pass wall sweep (`f43d0ef565a18eb347dabdf4ec30d2a35afaa02fcbfe5dbe2e7747922be07f59`).

Linux and Windows both passed **71/71 CTests**. [Windows MSVC milestone #39096508171](https://github.com/Hyperbog/RLOFT/actions/runs/39096508171) also passed original installed INIT/INFO.DAT and all eleven RES0..RES10 archive source-startup gates, plus genuine RES4:0/100/500/600 indexed parity across compiled 335DC/3263C compositor and direct sampler. The installed INFO.DAT uses D:\\RLOFT for its CD path prefix; the read-only host resolves CD RES9/10 under the preserved CD_Extracted/RLOFT root. No original-game art was distributed. [Engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39096508171/artifacts/12123819892) · [evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39096508171/artifacts/12125225228). R04 remains unaccepted.

## Real level-grid source checkpoint after synthetic wall sweep

A subsequent [source-backed `LEVELS.DAT` bridge](ORIGINAL_LEVEL_VIEW_PREP_20261018.md) loads **all 32** authenticated retail level headers and their 115,584 packed cell words at the guest-memory offsets established by original `181B4`, runs compiled **`334C4`** and checks the original source clears only transient visibility bit `0x0040` before invoking the `32954` callback. [Windows 72/72 milestone #39097776940](https://github.com/Hyperbog/RLOFT/actions/runs/39097776940) validated all authentic grids and [Linux source test #39097755657](https://github.com/Hyperbog/RLOFT/actions/runs/39097755657) passed. The original `32954` ray sweep is deliberately **not executed** in this checkpoint; trig/camera arguments are synthetic and the existing compositor still uses synthetic sweep records. The next graphics acceptance task is connecting the actual `181B4` loader, authentic camera/trig state, `32954` ray generation, and resulting `335DC/3263C` wall compositor.
