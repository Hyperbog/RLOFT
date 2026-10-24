# Part 35 — Private indexed VGA export accepted; original 1FC14 input-driven scene frontier remains open

**Branch:** `Recomp`. **Date:** 2026-10-24. Scope: reconstructed original CODE.2, retained Level 22 guest; no native substitute renderer.

## Accepted private original-source framebuffer export

- [bca189b9bd57](https://github.com/Hyperbog/RLOFT/commit/bca189b9bd57ebdc262a65ecbb30063cc9466451): explicitly opt-in raw output of the current selected hardware VGA scanout as `<stem>.idx` (320×400, 128,000 bytes) and the entire RGB6 DAC as `<stem>.pal` (256×3, 768 bytes). The framebuffer uses `OriginalVgaHost::scanout_indices`, which reads original planar VGA state at the selected CRTC offset, including line compare. The DAC comes from the actual port-programmed `OriginalVgaHost`, not from fabricated native art.
- [633036f00633](https://github.com/Hyperbog/RLOFT/commit/633036f00633e01da7dbedb56f966fb0f7bea455): close the output stream before any fail-closed source stop so no stream destructor is skipped by guest longjmp.
- [c30393c70088](https://github.com/Hyperbog/RLOFT/commit/c30393c70088582208027b6beb341628e0e18a18): Windows CI privately exports both files in the runner's temporary directory; computes fresh SHA-256 hashes and lengths independently of the original source's log receipt; compares them exactly; **deletes the raw files without uploading them**. The fixture requires one completed source `1F8FC` frame and authentic `1A5B0` presentation.

**Windows original-media acceptance:** [run 40057878105](https://github.com/Hyperbog/RLOFT/actions/runs/40057878105), **success**, including:

`ORIGINAL_1F8FC_PRIVATE_FRAME_EXPORT_PASS frame_bytes=128000 dac_bytes=768 raw_media=DELETED_FROM_RUNNER`

Portable C++ regression [40057854773](https://github.com/Hyperbog/RLOFT/actions/runs/40057854773), **success**, and canonical source-link [40057854769](https://github.com/Hyperbog/RLOFT/actions/runs/40057854769), **success**.

Private export is **disabled unless** `RLOFT_RECOMP_PRIVATE_FRAME_STEM` is set to a nonempty local path. Example for a developer environment: `RLOFT_RECOMP_PRIVATE_FRAME_STEM=C:\private\rloft_level22`. The startup executable must otherwise be run with the established Level 22 original-media and complete-compositor options from the Windows workflow. When export is enabled it produces `rloft_level22.idx` and `rloft_level22.pal` beside the chosen stem. Treat both files as private.

## Observed hardware fingerprint

- Recompiled source selected indexed scanout SHA-256: `b9649e13444cbdef42406a5077177ff0e15bb89325269067df219fbfbd85ef01`.
- Recompiled source full RGB6 DAC SHA-256: `0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812`.
- Selected CRTC start: `32768` (`0x8000`).
- The full DAC hash is **byte-for-byte equal** to the independently preserved original DOSBox-X 2026.10.01 hardware DAC hash in [SECOND_DOSBOXX_CAPTURE_COLORS_BANK3_DAC_RECONCILIATION_20261019.md](SECOND_DOSBOXX_CAPTURE_COLORS_BANK3_DAC_RECONCILIATION_20261019.md). This is a genuine retail palette parity result, but not indexed framebuffer parity.

## Exact DOS indexed comparator already present

```sh
python Recomp/tools/compare_dos_indexed_frames.py --dos-index original-level22.idx --native-index rloft_level22.idx --dos-palette original-level22.pal --native-palette rloft_level22.pal --report level22-comparison.json
```

It reports exact mismatched indices, row counts and first pixel, plus all DAC channel differences, and fails if nonidentical. The original files must be an independently authenticated, synchronized DOSBox-X or real-DOS capture of **the same Level 22 camera, sprite-animation phase, page, and input tick**. The historical retail DOSBox-X screenshot and `1.sav` were temporally mismatched and are not suitable reference bytes for this new source frame. There is no synchronized original Level 22 indexed frame in this project context. **Do not report an indexed-pixel match or mismatch yet.**

## Next authentic input-loop integration

The original `1FC14` runtime consumes source virtual scan codes on its original guest stack. The newly explored per-tick `1A510` callback is gated behind `RLOFT_RECOMP_EXPERIMENTAL_1FC14_PAGE_HANDOFF=1`, off by default, as documented in [ORIGINAL_1FC14_INPUT_LOOP_1A510_CURSOR_FRONTIER_20261024.md](ORIGINAL_1FC14_INPUT_LOOP_1A510_CURSOR_FRONTIER_20261024.md). Two opt-in source runs proved original `29804` reaches a cursor clip boundary but cannot yet distinguish a fully culled original draw from unsupported raster access. First collect `function_29804`'s actual return code, computed clip record, and VGA invalid flags. Only after the original cursor/page handoff passes should the persistent `Application43/44` scene callbacks be attached to the genuine `1FC14` loop and two successive input-driven source scenes be accepted and hashed.

**Acceptance accounting:** 1 retained original composed frame; 1 original `1A5B0` presentation; private raw VGA/DAC export verified; exact independent retail RGB6 DAC parity; input-driven consecutive full frames **not yet accepted**; synchronized DOS index parity **not yet measured**.
