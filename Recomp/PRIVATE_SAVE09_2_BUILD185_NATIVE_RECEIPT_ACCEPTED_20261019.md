# Original private SAVE09(2) — Build #185 complete native frame receipt accepted

**Audit date:** 2026-10-19 · **Project branch:** `Recomp`.
**Scope:** Independently verify user-uploaded `private_validation_receipt.txt`,
`native_source_full_frame.log` and `two_skeleton_complete_native.bmp`
from owner-local `CHATGPT_Full_Frame_7`, using the source-native
four-party HUD and two-skeleton renderer. **No original game media or
native image/palette data are stored in this repository.**

## Result

**Source-native visual composition and private receipt: PASS.**
The original owner-local executable completed, its source log ends
with `CODE2_GENUINE_SAVED_SOURCE_FULL_FRAME_PASS`, and the
corrected runner wrote `CODE2_PRIVATE_COMPLETE_FRAME_VALIDATED=YES`.
The PNG/BMP visual audit confirms distinct original two-skeleton
shapes, four source-rendered party portraits, four equipment panels,
original source-based status number and a populated movement/control
HUD. The source label
`original_party_compositor_executed=YES` replaces the stale
`party_portraits_complete=NO` left by the earlier HUD-base milestone.

**Original DOS indexed-frame, CRTC and RGB6 DAC parity: NOT VERIFIED.**
This milestone must NOT accept continuous original CODE.1 boot, all
alternative/status party overlays, R08/R09/R12 DOS pixel parity or the
200 source sprite changed pixels at y=256..260.

## Exact original-private frame evidence

The receipt reports the original SAVE.DAT SHA256
`28a4b28473997d2682d0a4671e104b13fbad078eea49b0369205ed8ed3669542`
and LEV005.DAT SHA256
`b058964ce39d931e89417ef7f36f93640e48c57cbfdc9d5b363457fba238a672`.
These agree with previously independently inspected owner-original
private save identifiers. The save ZIP itself was **not** supplied in
this specific upload, so this audit independently verifies the
receipt/log/image, not those underlying ZIP member bytes again.

Receipt and log counters:
- Level 5, heading 198; 2 original source `36CEC` actor calls,
  5,515 generated raster CPU writes, **0** writes outside the
  modeled A000 aperture.
- Original source `RES0` gameplay base: 400 raster-frame blits.
- `1F494`: 4 original character records, 1,015 nonzero
  translation-table bytes, 25 resource lookups, 15 `298F0 -> 2A584`
  original masked blits, 6 `2975C` direct blits, 16 HP/status
  rectangles, 1 `2E374` border, 8 `1EA90` hand callbacks.
- Original status text: 1 signed `2E6F0` decimal conversion, 1
  centered `30508`, 1 authentic `303C8` glyph draw. This is
  expected for the **actual private saved state**. The synthetic
  two-status archived fixture is not an owner-private requirement.
  `unsupported_text=0`, `callback_failure=0`.
- Genuine source sprite #1 RES3 132 at bbox (55,105)..(106,253),
  2,276 changed indexed pixels, all above y256.
- Genuine source sprite #2 RES3 164 at bbox
  (139,87)..(246,260), 6,968 changed indexed pixels,
  6,768 above y256, 200 at y256..260. DOS parity for those
  200 pixels still awaits the independently captured retail VGA
  CRTC/split-screen state.

## Independent BMP and palette checks

User-uploaded 320×400 palettized BMP SHA256:
`dee27c6ca3f047bef1914a2f989b8c7aeebeb1613226d34b8f27cac2351c603c`.

The 128,000 decoded top-down indexed pixels have SHA256
`bdd0d5090c50802ae27b4b951af028d69c6981220cedb37f5fc9559567da9f10`,
**identical** to the receipt's native-frame SHA.

The 768 six-bit RGB palette bytes decoded as `BMP palette RGB8 >> 2`
have SHA256
`1f60d7831cd7b034ae67c894dc91662f15d4341fbf28e2a954b1b7e3e780f64f`,
**identical** to the receipt's native palette SHA.
All RGB6 values are within 0..63.

Additional independent image metrics:
- 119,130 nonzero indexed pixels; 210 used palette indices.
- Original lower HUD rows 260..399: 36,685 nonzero indexed pixels,
  0 wholly black rows.
- Four observed portrait/gear-panel vertical regions `y=300..399`:
  x0..52 has 3,576 nonzero pixels;
  x53..106 has 3,917;
  x211..263 has 3,904;
  x264..319 has 3,559.
- Current image **exactly equals** the prior independently audited
  Build #183 image at the indexed-pixel and RGB6 palette hash levels.
  The #185 correction was to the *validation rule* and the
  diagnostic, not to the original source rendering algorithm or
  saved original character records.

## Source revisions, build gates and next scope

- [Private one-status acceptance correction](https://github.com/Hyperbog/RLOFT/commit/6d4d1c448e8097423190c83527e2d685242503e5):
  runner changed original `$decimalText -lt 2` (incorrect synthetic
  fixture assumption) to genuine-private `$decimalText -lt 1`,
  preserving the numeral->centered->glyph and fail-closed checks.
- [Source party-compositor diagnostic correction](https://github.com/Hyperbog/RLOFT/commit/b310c9637899d19407703015decab7f2393b12ab):
  logs actual party masked-blit and status-text counters instead of
  claiming that source-rendered portraits remain incomplete.
- [Windows MSVC Milestone #185](https://github.com/Hyperbog/RLOFT/actions/runs/39225595722):
  76/76 CTests, PowerShell parsing, artifact publication
  [Windows package](https://github.com/Hyperbog/RLOFT/actions/runs/39225595722/artifacts/12173601567)
  succeeded. [Linux CTests](https://github.com/Hyperbog/RLOFT/actions/runs/39225595858),
  [source link](https://github.com/Hyperbog/RLOFT/actions/runs/39225595738),
  [authentic archived save](https://github.com/Hyperbog/RLOFT/actions/runs/39225595908),
  [RES3 actor](https://github.com/Hyperbog/RLOFT/actions/runs/39225595848)
  and [RES4 graphics](https://github.com/Hyperbog/RLOFT/actions/runs/39225595687)
  all passed.

**Native rendering acceptance:** private saved game source frame,
two active skeletons, original RES0 frame, four original RES1 party
portraits/hand/HP/status callbacks, raw indexed-frame and RGB6
palette receipt integrity — ACCEPTED.

**Outstanding:** independently capture original retail DOSBox or
hardware VGA four-plane memory, CRTC start/split/line-compare,
320×400 indexed scanout and RGB6 DAC palette from this precise saved
game/camera. Then run `Recomp/tools/compare_dos_indexed_frames.py`
against the native `.idx` and `.pal`. A matching native-only
receipt cannot be used as an independent DOS reference.
