# Part 23 — Private two-skeleton save 1F494 status-text failure and original-font repair
Date: 2026-10-19. Branch: `Recomp`.

## Owner-private failure evidence (not a completed frame)

Owner invoked Windows Milestone #176 with untouched `SAVE09(2).zip`,
and pasted PowerShell's `CHATGPT_Full_Frame_5` exception. The
read-only original private SAVE.DAT/LEV005.DAT identities remained
those previously verified. This log contained:

- original Level 5 x16=1419090, y16=1436586, heading=198, party=4;
- source RES3 132 and 164, two `36CEC` callbacks, 5,515 `369D8`
  generated CPU stores, **zero** off-aperture writes;
- independent native source sprite draw-page bboxes
  (55,105)..(106,253) and (139,87)..(246,260);
- authentic 400 `1F8FC` RES0 gameplay frame blits;
- `CODE2_ORIGINAL_1F494_1EA90_PARTY_HUD`: 4 original characters,
  1,015 active LUT bytes, 8 initial resource attempts, 5 masked RES1
  blits, 3 direct blits, 16 fills, 1 border, 8 `1EA90` hand calls,
  **unsupported_text=2 callback_failure=1**.
- isolated sprite fallback **succeeded** on the same private camera.

This is NOT a RES3/world failure: the old native bridge explicitly
set failure on both `1EA90 -> 2E6F0 -> 30508` status-text calls.
The subsequent `CODE2_AUTHENTIC_WORLD_RES4_FRAME_FAIL` was
the correct propagated result; it did not implicate RES4.

## Exact source correction

- Original retail `2E6F0` is a wrapper for the signed decimal
  `373BF` routine, targeting CODE.2's ten-byte guest scratch
  string at `0x2B64E`. The host now calls both ORIGINAL compiled
  source routines with bounded guest writes and NUL-terminated
  scratch, accepting only decimal ASCII and minus sign.
- Retail `30508` centers the numeral based on real source-font
  widths and executes `303C8`, which invokes source `2939C`
  or `293EC` per character depending on `F56C`. The host now
  passes the original `Application70Boundary` and
  `Application56Boundary`, uses original 4-plane
  `2A380` or translated `2A584` raster, and preserves original
  `2B680`/ `2B684` guest text-cursor updates. The host checks
  that raster placement remains within 320x400 rather than
  inventing bitmap font pixels.
- Authenticated original `RES0` item 0 is a **type-2 font**, not
  a type-1 ordinary sprite. Original `2ECB4` branches to
  `301A8` on type 2, which builds a font width-table descriptor
  and decodes its embedded original `2FD9C` image. The earlier
  host `WorldGraphicsSource::resource_loader` implemented only
  type 1 (`2FD9C`) and type 6/7 (`37590`), so
  `2F29C(0,0)` returned 0 and privately activated status
  text failed. Registered the original `301A8` adapter with
  the existing bounded guest allocator, stream decoder, cache and
  original image-loader callbacks.
- Signed `30508` centering accepts authentic numerals wider than
  the nominal field width, as in the retail SAR computation;
  it rejects only actual off-screen guest VGA bounds, not broad
  host-authored text clipping.
- The owner-local runner now requires 2+ source decimal formats,
  matching original centered-text callbacks, positive original
  glyph draws and zero unsupported callbacks for this **exact**
  private-skeleton save. Native exception summaries are bounded
  regex-extracted scalar diagnostics, not the entire literal
  backslash-n-delimited stdout stream.

## Independent regression/integration evidence

- New asset-free CTest
  `canonical_code2_original_373bf_30508_status_text_vga_glyphs`
  builds synthetic guest font resource-cache metadata (NOT asset
  data), executes original compiled `373BF` and
  `30508/303C8`, validates negative numbers, source glyph
  row/columns, centering, and original direct + translated
  `2939C/293EC` branches. Linux passes **76/76**.
