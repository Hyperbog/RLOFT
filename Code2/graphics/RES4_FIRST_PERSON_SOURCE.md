# CODE.2 source graphics: 37700 stop-control recovery, startup resource slots and RES4 column rendering

**Workstream:** Recomp (runnable adapter) with source-only facts synchronized to Decomp. **Scope:** original 37700 control semantics, resource-table initializer 2E7F0, RES4 type-6 import and compiled fixed-point 338C8 raster. **Not:** full DOS game boot, world camera/ray geometry, complete authentic first-person view or DOS screenshot fidelity.

## Original executable evidence

Owner-supplied original `CODE.2`: 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Object-1 spans are relative to physical file offset `0x3AF24`; each SHA below was independently checked against those exact immutable bytes and is included in [the 26-span verifier](verify_original_vga.py).

| Original object-1 span | SHA-256 | Finding |
| --- | --- | --- |
| `37700..3780D` | `61c42dd14fd8db7a22e439ff433ed851343e160d38b1b0d88ac5548ecc7f9915` | Decoder tests **global guest byte [2B6C2h]** at `3772E`, `3774D`, `37785`, `37797`. It is not a static function parameter. |
| `2E7F0..2E9CB` | `b762134b4bc2d7e4d1c3da2ea9db97f303235212e4c7c1dc55e096baee7bb7c7` | Original slot initializer allocates 16-byte archive records and nine-byte *unloaded* per-item cache records using callback-provided read-only file handles. |
| `37590..375F0` | `47c69a86484cfb139763bc18bee1fe72cfba523ddf58a9abd7fbbff3464f1441` | Original generic type-6/7 loader; its reconstructed CODE.2 implementation now executes with original stream callbacks and canonical 37700. |
| `33647..33665` | `d1e0b75e4cf429c81f6a4294f6266c4ee508d609231b28355296438fe518d226` | First-person renderer uses archive 4, selects decoded body plus 128 bytes per source column. |
| `338C8..3395A` | `80e61b5265ca7c9512332895079f07b02ab7e8dee610ffc6b000173cc76946d7` | Actual compiled 128-texel fixed-point vertical source sampler with shade subtraction, 80-byte-per-plane target stride and 256-row clipping. |
| `3395C..339A2` | `7bccc9fd1f104e0d2cbae0fb041c2df398e1f8708919aeab0159ad582576c6dd` | Actual compiled VGA sequencer map-mask and related guest memory helper. |

## 37700 correction

The original assembly `3772E` compares `byte [2B6C2h]` with `1`, jumping to `3780A` when complete. The checks repeat before a literal, before the first backreference input byte, and before the second backreference input byte. **Original `2FA30` changes the same guest byte when it has collected the declared image output**, including the final frame.

The [instruction-derived decoder](original_37700_control.hpp) now executes those four stop checks against a live guest callback and preserves flag-bit order, the 4,096-byte ring, the first `0xFEE` bytes initialized to `0xFE`, the initially undefined last 18 bytes, inclusive backreference length `(hi & 15)+3`, and the behavior where the rest of a backreference is copied after the output callback asserts stop. Safe byte/output and unknown-dictionary limits are **host additions**, not DOS semantic claims.

The Recomp [graphics source harness](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/code2_compiled_graphics_frame.cpp) calls this control recovery using CODE.2's original buffered byte input (`2F9E4`), original output function (`2FA30`) and guest stop byte `2B6C2`. It replaces the former count-only decompression callback. The old static stop-argument function has now been replaced **in canonical Code2.cpp** by `function_37700(Machine&,Runtime126Boundary&,u32 output_limit)`. The operational graphics adapter calls this canonical implementation, using the recovered control helper.

## Resource initialization and current startup boundary

For the graphics integration, source `2E7F0` now opens an owner-provided archive via a **read-only virtual DOS-open callback**, reads the resource count, allocates guest archive records and initializes original unloaded item tables; `2ECB4` fills a cache on first render, and `2F29C` then hits the same source cache without a second seek. These are **actual compiled source calls**, not manually prepopulated guest records.

The per-image graphics harness still repeats one archive for earlier placeholder slots. A separate original-source startup gate now executes `16D04` INIT, `2E768` INFO.DAT and `2E7F0` to initialize all eleven active RES0..RES10 archives from bounded installed and CD read-only roots; the Windows authentic-file run passed. It is a **subsystem boot**, not the CODE.1-to-CODE.2 continuous game entry, full INIT scene parsing or gameplay main loop.

