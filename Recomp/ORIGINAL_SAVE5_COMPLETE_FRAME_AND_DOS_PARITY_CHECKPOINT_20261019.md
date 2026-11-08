# Part 23 — native complete saved frame and independent DOS comparison boundary

**Owning branch:** `Recomp`. **Date:** 2026-10-19.
**C++ implementation:** [`335a9d1`](https://github.com/Hyperbog/RLOFT/commit/335a9d1343bcdcb9dbc6c7506fc1cce05dacdead).
**Owner-local runner:** `Recomp/tools/run_two_skeleton_complete_frame.ps1`.
**Source-host CI:** [`39209293953`](https://github.com/Hyperbog/RLOFT/actions/runs/39209293953).
**Windows MSVC engineering build #153:** [`39208908321`](https://github.com/Hyperbog/RLOFT/actions/runs/39208908321) (compiled from `335a9d1`).

## Implementation

The `rloft_original_code2_world_frame` engineering executable now
supports `--save-complete-frame SAVE.DAT LEV005.DAT SINTAB RES4 COLORS
CODE.2 INIT RES0 NPCS.DAT OBJECTS.DAT RES3 output-stem` (12 file
arguments plus output stem). It intentionally shares the *proven*
`16D04 -> 07938 -> 181B4 -> 334C4 -> 32954` source
initialization and camera preparation with `--save-sprite-probe`,
then calls `WorldFrame::render(true,...)` instead of emitting onto
an isolated blank VGA buffer.

Composed draw order is original RES4 sky/ground, source world quads,
original ray-depth walls, authentic `2C294 -> 36CEC -> 369D8` sprite
rasterization and original RES0 HUD. Native output is a 128,000-byte
320x400 indexed file, a 768-byte DAC RGB6 file and a palettized BMP.
The code retains the `DOS_VGA_PARITY=NOT_VERIFIED` provenance boundary.

## Independently verified host integration test, not owner two-skeleton validation

The separate unmodified archive `cloud_saves/RLOFT/SAVE09` Level 5,
heading 187 degrees, was exercised in the new workflow integration
step on Linux. **Workflow completed successfully** at `22979cde`.
It asserts `source_36cec_calls=1`, `raster_indexed_writes=127`,
the full world/HUD pass, the indexed frame length, palette length
and BMP nonemptiness. The resulting private native hashes were:

- Indexed frame SHA256:
  `63d22eab3b9465983a53c887b78bd979f7045bac4a29673b85c0964b3fae6e56`
- Native DAC RGB6 palette SHA256:
  `1f60d7831cd7b034ae67c894dc91662f15d4341fbf28e2a954b1b7e3e780f64f`

All source media and rendered output from CI were deleted; the workflow
published only scalar counters and hashes. This **does not** prove the
private camera with two onscreen skeletons produces the same complete
scene, and this hash is not an original-DOS reference.

## Exact private two-skeleton save

The user-provenanced, **not repository-hosted** file
`SAVE09(2).zip` contains original `SAVE.DAT` SHA256
`28a4b28473997d2682d0a4671e104b13fbad078eea49b0369205ed8ed3669542`;
its companion `LEV005.DAT` SHA256 is
`b058964ce39d931e89417ef7f36f93640e48c57cbfdc9d5b363457fba238a672`.

The previous Windows Build #152 *sprite-only* owner probe verified
Level 5, heading 198, actors #38/#70, two original projected
callbacks, 2 genuine `36CEC` calls and **5,515 indexed writes**.
The current user's exact private ZIP has NOT yet been executed
through the new *complete-frame* command or visually inspected.
The local PowerShell runner verifies these exact SHA256 identities
and source counters, runs the updated Windows #153 executable
without editing any original files, and retains private
`two_skeleton_complete_native.idx`, `.pal` and `.bmp`.
Private intermediate copies are deleted after execution.

## Independent DOS VGA comparator

`Recomp/tools/compare_dos_indexed_frames.py` already performs
strict byte-for-byte frame (320x400) and DAC RGB6 (256x3)
comparisons and reports the first divergent pixel/channel. The
new PowerShell runner can invoke this *only when both original DOS*
`--dos-index` and `--dos-palette` inputs are explicitly supplied.
The user must acquire those by an independent protected-mode
original-DOS execution under an attested emulator/debugger or
real VGA hardware capture, with exact matching save/camera,
animation phase and display page.

DOSBox-X debugger `VGA`/`DACPAL` can inspect graphics state,
but a conventional `MEMDUMPBIN` or host screenshot alone is NOT
proof of all four displayed planar VRAM bytes and exact original
RGB6 DAC register contents. An acceptable capture adapter must
document four planes, CRTC start/panning/line-compare/display
mode, 320x400 palette indices and the complete 256-entry RGB6 DAC
at a specific synchronized frame. No independently attested original
DOS frame has been captured in this workstream.

**Acceptance accounting:** composed native source-scene path verified
on archive SAVE09; owner's exact two-skeleton full frame and independent
DOS pixel parity remain open. R04, R08, R09 and R12 formally unaccepted.
