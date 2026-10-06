# LEV022.DAT

Status: **standalone mutable level-save structure and Level-22 runtime delta are preservation-grade reconstructed against CODE.2 writer/loader and the LEVELS.DAT baseline. One 40-cell generated-cache provenance discrepancy is explicitly bounded and preserved.**

- Size: 6,028 bytes (`0x178C`)
- SHA-256: `f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e`
- Correlates structurally with `LEVELS.DAT` archive record 22 at file offset `0x5AD83`.

## Proven preamble

The first byte is `0xDF`. The writer is now instruction-traced far enough to prove that byte 0 is **not writer-owned serialized level state**: it creates/reopens the file, seeks directly to `0x1D` before the first payload write, and at the end seeks to offset `1` to write the seven boundary dwords. No writer operation writes offset 0. Existing files should preserve the byte, but a native format model should not assign it gameplay meaning.

Seven little-endian dword boundaries begin at file offset 1:

`0x1D, 0x645, 0x722, 0x738, 0x10AE, 0x10AE, 0x15C2`

These are section boundaries, not the initially suspected dword sequence beginning at offset zero.

## Proven level properties

- Width: 30
- Height: 15
- Header count at DGROUP-derived offset `0x258A2`: 1
- Header count at `0x258A6`: 110
- Header count at `0x258A4`: 0
- Final length-prefixed text size: 0

Unlike the baseline `LEVELS.DAT` record, this standalone file stores the expanded direct-save form: two width-indexed 10-byte-list groups separated by one byte of state, followed by 22-byte runtime records. The corresponding archive record uses one legacy list group and compact 11-byte records which CODE.2 expands while loading.

The exact Level-22 comparison is now closed at the mutable-state level:

- exactly 313/450 cell words changed, and every change is precisely `archive ^ direct == 0x0440` (current-visibility + explored bits only);
- all ten group-1 10-byte records are unchanged, active-animation count is zero, and group 2 is empty;
- the single family-A ASSASSIN moved from x-cell 13 to 18, changed facing 2 to 6, and the standalone record preserves `current_hp = 15` plus `action_state_countdown = 4`;
- all 110 family-B records are exactly the deterministic archive expansion, with no mutable delta;
- family C remains empty and the `0x514` event table remains byte-identical/all zero;
- the saved automap cache reflects the explored state, subject to the separately documented 40-cell provenance discrepancy.

See `MUTABLE_DELTA.md` and `../LEVELS.DAT/FAMILY_A.md`.

The complete `LEV022.DAT` section geometry and serialized state ordering validate against writer `171FC..17837` and the direct-load path in `181B4..18D8B`. Recomputing the final cache through certified routine `01558` differs from the supplied cache at exactly 40 unexplored code-1 cells; this is retained as a provenance/production-path discrepancy rather than being hidden by changing the reconstructed writer.

See `../LEVELS.DAT/FORMAT.md` and `../LEVELS.DAT/inspect_levels.py`.

## Current boundary

LEV022.DAT now serves as a certified mutable-state fixture rather than an undecoded sample. Its structural form, direct family-A actor state, deterministic family-B expansion, cell exploration delta, generated cache relationship, and writer-unowned byte 0 are documented. Remaining questions are bounded shared provenance/source-name issues tracked in `../LEVELS.DAT/README.md`; there is no unidentified LEV022 record region. See `../LEVELS.DAT/FINAL_AUDIT_2026-10-06.md` for the combined archive/direct-save certification.