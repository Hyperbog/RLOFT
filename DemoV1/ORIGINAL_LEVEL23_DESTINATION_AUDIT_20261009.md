# Original Level22 mist destination — LEVELS.DAT Level23 geometry audit

**Purpose:** original-game code recreation, not a new or convenient level teleport.  
**Original sources:** immutable supplied CODE.2 and LEVELS.DAT.

## Exact source identities

- Original CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Original LEVELS.DAT SHA-256: `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`.
- Original CODE.2 object-1 physical offset `0x3AF24`.
- The binary's `22361..223A2` sequence invokes `181B4(0x17)` (Level23) **after** mist frame index `0x14` (20); stores signed column 9 at `255D2`, signed row 58 at `255D6`, low fractional words `0x8000`, and facing `0xB4` (180 degrees).

## Actual original archive inspection

`LEVELS.DAT` begins with a DWORD count of **32** and 32 little-endian DWORD record offsets. Records 22 and 23 start at the following physical byte positions:

| Archive record | Byte range [start,end) | Original map dimensions | Signed header +7 RES4 selector |
| --- | --- | --- | --- |
| Level 22 | `0x5AD83..0x5C00B` | 30×15 | +55 |
| Level 23 | `0x5C00B..0x605E7` | 64×64 | +17 |

The destination `(coordinate_a=0x00098000, coordinate_b=0x003A8000)` is therefore **valid on actual Level23** (column 9, row 58). It would be out of bounds on Level22's 30×15 grid. **This is a materially different level**, not merely a ground texture swap or a camera movement within the original Level22 testbed.

The `INIT+0x0E` base plus **Level23 selector +17** must be resolved afresh on entry; using **Level22's selector +55** would render the wrong terrain after the original frame-20 handoff.

## Implementation impact

The `LevelContinuityStore` supports the original Level23 entry geometry and carries the current session and mutable Level22 world. The expanded `level_continuity_tests` includes the exact original event destination coordinate/facing, validates the 64×64 bounds and preserves the departing Level22 direct-LEV source flag in its saved snapshot.

The original code event/scene compositor and Level23 asset loading are NOT yet called by a finished live mist cutscene. The geometry assertions are synthetic test fixtures cross-checked against the original binary/archive metadata, not a DOS-vs-native acceptance claim.

Current registered CTests: **175** (Quick 19). Last compiled Windows build: **#647, 172/172** at older source `fb4a0145`. The current-head tests remain **unbuilt**.