- The first authentic-RES archive status fixture (before font
  loader repair), [CI #39223264122](https://github.com/Hyperbog/RLOFT/actions/runs/39223264122),
  correctly **failed** with
  `CODE2_ORIGINAL_1F494_RESOURCE_ERROR slot=0 id=0 reason=original party resource decode`.
  This isolated the distinct original RES type-2 font loader.
- [Authentic original-media status-font fixture CI #39223813874](https://github.com/Hyperbog/RLOFT/actions/runs/39223813874)
  **passed** after `301A8` was connected: real owner-preserved
  RES0/RES1, original archived SAVE09, two deliberately
  injected ***synthetic guest status fields***, real RES0 font
  image, original text raster, two decimal/centered text calls,
  positive glyph count, zero unsupported callbacks and successful
  128,000-byte indexed framebuffer / 768-byte RGB6 palette.
  Original SAVE09 archive on disk is untouched; the fixture's
  guest-state modifications are clearly logged
  `CODE2_SYNTHETIC_STATUS_FIXTURE_ONLY`, and its rendered frame
  is explicitly **not** original retail camera parity.
- [Authentic archive + status fixture CI #39224182491](https://github.com/Hyperbog/RLOFT/actions/runs/39224182491)
  also passed on the signed-centering correction.

## Open acceptance and next owner test

Source code / synthetic status integration have passed. Do **not**
state that the exact owner's private `SAVE09(2).zip` now
completes until they execute the matched Windows source package.
The current CI uses a different archived SAVE09 with a different
camera heading; no original private owner file is committed.

To approve private result:
1. Execute the matched packaged `run_two_skeleton_complete_frame.ps1`
   using an empty `CHATGPT_Full_Frame_6` directory; keep original
   SAVE09(2), CODE.2, RES0, RES1 and all game media unchanged.
2. Require 2 independent original skeleton on-screen footprints,
   5,515 original source sprite raster stores, no out-of-aperture
   writes, the authentic RES0 frame, four original character panels,
   eight hand calls, two real status-text draws and positive source
   font glyph raster.
3. Visually inspect the new `two_skeleton_complete_native.bmp`,
   do not infer correctness from callback counts alone.
4. Final DOSBox indexed VGA/CRTC and independent RGB6 DAC frame
   capture remains a separate acceptance gate, `NOT_VERIFIED`.

No proprietary code blobs, original art, source media, saved party
members, raw palette, BMP, or private SAVE09(2) bytes are committed.


## Verified Windows deliverable: Milestone #183

[Windows CI #39224182470](https://github.com/Hyperbog/RLOFT/actions/runs/39224182470)
at source commit `cbb3135016d4a3adb6eb7c99d2b98a088b9a9b15`
**SUCCESS**, **76/76 MSVC CTests** including
`canonical_code2_original_373bf_30508_status_text_vga_glyphs`,
and owner PowerShell script parse/syntax verification passed.

Matched Windows package
[`RLOFT-Recomp-Windows-Milestone-183`](https://github.com/Hyperbog/RLOFT/actions/runs/39224182470/artifacts/12172189656)
artifact ID `12172189656`, ZIP SHA256
`c46266360bfa2d1afe9973b399ade9541edb4a9e76dd5e3ba587efc3a3a69754`.
Package contains the updated executable and updated
`run_two_skeleton_complete_frame.ps1` from the **same** build.
Compiled source/runner commit includes the original 301A8 font loader,
2E6F0 signed-decimal, 30508/303C8 glyph drawing, and bounded native
diagnostics.

[Linux regression CI #39224182652](https://github.com/Hyperbog/RLOFT/actions/runs/39224182652)
passed. [Authentic archive plus original RES0 font synthetic-status
CI #39224182491](https://github.com/Hyperbog/RLOFT/actions/runs/39224182491)
passed after source-signed centering update.
[CODE1/CODE2 link gate #39224182432](https://github.com/Hyperbog/RLOFT/actions/runs/39224182432)
records the same build revision.

**Strict conclusion:** Original party font/status source path now has
green authentic-resource activation and a matched Windows deliverable.
Exact privately held two-skeleton image from Milestone #183 has
not been returned, and independent retail DOS VGA/DAC palette parity
remains not verified.
