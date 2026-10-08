# DDA4 and DBD4: original class thresholds and attack target

Checkpoint: 2026-10-08. Source-only; no Windows EXE or live level-up applied.

Original CODE.2 SHA256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

## Proven original byte and data dependencies

At logical DDA4:DECF the signed class byte at actor +15
selects a 40-byte row at DGROUP +7D9C. The DEE0 word read
scans at most 20 little-endian unsigned thresholds per class;
the comparison is against unsigned DWORD class XP divided
by 1000 (DEEA). A threshold equal to the quotient qualifies.
The scan stops at the first higher threshold, giving a
zero-to-20 new level. The old level at actor +20 +2*lane is
read signed at DEFF.

Six class rows (codes 0..5) of 20 WORDs are confirmed in the
supplied CODE.2 data object; the 240-byte SHA256 is
`61e4f494c427a51e457a52da8e704477f1e5e7fff823e09b162fab7c5c701308`.

At DBD4..DC63 the routine considers the first (strict) maximum
signed class level across counted class lanes (first tie wins).
It takes the selected signed class code and uses two original
BYTE tables at DGROUP +6827/+6828. The calculation is
`20 - (highest_level / denominator) * numerator` using x86
signed quotient truncation, and DDA4:DFE3 stores only the
low resulting byte at actor +10A. The six coefficient pairs
are verified from original bytes without hardcoding the
table into the native executable. The 12-byte SHA256 is
`096e8cc68d13fba7899b37774f0e66c8f412869aacc4d77be93877835bead8d1`.

29 more x86 opcode signatures checked directly against the
original CODE.2 bring the total to **347 opcode anchors**,
plus one direct Strength data anchor and two hashed table
anchors.

## Native reconstruction

`CombatProgressionTables::from_code2` reads the original
owned CODE.2 data object. It verifies a 64-bit fingerprint
over the full 240+12 bytes and rejects unsupported data.
`CombatXpLevelModel::inspect` consumes the previously
source-backed read-only DDA4 XP projection, reads each
valid class lane and calculates the original threshold level.
It reports changes as **pending original callbacks**, not
applied changes.

`CombatAttackTargetModel::inspect` separately evaluates
DBD4 on an **observed actor state after any callbacks**.
It rejects missing positive class levels, unsupported class
codes and a zero class denominator rather than simulating
undefined original memory access.

The model does not mutate actor records. In particular,
it does **not** synthesize side effects from original calls
50DC, 3EA4, 3DE4, 3210F, 1C21C and DC7C, or modify
the actual attack-target byte. Later lane projections cannot
serve as exact live execution traces until those prior
callbacks have been integrated in original order.

## Validation and next gate

Source and focused regression are registered in CMake.
An isolated strict C++17 core model compiled and ran against
the owned original CODE.2 in Debug and Release; this was
**not** the consolidated repository/Windows CTest. Full
original DOS progression timing and callback-state comparison
remain outstanding.

Next: reverse each class-change callback and the DC7C
post-level service, then connect completed DDA4 progression
to the Level 22 death event without changing its original
BF44/drop/presentation/corpse ordering. Overall five-gate
demo acceptance stays 20%; manual Actions only.
