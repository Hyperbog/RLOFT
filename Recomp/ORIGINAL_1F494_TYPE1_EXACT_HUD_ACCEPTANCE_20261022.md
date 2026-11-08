# Part 27 — Exact RES0/RES1 type-1 ledger and retained Level 22 HUD acceptance (2026-10-22)

Branch: `Recomp`. **ACCEPTED: original-media retained Level 22 pre-transition `1F494/1EA90` party HUD and all ten `2FD9C` type-1 resource decodes.** **NOT ACCEPTED: complete mode-four `1F8FC`, exterior `1F1C8`, actual frame presentation `1A5B0`, gameplay/DOS screenshot parity, rare alternate HUD text/group-list branches.**

## Root cause and corrected original initialization

Original `1EA90` derives RES1 graphics by reading `s16([ [255D8]+(equipped_item*0x41)+0x2D ])`. With `[255D8]=0` and equipped item 37, the load incorrectly addressed DGROUP `0x992`, where authentic CODE.2 initialized bytes are `54 41` (`TA`) = little-endian **16724**. The actual retail `16D04` calls `16810` for `NPCS.DAT` and `168C8` for `OBJECTS.DAT` before drawing; the existing retained callback hierarchy omitted those loaders.

The repaired retained second initializer executes both **compiled original** catalog loaders via `Application29 → Application28 → Application25/26`, source `312A0` read-only media, `3104C` Watcom heap, and `312EC` guest read. Independent original-media tests confirm **83 NPC records (5727 bytes)** and **336 object records (21840 bytes)** loaded into preserved guest memory, with byte-identical payloads, and item 37 / item 8 graphics 37 / 8. No host-authored replacement sprite ID is injected.

## Exact retained 2FD9C decoder sequence

The source decoder records an ordered entry **only after successful `2FD9C` return, correct resource handle and fully consumed record**. The original-media Windows probe now requires exactly these ten entries in order:

| Ordinal | Original archive | ID | Source checkpoint |
|---|---|---:|---|
| 1 | RES0 | 59 | Initial resource image |
| 2 | RES0 | 62 | Mouse cursor |
| 3 | RES1 | 338 | Normal portrait 0 |
| 4 | RES1 | 362 | Normal portrait 1 |
| 5 | RES0 | 54 | HUD |
| 6 | RES0 | 49 | HUD |
| 7 | RES1 | 37 | Equipment |
| 8 | RES1 | 34 | Equipment |
| 9 | RES0 | 60 | HUD |
| 10 | RES0 | 34 | HUD |

All ten source IMAGE allocations are checked against original archive directory/header bounds and independently decoded original bytes. The independent construction includes planar layout and the original `37810` frame permutation when applicable. Each cached IMAGE byte is compared with the independent reference. Each resource is then re-requested through compiled original `2F29C`; all **ten cached pointers are preserved and the `2FD9C` decoder count stays at ten**, not silently re-decoded.

Mandatory CI markers:
```text
ORIGINAL_CODE2_RETAINED_TYPE1_EXACT_LEDGER_PASS decodes=10 sequence=RES0_59_62_RES1_338_362_RES0_54_49_RES1_37_34_RES0_60_34 independent_bytes=ALL_MATCH cache_reuses=10 guest=RETAINED
```

The former `original_type1_decodes==4` historical fixture assertion has been replaced by exact ten-entry validation, **not** by a weak `>=4` check.

## Retained four-panel source VGA acceptance

The explicitly opted-in `RLOFT_RECOMP_EXPERIMENTAL_HUD=1` test invokes compiled `1F494` and its normal-slot `1EA90` branches on the same preserved Level 22 guest and indexed VGA device. All four original party dirty flags are set to the genuine compositor redraw precondition, checked cleared after execution, and restored. The original HP, equipment/resource lookup, 298F0/2A584 translation, 2975C/2A380 raster and high-bit terminal-frame drawing callbacks run against original media; unsupported rare text and alternate group UI still fail closed.

The exact accepted source trace from original-media Windows run [#39879605865](https://github.com/Hyperbog/RLOFT/actions/runs/39879605865):
```text
CODE2_ORIGINAL_LIVE_BOOT=1F494_1EA90_RETAINED_HUD_PROBE original_source=1F494_1EA90_22F40_298F0_2975C normal_party=2 terminal_party=2 changed_draw_page_pixels=12966 four_panels=NONZERO panel_pixels=1933,1774,5990,6189 mode=3_PRETRANSITION_ONLY frame_status=HUD_ONLY_NOT_ORIGINAL_1F8FC
```

Here **12,966** counts changed pixels in rows `0x138..399`; the four separate per-panel footprints (1,933 / 1,774 / 5,990 / 6,189) cover rows `0x113..399` and therefore need not sum to 12,966. Original terminal-frame RES0 image at x=266 extends 2 pixels across the 320px scanline; retained `2A380` accepts its genuine planar row spill while rejecting out-of-page writes.

## Acceptance runs and code

- **PASS** [Windows original-media #39879307471](https://github.com/Hyperbog/RLOFT/actions/runs/39879307471): ten decoded records, independent-byte equivalence, original cache reuse, all existing CTests.
- **PASS** [Windows original-media #39879605865](https://github.com/Hyperbog/RLOFT/actions/runs/39879605865): additionally gates all four party panel regions, positive source VGA changes, restored normal startup.
- Source changes: [exact decoder ledger `a6bc286f`](https://github.com/Hyperbog/RLOFT/commit/a6bc286f7963a3aca006047a1d27c35dac1003f9); [qualified independent verifier `475c8d71`](https://github.com/Hyperbog/RLOFT/commit/475c8d7139b0cbe1ffb7d85ef9cca6a7539e68aa); [four-panel source VGA gate `ad8d331a`](https://github.com/Hyperbog/RLOFT/commit/ad8d331abc39f890574ac78432a97f98ba80b4d1); [mandatory CI `cc9547af`](https://github.com/Hyperbog/RLOFT/commit/cc9547af24d88b89d017d2658c42dd60de6dccee).

## Next boundary

Do **not** treat a pre-transition mode-three HUD as a complete `1F8FC` frame. Next integrate original `1F1C8` scene composition callbacks (`33E28`, `33DE0`, `34DE8`, `1BDAC`, `2C294`, `274A4`), then enable actual mode-four `1F8FC`, native `1A5B0` presentation, and consecutive input-driven screenshot parity. Preserve the verified HUD/type-1 gates intact.
