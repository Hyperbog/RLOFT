# Original CODE.2 ordinary SINTAB wall pixels — first live integration

**Branch:** `demo-v1` · **Status:** implemented and committed; new CTests
are registered but a complete current-HEAD Windows build and original-DOS
pixel comparison have not yet run.

## Problem closed in source

The prior Level 22 presenter used generic `338C8` scaling only above
the SINTAB threshold and left every ordinary main-pass run at or below
that threshold unpainted. Consequently substantial original wall content
could not appear even though the underlying SINTAB interpreter existed.

The `SurfaceSintabMainExecutor` and `ExplorationPresenter::present`
now use the existing original generated-code interpreter to paint
supported ordinary `FCD0=0` wall runs into actual four-plane VGA video,
after the original `FCD0=1` preliminary pass and before `338C8`
large-extent surfaces and type-7 scene objects.

## Immutable original-byte evidence

Original `CODE.2` SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Object 1 begins at physical file `0x3AF24`. Source procedure
`3263C..32951`, with the `FCD0=0` branch at `3285B..3294A`:

- `3267A..32688` constructs the original extent from the live
  sweep record; `326F9..32708` selects the runtime SINTAB block using
  `extent & FFFCh`.
- `3268B..326F6` selects shade/decrement, clamps its two byte lanes,
  swaps on odd ray columns, and caps at `0808h`. The existing
  `SurfaceShadingModel::packed_subtract` supplies these original values.
- `3270B..32719`: page base + `C6E0 + (first_ray >> 2)` is the
  main-pass destination. It has **no** preliminary-pass vertical shift.
- `328EE..3294A`: three original `C668/C688/C6A8` sequencer mask
  passes, each invoking original `32620` with unchanged generated-code
  **entry**, ESI decoded RES4 source column, EDI destination, and ECX
  packed shade. The second and third calls are mask-conditional and
  increment destination BYTE addressing independently.
- The preliminary `FCD0=1` branch at `327A1..32856` is **different**:
  it temporarily patches the code with a RET and selects a nonzero
  generated-code entry. That path remains the separate prepass executor.

Original `SINTAB` SHA-256:
`19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87`.
The supplied file has 127 scale blocks and `C730=508`.
The original `CODE.2` phase-3/width-3 mask triplet is
`08h,03h,00h`; direct original-file crosscheck succeeded.

## Scope and safeguards

- Source-authenticated RES4 128-byte texture columns; no generated
  wall art and no interpolation in the in-range path.
- The existing canonical, six-opcode SINTAB interpreter runs a full
  generated code body from its initial instruction until its natural RET.
- The original plane masks only have verified grouped-run widths 1..7.
  Unsupported wider runs and incomplete/invalid resources remain
  pending with explicit per-resource/first-ray failure diagnostics.
- The four-plane target is transactional for each complete run.
  Failed later reads do not partially corrupt prior prepass/main pixels.
- `ExplorationSurfaceCoverage` now independently reports ordinary
  main runs rasterized, indexed writes, still pending, and failed.
  Failures never silently count as successful rendering.
- `338C8` remains active exclusively for extents above 508.

## Verification and remaining acceptance

Registered `surface_sintab_main_executor_tests` covers actual lane
masks, odd-column shade-byte swap, ordinary entry semantics,
page isolation, invalid geometry/source, and late failure rollback.
The existing presenter regression also checks failure accounting.

**Not yet established:** a successful consolidated Windows MSVC
build/CTest at this source SHA, full original Level 22 scene comparisons,
valid segmentation for coalesced runs wider than 7, and DOS time/pixel
equivalence. These are acceptance blockers, not implicit successes.

The next focused step is the current-HEAD manual Windows build (now
158 registered tests if Python is available), then review actual
Level 22 frames and any reported remaining ordinary run widths.
Do not mark this graphics milestone or the five-gate demo accepted
solely because the code was committed.
