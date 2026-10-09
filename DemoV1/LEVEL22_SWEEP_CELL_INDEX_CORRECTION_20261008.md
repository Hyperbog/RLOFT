# Level 22 first-person sweep cell-index correction — 2026-10-08 EDT

**Classification:** confirmed native reconstruction defect, not malformed
shipped level geometry. **Status:** original-machine-code comparison,
fixed source and assertion-enabled regression committed. Current-HEAD
Windows build **not run**; GitHub Actions remains manual-only.

## Failure report and independently preserved input

User screenshot displays:

```text
Ravenloft Demo Build v1
first-person sweep failed to leave malformed level
```

The user supplied archive `1791508927564.zip`, SHA-256
`4cae065ba0c045664a446f0f46fb0671376ef90577d438c27c6b823b634656ba`,
has **16 indexed native frames**, each 128,000-byte `.idx`,
768-byte `.pal`, and optional v2 `.tsv`. The latest recorded
pose is cell (11,7), heading 298, with original 16.16 coordinates
`A=759417`, `B=468295`. This is the last *saved* pose, not
an assertion about the exact pose/heading when the later exception
occurred.

All captures recorded active original `C029=1` RES3
individual-column rendering. The 16th frame reported
**51 rasterized / 0 pending / 0 failed** original RES3
scene calls, **848 rasterized columns**, and zero plan/trace
failures. The runtime exception belongs to the later
`ViewSweepTraversalModel::trace_ray` path rather than an
RES3 raster failure.

## Root cause: invalid extra division of the CELL INDEX

Both source-machine branches already compute a **cell index**:

- left `CODE.2 32BE6..32BFA`: `MOVSX EAX,WORD [248E6]`,
  `MUL width`, `ADD EAX,EBX`, `MOV ESI,EAX`,
  **`SHL ESI,1`**. At `32C1F`, **`SHR ESI,1`**
  recovers the unchanged cell index for a bounds check;
  `32C39` doubles it *again* to access 16-bit
  `cell_words`.
- right `CODE.2 32BFC..32C10`: `MOV EAX,ECX`,
  `MUL width`, **`ADD AX,WORD [248EA]`** (important:
  low-word-only), `MOV EDI,EAX`, **`SHL EDI,1`**.
  On the right-hand path at `32D09`,
  **`SHR EDI,1`** recovers the exact original index.

The previous native code stored the **undoubled** flattened
index in `left_expr`/`right_expr`, but erroneously did
`raw_index=expr>>1` again when checking the cell. This
selected different cells, corrupted depth, visibility,
automap exploration, source resource identity, and scene
occlusion, and could fail to progress for near-cardinal
rays.

The narrow original-faithful correction is to use
`raw_index=left_expr` and `raw_index=right_expr`
directly. All 32-bit wrapping, signed axes, low-AX-only
right arithmetic, nontrivial dynamic-edge branches,
resource-bank selection and malformed-map loop guard
remain unchanged.

## Reproduction using immutable original data

Original inputs used in the offline sweep simulation:

- `CODE.2` SHA-256:
  `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- `SINTAB` SHA-256:
  `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87`.
- Original `LEV022.DAT`: 30×15, **364 empty
  interior cells and 86 code-1 perimeter cells**.
- Last saved camera A/B from user's TSV, and heading 104
  as a **separate reproducible test orientation**, not
  an assertion that the user actually turned to 104.
  The original SINTAB angle offset at ray 178 is -159
  tenths, giving `1040-159=881` (88.1°).

With the former extra shifts, a sample near-cardinal
original ray remained in an interior cell through more
than 3,600 simulated side-steps and hit the C++ guard.
With the corrected source indexing, **all 256** tested
rays terminated at original map borders: the 88.1°
representative reached cell index **210 (0,7)** after
21 loop guards. The original heading-298 center ray
reached cell index **444 (24,14)**, a perimeter cell.
Independent corrected-sweep simulations at positions
(759417,468295), (819200,491520) and several headings
returned 256/256 terminating rays without the guard
trigger. This simulation is **not** a current-HEAD
MSVC build or original DOS runtime frame comparison.

## Tests and acceptance implications

`view_sweep_traversal_tests.cpp` was corrected to
expect cell index 20 instead of 10 in the existing
6×4 fixture (3×6+2, not half of it), and adds:

1. A synthetic copy of the exact original Level-22
   perimeter and measured original SINTAB sine/tangent
   constants, with source-recorded fixed camera A/B.
2. Near-cardinal ray (881 tenths) returning actual
   perimeter cell index 210; no malformed-map exception.
3. Opposite sweep branch at heading 298, cell index
   444, verifying resource 0x93 and visibility bits.

**Earlier native Level 22 captures have the wrong
cell-index traversal semantics.** They remain useful
for tracking source draw-call counts and regression
history but cannot certify original DOS wall-ray,
occlusion, automap or framebuffer parity. Fresh captures
with this corrected build are needed.

Build #639 was successful at an **earlier** SHA; no
new GitHub Actions run was triggered here. Formal demo
acceptance remains **1/5 gates (20%)**, pending current
Windows compilation, all CTests, and DOS comparison.

## Next manual validation

Run the Windows `demo-v1` workflow on the new commit,
leave Linux unchecked, capture Level 22 at (11,7)
and nearby poses, rotate through directions with
near-90° rays, and check for the absence of the
previous exception. Compare corrected scene
projection/occlusion and exact indexed output to
real DOS emulator captures separately.
