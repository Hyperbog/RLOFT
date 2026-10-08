# Level 22 / CODE.2 E0F8 respawn candidate model — 2026-10-08

## Evidence and scope

**Original authority:** immutable `CODE.2` SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`,
object-1 file start `0x3AF24`. Original x86 at logical `E0F8..E281`,
`290D0..290F1`, `30700`, `30CA2`, and `1C328..1C365` was re-examined
directly. The existing executable-byte verifier now contains **293 original
instruction signatures plus one distinct 26-byte Strength table anchor**;
20 additional E0F8 dependency instructions were matched in the supplied
original on this checkpoint.

## Newly implemented read-only model

`CombatNpcRespawnModel::inspect` operates on a caller-supplied snapshot. It:

1. Reads the **low 16 bits** of original IRQ time `30700`, invoking
   `290D0`'s fallback to the BIOS timer **word at `0x46C` only when zero**,
   then seeds the original `30CC4`/LCG stream.
2. Draws x and y through two successive `30CA2` calls for each of **at
   most three** attempts: `1 + ((random15 * (signed_dimension-2)) >> 15)`.
3. Tests map-cell **low byte** bits `0x40` and `0x3F` at
   `DGROUP:198E0 + 2*(y*width+x)`; checks **both** independent
   signed-coordinate distance thresholds `>=15` from party cell
   `255D2/255D6`; rejects candidate cells occupied by non-dead live
   NPCs by original `+0x0B & 0x02` and `+0x0C` cell index.
4. On acceptance, plans a change to the existing 22-byte NPC slot:
   half-cell `0x8000` coordinate fractions, new cell index,
   definition `DWORD+0x35` runtime flags, `WORD+0x10=0`,
   previous packed `WORD+0x14 & 0xFE39`, and original `1C328`
   `Nd6` HP using definition `BYTE+0x1D`. The NPC type at
   `WORD+0x0E` is **not rewritten**. On failure it performs **six RNG
   calls and no HP roll**.

The model returns an optional result: invalid/incomplete snapshots are
**unsupported**, not silently treated as `E0F8` returning zero. It does
**not** mutate the world or the shared RNG, invent tracker entries, generate
unobserved IRQ ticks, or connect to a live player attack.

## Original Level 22 geometric constraint

The existing direct `LEV022.DAT` map is **30 columns by 15 rows**.
For valid party positions y=0..14, the E0F8 generated y candidates are
always 1..13. Therefore `abs(candidate_y-party_y) < 15` throughout:
**E0F8 cannot succeed in this map while the party remains in bounds.**
That is a *conditional property of this specific routine and map*, not proof
that the original game invokes E0F8 here: the signed type entries at
`258AA` and live caller context still need to be observed. When actually
invoked, the rejected attempts can still change the original RNG state;
the 290D0 reseed from original IRQ timing is equally material.

## Verification boundary

- `combat_npc_respawn_tests.cpp` checks first-try success/record mutations,
  expected RNG/HP state, map `0x40/0x3F` versus high-byte flags,
  live/corpse NPC occupancy, exact three-try Level 22 failure, BIOS
  fallback, and unsupported snapshots.
- The isolated source model compiled with strict C++17 warnings and
  tests passed **locally** (including `-DNDEBUG` with active test asserts),
  using a matching minimal declaration of the repository's
  `LiveNpcRecord` and the repository-equivalent `OriginalRandom`.
  **This is not a full repository or Windows build.**
- The CMake library source and new CTest target are registered but not
  yet executed in GitHub Actions. Do not spend private Actions minutes
  without deliberate authorization. No Windows executable was refreshed.

## Follow-on gates

1. Observe actual Level 22 original `258AA` tracker words, `E0F8`
   caller arguments, NPC slot allocation and IRQ tick trace.
2. Connect `E284` corpse presentation, conditional real `E0F8`
   invocation and original RNG shared state through a single temporal
   transaction; reject incomplete snapshots rather than fabricating
   outcomes.
3. Finish `BF44`/presentation `38110`/`1DDA4` story and award
   effects, original `24478/26018/E5F0` live hand dispatch, and
   reference-DOS timing/frame comparison.
4. Run deliberate consolidated complete build/CTest and original-asset
   comparison before any new playable-combat or 100%-fidelity claim.

**Bounded E0F8 source-model milestone:** coded, locally exercised and
committed, **not live integrated**. Overall five-major-gate faithful-demo
acceptance is unchanged: **20% (1/5)**.
