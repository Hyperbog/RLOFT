# Part 25 checkpoint — 20440 original RES0 type-1 image and 2F184 retention

**Owner:** `Recomp`. **Validated original-media run:** https://github.com/Hyperbog/RLOFT/actions/runs/39707502111
**Status:** original source `20440 → 2ECB4 → 2FD9C → 2F184` verified on the retained, unmodified guest stack for the default existing-CHARS Level-22 startup. **Mouse/cursor source callbacks and complete world frame are NOT connected.**

## Original record identity and decode

Source `20440` reads `DGROUP[0x258DC]+0x2C`; in the original owner media this selects **RES0 record 59** from a 141-entry slot-0 catalogue. Indexed archive entry offset 222102 contains the genuine `EH` envelope, type **1**, typed descriptor length **8**. The entry is not preloaded before `20440`. Record 59's header describes **14 frames**, **one row**, **29 width units**, and compression mode **3** (original LZSS), with 3,248 bytes of independently decoded raster data. Original source `2EB60`, `2ECB4`, `2FD9C`, `2F96C`, `37700`, `2FA30`, `2A20C`, `37810`, and Watcom heap callbacks execute against a single persistent `CODE.2` Machine, same indexed RES stream and original file bytes. After loading the source `2F184` sets cache flag `0x10` on the nonzero decoded image.

`Recomp/src/code2_original_res_startup.cpp` connects only the typed decoder known to be required here; it does not install null decoders for other types or mark unloaded records as loaded. Stream checks reject invalid envelope, unexpected type/method, out-of-bounds reads, and failure to consume the expected entry. There is no synthetic asset byte insertion. The isolated `Code2/graphics/original_res_image_archive.hpp` independently decompresses original RES0:59 without writing guest memory; a separate manual packed-plane and `37810` permutation oracle constructs the expected image. All 3,248 decompressed bytes and the derived packed guest image match the actual `2FD9C` result.

### Source and CI evidence

- `25725b8`: original `2ECB4` authentic loader + `2F184` retention + persistent Level-22 and keyboard-loop path, Windows successful, 85/85 CTests.
- `2eca826`: CI adds mandatory independent source-vs-archive image-byte proof.
- `58112e7`: handles original **frame permutation**, corrected after the first no-permutation parity assertion failed. [Windows #39707502111](https://github.com/Hyperbog/RLOFT/actions/runs/39707502111) **passed**, including `ORIGINAL_CODE2_20440_RES0_SOURCE_ACCEPTED` and 85/85 CTests.
- Exact accepted evidence: `ORIGINAL_CODE2_20440_RES0_TYPE1_DECODE_PARITY_PASS entry=59 source=2ECB4_2FD9C byte_count=3248 frames=14 method=3 all_source_planar_bytes=MATCH retention_2F184=YES guest_memory=PRESERVED`.

## Original mouse/cursor dependency — independently identified

The original-media Windows gate [#39707725281](https://github.com/Hyperbog/RLOFT/actions/runs/39707725281) **passed** all 85 CTests and the mandatory RES0 59 source parity. Its read-only live-guest probe, after real `20440` loading, reports:

```
ORIGINAL_CODE2_20440_CURSOR_DEPENDENCY_DISCOVERY source_mouse_2E340=65535 cursor_item=62 archive=RES0 cursor_type=1 cursor_header_bytes=8 source_1A0B0=NOT_YET_CONNECTED source_1A288=NOT_YET_CONNECTED
```

The actual source mouse-present word is **0xFFFF**, so the `1A0B0` present-mouse branch MUST be implemented; treating it as a no-op or taking the absent-mouse branch is not correct. Original cursor image is **RES0 entry 62, EH type-1 descriptor length 8**, and its source loader can reuse the newly verified typed decoder with a separately bounded entry/stream. That cursor image is not yet decoded on the current startup worker. Original `33D38` Y range, `33D18` X range, `33C90` function-4 pointer positioning, `33CB0` function-3 button/coordinates and `1A064` DOS/DPMI callback installation require real host services in this source order; the current bounded mouse-range service does not support the function-3/4 calls or the updated Y range. Do not make up their return values. **Source 1A0B0 and 1A288 remain unconnected.**

## Strict next original source frontier

1. Inspect original `2E340` guest mouse-presence state and the actual `0x258DC+0x2F` cursor-image archive record (a diagnostic is now in the original-media test). Do **not** substitute a mouse-present value.
2. Bind `1A0B0` with its actual `33D38/33D18` range, `33C90` pointer positioning, `33CB0` guest state, `2F29C` cursor resource access, `30CD4` copy, and `1A064` Int33 callback install. These must be implemented before accepting a source-present mouse branch.
3. Execute `1A288` source page-stack and cursor rectangle/copy drawing with retained VGA; verify its original count/state and non-fabricated cursor bytes. If no authentic input hardware service exists, suspend the retained guest at the exact unmet callback and do not claim a displayed cursor.
4. Finish all `181B4` level-transition adjuncts (`171FC`, palette/scenes via `1F8E0`, actor/event processing, audio and HUD) and the actual `1FC14` scene renderer and keyboard/mouse interaction. A Level-22 packed-grid match is necessary but not sufficient for world initialization.

**Not accepted:** mouse/cursor initialization, nondefault character-creation startup, input-controlled world frames, complete DOS replay, retail pixel parity, or a playable native executable. Original commercial media, raster bytes and captures remain private/read-only.
