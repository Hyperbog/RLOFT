# Original 1F120 / 1F8FC: one complete retained source frame and retail DOS DAC agreement

Date: 2026-10-24. Branch: `Recomp`. Scope: reconstructed original CODE.2 running inside the retained Level 22 guest; no substitute renderer.

## Accepted source changes and workflows

- [460d73144729](https://github.com/Hyperbog/RLOFT/commit/460d73144729cc8cbbe279dc19b391401cebae55): authentic `1F120` actor census, sound mode, sound selection, volume, tempo and original random-state callbacks. Audio hardware branches still fail closed if an actual device is active.
- [fc2cd089aada](https://github.com/Hyperbog/RLOFT/commit/fc2cd089aada293c9314dc24284a7ec8e2db24c8): original `1F8FC` reloads original `COLORS` **bank 3**, matching live `C034`; original `325B0` independently verifies all 768 bytes against retail `COLORS` and source `29234` streams the VGA DAC.
- [a92ce75bbd0c](https://github.com/Hyperbog/RLOFT/commit/a92ce75bbd0ca494da16c6f4011eb5fb1ae3f21f): Windows acceptance demands a source frame return, palette/DAC execution, one `1A5B0` present and page flip.
- [ce65d9f1e336](https://github.com/Hyperbog/RLOFT/commit/ce65d9f1e33637252ad2c56306b080ae585d5004): `1FC14 → 30B40` keyboard reentry recognizes the *actual* incremented `325B0` and `29234` call counters while preserving all other startup prerequisites.
- [28439da48866](https://github.com/Hyperbog/RLOFT/commit/28439da48866cf910e9ddb1cbd5edebfbfa5c84b): hash the hardware-selected 320×400 indexed scanout and 768 original DAC RGB6 bytes without publishing art.
- [3e31da9af888](https://github.com/Hyperbog/RLOFT/commit/3e31da9af8884f26dfd2e75a46c090462515da35): Windows CI requires the full independent retail DOSBox-X DAC fingerprint, not merely a syntactically valid hash.

**Final hardened Windows original-media acceptance:** [run 40056789148](https://github.com/Hyperbog/RLOFT/actions/runs/40056789148), **success**. The earlier [run 40056636847](https://github.com/Hyperbog/RLOFT/actions/runs/40056636847) also passed for the fingerprint-instrumented source. Linux regression [40056618503](https://github.com/Hyperbog/RLOFT/actions/runs/40056618503) and canonical source-link [40056618502](https://github.com/Hyperbog/RLOFT/actions/runs/40056618502) passed.

## Executed original source chain

`1F8FC → 1F494 → 1F1C8 → 34DE8 → 33E6C → 34110 → 1BDAC → 2C294 → 335DC → 36CEC → 274A4 → 273F0 → 1F120 → 325B0(bank 3) → 29234 → 1A5B0 → 1F8FC return → 1FC14 keyboard poll`.

Accepted Windows Level 22 frame: 37 exterior sprite draws, 8 polygon quads, 212 original polygon scanline draws, 175 original RES3 sprite calls with 41,231 sprite pixel writes, 2 original compass RES0 draws and 280 direct compass VGA writes, 1 actor update, 1 scene return, 1 compass return, 1 `1F120` actor census, **one source `1A5B0` presentation**, **one source compositor return**, flipped output page, and resumed source `30B40` keyboard polling.

## Exact presented VGA evidence

The original `OriginalVgaHost::scanout_indices()` reads the actual selected CRTC scanout from all four retained planar VGA planes, including CRTC start and line compare (not a rendered substitute or BMP).

- Indexed 320×400 (128,000 bytes) SHA256:
  `b9649e13444cbdef42406a5077177ff0e15bb89325269067df219fbfbd85ef01`.
- Live 256×RGB6 (768 bytes) DAC SHA256:
  `0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812`.
- Source scanout CRTC start: `32768` (`0x8000`).
- Data are identified as source-reconstruction hardware state; pixel/DAC payload is not uploaded to GitHub or printed.

**Independent retail DOS oracle:** [SECOND_DOSBOXX_CAPTURE_COLORS_BANK3_DAC_RECONCILIATION_20261019.md](SECOND_DOSBOXX_CAPTURE_COLORS_BANK3_DAC_RECONCILIATION_20261019.md) recovered the exact same complete RGB6 DAC SHA from actual DOSBox-X 2026.10.01 VGA state, with retail CRTC start also `0x8000`. Thus **all 768 RGB6 palette bytes agree with the independent retail captured DAC**, and the CRTC page start agrees. This is a genuine and independently sourced VGA palette parity milestone.

## Explicitly not yet accepted

- **Only one** original composed 1F8FC frame has been observed, not two or more consecutive frames following a user movement event. The retained original `1FC14` keyboard path reenters successfully. Separate virtual key tests have exercised `0x148` forward and `0x14B` turn-left on source guest records and toggled the page, but explicitly label physical input and resulting scene render `NOT_CONNECTED`. Do not count those as a second presented gameplay frame.
- **Indexed framebuffer DOS parity remains OPEN.** The earlier independently captured DOSBox-X screenshot and `1.sav` came from mismatched animation/window states and a different private Level 5 scene. They cannot be compared to this Level 22 frame, and no synchronized Level 22 original indexed capture is present. A non-equal hash across different scenes or ticks would be meaningless.
- A strict index/DAC byte comparator already exists: `Recomp/tools/compare_dos_indexed_frames.py`. It requires actual `--dos-index` and `--native-index` files; pass both palettes to establish full pixel/DAC equality. Independent DOS provenance and exact save, camera, animation tick and CRTC state still must be established.
- Visual screenshot parity, input-driven multi-frame animation, and retail gameplay acceptance remain unverified.

## Next source frontier

1. Connect the original `1FC14` per-tick game-loop callbacks (including authentic `1F1C8` drawing and mouse/keyboard input), on the **same retained machine**, rather than assuming the one-shot `1F8FC` diagnostic is the full input-driven loop. Count and hash two consecutive authentically page-flipped frames.
2. Capture the same-paused-frame original DOSBox-X 320×400 indices and full RGB6 DAC for the **same Level 22 state and animation phase**, and invoke the strict comparator; do not use an unsynchronized screenshot as pixel parity evidence.
3. Keep Windows CI fail-closed and require source frame return, actual VGA scanout fingerprint, independent DAC hash, and next authentic input frontier.
