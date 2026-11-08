# Part 28 — exhaustive untouched original saved-camera RES3 actor search

**Branch:** `Recomp`. **Retail source:** DnD-Classics original preserved
`MASTER/Ravenloft - Strahd's Possession` media.
**CODE.2 SHA256:** `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

## Implemented

`Recomp/src/code2_original_save_game.cpp` now supports
`--find-visible-res3 <read-only original save directory> <retail CODE.2>`.
It runs the genuine compiled `16D04 -> 16990` metadata loader, original
`07938 -> 181B4` SAVE and matching level loader, source SINTAB, and
`334C4 -> 32954` raycaster. It then:

1. Iterates original `0x14D80` primary actor records and applies the
   original `2B6D0` actor-flag `0x02` gate and original ray-marked
   source `0x198E0 + 2*cell` bit `0x40`.
2. Computes the original signed `35798` near-plane value using
   **compiled** original `35680`, actual `255EC/255F4` basis and
   actual `255F8/255FC` SINTAB sine/cosine.
3. Calls original compiled `35798` with temporary guest scratch
   output words, then restores their exact prior bytes. Counts
   depth `>=0x5800`, projected screen coordinate above `C02D` and
   nonzero scale. The original actor positions, camera, saved data,
   ray flags, tiles and resource descriptors are never rewritten.
4. Distinguishes actors passing projection but **not** the ray mark,
   actors ray-marked but behind the near plane, and actors satisfying
   both. A near-plane pass **alone does not prove visible sprite pixels**.

New workflow `.github/workflows/recomp-find-untouched-res3.yml` finds
all original saved snapshots in the preserved cloud directory plus the
original RLOFT/SAVE01 and CD-extracted root game save. It records source
save hashes (no media bytes) and runs the compiled original
`2C294` four-pass scene traversal and connected `36CEC -> 369D8` VGA
executor for **every** save, including those with no ray-visible
primary actors. Scene render outputs are private temporary files,
deleted after the run; no commercial frame bytes are uploaded.

## Complete verified inventory

**10** files = **8** distinct saved camera poses. Duplicate camera
positions occur for cloud SAVE00/SAVE10 and root SAVE01/CD-extracted
SAVE.DAT; all files were still independently loaded.

| Owner untouched snapshot | Level | Facing | Flag-allowed primaries | Primary actors passing 35798 near plane | Ray-marked eligible | Pass both | Original 36CEC calls |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| CD_Extracted/RLOFT/SAVE.DAT | 22 | 270 | 1 | 1 | 0 | 0 | 0 |
| RLOFT/SAVE01 | 22 | 270 | 1 | 1 | 0 | 0 | 0 |
| cloud SAVE00 | 5 | 252 | 77 | 70 | 0 | 0 | 0 |
| cloud SAVE01 | 23 | 50 | 33 | 1 | 3 | 0 | 0 |
| cloud SAVE02 | 21 | 216 | 2 | 1 | 0 | 0 | 0 |
| cloud SAVE03 | 5 | 15 | 80 | 19 | 0 | 0 | 0 |
| cloud SAVE04 | 5 | 190 | 41 | 7 | 0 | 0 | 0 |
| cloud SAVE08 | 5 | 181 | 22 | 8 | 0 | 0 | 0 |
| cloud SAVE09 | 5 | 187 | 25 | 24 | 0 | 0 | 0 |
| cloud SAVE10 | 5 | 252 | 77 | 70 | 0 | 0 | 0 |

On Level 5 SAVE00 and SAVE10, 69 of the 70 depth-passing primary
actors also satisfy nonzero-scale/above-horizon; the 70 count must
not be confused with actual sprite callbacks. All remaining
positive near-plane counts have matching projected count.
On Level 23 three genuine ray-marked actor cells have negative signed
camera depth (`-2815566 .. -246671`). Level 22 genuinely has an
actor passing near-plane projection, but that actor's cell is not
marked ray-visible by original `32954`.

### Source scene acceptance

All ten complete isolated `2C294` calls exited successfully with
**zero** `36CEC` invocations and **zero** indexed `369D8`
sprite writes. This includes the other original 2C294 record passes,
not only primary actors. There is **no untouched saved-camera
fixture in this verified original file inventory that can yet
exercise genuine RES3 sprite pixels.**

**CI source discovery:** [run 39198393585](https://github.com/Hyperbog/RLOFT/actions/runs/39198393585)
— checked=10, source_near_plane_candidates=0,
original_36cec_calls=0, original_369d8_indexed_writes=0,
full_source_scene_failures=0, genuine_sprite_pixels_confirmed=NO.
The previous narrow near-plane-only sweep succeeded as
[run 39198172931](https://github.com/Hyperbog/RLOFT/actions/runs/39198172931).

Original Level22 INIT and camera tests remain verified under
[ASan cross-level run 39196153446](https://github.com/Hyperbog/RLOFT/actions/runs/39196153446)
and [3/3 source census 39196153464](https://github.com/Hyperbog/RLOFT/actions/runs/39196153464).

## Interpretation and next original-only boundary

The static eight-camera inventory is now exhausted. Do not convert
non-ray-visible yet near-plane-passing actors into fake visible
source draws, invent DOS pixel equivalence, force ray flags, rotate
a saved camera, or overwrite source save bytes just to obtain an
acceptance test. The current synthetic source raster regression still
exercises `36CEC/369D8`, but genuine acceptance **remains open**.

If any additional owner-original SAVE.DAT/LEVnnn.DAT pair is added
under `DnD-Classics/cloud_saves/RLOFT`, this discovery workflow will
automatically enumerate it on a subsequent run. Otherwise, the next
reliable avenue is authentic DOS gameplay followed by a new
unmodified DOS-produced save at a naturally visible actor, or an
independently sourced/provenanced retail save pair with an actor
in view. Such a new original-generated save must be hashed and
archived before any testing. The same finder then runs without
modifying it.

**Independent DOS VGA 320x400 indexed data, DAC RGB6 palette,
genuine 36CEC sprite pixels and full DOS frame parity remain
unverified.**
