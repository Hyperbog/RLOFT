# Original DOS sprite machine-state and raster binding checkpoint

**Date:** 2026-10-08. **Branch:** `demo-v1`.
**Scope:** `2A380`, `2A584`, `2A590/2A595`, VGA sequencer
port `03C4h` index `02h`, DGROUP `F009`, and
`2939C/293EC -> 2A380/2A584 -> 2E758` callback binding.

## Source and provenance

Original user-owned `CODE.2` SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
CODE.2 object-1 begins at physical file offset `0x3AF24`.
All code instruction addresses below are object-local logical offsets.

**Shaded code self-modification:** `2A584:2A590` stores full shader-table
DWORD at executable operand `2A60B`; `2A595` stores the same
DWORD at operand `2A672`, before `2AB38` or the sprite resource
header reads. Both operand instructions are `8A 83 <imm32>`
(MOV AL,[EBX+imm32]). The supplied executable initially contains
`0x12345678` at both locations; the live code may contain a
previously patched value after a shaded draw.

**Video port:** Both rasterizers execute `OUT DX,AX` to
`0x3C4` (sequencer index `0x02`) initially and before
each subsequent plane. The low four mask bits select the active
VGA plane(s); the resulting mask remains a hardware-register
value after the sprite pixels have been written. Clearing VGA
pixels does not reset this register.

**Transparent-height global:** The normal branch at `2A42B`
and shaded branch at `2A651` each store the current `ECX`
height as `DWORD[F009]` after the first sequencer OUT and
before per-byte transparency copying. Opaque raster branches
do not change F009.

Five new opcode signatures independently matched original
CODE.2 at `2A5F3`, `2A62C`, `2A64F`, `2A651`
and `2A6A3`. The earlier compiler-opcode ledger already
contained both shade patches and the normal F009 store.
**Cumulative ledger: 653 named checks at 645 distinct
instruction sites** (not a measure of whole-game coverage).

## Reusable implementation

The following shared-engine updates were committed:

- `VgaPlanarMemory` now records an optional sequencer
  map-mask register. Its initial value is **unknown** until an
  observed port write; clear/import/export operations do not
  fabricate a sequencer reset.
- `CombatOriginalSpriteMachineState` owns a mutable
  captured **object-1 code byte array** and optional observed
  `F009` value. It is not a global singleton.
- `CombatOriginalSpriteCodePatchModel::patch_2a584`
  checks both `8A 83` instruction prefixes before writing
  either 4-byte little-endian shader-pointer operand. Repatches
  preserve adjacent instructions.
- `CombatOriginalSpriteRasterModel` takes an optional
  machine-state pointer, writes the original VGA sequencer
  plane mask before each pixel-copy pass, writes F009 on the
  first pass of transparent draws, and patches both shaded
  instruction operands before sprite setup. Normal/opaque
  calls do not change those operands or F009.
- `CombatOriginalSpriteRasterBinder::bind` connects the
  existing `2939C/293EC` dispatch callbacks to actual normal
  and shaded `VgaPlanarMemory` writes. It requests raw
  original sprite and shader memory by their **original DOS
  pointers**, reads mutable video-register inputs per call,
  retains original row/column argument order, and lets the
  original `2E758/F0C8` callback run **after rasterization**.
  The shade pointer is patched before the supplied resource
  readers execute, matching original observable sequencing.

The binder fails closed if authentic sprite/shader backing
bytes or a required callback are missing. It never invents
a sprite header, shading palette, pixel format or VGA clock.
Original unchecked memory behavior on corrupted resources
is not claimed to be safely emulated.

## Tests and remaining boundaries

New CMake/CTest target
`combat_original_sprite_machine_tests` covers actual original
CODE.2 instruction bytes when the owned file is provided,
repeated full-DWORD patches, neighboring-code preservation and
invalid capture rejection.
`combat_original_sprite_raster_binding_tests` verifies the
2939C and 293EC raster handoff, real indexed VGA pixels,
shader patch visibility **before** the resource reader, final
sequencer mask and deferred F0C8 call order. Existing raster
and VGA memory tests now also assert sequencer/F009 behavior.

The exact committed machine, raster and binder source Git
blobs were checked against local independent copies.
Isolated strict C++17 `-Wall -Wextra -Werror -pedantic`
Debug, optimized Release (assertions enabled), and UBSan
runs passed, including original-owned CODE.2 operand tests
and all 320 sprite starting-column offsets. One local
Release test harness initially removed assertions through
`NDEBUG`; that harness was corrected before final checks.
These are **isolated component builds**, with a minimal
VGA/dispatcher interface where needed, not a full Windows
repository CMake/CTest run for the new revision.

The previous complete Windows MSVC build
[#634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281)
passed **130/130 CTests**, but at earlier source SHA
`40e3f14b1072a7a0180ea1b0b4c1cea437137631`.
None of these newer registered tests is included in that
historical green result. No GitHub Actions run was triggered
for this checkpoint.

## Next verifiable gate

Bind actual DOS resource and shader-memory providers to
the currently injectable reader interface. Then reconstruct
original `1A8C8` compositing and `2E758/F0C8` refresh
targets, and compare the native four VGA plane byte streams,
original IRQ observations and frames with a reference DOS
execution. The binder is **reusable for the entire game**,
not a Level 22 hardcoded drawing shortcut.

**Overall faithful-demo acceptance remains 20% (1/5).**
The private Windows workflow remains **manual-only**.