## Authentic RES4 type-6 structure and rendering

The preserved `RES4` file has 667 type-6 records (1,975,542 bytes, SHA-256 `d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46`). Authentic entries 0, 100 and 500 all begin with **EH type 6, typed-header size zero, then an independent 16-bit decoded-size word 8192**. They are not type-1 sprites and the decoded-size word is not part of the EH typed-header.

The shared [bounded read-only type-6 importer](original_res_type6.hpp) parses the actual entry offsets, checks header size **0** (corrected after authentic test failure), and decompresses exact 4,096/8,192 byte column-major bodies (32/64 columns, each 128 bytes). Original CODE.2 `2ECB4` selects its type-6 branch, and **compiled original 37590** invokes source `2F960`/`2F96C`/`2FB04`/corrected canonical `37700` to populate the original nine-byte cache. The source lookup `2F29C` returns the decoded allocation.

Recomp then invokes **compiled CODE.2 `3395C`** to select the VGA plane and **compiled `338C8`** to resample 128-texel source columns to an output height, retaining the source subtraction for shading. Because reconstructed `338C8` currently writes directly to guest A0000 shadow memory, a bounded adapter mirrors only that column's indexed bytes into the host VGA planes. The deterministic sample phase is zero; source stack residue on the actual DOS path is unproven. The preview displays a flat strip of source columns, **not** walls projected from Level geometry/camera.

Run with owned media:

```powershell
.\rloft_original_code2_graphics_frame.exe --res4 "C:\Games\RLOFT\RES4" 100 128 0 "C:\Temp\RES4-preview" "C:\Games\RLOFT\COLORS" 0 0
```

The output is an owner-local 320×400 raw-index framebuffer (`.idx`) and palettized 8-bit BMP (`.bmp`). Do not publish derived copyrighted pixels.

## Validation

[Linux](https://github.com/Hyperbog/RLOFT/actions/runs/39091993852) passed 70/70 CTests after correcting the typed-header fixture. [Windows/MSVC](https://github.com/Hyperbog/RLOFT/actions/runs/39092086421) passed **70/70** and additionally validated real RES4:0/100/500 and RES0:15/RES1:336 using original `COLORS`. Original raw RES and generated textures remained in temporary CI directories, and **no retail files or decoded images are in public artifacts**.

| RES4 resource | Original encoded material | Real-source, indexed frame SHA-256 | Nonzero pixels |
| --- | --- | --- | ---: |
| `RES4:0` | 64 × 128 texture | `93bd84ded6650ac000919e772435a83adff7f4c888330b81fe79b850edd2a975` | 8,192 |
| `RES4:100` | 64 × 128 texture | `e8485ecae052f49ab511465bc7cdf7c6131121ebb1ac7b416ccac90a6c3d69bf` | 8,192 |
| `RES4:500` | 64 × 128 texture | `5280d7919b6848dc56ce19aa0ce23220ea1f15b991c6335285b30628dd6370b6` | 8,192 |

**Additional 32-column original resource verified:** [Windows/MSVC milestone #39092562187](https://github.com/Hyperbog/RLOFT/actions/runs/39092562187) passed **70/70 CTests** and identified `RES4:600` as a real **32×128** image (decoded size 4096), rendering **4,096** nonzero indexed pixels. Its original-frame SHA-256 was `74cad7087f39fd4229c9a4f451529d3ba85c79d4ce2433694dfe09d1e1d68ed1`. This proves both the 32-column and 64-column shipped RES4 families reach the compiled `338C8` path; no original art was packaged. [Engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39092562187/artifacts/12123004192) and [JUnit/evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39092562187/artifacts/12123262884) expire December 17, 2026.

The first authentic RES4 Windows run failed because the prototype incorrectly demanded EH typed-header size `2`; actual media uses `0`. The parser and synthetic fixture were corrected, and the subsequent real-media test passed.

## Remaining milestones

1. Connect the already-tested CODE.2 resource-subsystem startup and remaining INIT tables to the actual CODE.1-to-CODE.2 game handoff; it is not yet a full game boot.
2. Drive source `334C4`/`32954` camera and grid sweep with real LEVELS and game state, instead of synthetic sweep records for `335DC` → `3263C` → `338C8`.
3. Complete authentic wall occlusion, near-wall SINTAB rasterization, sprites and HUD compositor; compare full frames against DOS reference captures.
4. Test actual original DOS screenshot parity, CRTC timings, VGA reads and game presentation before accepting full video functionality or changing R04.
