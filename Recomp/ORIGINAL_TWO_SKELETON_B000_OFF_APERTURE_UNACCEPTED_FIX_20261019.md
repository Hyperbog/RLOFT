# Part 23 — exact two-skeleton page-1 B0000 CPU-write boundary (unaccepted fix)

Date: 2026-10-19. Owning branch: `Recomp`. **NOT ACCEPTED:** Current
Windows/Linux CTest suite is red at `09faf0978c7322b6f09280e7f138a18c174e0f47`.
Do not present any Windows build from that revision as a verified fix.

## User-original evidence

Untouched owner `SAVE09(2).zip` produced a genuine full-frame rejection:
`36CEC res3_id=132 projected_size=411 vertical_mode=0`;
`369D8 SINTAB reason=6` (vga_write_out_of_range),
`ip_offset=1760`, `opcode=136` (`0x88`),
`modrm=135` (`0x87`), rejected write
`720946` (`0xB0032`), decoded RES3 allocation
`[4463888,4465729)`. The authentic camera and two primary
skeleton callbacks still passed. The earlier isolated sprite-only
capture produced two `36CEC` calls and 5,515 indexed writes.

Retail CODE.2 object 1 `369D8..36B0E` computes page-local EDI at
`369DF..36A15` and calls the generated `369C0` instruction
at `36AA9`. It does **not** apply the host interpreter's
`0xB0000`-exclusive write restriction. The host was failing before
staging a source CPU write to the B0000-BFFFF legacy-address region.

## Implementation attempted

- `original_sintab_raster.hpp` accepts a caller-defined destination
  limit. Existing wall call sites retain the strict 64K limit.
- Sprite-only `execute_original_369d8` permits bounded
  `A0000..BFFFF` CPU destinations, stages upper-window writes in
  guest memory, and does **not** cast them to a visible 16-bit planar
  aperture offset (which would introduce spurious first-page pixels).
- `original_code2_36cec_scene.hpp` atomically commits the bounded
  guest CPU address range and genuine planar host snapshot.
- Additional off-aperture CPU-write counters are emitted separately
  from the original source indexed-store count, and the private owner
  runner verifies the expanded marker.

**Scope caveat:** This host shadow policy is still a hypothesis about
offscreen VGA memory mapping, not independent DOS VGA/DAC behavior.
No DOS original indexed framebuffer oracle is available, and no
owner-private complete frame has passed this revision.

## Test results / precisely isolated test defect

Archived, different Level 5 genuine world frame regression
[39212657036](https://github.com/Hyperbog/RLOFT/actions/runs/39212657036)
**passed** at `5072da28`, including original save SAVE09 composed
frame, 127 authentic sprite writes, 128,000 indexed bytes, 768 RGB6
bytes, no published artwork.

Current Linux [39212772077](https://github.com/Hyperbog/RLOFT/actions/runs/39212772077)
and Windows [39212772017](https://github.com/Hyperbog/RLOFT/actions/runs/39212772017)
**failed** because CTest 74 asserts both
`guest[0xB0020u]==40u` and `guest[0xB0070u]==38u`
in `test_original_code2_369d8_raster.cpp:131`.
The test's existing `word(guest,code+2,0x210)` means `369D8`
patches instruction `code+0x210` to RET, stopping *before*
the synthetic second store at +80. Therefore the second store is
**not executed**, and `guest[0xB0070u]` must retain zero. The
precise expected test assertion is:

```cpp
assert(guest[0xB0020u]==40u && guest[0xB0070u]==0u);
```

The test *does* assert `offscreen.indexed_writes==2u` because
the first store executes in two VGA-mask passes. Current CTests
cannot be marked green. The project owner requested a maximum of
two autonomous correction attempts; stop here and do not claim a
validated executable. After explicit continuation, apply the single
assertion correction, rerun all 74 MSVC/Linux tests and independent
genuine saved-world CI before any new Windows artifact recommendation.

The two-skeleton private full-frame and original DOS pixel comparison
remain open. No saves, extracted RES, VGA frames or original graphics
are committed.
