# Retained Level 22 RES1 / ray / RES4 CI acceptance checkpoint (2026-10-22)

## Current source status

The `Recomp` branch already includes, in its real retained guest (not only independent fixture execution):

- Original RES1 `2F29C -> 2ECB4 -> 2FD9C` decoder and `298F0 -> 2A584` guest-table portrait raster. Authentic Level 22 portrait identifiers **338 and 362**, two active portrait slots, verified **1273 changed VGA indexed pixels** on the DRAW page in original-media run [39870651235](https://github.com/Hyperbog/RLOFT/actions/runs/39870651235). This is a **bounded original-media diagnostic**, not a complete `1F494` source party-HUD pass.
- Original `334C4 -> 32954` ray-pass on retained Level 22 camera, 256 original rays, heading 270, 450 world cells. Original `335DC -> 3263C` raster traversal is wired as a source **preview**, independently of the original `1F8FC` scene compositor.
- Original `37590` RES4 type-6 decoder loads authentic texture data into guest memory through the original resource cache. Bytes are compared against an independent decoder of the same original media. **Level 22's observed exterior-view material request set contains no textured RES4 wall**, so a real decoded texture witness is **not evidence of a rendered wall at Level 22**.

The authentic media Windows trace on latest prior source commit `8afda530414f143d71d21687b15bc3ede478727c` passed [Windows 39870651235](https://github.com/Hyperbog/RLOFT/actions/runs/39870651235), [Linux 39870651323](https://github.com/Hyperbog/RLOFT/actions/runs/39870651323) and [canonical source-link 39870651279](https://github.com/Hyperbog/RLOFT/actions/runs/39870651279).

## New acceptance gate

Commit `d7b1915b5763a5ea03500f07b2a10907bb014bfd` strengthens `.github/workflows/recomp-milestone-windows.yml` by requiring all four authentic runtime traces:

1. Two real RES1 portraits with strictly positive changed indexed pixels, in the retained CODE.2 guest, with full `1F494` explicitly still pending.
2. All 256 `334C4/32954` ray columns, verified heading 270 and Level 22 grid count 450.
3. Authentic `37590` type-six RES4 texture-byte equivalence to an independent original archive decoder, explicitly recognizing that Level 22 did not request a textured wall.
4. Original `335DC/3263C` retained SINTAB/VGA preview with an explicit **PREVIEW_ONLY_NOT_ORIGINAL_1F8FC** status.

These requirements distinguish source execution, independent pixel equivalence, and final screen completion. Do not replace these witnesses with source-level mock callbacks.

## Next real functional integration

Connect original `1F494` full actor HUD including `22F40`, `1EA90`, all masked layers and bars; integrate outdoor `1F1C8` backdrop/foreground callbacks `33E28`, `33DE0`, `34DE8`, `1BDAC`, `2C294`, and `274A4` on the same physical VGA guest. This must precede enabling `181B4.call_1f8e0` and `1F8FC` full scene composition and input-driven framebuffer acceptance. For actual textured wall parity, use a separately verified interior original level whose `335DC` material requests are nonempty, rather than introducing fictitious RES4 walls into Level 22.

**Status:** Full scene compositor, input-driven VGA frames, and independent DOS screenshot parity are NOT accepted.
