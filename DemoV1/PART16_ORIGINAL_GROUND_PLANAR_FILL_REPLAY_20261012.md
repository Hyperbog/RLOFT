# Original CODE.2 C050/FDBC dual-page planar fill proof — Pre-Alpha

Source owner: `demo-v1`; this is a **diagnostic graphics building
block**, not an installed replacement for normal `ExplorationPresenter`.
The eventual Alpha requires a full first-Level22 field/DOS parity pass.

The immutable original `CODE.2` is **558689 bytes**, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Its LE object-1 physical base is `0x3AF24`.

- Original `1F1D6` loads DWORD[C050], shifts left 15, adds
  `0xA0000`, and stores `FDBC`: the page stride is `0x8000`
  per planar **byte**, not a chunky-pixel stride.
- Original `33DE0..33E25` is 70 bytes, SHA-256
  `af26ff06080d1a9c321f187185a2c751c8ae1f982b26380607daf62a4c4f0dbe`.
  EDI starts `FDBC+50F8`; `REP STOSB` writes 64 bytes with all
  planes selected (`OUT 03C4h,0F02h`), then net row advance **−80**.
- Original `33E28..33E6A` is 67 bytes, SHA-256
  `8e9622be88f5265cddb916d4cc41ff0ee1c93714952ce31d7a37863495844da0`.
  EDI starts `FDBC+0148`; the net row advance is **+80**.
- Both source loops run EBX 128..1 and write each pixel value
  `byte(base-[FE94+EBX])` (wrapping unsigned byte subtraction).

New `OriginalGroundPlanarFillReplay` writes exactly these addresses,
colors and four-plane masks to a caller-owned `VgaPlanarMemory`
simulation for C050 page 0 or page 1. Any other page fails before
mutating it. A C++ Release-asserted regression compares **all 320×400
indexed pixels** against the existing independent chunky
`SurfaceFillModel`, checks both page banks, original write counts
and edge addresses, and verifies all four plane lanes. Original CODE.2
SHA plus both **complete original function hashes** are optionally
validated by `verify_original_ground_planar_fill.py --original`,
which also rejects three mutated source assumptions. Public CI
runs the source-only guard; original copyrighted data is not committed.

**Critical remaining condition:** Original ES's DOS/4GW selector
mapping is **not** yet independently established for ordinary
gameplay. This replay is explicitly named `replay_flat_es` and
does not touch live native drawing or certify original 22220/FF50
depth, C026/C027 mode, complete event callback order or DOS pixels.
Do not mark ALPHA-VIEW complete from a passed CTest.
