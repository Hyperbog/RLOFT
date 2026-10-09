# Level 22 original RES4 forest-floor live integration — source checkpoint

**Date:** 2026-10-08 EDT · **Branch:** `demo-v1` · **Status:**
original-code-backed C++ live integration committed, tests registered;
**current-HEAD Windows build not run**. All GitHub Actions builds
remain manual dispatch only. Latest verified Windows run is #640 at
an earlier source SHA with 160/160 tests. Current source expects
**164 CTests** with Python enabled.

## Source-evidence chain now complete for the OPEN Level22 case

Original `CODE.2` SHA256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
requires each of these steps:

1. `16E55..16E90`: copy unsigned `INIT[+0x0E]`
   into live DWORD `DGROUP:258F8`, the RES4 bank base.
2. `1F388..1F398`: signed `LevelState.header[+7]`
   (Level 22 **+55**), add the original base, and
   load type-1 resource from archive bank 4.
3. `2F29C` verifies original EH type 1; the actual
   decoded palette-index tile is 64×64, 4096 bytes.
   `34AAC` samples from body+2. No wall-texture/RES8
   image or procedural grass is substituted.
4. `3838..38C3` and `34998`: original `SINTAB`
   supplies 128 perspective rows from projection_A/B
   and original sine quadrant handling. The 16.16
   camera's low-word high bytes set C048/C044 phases.
5. Original wall scan `335C8` initializes
   `DWORD [22220]=0x800` (nearest-wall sentinel).
   At `330F5`, drawable source material can reduce
   it; original **non-drawable 0x93** perimeter cells
   do not enter that branch.
6. `1F255..1F2B1`: if `22220==0x800`, original
   `FF50=0x80` and `EBX=0`. Then
   `1F372..1F384` computes lower `FF50=0x100-
   (EBX+FF50)=128`. For this open terrain
   the original `C026` initial value is **1**,
   selecting the textured lower path.
7. `1F3B4..1F3C3` sets lower planar byte
   offset `0x50F8`, row step -80, shade index
   step -1, then executes `34A74`.
   That raster writes x32..287, y259..132 in
   80 full four-plane VGA rows and 48 paired-plane
   VGA rows, 32,768 palette-index pixel stores.
8. Normal original scene layering: lower terrain
   is composed before the later RES3 type-7
   sprites, SINTAB walls and party/HUD overlays.

The old code had nearly all low-level pieces but drew
the flat `33DE0` lower surface unconditionally.

## Guarded live source-path

`ExplorationRuntime::initialize_gameplay_state` now
loads actual original `INIT` + level header +
original `RES4`, resolves the exact source-selected
absolute texture ID, and preserves original
decoded 4096 indexed pixels once per fresh Level22
runtime. Failures remain diagnostic rather than
guessed resources.

Each runtime frame carries the original source tile
(4098-byte native body with two **unused**
C040+2 alignment bytes), actual selection base,
original absolute index, SINTAB FF58 perspective
and C044/C048 camera phases.

`ExplorationPresenter::original_full_ground_eligible`
requires **all** of the following:

- Source level ID exactly 22;
- Authentic original RES4 type1 decoded 64×64 resource
  successfully verified;
- Authentic original 34998 FF58 perspective ready;
- Exact texture source length 4098 bytes;
- Exactly **256** original sweep records (never an
  empty/partial sweep fixture);
- On **every** original ray, both resource_inner
  and resource_outer are the non-drawable original
  sentinel `0x93`;
- The original immutable CODE.2 initial C026 byte
  selecting textured mode is nonzero.

Only when all are true is original
`OriginalGroundTextureRaster::draw` called with
full 128 lower rows, source SINTAB phases, authentic
indexed tile bytes and original FE15..FF14 shade
table. All original palette indices and planar
mask/sample logic come from verified code;
no procedural grass or copied stock artwork.

**Anything else remains on the flat-fill fallback**
with a precise reason recorded. This includes
partial/invalid sweeps, any drawable wall,
unknown other levels, bad RES4 resource, invalid
perspective and unimplemented dynamic clipping.
This is deliberately narrower than claiming
a universally complete original 34A74 mode.

The renderer reports `ground_textured_rasterized`,
`ground_indexed_writes` and
`ground_render_failure`. Opt-in TSV v2 now records
`native_lower_surface` as
`original-34A74-128row-open-field` versus
`flat-fill-fallback;other-modes-pending`, plus the
write count, fallback reason, original RES4
numeric source IDs and four original FF58 row
boundary checks. The Win32 preview title shows
`Ground RES4 textured` or `Ground flat fallback`.

## Regression coverage

Existing `original_ground_texture_raster_tests`
check SINTAB projection, authentic FE15 shading,
texel wrapping, both original palette-index
VGA phases, 80/48 split, upper/lower addresses
and transactional error rollback.

The new `original_ground_res4_audit_tests`
checks original EH type1 archive layout,
raw/LZSS/DC90 decoded-tile equality, exact
INIT+0x0E base, signed level-header+7
selection, original one-row pixel output
and failing/invalid lookup states.

Extended `exploration_presenter_tests` checks
strict eligibility of 256 all-0x93 ray records
and rejects a missing ray, changing either
outer or inner wall resource, truncated tile,
unverified RES4 source or wrong level.

Extended `exploration_frame_capture_tests`
checks both textured-render and flat-fallback
TSV reporting, source identity and error
values. All tests are **source-authored**;
no new current-HEAD MSVC/CTest results exist.

## Important visual acceptance boundaries

- This is the **FIRST actual live original forest-floor
  rendering code**, but its real GOG/Steam output has
  not yet been displayed/inspected.
- The original 256-ray all-0x93 Level 22 open-field
  case is the *only* supported texture activation.
  Other levels, visible walls, clipped ground
  `22220!=0x800`, dynamic C026/C027 changes,
  full original FF50 transitions and upper
  texture path remain unimplemented.
- The generated original tile's indexed output must be
  compared with actual DOS 320x400 raw frames,
  palette, freshly created party, exact 16.16 camera
  and original timer/NPC simulation state.
- The native trees, distant horizon, NPC AI, weapon
  animation and inventory visuals are separate
  outstanding equivalence gaps.
- C++ render path and synthetic regression tests
  are **not** sufficient grounds to advance any
  formal accepted demo gate.

## Next manual validation

1. Manually trigger
   [`demo-v1-build.yml`](https://github.com/Hyperbog/RLOFT/actions/workflows/demo-v1-build.yml)
   on `demo-v1` with optional Linux unchecked.
2. Require all **164** expected Windows CTests to
   pass; if compilation/tests fail, triage and fix
   before release.
3. Download a **new** artifact and launch the
   authentic fresh-game workflow, or opt-in
   `Capture-Level22.cmd`.
4. Check preview title `Ground RES4 textured`
   and TSV `ground_original_res4_verified=1`,
   `ground_original_res4_absolute_entry`,
   `native_lower_surface=original-34A74-128row-open-field`,
   `ground_raster_indexed_writes=32768`.
   If flat fallback, report exact error and
   source identities; do not hide it.
5. Upload 320x400 indexed captures, reference DOS
   screenshots/raw framebuffer and authentic source
   state for per-pixel side-by-side correction.

**Formal acceptance remains 1/5 gates (20%).**
No build was triggered.
