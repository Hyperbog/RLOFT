# Independent Native Engine — first original GOG asset-loader field test

**Recorded:** 2026-10-11.
**Tested compiled native source SHA:** `fd314b89e96db07c739176d25f1fc9be94ecaa19` (Windows x64/MSVC asset preview).
**External evidence:** user-provided screenshot of the newly built `rloft_native.exe` after opening their actual original GOG game installation. The image and local absolute path are not committed, for personal-path privacy and copyrighted-artwork reasons.
**CI build:** [#38164976411](https://github.com/Hyperbog/RLOFT/actions/runs/38164976411), 9/9 tests passed, [build report](WINDOWS_NATIVE_ASSETS_VERIFIED_BUILD_20261011.md).

## Direct observations from screenshot

| Original-media property | Observed |
| --- | --- |
| Source layout | GOG `game.gog` original CD image |
| Independent ISO parser | Detected **2048-byte sectors** |
| Indexed ISO files | **69** |
| Known resources structurally validated | **19** |
| Known resources missing | **0** |
| Known resources structurally invalid | **0** |
| Native 256-color display | Original **COLORS palette 12** visible |

Selected observed asset figures (do not infer content parity from this summary):

| Asset | Byte size | RES record count |
| --- | ---: | ---: |
| INIT | 1,879 | — |
| RES0 | 2,186,991 | 141 |
| RES1 | 354,310 | 428 |
| RES2 | 63,736 | 8 |
| RES3 | 5,228,638 | 4,234 |
| RES4 | 1,975,542 | 667 |
| RES5 | 1,510,960 | 98 |
| RES6 | 621,089 | 266 |
| RES7 | 211,133 | 62 |
| RES8 | 58,772 | 73 |
| RES10 | 46,060,354 | 219 |
| COLORS | 11,521 | — |
| LEVELS.DAT | 499,878 | — |
| OBJECTS.DAT | 21,844 | — |
| NPCS.DAT | 5,731 | — |
| SINTAB | 33,757 | — |
| CINE00.ANM | 1,378,193 | — |
| CINE12.ANM | 1,446,666 | — |

T was also present/validated at 133,584 bytes. All 19 original resource labels displayed `[OK]`.

## Accepted gates

- **PASS observed by user:** independent native Windows app launches, user picks owned GOG installation and the *new original asset reader* opens `game.gog` (not the old decomp demo/old inherited-parser bootstrap).
- **PASS observed by user:** parser recognizes ISO2048 original media and indexes 69 CD files.
- **PASS observed by user:** all 19 recognized required/candidate assets present and satisfy the **implemented structural checks**, including RES entry tables and original 15-bank COLORS DAC6 range.
- **PASS observed by user:** actual source palette12 renders visibly in the independently developed native desktop.
- **No game-media extraction** and no original-file mutation is part of this reader's designed execution path.

## Outstanding gates and precise boundaries

- **Not yet shown:** independently hashed original game.gog before/after; screenshot alone does not prove no other process touched the file.
- **Not tested by this image:** actual contents of every RES record, EH raster decoding, animations, ANM, original DAT semantics, world geometry, save compatibility, renderer parity, or gameplay.
- **No claim of exact DOS palette-frame matching:** only the original palette's 256 decoded colors are shown.
- The decomp demo remains a distinct full recompilation/reference project. Native engine must never link its runtime.
- The absolute GOG install path is intentionally omitted here.

**Next:** implement an independent native EH type-1 raster decoder with strict bounds + LZSS tests; use the original `INIT`→`RES0` default-cursor selection as a read-only installed-media visual proof. Continue to load original assets directly, with optional engine-owned caches only when justified.
