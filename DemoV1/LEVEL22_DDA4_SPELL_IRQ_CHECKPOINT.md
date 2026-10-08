# CODE.2 3DE4 / 3EA4 spell-state callbacks and 1C21C IRQ queue

**Date:** 2026-10-08. **Branch:** `demo-v1`.
**Status:** original-byte audited; read-only C++ source and CMake tests committed.
No live character, spell, message-queue, clock or renderer mutation is claimed.

## Immutable executable provenance

User-supplied CODE.2 full SHA256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Object-1 original code begins at physical file `0x3AF24`; original
object-2 DGROUP image begins at physical `0x75F24`.

Original CODE.2 `DDA4:DF1F` calls `50DC(0,actor)` after
writing a class level, and **only when this returns nonnegative**
chooses `3EA4(actor,new_level)` if the actor's first class byte
is 1; otherwise it chooses `3DE4(actor,new_level)`.
It then executes `3210F`, `1C21C`, `DC7C`, and the eventual
`DBD4` recomputation in their original temporal order.

## 3DE4 / 3EA4 are different original routines

Both callbacks use a 20-by-8 **level/group** byte table indexed
by `(new_level-1)*8+group` for group 0..7, but not the same table:

- `3DE4:3E1C` indexes **DGROUP +62B0** (160 bytes).
  SHA256: `7bbea2507e78c36ee27c262f5f02942487594bd3ebb5bdf1e80a7ebdf1455378`.
- `3EA4:3EFA` indexes **DGROUP +6350** (160 bytes).
  SHA256: `afe212e67ee3aa1007ecc3688967d53c513fa0f968df4a1a3695faf2f25a64ea`.
- Both scan a shared **eight signed DWORD group-count table**
  at DGROUP +6420; its values in the supplied binary are
  `[7,6,5,4,4,2,0,0]`.
  SHA256: `20a62fd57d3da4cf6d4b2a6df39fff88a8a6df8522efdda5b839331aa8716bb9`.

Both use the family member `0x248` actor stride. For group `g`
and counted slot `s`, the touched byte is `actor + 0xBA+10*g+s`:

- **Nonzero table entry:** if the original byte's *low nibble*
  equals `0x0F`, overwrite its entire byte with zero; otherwise
  preserve the entire original byte.
- **Zero table entry:** if the low nibble differs from `0x0F`,
  overwrite its entire byte with `0xFF`; otherwise preserve it.
- Never scan the padded bytes beyond the signed count. A
  `0x2F` sentinel is treated like `0x0F`, **not** like
  a normal non-sentinel value.
- `3DE4` always overwrites actor **byte +0xC2** with `0x11`.
  `3EA4` always overwrites actor **byte +0xC1** with `0x11`
  and additionally overwrites **+0xC2** with `0x11` when
  `new_level >= 4`. Neither resets unrelated character bytes.

The original performs unchecked pointer arithmetic for invalid
new levels and source counts. The bounded native model supports
level 1..20 and actor 0..3; other situations return unsupported
rather than fabricating original memory behavior.

New source: `CombatLevelSpellTables::from_code2` and
`CombatLevelSpellModel::inspect`. The loader checks a
combined FNV64 fingerprint over three original table spans;
the external byte verifier checks the **separate SHA256**
for each span. The result contains a projected whole-party
snapshot but **does not mutate the caller's party image**.

## 1C21C IRQ message enqueue

Original `1C21C..1C257`:

1. Calls `30700`, reading the **full 32-bit independent IRQ
   counter** (not the pausable `A7E4` gameplay counter).
2. Stores `DWORD[CF54] = IRQ_tick + 0x5A`, with 32-bit wrap.
3. Copies source message bytes, including the first NUL, into
   original DGROUP buffer `14580`. Assembly copies in
   one- or two-byte steps and stops on either NUL position.
   It does **not** clear the bytes beyond the terminator.
4. Sets `BYTE[CF59]=1`.

Original `1C2CC..1C2D7` later reads `30700` again and
uses `CMP/JBE`: an expiry action runs only when
**unsigned current_tick > unsigned deadline**. This
is deliberately not a wrap-safe elapsed-time comparison.
Other original presentation/display actions in
`1C258..1C326` are not yet implemented.

`CombatIrqMessageModel::queue` and
`deadline_passed` preserve the *planned* writes and
unsigned expiry comparison. A missing source terminator or
insufficient provided writable destination bytes returns
unsupported. The original DOS routine had no such memory
checks; these host bounds protect incomplete captures and
must not be interpreted as DOS behavior.

## Verification and limitations

- New original instruction anchors: **50/50 byte matched**
  (33 in `3DE4/3EA4`, 17 in `1C21C/1C2CC`).
- Three independently verified SHA256 hashes for original
  spell tables and group counts; CODE.2 full SHA256 matched.
- Cumulative combat verifier: **457 named instruction checks
  at 449 distinct logical locations**, one direct Strength
  data table, seven hashed DGROUP data tables, one hashed CS
  jump table. Counts are not executable-coverage percentages.
- An independent **source-equivalent C++17** local harness
  tested **960 spell-state combinations**, reading original
  level rows/counts directly from CODE.2 and comparing
  complete projected actor images. IRQ bounds and 32-bit
  deadline behavior were tested alongside it. Strict
  `-Wall -Wextra -Werror -pedantic` Debug, optimized
  Release with enabled assertions, and UBSan **all passed**.
- The exact new repository C++ source/test files are committed
  and CMake-registered, but a **full checkout / consolidated
  CMake CTest / Windows executable build is not run**.
  No runtime DOS-versus-native timed trace is claimed.
- Neither spell-state projection nor queued-message projection
  is yet wired to live `DDA4` progression. Completing and
  integrating presentation/display callbacks and the shared
  RNG/IRQ event timeline remains necessary before claiming
  fidelity of an actual level-up.

**Overall faithful-demo acceptance remains 20% (1/5 gates).**
No private-repository GitHub Actions workflow was run.

## Next boundary

Reconstruct and test `1C258..1C326` display/expiry
effects, then consume exact original formatted `3210F`
message bytes and integrate the callback sequence in strict
`DDA4` order. Continue to distinguish real tick
observations from modeled inputs, and preserve all original
character state and RNG side effects before enabling
live XP/HP mutation.
