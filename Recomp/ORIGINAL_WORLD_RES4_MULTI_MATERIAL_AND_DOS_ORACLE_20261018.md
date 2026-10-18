# Part 21 — Original multi-material world compositor and independent DOS pixel oracle

**Branch:** `recomp` · **State:** native original-source world rendering proven against authenticated owner media; **independent original-DOS pixel parity NOT established**.

## What executes

`Recomp/src/code2_original_world_frame.cpp` runs the original `181B4` DOS file-backed LEVELS.DAT loader, `03838` SINTAB initializer, source-specified `13C88` Level 11 transition camera (tile 2,65; sub-tile `0x8000`; 270°), `334C4` view preparation, `32954` ray traversal, `335DC` pass selection, `3263C` shading/masks/dispatch, and `32620` generated-SINTAB scaling callbacks. It calls the unmodified original `2F29C` → `2ECB4` → `37590` → `37700` resource chain for every distinct RES4 material and caches the resulting decoded type-6 allocations in guest memory.

The adapter preflights the exact original ray records (no fabricated wall materials): first-pass entries with `record+0x0C==1`, followed by all non-`0x93` second-pass materials. For each distinct item it validates the source EH6 record, observes exactly one original load, verifies the original nine-byte item-cache state and decoder result, checks allocation/length bounds (4,096 or 8,192 texels), and re-invokes original `2F29C` to verify a read/seek-free cache hit. Every SINTAB memory read is checked against the bounded allocation for the material actually selected in guest `C700`. Unknown materials, unsupported image formats, invalid generated opcodes, out-of-range samples, and invalid VGA addresses fail closed.

The original `338C8` remains available on the actual fallback branch, but the verified Level 11 first-pass short-extent scene uses the source `32620`/SINTAB generated rasterizer. It is **incorrect** to claim `338C8` executed for that scene or to force the original data into its branch.

## Verified authentic-media output

[Dedicated Linux source-world action #39102551268](https://github.com/Hyperbog/RLOFT/actions/runs/39102551268) passed with:

| Signal | Observed |
|---|---:|
| Rays | 256 |
| Eligible first-pass material IDs | 48, 53, 59 (6, 2, 1 rays) |
| Distinct RES4 type-6 materials loaded by original source | 18 |
| Original resource archive reads / source type-6 loads | 112 / 18 |
| `335DC/3263C` flushes / `32620` callbacks | 263 / 263 |
| Generated-SINTAB raster VGA writes | 3,218 |
| Nonzero indexed scanout bytes | 3,188 |
| Palette size / indexed frame size | 768 / 128,000 bytes |
| Source-view frame SHA-256 | `a7d482ac9478b15af9c185e8eed0c96be43968755a79b5f32c9d4876dbebaade` |
| Native DAC RGB6 SHA-256 | `1f60d7831cd7b034ae67c894dc91662f15d4341fbf28e2a954b1b7e3e780f64f` |

The source output is only a native diagnostic, not an independent original-DOS capture. The indexed file and BMP and palette are erased on the hosted CI runner rather than published with original textures. Windows regression and the full canonical-source regression retain separate green status. See the latest CI for the post-checkpoint C700 material ownership assertion.

## Independently capturing and comparing an original DOS frame

**No independently observed DOS original-game Level 11 indexed frame is present in the repository or in the current original-file branch.** The prior DOS oracle protocol explicitly states its reference capture has not been performed. Do not compare a native-to-native frame, emulator screenshot resized by a host desktop, or a CI hash against itself and label the result DOS parity.

1. Privately run an independently authenticated original DOS game launcher in an instrumented DOS emulator/real DOS; record original executable hashes, emulator version, configuration, capture date, actual level/camera (Level 11; x=2.5, y=65.5; 270°), game state and frame number. Stop after the original frame is fully drawn; record page/split-line and CRTC start-address state.
2. Capture raw VGA planar video and DAC register values **from that original guest session**, not from native C++. Assemble 320×400 row-major indices using `index[y*320+x] = vga_plane[x%4][y*80+x//4]`, adjusted for the *observed* original VGA display start/page state. Save exactly 128,000 `original-dos-level11.idx` bytes and 768 `original-dos-level11.pal` bytes (256 RGB6 triplets, R/G/B, values 0..63). Retain raw guest debugger evidence privately.
3. Run the source-world tool locally on the owner's exact retail files:

```sh
rloft_original_code2_world_frame --world /owner/LEVELS.DAT /owner/SINTAB /owner/RES4 /owner/COLORS /private/level11-source
python3 Recomp/tools/compare_dos_indexed_frames.py \
  --dos-index /private/original-dos-level11.idx \
  --native-index /private/level11-source.idx \
  --dos-palette /private/original-dos-level11.pal \
  --native-palette /private/level11-source.pal \
  --report /private/dos-vs-recomp-level11.json
```

The comparator rejects incorrect byte lengths, non-RGB6 palettes, and any unequal pixel or palette byte, identifies the first differing coordinate/value, and returns nonzero for mismatches. Its JSON receipt contains only numerical mismatch evidence and hashes; **the code does not authenticate the provenance of the supplied DOS capture**. Actual capture provenance and alignment of emulator video state require independent review. Exact equality to an owner-supplied file alone is not DOS game parity.

The comparator has six independent original-free unit tests (exact equality, index mismatch, palette mismatch, bad sizes, invalid RGB6, missing palette pair), all green in [Linux #39102551268](https://github.com/Hyperbog/RLOFT/actions/runs/39102551268). No reference frame has been supplied, so **actual original-DOS-to-native byte comparison remains open**.

## Next acceptance boundaries

- Obtain independent original DOS frame(s), metadata and raw VGA/DAC evidence for the same camera/frame and compare strictly. Diagnose differing indices or DAC values instead of weakening the matcher.
- Compare additional original camera positions and lighting changes, naturally exercising the `338C8` fallback, then original occlusion and sprite/HUD overlays.
- Execute continuous CODE.1→CODE.2 original gameplay boot; separately satisfy R04 protected-mode original DOS oracle and remaining acceptance gates.
