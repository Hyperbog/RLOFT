# LEV022.DAT

Status: **standalone mutable level-save structure proven against CODE.2 writer/loader**.

- Size: 6,028 bytes (`0x178C`)
- SHA-256: `f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e`
- Correlates structurally with `LEVELS.DAT` archive record 22 at file offset `0x5AD83`.

## Proven preamble

The first byte is `0xDF`; CODE.2's examined level loader/writer does not consume or assign byte 0, so its meaning remains unresolved. Seven little-endian dword boundaries begin at file offset 1:

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

The entire `LEV022.DAT` file validates exactly against the audited CODE.2 writer `171FC..17837` and direct-load path in `181B4..18D8B`.

See `../LEVELS.DAT/FORMAT.md` and `../LEVELS.DAT/inspect_levels.py`.

## Next phase

Use this file as the mutable comparison sample while semantically decoding each field against certified `Code2.cpp`. Do not infer gameplay meaning from numerical patterns alone.